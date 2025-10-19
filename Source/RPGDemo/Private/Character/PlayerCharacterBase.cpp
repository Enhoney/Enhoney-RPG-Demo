// Copyright Enhoney.


#include "PlayerCharacterBase.h"

#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "EnhoneyWeaponComponent.h"
#include "DamageBoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "EnhoneyPlayerState.h"
#include "EnhoneyPlayingHUD.h"
#include "EnhoneyAbilitySystemComponent.h"
#include "EnhoneyPlayerController.h"
#include "EnemyCharacterBase.h"
#include "EnemyInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhoneyGameplayTags.h"
#include "EnhoneyPlayerAbilityInfo.h"
#include "EnhoneyAbilityBase.h"

#include "PlayerAttributeSet.h"
#include "EnhoneyAbilitySystemLibrary.h"

#include "Net/UnrealNetwork.h"

#include "RPGDemo.h"
#include "EnhoneyLogChannel.h"

#include "TaskWidgetController.h"

APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->TargetArmLength = 600.f;
	CameraBoom->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraBoom);
	Camera->bUsePawnControlRotation = false;

	// 不要使用Controller的Rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// 设置朝向为移动方向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	// 设置转向速度
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	// 索敌查询球型碰撞检测，初始化设置为无碰撞
	EnemyLockingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("EnemyLockingCollision"));
	EnemyLockingSphere->SetupAttachment(GetRootComponent());
	EnemyLockingSphere->SetSphereRadius(500.f);	// 半径为5m
	EnemyLockingSphere->SetCollisionObjectType(ECC_EnemyLocking);
	EnemyLockingSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	EnemyLockingSphere->SetCollisionResponseToChannel(ECC_CombatMesh, ECR_Overlap);
	EnemyLockingSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PlayerWeaponComponent = CreateDefaultSubobject<UEnhoneyWeaponComponent>(TEXT("WeaponComponent"));
	PlayerWeaponComponent->SetupAttachment(GetMesh());
	PlayerWeaponComponent->SetIsReplicated(true);

	MeleeDamageBox = CreateDefaultSubobject<UDamageBoxComponent>(TEXT("MeleeDamageBox"));
	MeleeDamageBox->SetupAttachment(PlayerWeaponComponent);
	MeleeDamageBox->SetIsReplicated(true);
}

void APlayerCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (GetLocalRole() == ENetRole::ROLE_Authority)
	{
		InitAbilityActorInfo();

		// 初始化属性
		LoadProgress();

		// 赋予固有技能
		EndowPlayerInherentAbility();

		// 显示PlayingWidget 主UI，在DS服务器上没有HUD，所以没有问题，这里调用的目的是让单机模式能够看到UI
		InitPlayingUI();
	}
}

void APlayerCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (GetNetMode() == ENetMode::NM_Client)
	{
		if (!IsValid(PlayerAbilitySystemComponent))
		{
			InitAbilityActorInfo();

			// 初始化属性
			LoadProgress();
		}
		
		// 显示PlayingWidget 主UI
		InitPlayingUI();
		
	}
}

void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bTargetingEnemy)
	{
		HandlePlayerRotationOnEnemyLocking(DeltaTime);
		
	}
}

void APlayerCharacterBase::InitPlayingUI()
{
	if (AEnhoneyPlayerController* EnhoneyPlayerController = Cast<AEnhoneyPlayerController>(GetController()))
	{
		if (AEnhoneyPlayingHUD* EnhoneyHUD = Cast<AEnhoneyPlayingHUD>(EnhoneyPlayerController->GetHUD()))
		{
			AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState());
			EnhoneyHUD->InitPlayingWidgetUI(EnhoneyPlayerController, EnhoneyPlayerState, PlayerAbilitySystemComponent, PlayerAttributeSet);
			// 初始化DamageFloat对象池
			EnhoneyHUD->InitializeDamageFloatWidgetController(EnhoneyPlayerController, EnhoneyPlayerState, PlayerAbilitySystemComponent, PlayerAttributeSet);
		}
	}
}

