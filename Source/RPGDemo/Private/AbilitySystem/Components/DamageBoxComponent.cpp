// Copyright Enhoney.


#include "DamageBoxComponent.h"

#include "PlayerInterface.h"
#include "EnemyInterface.h"
#include "CombatInterface.h"

#include "EnhoneyAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhoneyAbilitySystemLibrary.h"

#include "EnhoneyGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "Actor/EnhoneyMineralBase.h"

#include "RPGDemo.h"

UDamageBoxComponent::UDamageBoxComponent()
{
	SetCollisionObjectType(ECC_DamageObject);
	SetCollisionResponseToAllChannels(ECR_Ignore);
	SetCollisionResponseToChannel(ECC_CombatMesh, ECR_Overlap);
	SetGenerateOverlapEvents(true);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UDamageBoxComponent::SetCombatBoxCollisionEnable(bool bIsEnable)
{
	SetCollisionEnabled(bIsEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

void UDamageBoxComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UDamageBoxComponent, DamageEffectParams);
}

void UDamageBoxComponent::BeginPlay()
{
	Super::BeginPlay();

	OnComponentBeginOverlap.AddDynamic(this, &UDamageBoxComponent::OnDamageBoxBeginOverlap);
}

void UDamageBoxComponent::OnDamageBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 打到自己直接忽略
	if (OtherActor == DamageEffectParams.WorldContextObject)
	{
		return;
	}

	//如果是打到矿物了
	if (OtherActor->IsA(AEnhoneyMineralBase::StaticClass()))
	{
		// 在命中之后，立即取消碰撞
		SetCombatBoxCollisionEnable(false);

	}

	// 如果是打到角色了
	if (!OtherActor->Implements<UCombatInterface>())
	{
		return;
	}

	// 是否可以造成近战伤害
	bool bCanCauseMeleeDamage = bIsMeleeAttack && ((GetOwner()->Implements<UPlayerInterface>() && OtherActor->Implements<UEnemyInterface>())
		|| (GetOwner()->Implements<UEnemyInterface>() && OtherActor->Implements<UPlayerInterface>()));
	// 是否可以造成远程伤害
	bool bCanCauseRemoteDamge = !bIsMeleeAttack && IsValid(DamageEffectParams.WorldContextObject) &&
		((DamageEffectParams.WorldContextObject->Implements<UPlayerInterface>() && OtherActor->Implements<UEnemyInterface>())
			|| (DamageEffectParams.WorldContextObject->Implements<UEnemyInterface>() && OtherActor->Implements<UPlayerInterface>()));

	// 只对存活的目标造成伤害
	if (ICombatInterface::Execute_IsCharacterAlive(OtherActor))
	{
		// 恢复Arcane
		if (DamageEffectParams.AbilityTags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("Ability.Offensive.Inherent.CommonAttack"))))
		{
			UEnhoneyAbilitySystemLibrary::ApplyAttributeEffectToSelf(DamageEffectParams.WorldContextObject, DamageEffectParams.RecoveryArcaneEffectClass, DamageEffectParams.SourceASC);
		}

		if (bCanCauseMeleeDamage)
		{
			// 发送GameplayEvent，让GA直接对敌人施加伤害
			FGameplayEventData CombatPayload;
			CombatPayload.Instigator = OtherActor;	// 将这个敌人发送出去，传递给Ability
			FGameplayEffectContextHandle ImpactContextHandle = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())->MakeEffectContext();
			ImpactContextHandle.AddHitResult(SweepResult);
			CombatPayload.ContextHandle = ImpactContextHandle;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), FEnhoneyGameplayTags::Get().AbilityEventTag_CommonAttack_Melee, CombatPayload);

			// 在命中之后，立即取消碰撞
			SetCombatBoxCollisionEnable(false);
		}

		if (bCanCauseRemoteDamge)
		{
			// 触发击退
			if (FMath::RandRange(1, 1000) < DamageEffectParams.KnockbackChance * 1000)
			{
				FVector SourceActorLocation = GetOwner()->GetActorLocation();
				FVector TargetActorLocation = OtherActor->GetActorLocation();
				FRotator KnockbackRotation = (TargetActorLocation - SourceActorLocation).Rotation();

				DamageEffectParams.KnockbackVector = KnockbackRotation.Vector().GetSafeNormal() * DamageEffectParams.KnockbackForceMagnitude;
			}
			DamageEffectParams.TargetASC = Cast<UEnhoneyAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor));
			DamageEffectParams.ImpactResult = SweepResult;
			// 造成伤害
			UEnhoneyAbilitySystemLibrary::CauseDamageByParams(DamageEffectParams);

			// 在命中之后，立即取消碰撞
			SetCombatBoxCollisionEnable(false);

			// 销毁
			GetOwner()->Destroy();
		}
	}
	
}
