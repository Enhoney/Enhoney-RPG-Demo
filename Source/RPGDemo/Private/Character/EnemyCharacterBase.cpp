// Copyright Enhoney.


#include "EnemyCharacterBase.h"

#include "EnhoneyAbilitySystemComponent.h"
#include "EnemyAttributeSet.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhoneyAbilityBase.h"

#include "Net/UnrealNetWork.h"
#include "PlayerCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "RPGDemo.h"
#include "EnhoneyPlayingHUD.h"
#include "DamageFloatWidgetController.h"

#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "EnhoneyGameplayTags.h"

#include "AI/EnhoneyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/PawnSensingComponent.h"

#include "MotionWarpingComponent.h"

#include "InventoryComponent.h"
#include "PlayerTaskComponent.h"
#include "PlayerInterface.h"


bool FDroppedItemConfig::GetRandomDropItem(FInventoryItem& OutDroppedItem) const
{
	// 先判断是否掉落
	if(FMath::FRand() <= DropProbability && ItemType.IsValid())
	{
		// 计算掉落数量
		int32 ItemDropNum = FMath::RandRange(MinDropNum, MaxDropNum);
		OutDroppedItem = FInventoryItem(ItemType, ItemDropNum);
		return true;
	}
	return false;
}


void FEnemyDiedDropsConfig::GetAllDropItems(TArray<FInventoryItem>& OutDroppedItems) const
{
	for (const FDroppedItemConfig& DropConfig : DroppedItemsConfig)
	{
		FInventoryItem DroppedItem;
		if (DropConfig.GetRandomDropItem(DroppedItem))
		{
			OutDroppedItems.Add(DroppedItem);
		}
	}
}



AEnemyCharacterBase::AEnemyCharacterBase()
{
	EnemyAbilitySystemComponent = CreateDefaultSubobject<UEnhoneyAbilitySystemComponent>(TEXT("EnemyAbilitySystemComponent"));
	EnemyAbilitySystemComponent->SetIsReplicated(true);
	EnemyAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	EnemyAttributeSet = CreateDefaultSubobject<UEnemyAttributeSet>(TEXT("EnemyAttributeSet"));

	// 对EnemyLocking 执行Overlap
	GetMesh()->SetCollisionResponseToChannel(ECC_EnemyLocking, ECR_Overlap);

	// 锁敌UI
	EnemyTargetWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("EnemyTargetWidget"));
	EnemyTargetWidgetComponent->SetupAttachment(GetRootComponent());
	EnemyTargetWidgetComponent->SetVisibility(false);

	// 默认巡逻状态下，移动速度较慢
	GetCharacterMovement()->MaxWalkSpeed = 300.f;

	// 感知组件
	SensingComponent = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("SensingComponent"));

	// MotinWarpingComponent
	MotinWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotinWarpingComponent"));

}

UAbilitySystemComponent* AEnemyCharacterBase::GetAbilitySystemComponent() const
{
	return EnemyAbilitySystemComponent;
}

int32 AEnemyCharacterBase::GetCharacterLevel_Implementation() const
{
	return CharacterLevel;
}

bool AEnemyCharacterBase::IsCharacterAlive_Implementation() const
{
	return EnemyState != EEnemyState::EES_Dead;
}