void APlayerCharacterBase::HandlePlayerRotationOnEnemyLocking(float InDeltaTime)
{
	if (!IsValid(PlayerTargetEnemy))
	{
		EmptyEnemyTime += InDeltaTime;
	}
	else
	{
		EmptyEnemyTime = 0.f;
	}

	// 如果空锁敌超时，就手动结束这个能力
	if (EmptyEnemyTime > 5.f)
	{
		FGameplayEventData Payload;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FEnhoneyGameplayTags::Get().AbilityEventTag_EnemyLocking_EndAbility, Payload);
		return;
	}

	// 设置PlayerCharactetr朝向
	if (IsValid(PlayerTargetEnemy))
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;

		FVector TargetEnemyLocation = PlayerTargetEnemy->GetActorLocation();
		FRotator NewRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetEnemyLocation);

		NewRotator = FRotator(0.f, NewRotator.Yaw, 0.f);

		SetActorRotation(NewRotator);
	}
	else
	{
		// 如果锁定的敌人无效，就恢复原来的Rotation配置
		GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

void APlayerCharacterBase::EndowPlayerInherentAbility()
{
	if (HasAuthority())
	{
		if (AEnhoneyPlayerState* EnhoneyPlayerState = GetPlayerState<AEnhoneyPlayerState>())
		{
			UEnhoneyAbilitySystemComponent* EnhoneyASC = CastChecked<UEnhoneyAbilitySystemComponent>(GetAbilitySystemComponent());
			// 赋予固有主动技能
			TArray<FPlayerAbilityInfo> InherentOffensiveAbilities = EnhoneyPlayerState->PlayerAbilityInfo->GetInherentOffensiveAbilityInfo();
			if (!InherentOffensiveAbilities.IsEmpty())
			{
				for (const FPlayerAbilityInfo& InherentOffensiveAbility : InherentOffensiveAbilities)
				{
					EnhoneyASC->EndowInherentAbility(InherentOffensiveAbility.AbilityClass, false);
				}
			}
			// 赋予固有被动技能
			TArray<FPlayerAbilityInfo> InherentPassiveAbilities = EnhoneyPlayerState->PlayerAbilityInfo->GetInherentPassiveAbilityInfo();
			if (!InherentOffensiveAbilities.IsEmpty())
			{
				for (const FPlayerAbilityInfo& InherentPassiveAbility : InherentPassiveAbilities)
				{
					EnhoneyASC->EndowInherentAbility(InherentPassiveAbility.AbilityClass, true);
				}
			}
		}
	}
}

UAbilitySystemComponent* APlayerCharacterBase::GetAbilitySystemComponent() const
{
	return PlayerAbilitySystemComponent;
}

int32 APlayerCharacterBase::GetCharacterLevel_Implementation() const
{
	AEnhoneyPlayerState* EnhoneyPlayerState = CastChecked<AEnhoneyPlayerState>(GetPlayerState());

	return EnhoneyPlayerState->GetCharacterLevel();
}

bool APlayerCharacterBase::IsCharacterAlive_Implementation() const
{
	AEnhoneyPlayerState* EnhoneyPlayerState = CastChecked<AEnhoneyPlayerState>(GetPlayerState());

	return EnhoneyPlayerState->IsPlayerAlive();
}

void APlayerCharacterBase::CharacterDie_Implementation()
{
	AEnhoneyPlayerState* EnhoneyPlayerState = CastChecked<AEnhoneyPlayerState>(GetPlayerState());

	// 死亡
	EnhoneyPlayerState->SwitchAliveState(false);


}

FVector APlayerCharacterBase::GetFireSocketLocation_Implementation(FName FireSocketName)
{
	return GetMesh()->GetSocketLocation(FireSocketName);
}

void APlayerCharacterBase::PlayHitReactionAnim_Implementation(const FHitResult& ImpactResult)
{
	if (!PlayerHitReactionMontages.IsEmpty())
	{
		// 计算受击方向
		EEnhoneyHitDirection HitReactionDirection = UCommonAlgorithmLibrary::GetHitDirection(ImpactResult, GetActorForwardVector(), GetActorLocation());
		// 找到受击动画蒙太奇
		UAnimMontage* HitReactionMontage = PlayerHitReactionMontages.FindChecked(HitReactionDirection);
		// 播放受击动画
		if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Play(HitReactionMontage);
		}
	}
}

AEnhoneyPlayerController* APlayerCharacterBase::GetEnhneyPlayerController_Implementation()
{
	if (AEnhoneyPlayerController* PlayerController = Cast<AEnhoneyPlayerController>(GetController()))
	{
		return PlayerController;
	}
	return nullptr;
}

bool APlayerCharacterBase::IsPlayerLocallyControlled_Implementation() const
{
	return IsLocallyControlled();
}

bool APlayerCharacterBase::CanAttributeConsumed_Implementation(int32 AttributePointToComsume) const
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		return EnhoneyPlayerState->CanAttributeConsumed(AttributePointToComsume);
	}
	return false;
}

void APlayerCharacterBase::ConsumeAttributePoint_Implementation(int32 AttributePointToComsume)
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		EnhoneyPlayerState->ConsumeAttributePoint(AttributePointToComsume);
	}
}

