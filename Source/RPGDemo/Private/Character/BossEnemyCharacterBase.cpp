// Copyright Enhoney.


#include "BossEnemyCharacterBase.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

#include "PlayerInterface.h"
#include "RPGDemo.h"

#include "EnhoneyPlayingHUD.h"
#include "PlayingWidgetController.h"
#include "CommonAlgorithmLibrary.h"
#include "EnemyAttributeSet.h"
#include "EnhoneyAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


ABossEnemyCharacterBase::ABossEnemyCharacterBase()
{
	// 入侵范围
	InvaderSphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InvaderSphereCollision"));
	InvaderSphereCollision->SetupAttachment(GetRootComponent());
	InvaderSphereCollision->InitSphereRadius(800.f);
	InvaderSphereCollision->SetGenerateOverlapEvents(true);
	InvaderSphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InvaderSphereCollision->SetCollisionObjectType(ECC_MonitoringCollision);
	InvaderSphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InvaderSphereCollision->SetCollisionResponseToChannel(ECC_CombatMesh, ECR_Overlap);

	// BOSS默认是休憩状态
	EnemyState = EEnemyState::EES_Rest;

	// 设置常态移动速度
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed_Commom;
}

void ABossEnemyCharacterBase::MulticastSetBossHealthBarVisibility_Implementation(ACharacter* InPlayerCharacter, bool bIsVisiblie)
{
	// 这是在各个客户端
	UPlayingWidgetController* PlayingWidgetController = nullptr;
	if (UCommonAlgorithmLibrary::GetPlayingWidgetControllerByCharacter(InPlayerCharacter, PlayingWidgetController))
	{
		if (IsValid(PlayingWidgetController))
		{
			PlayingWidgetController->OnBossHealthChangedDelegate.Broadcast(bIsVisiblie, BossName, EnemyAttributeSet->GetHealth(), EnemyAttributeSet->GetMaxHealth());
		}
	}
}

void ABossEnemyCharacterBase::MulticastUpdateHealthBar_Implementation(float NewCurrentHealth, float NewCurrentMaxHealth)
{
	if (!Invaders.IsEmpty())
	{
		// 这里还是服务器，执行RPC
		MulticastUpdateBossHealthBar(BossName, NewCurrentHealth, NewCurrentMaxHealth);
	}
}

void ABossEnemyCharacterBase::CharacterDie_Implementation()
{
	Super::CharacterDie_Implementation();
	// 死亡时，隐藏所有入侵者的BOSS血条
	InvaderSphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (!Invaders.IsEmpty())
	{
		for (ACharacter* InvadeCharacter : Invaders)
		{
			MulticastSetBossHealthBarVisibility(InvadeCharacter, false);
		}
	}
}

EBossPhase ABossEnemyCharacterBase::GetBossPhase_Implementation() const
{
	return BossPhase;
}

void ABossEnemyCharacterBase::SwitchToPhaseTwoForAbility_Implementation()
{
	// 尝试激活切换阶段的技能
	if (IsValid(EnemyAbilitySystemComponent))
	{
		BossPhase = EBossPhase::EBP_Phase2;
		EnemyAbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FEnhoneyGameplayTags::Get().Ability_Offensive_Inherent_Common_SwitchPhase));
	}
}

void ABossEnemyCharacterBase::SwitchToPhaseTwoForBuff_Implementation()
{
	if (IsValid(EnemyAbilitySystemComponent))
	{
		// 移除一阶段Buff
		EnemyAbilitySystemComponent->RemoveActiveGameplayEffect(PhaseBuffHandle);
		// 施加二阶段Buff
		FGameplayEffectContextHandle BuffEffectHandle = EnemyAbilitySystemComponent->MakeEffectContext();
		BuffEffectHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle BuffEffectSpecHandle = EnemyAbilitySystemComponent->MakeOutgoingSpec(BuffOnPhase2, 1, BuffEffectHandle);
		PhaseBuffHandle = EnemyAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*BuffEffectSpecHandle.Data);
	}
}

void ABossEnemyCharacterBase::SwitchToCombatState_Implementation()
{
	if (EnemyState == EEnemyState::EES_Rest)
	{
		// 如果原来是休息状态，就移除恢复生命的Buff
		if (IsValid(EnemyAbilitySystemComponent) && QuickRecoveryHealthBuffHandle.IsValid())
		{
			EnemyAbilitySystemComponent->RemoveActiveGameplayEffect(QuickRecoveryHealthBuffHandle);
			QuickRecoveryHealthBuffHandle = FActiveGameplayEffectHandle();
		}
	}

	EnemyState = EEnemyState::EES_Combat;
}

void ABossEnemyCharacterBase::SwitchToFleeState_Implementation()
{
	// 加速
	GetCharacterMovement()->MaxWalkSpeed = MaxSpeed_Flee;

	EnemyState = EEnemyState::EES_Flee;
}