void AEnemyCharacterBase::CharacterDie_Implementation()
{
	EnemyState = EEnemyState::EES_Dead;

	// 发放奖励--只在服务器执行
	if(HasAuthority() && KillerPawn.IsValid())
	{
		if (KillerPawn->Implements<UPlayerInterface>())
		{
			UInventoryComponent* PlayerInventoryComponent = IPlayerInterface::Execute_GetPlayerInventoryComponent(KillerPawn.Get());
			check(PlayerInventoryComponent);
			// 掉落物品
			TArray<FInventoryItem> DroppedItems;
			EnemyDiedDropsConfig.GetAllDropItems(DroppedItems);
			// 直接添加物品到背包--在服务器上调用ServerRPC函数，就会像普通函数一样在服务器上执行
			PlayerInventoryComponent->ServerAddItems(DroppedItems);

			if (UPlayerTaskComponent* PlayerTaskComp = IPlayerInterface::Execute_GetPlayerTaskComponent(KillerPawn.Get()))
			{
				// 尝试增加击杀计数--任务系统
				PlayerTaskComp->TryAddCommitItemCount(EnemyTypeTag);

				// 尝试增加物品计数--任务系统
				for (const FInventoryItem& DroppedItem : DroppedItems)
				{
					if (DroppedItem.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Task"))))
					{
						PlayerTaskComp->TryAddCommitItemCount(DroppedItem.ItemTag, DroppedItem.ItemNum);
					}
				}
			}
			
			// 掉落金币
			IPlayerInterface::Execute_AddCoinNum(KillerPawn.Get(), EnemyDiedDropsConfig.GoldNum * CharacterLevel);

			// 掉落经验
			IPlayerInterface::Execute_AddCurrentExp(KillerPawn.Get(), EnemyDiedDropsConfig.ExpNum * CharacterLevel);
		}
	}

	// 向所有玩家发送Event，让它更换锁定的目标
	if (bTargetLocked)
	{
		TArray<AActor*>	PlayerCharacters;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerCharacterBase::StaticClass(), PlayerCharacters);

		for (AActor* PlayerCharacter : PlayerCharacters)
		{
			FGameplayEventData Payload;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(PlayerCharacter, FEnhoneyGameplayTags::Get().AbilityEventTag_EnemyLocking_FindNextEnemy, Payload);
		}
	}

	// 隐藏被锁定的图标
	EnemyTargetWidgetComponent->SetVisibility(false);
	// 死亡动画，或者布娃娃-由动画蓝图来控制死亡动画播放，这里只需要取消碰撞即可，动画在动画蓝图中控制
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// 三秒后销毁
	SetLifeSpan(3.f);
}

FVector AEnemyCharacterBase::GetFireSocketLocation_Implementation(FName FireSocketName)
{
	return GetMesh()->GetSocketLocation(FireSocketName);
}

void AEnemyCharacterBase::PlayHitReactionAnim_Implementation(const FHitResult& ImpactResult)
{
	// 播放受击动画
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		check(HitReactionMontage);
		AnimInstance->Montage_Stop(0.1f);
		AnimInstance->Montage_Play(HitReactionMontage, 1.f);
	}
}

void AEnemyCharacterBase::SetAsTargetLocking_Implementation()
{
	bTargetLocked = true;

	// 显示被锁定的图标
	EnemyTargetWidgetComponent->SetVisibility(true);
}

void AEnemyCharacterBase::QuitTargetLocking_Implementation()
{
	bTargetLocked = false;

	// 隐藏被锁定的图标
	EnemyTargetWidgetComponent->SetVisibility(false);
}

void AEnemyCharacterBase::MulticastShowDamageFloat_Implementation(const FGameplayTag& DamageType, bool bIsCriticalHit, float InDamageValue)
{
	MulticastShowDamage(DamageType, bIsCriticalHit, InDamageValue);
}

void AEnemyCharacterBase::MulticastUpdateHealthBar_Implementation(float NewCurrentHealth, float NewCurrentMaxHealth)
{
}

AActor* AEnemyCharacterBase::GetCurrentCombatTarget_Implementation() const
{
	return CombatTarget.IsValid() ? CombatTarget.Get() : nullptr;
}

void AEnemyCharacterBase::SetCombatTarget_Implementation(AActor* NewCombatTarget)
{
	CombatTarget = NewCombatTarget;
}

AActor* AEnemyCharacterBase::GetCurrentPatrolPoint_Implementation() const
{
	return CurrentPatorlTarget.IsValid() ? CurrentPatorlTarget.Get() : nullptr;
}

void AEnemyCharacterBase::SwitchPatrolPoint_Implementation()
{
	// 仅在服务器执行切换
	if (!HasAuthority())
	{
		return;
	}

	// 巡逻点不能为空，巡逻点数量不能少于2个
	if (PatrolPoints.IsEmpty() || PatrolPoints.Num() < 2)
	{
		return;
	}

	// 如果当前巡逻点就在指定的巡逻点中，就找下一个巡逻点
	if (PatrolPoints.Contains(CurrentPatorlTarget))
	{
		// 获取当前巡逻点的Index
		int32 CurretPatrolIndex = PatrolPoints.Find(CurrentPatorlTarget);
		// 如果是最后一个巡逻点，就将其设置为第一个巡逻点
		if (CurretPatrolIndex == PatrolPoints.Num() - 1)
		{
			CurrentPatorlTarget = PatrolPoints[0];
		}
		else
		{
			// 如果不是，就设置为下一个巡逻点
			CurrentPatorlTarget = PatrolPoints[CurretPatrolIndex + 1];
		}
	}
	else
	{
		// 如果没有，就将第一个巡逻点设置为当前巡逻点
		CurrentPatorlTarget = PatrolPoints[0];
	}
}