void APlayerCharacterBase::AddAttributePoint_Implementation(int32 AttributePointToAdd)
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		EnhoneyPlayerState->AddAttributePoint(AttributePointToAdd);
	}
}

bool APlayerCharacterBase::CanSkillPointConsumed_Implementation(int32 SkillPointToComsume) const
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		return EnhoneyPlayerState->CanSkillPointConsumed(SkillPointToComsume);
	}
	return false;
}

void APlayerCharacterBase::AddSkillPoint_Implementation(int32 SkillPointToAdd)
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		EnhoneyPlayerState->AddSkillPoint(SkillPointToAdd);
	}
}

void APlayerCharacterBase::ConsumeSkillPoint_Implementation(int32 SkillPointToComsume)
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		EnhoneyPlayerState->ConsumeSkillPoint(SkillPointToComsume);
	}
}

void APlayerCharacterBase::AddCoinNum_Implementation(int64 CoinNumToAdd)
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		EnhoneyPlayerState->AddCoinNum(CoinNumToAdd);
	}
}

bool APlayerCharacterBase::CanCoinNumConsumed_Implementation(int64 CoinNumToComsume) const
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		return EnhoneyPlayerState->CanCoinNumConsumed(CoinNumToComsume);
	}
	return false;
}

void APlayerCharacterBase::ConsumeCoinNum_Implementation(int64 CoinNumToComsume)
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		EnhoneyPlayerState->ConsumeCoinNum(CoinNumToComsume);
	}
}

void APlayerCharacterBase::AddCurrentExp_Implementation(int32 ExpToAdd)
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		EnhoneyPlayerState->AddCurrentExp(ExpToAdd);
	}
}

int32 APlayerCharacterBase::GetCurrentExp_Implementation() const
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		return EnhoneyPlayerState->GetCurrentExp();
	}
	return 0;
}

int32 APlayerCharacterBase::GetExpToLevelUp_Implementation() const
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		// TODO ：设计升级所需经验值曲线表
		return 0;
	}
	return 0;
}

void APlayerCharacterBase::PlayerLevelUp_Implementation(int32 TargetLevel)
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		EnhoneyPlayerState->SetCharacterLevel(TargetLevel);
		// 播放升级特效
		/*if (IsValid(LevelUpEffect))
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), LevelUpEffect, GetActorLocation(), FRotator::ZeroRotator, true);
		}*/
	}
}

UPlayerAttributeSet* APlayerCharacterBase::GetPlayerAttributeSet_Implementation() const
{
	return GetPlayerAttributeSet();
}

bool APlayerCharacterBase::GetPlayerTargetEnemy_Implementation(AActor*& OutPlayerTargetEnemy)
{
	OutPlayerTargetEnemy = PlayerTargetEnemy;

	return bTargetingEnemy;
}

void APlayerCharacterBase::EnterTargetEnemyLocking_Implementation()
{
	bTargetingEnemy = true;

	// 启用碰撞
	EnemyLockingSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EnemyLockingSphere->SetGenerateOverlapEvents(true);
}

void APlayerCharacterBase::UpdateTargetEnemy_Implementation(AActor* NewTargetEnemy)
{
	PlayerTargetEnemy = NewTargetEnemy;

	// 如果更新之后找到的TargetEnemy无效，就表明范围内没有敌人了，就重新启用碰撞
	if (bTargetingEnemy)
	{
		if (!IsValid(NewTargetEnemy))
		{
			EnemyLockingSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			EnemyLockingSphere->SetGenerateOverlapEvents(true);
		}
		else
		{
			EnemyLockingSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			EnemyLockingSphere->SetGenerateOverlapEvents(false);
		}
	}
	else
	{
		EnemyLockingSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		EnemyLockingSphere->SetGenerateOverlapEvents(false);
	}
}

void APlayerCharacterBase::QuitTargetEnemyLocking_Implementation()
{
	if (IsValid(PlayerTargetEnemy))
	{
		// Enemy退出被锁定的状态
		IEnemyInterface::Execute_QuitTargetLocking(PlayerTargetEnemy);
	}

	bTargetingEnemy = false;
	PlayerTargetEnemy = nullptr;
}

void APlayerCharacterBase::SwitchEquippedWeapon_Implementation(const FGameplayTag& NewWeaponTag)
{
	PlayerWeaponComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, 
		PlayerWeaponComponent->GetWeaponAttachmentSocketName(NewWeaponTag));
	PlayerWeaponComponent->SwitchEquippedWeapon(NewWeaponTag);
}

UEnhoneyWeaponComponent* APlayerCharacterBase::GetPlayerWeaponComponent_Implementation()
{
	return PlayerWeaponComponent;
}