void ABossEnemyCharacterBase::SwitchToRestState_Implementation()
{
	if (EnemyState == EEnemyState::EES_Flee)
	{
		// 如果原来是逃跑状态，就降低移动速度
		GetCharacterMovement()->MaxWalkSpeed = MaxSpeed_Commom;

		// 移除原来的恢复生命效果
		if (IsValid(EnemyAbilitySystemComponent) && QuickRecoveryHealthBuffHandle.IsValid())
		{
			EnemyAbilitySystemComponent->RemoveActiveGameplayEffect(QuickRecoveryHealthBuffHandle);
			QuickRecoveryHealthBuffHandle = FActiveGameplayEffectHandle();
		}

		// 施加快速恢复生命的Buff--每秒恢复500点
		FGameplayEffectContextHandle RecoveryHealthBuffEffectHandle = EnemyAbilitySystemComponent->MakeEffectContext();
		RecoveryHealthBuffEffectHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle RecoveryHealthBuffEffectSpecHandle = EnemyAbilitySystemComponent->MakeOutgoingSpec(QuickRecoveryHealthBuff, 1, RecoveryHealthBuffEffectHandle);
		QuickRecoveryHealthBuffHandle = EnemyAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*RecoveryHealthBuffEffectSpecHandle.Data);

		// 切换到休息状态
		EnemyState = EEnemyState::EES_Rest;
	}
}

FVector ABossEnemyCharacterBase::GetBirthPointLocation_Implementation() const
{
	return BirthPoint.IsValid() ? BirthPoint->GetActorLocation() : GetActorLocation();
}

void ABossEnemyCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABossEnemyCharacterBase, Invaders);
	DOREPLIFETIME(ABossEnemyCharacterBase, BossPhase);
}

void ABossEnemyCharacterBase::OnPlayerCharacterStartInvade(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		if (OtherActor->Implements<UPlayerInterface>())
		{
			ACharacter* InvaderEnter = Cast<ACharacter>(OtherActor);

			Invaders.Add(InvaderEnter);

			// 显示BOS血条
			MulticastSetBossHealthBarVisibility(InvaderEnter, true);

			// 如果这个时候处于待机状态，就切换到战斗状态，并将这个入侵者设为战斗目标
			if (EnemyState == EEnemyState::EES_Rest)
			{
				IBossInterface::Execute_SwitchToCombatState(this);
				CombatTarget = InvaderEnter;
			}
		}
	}
}

void ABossEnemyCharacterBase::OnPlayerCharacterQuitInvade(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		if (OtherActor->Implements<UPlayerInterface>())
		{
			ACharacter* InvaderQuit = Cast<ACharacter>(OtherActor);

			// 隐藏BOSS血条
			MulticastSetBossHealthBarVisibility(InvaderQuit, false);

			Invaders.Remove(InvaderQuit);

			// 如果跑出去的是当前攻击目标，就找下一个目标来攻击
			if (EnemyState == EEnemyState::EES_Combat && InvaderQuit == CombatTarget)
			{
				FindNextInvaderAsTarget();
			}
		}
	}
}

void ABossEnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	InvaderSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemyCharacterBase::OnPlayerCharacterStartInvade);
	InvaderSphereCollision->OnComponentEndOverlap.AddDynamic(this, &ABossEnemyCharacterBase::OnPlayerCharacterQuitInvade);

	// 施加一阶段的Buff
	if (EnemyAbilitySystemComponent)
	{
		FGameplayEffectContextHandle BuffEffectHandle = EnemyAbilitySystemComponent->MakeEffectContext();
		BuffEffectHandle.AddSourceObject(this);
		FGameplayEffectSpecHandle BuffEffectSpecHandle = EnemyAbilitySystemComponent->MakeOutgoingSpec(BuffOnPhase1, 1, BuffEffectHandle);
		PhaseBuffHandle = EnemyAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*BuffEffectSpecHandle.Data);
	}

}

void ABossEnemyCharacterBase::FindNextInvaderAsTarget()
{
	// 如果没有入侵者了，就进入逃跑状态
	if (Invaders.IsEmpty())
	{
		CombatTarget = nullptr;
		IBossInterface::Execute_SwitchToFleeState(this);
	}
	else
	{
		// 还有入侵者，就找到下一个作为攻击目标
		CombatTarget = Invaders[0];
	}
}

void ABossEnemyCharacterBase::MulticastUpdateBossHealthBar_Implementation(FName InBossName, float NewCurrentHealth, float NewCurrentMaxHealth)
{
	// 这里就是客户端了
	UPlayingWidgetController* PlayingWidgetControler = nullptr;

	if (!Invaders.IsEmpty())
	{
		for (TWeakObjectPtr<ACharacter> InvadeCharacter : Invaders)
		{
			// 能够拿到Controller，就表明这个角色就是在范围内的入侵者
			if (UCommonAlgorithmLibrary::GetPlayingWidgetControllerByCharacter(InvadeCharacter.Get(), PlayingWidgetControler))
			{
				if (IsValid(PlayingWidgetControler))
				{
					PlayingWidgetControler->OnBossHealthChangedDelegate.Broadcast(true, InBossName, NewCurrentHealth, NewCurrentMaxHealth);
				}
			}
		}
	}
	
}