EEnemyState AEnemyCharacterBase::GetEnemyState_Implementation() const
{
	return EnemyState;
}

void AEnemyCharacterBase::EnemyEnterCombat_Implementation(APawn* NewCombatTarget)
{
	EnterCombat(NewCombatTarget);
}

void AEnemyCharacterBase::SetWarpingFacing_Implementation()
{
	if (CombatTarget.IsValid())
	{
		FVector CombatTargetLocation = CombatTarget->GetActorLocation();
		MotinWarpingComponent->AddOrUpdateWarpTargetFromLocation(TEXT("FacingTarget"), CombatTargetLocation);
	}
}

void AEnemyCharacterBase::SetKillerPawn_Implementation(APawn* InKillerPawn)
{
	KillerPawn = InKillerPawn;
}

UEnemyAttributeSet* AEnemyCharacterBase::GetEnemyAttributeSet() const
{
	return EnemyAttributeSet;
}

void AEnemyCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyCharacterBase, EnemyState);
}

void AEnemyCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	EnemyAIController = Cast<AEnhoneyAIController>(NewController);
	check(EnemyAIController);

	// 初始化黑板
	check(EnemyBehaviorTree);
	EnemyAIController->GetBlackboardComponent()->InitializeBlackboard(*EnemyBehaviorTree->BlackboardAsset);
	EnemyAIController->RunBehaviorTree(EnemyBehaviorTree);
	
	// 初始化黑板资源
}

void AEnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// 初始化ASC
	InitAbilityActorInfo();

	// 初始化属性
	InitEnemyAttributes();

	// 赋予固有技能
	EndowEnemyInherentAbility();

	// 绑定代理，进入战斗状态
	SensingComponent->OnSeePawn.AddDynamic(this, &AEnemyCharacterBase::EnterCombat);
}

void AEnemyCharacterBase::InitAbilityActorInfo()
{
	EnemyAbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AEnemyCharacterBase::InitEnemyAttributes()
{
	UEnhoneyAbilitySystemLibrary::ApplyAttributeEffectToSelf(this, GameplayEffectForAttribute, EnemyAbilitySystemComponent);
	UEnhoneyAbilitySystemLibrary::ApplyAttributeEffectToSelf(this, GameplayEffectForVitalAttribute, EnemyAbilitySystemComponent);
}

void AEnemyCharacterBase::EndowEnemyInherentAbility()
{
	if (HasAuthority())
	{
		if (IsValid(EnemyAbilitySystemComponent))
		{
			// 主动技能
			if (!EnemyOffensiveAbilitiyClasses.IsEmpty())
			{
				for (const TSubclassOf<UEnhoneyAbilityBase> OffensiveAbilityClass : EnemyOffensiveAbilitiyClasses)
				{
					EnemyAbilitySystemComponent->EndowInherentAbility(OffensiveAbilityClass, false);
				}
			}

			// 被动技能
			if (!EnemyOffensiveAbilitiyClasses.IsEmpty())
			{
				for (const TSubclassOf<UEnhoneyAbilityBase> PassiveAbilityClass : EnemyPassiveAbilitiyClasses)
				{
					EnemyAbilitySystemComponent->EndowInherentAbility(PassiveAbilityClass, true);
				}
			}
		}
	}
}

void AEnemyCharacterBase::EnterCombat(APawn* NewCombatTarget)
{
	//  巡逻状态时，进入战斗状态
	if (EnemyState == EEnemyState::EES_Inspection)
	{
		if (NewCombatTarget->Implements<UCombatInterface>())
		{
			if (ICombatInterface::Execute_IsCharacterAlive(NewCombatTarget))
			{
				EnemyState = EEnemyState::EES_Combat;

				// 设置战斗目标
				CombatTarget = NewCombatTarget;
			}
		}
	}
}

void AEnemyCharacterBase::MulticastShowDamage_Implementation(const FGameplayTag& DamageType, bool bIsCriticalHit, float InDamageValue)
{
	if (GetWorld() && GetWorld()->GetFirstPlayerController())
	{
		AEnhoneyPlayingHUD* EnhoneyHUD = Cast<AEnhoneyPlayingHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());

		if (IsValid(EnhoneyHUD))
		{
			EnhoneyHUD->GetDamageFloatWidgetController()->EnableDamageFloatComponent(this, DamageType, bIsCriticalHit, InDamageValue);
		}
	}
}