void APlayerCharacterBase::SetMeleeCollisionEnabled_Implementation(bool bIsEnabled)
{
	if (IsValid(PlayerWeaponComponent))
	{
		MeleeDamageBox->SetCombatBoxCollisionEnable(bIsEnabled);
	}
}

UAnimMontage* APlayerCharacterBase::GetCurrentCommontAttackMontage_Implementation()
{
	if (IsValid(PlayerWeaponComponent))
	{
		return PlayerWeaponComponent->GetCurrentCommontAttackMontage();
	}

	return nullptr;
}

FGameplayTag APlayerCharacterBase::GetCommonAttackAbilityTag_Implementation() const
{
	FGameplayTag OutCommonAttackAbilityTag;

	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();

	if (PlayerWeaponComponent->GetCurrentEquippedWeapon().MatchesTagExact(EnhoneyTags.Weapon_CurvedKnife))
	{
		OutCommonAttackAbilityTag = EnhoneyTags.Ability_Offensive_Inherent_CommonAttack_Melee;
	}
	else if (PlayerWeaponComponent->GetCurrentEquippedWeapon().MatchesTagExact(EnhoneyTags.Weapon_FlyKnife))
	{
		OutCommonAttackAbilityTag = EnhoneyTags.Ability_Offensive_Inherent_CommonAttack_FlyKnife;
	}
	else if (PlayerWeaponComponent->GetCurrentEquippedWeapon().MatchesTagExact(EnhoneyTags.Weapon_Crossbow))
	{
		OutCommonAttackAbilityTag = EnhoneyTags.Ability_Offensive_Inherent_CommonAttack_ShootArrow;
	}
	else
	{
		UE_LOG(LogEnhoney, Error, TEXT("WeaponSystem Error : [%s] is not valid!"),
			*PlayerWeaponComponent->GetCurrentEquippedWeapon().ToString());
	}

	return OutCommonAttackAbilityTag;
}

UInventoryComponent* APlayerCharacterBase::GetPlayerInventoryComponent_Implementation()
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		return EnhoneyPlayerState->GetInventoryComponent();
	}
	return nullptr;
}

FOnInteractWithNPCSignature* APlayerCharacterBase::GetOnInteractWithNPCDelegate()
{
	if (AEnhoneyPlayerController* EnhoneyPlayerController = Cast<AEnhoneyPlayerController>(GetController()))
	{
		return EnhoneyPlayerController->GetOnInteractWithNPCDelegate();
	}

	return nullptr;
}

void APlayerCharacterBase::SetCanInteractWithNPC_Implementation(bool bInCanInteract)
{
	if (AEnhoneyPlayerController* EnhoneyPlayerController = Cast<AEnhoneyPlayerController>(GetController()))
	{
		return EnhoneyPlayerController->SetCanInteractWithNPC(bInCanInteract);
	}
}

UEnhoneyWidgetControllerBase* APlayerCharacterBase::GetStoreWidgetController_Implementation()
{
	if (AEnhoneyPlayerController* EnhoneyPlayerController = Cast<AEnhoneyPlayerController>(GetController()))
	{
		if (AEnhoneyPlayingHUD* EnhoneyHUD = Cast<AEnhoneyPlayingHUD>(EnhoneyPlayerController->GetHUD()))
		{
			return EnhoneyHUD->GetStoreWidgetController(FWidgetControllerParam(EnhoneyPlayerController, 
				Cast<AEnhoneyPlayerState>(GetPlayerState()), PlayerAbilitySystemComponent, GetPlayerAttributeSet()));
		}
	}
	return nullptr;
}

void APlayerCharacterBase::SwitchPlayerInputMode_Implementation(bool bShowMouseCursor, bool bIsUIOnly)
{
	if (AEnhoneyPlayerController* EnhoneyPlayerController = Cast<AEnhoneyPlayerController>(GetController()))
	{
		EnhoneyPlayerController->SetShowMouseCursor(bShowMouseCursor);

		if (bIsUIOnly)
		{
			FInputModeUIOnly InputUIOnly;
			EnhoneyPlayerController->SetInputMode(InputUIOnly);
		}
		else
		{
			FInputModeGameOnly InputGameOnly;
			EnhoneyPlayerController->SetInputMode(InputGameOnly);
		}
	}
	
}

UPlayerTaskComponent* APlayerCharacterBase::GetPlayerTaskComponent_Implementation()
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		return EnhoneyPlayerState->GetPlayerTaskComponent();
	}
	return nullptr;
}

UTaskWidgetController* APlayerCharacterBase::GetPlayerTaskWidgetController_Implementation()
{
	if (AEnhoneyPlayerController* EnhoneyPlayerController = Cast<AEnhoneyPlayerController>(GetController()))
	{
		if (AEnhoneyPlayingHUD* EnhoneyHUD = Cast<AEnhoneyPlayingHUD>(EnhoneyPlayerController->GetHUD()))
		{
			if (UTaskWidgetController* TaskWidgetController = Cast<UTaskWidgetController>(
				EnhoneyHUD->GetPlayerTaskWidgetController(FWidgetControllerParam(EnhoneyPlayerController,
				Cast<AEnhoneyPlayerState>(GetPlayerState()), 
					PlayerAbilitySystemComponent,
					GetPlayerAttributeSet()))))
			{
				return TaskWidgetController;
			}
		}
	}
	return nullptr;
}

UPlayerAttributeSet* APlayerCharacterBase::GetPlayerAttributeSet() const
{
	return PlayerAttributeSet;
}

void APlayerCharacterBase::ServerSwitchEquippedWeapon_Implementation ()
{
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();

	if (PlayerWeaponComponent->GetCurrentEquippedWeapon().MatchesTagExact(EnhoneyTags.Weapon_CurvedKnife))
	{
		IPlayerInterface::Execute_SwitchEquippedWeapon(this, EnhoneyTags.Weapon_FlyKnife);
	}
	else if (PlayerWeaponComponent->GetCurrentEquippedWeapon().MatchesTagExact(EnhoneyTags.Weapon_FlyKnife))
	{
		IPlayerInterface::Execute_SwitchEquippedWeapon(this, EnhoneyTags.Weapon_Crossbow);
	}
	else if (PlayerWeaponComponent->GetCurrentEquippedWeapon().MatchesTagExact(EnhoneyTags.Weapon_Crossbow))
	{
		IPlayerInterface::Execute_SwitchEquippedWeapon(this, EnhoneyTags.Weapon_CurvedKnife);
	}
	else
	{
		UE_LOG(LogEnhoney, Error, TEXT("WeaponSystem Error : [%s] is not valid!"), 
			*PlayerWeaponComponent->GetCurrentEquippedWeapon().ToString());
	}
}

void APlayerCharacterBase::ServerUpgradeEquippedWeapon_Implementation()
{
	PlayerWeaponComponent->UpgradeCurrentWeaponLevel();
}

void APlayerCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacterBase, bTargetingEnemy);
	DOREPLIFETIME(APlayerCharacterBase, bTargetingEnemy);

}

void APlayerCharacterBase::InitAbilityActorInfo()
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		EnhoneyPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(EnhoneyPlayerState, this);

		PlayerAbilitySystemComponent = CastChecked<UEnhoneyAbilitySystemComponent>(EnhoneyPlayerState->GetAbilitySystemComponent());
	}

}

void APlayerCharacterBase::InitPlayerAttributes()
{
	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetPlayerState()))
	{
		PlayerAttributeSet = EnhoneyPlayerState->GetPlayerAttributeSet();

		// 初始化属性
		UEnhoneyAbilitySystemLibrary::ApplyAttributeEffectToSelf(this, EffectForPrimaryAttribute, PlayerAbilitySystemComponent);	// 主要属性
		UEnhoneyAbilitySystemLibrary::ApplyAttributeEffectToSelf(this, EffectForSecondaryAttribute, PlayerAbilitySystemComponent);	// 次要属性
		UEnhoneyAbilitySystemLibrary::ApplyAttributeEffectToSelf(this, EffectForVitalAttribute, PlayerAbilitySystemComponent);	// 重要属性
	}

}

void APlayerCharacterBase::LoadProgress()
{
	// TODO：读档

	// 默认设置属性
	InitPlayerAttributes();

	// 装备默认武器，无论读档还是初始化，都装备这把武器，存档时只会存储武器的等级，读档时将等级设置回去
	IPlayerInterface::Execute_SwitchEquippedWeapon(this, FEnhoneyGameplayTags::Get().Weapon_CurvedKnife);
}

void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// 绑定重叠代理
	EnemyLockingSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacterBase::OnEnemyLockingSphereBeginOverlap);
}

void APlayerCharacterBase::OnEnemyLockingSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bTargetingEnemy)
	{
		if (!IsValid(PlayerTargetEnemy) && OtherActor->IsA<AEnemyCharacterBase>())
		{
			FGameplayEventData Payload;
			Payload.Instigator = OtherActor;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, FEnhoneyGameplayTags::Get().AbilityEventTag_EnemyLocking_EnemyEntryRange, Payload);

			// 取消EnemyLocking Sphere的碰撞
			EnemyLockingSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}
