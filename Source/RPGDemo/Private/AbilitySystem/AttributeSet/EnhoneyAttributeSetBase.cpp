// Copyright Enhoney.


#include "EnhoneyAttributeSetBase.h"
#include "Net/UnrealNetwork.h"

#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"

#include "AbilitySystemBlueprintLibrary.h"

void UEnhoneyAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, MaxHealth,					COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, AttackPower,				COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, DefenseCapacity,			COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, ElementProficiency,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, ArmorPenetration,			COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, PhysicalDamageBonus,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, FlameDamageBonus,			COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, FrostDamageBonus,			COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, CriticalChance,			COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, CriticalRatio,				COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, PhysicalDamageResistance,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, FlameDamageResistance,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, FrostDamageResistance,		COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyAttributeSetBase, Health,					COND_None, REPNOTIFY_Always);
}

void UEnhoneyAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	/** 属性约束*/
	// 生命值
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}

	// 最大生命值最小不能小于1
	if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.f, INT32_MAX);
	}
}

void UEnhoneyAttributeSetBase::GetEffectProps(FEffectProperties& OutEffectProps, const FGameplayEffectModCallbackData& InData)
{
	OutEffectProps.GEContectHandle = InData.EffectSpec.GetContext();
	OutEffectProps.SourceASC = OutEffectProps.GEContectHandle.GetInstigatorAbilitySystemComponent();

	// Source
	if (IsValid(OutEffectProps.SourceASC) && OutEffectProps.SourceASC->AbilityActorInfo.IsValid() && OutEffectProps.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		OutEffectProps.SourceAvatarActor = OutEffectProps.SourceASC->AbilityActorInfo->AvatarActor.Get();
		OutEffectProps.SourceController = OutEffectProps.SourceASC->AbilityActorInfo->PlayerController.Get();

		if (IsValid(OutEffectProps.SourceAvatarActor) && !IsValid(OutEffectProps.SourceController))
		{
			if (const APawn* SourcePawn = Cast<APawn>(OutEffectProps.SourceAvatarActor))
			{
				OutEffectProps.SourceController = SourcePawn->GetController();
			}
		}

		if (IsValid(OutEffectProps.SourceController))
		{
			OutEffectProps.SourceCharacter = Cast<ACharacter>(OutEffectProps.SourceController->GetPawn());
		}
	}

	//Target
	if (InData.Target.AbilityActorInfo.IsValid() && InData.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		OutEffectProps.TargetASC = InData.Target.AbilityActorInfo->AbilitySystemComponent.Get();
		OutEffectProps.TargetAvatarActor = InData.Target.AbilityActorInfo->AvatarActor.Get();
		OutEffectProps.TargetController = InData.Target.AbilityActorInfo->PlayerController.Get();
		OutEffectProps.TargetCharacter = Cast<ACharacter>(OutEffectProps.TargetAvatarActor);

	}
}

void UEnhoneyAttributeSetBase::HandleIncomingDamage(const FEffectProperties& EffectProps)
{
}

void UEnhoneyAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, MaxHealth, OldMaxHealth);
}

void UEnhoneyAttributeSetBase::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, AttackPower, OldAttackPower);
}

void UEnhoneyAttributeSetBase::OnRep_DefenseCapacity(const FGameplayAttributeData& OldDefenseCapacity)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, DefenseCapacity, OldDefenseCapacity);
}

void UEnhoneyAttributeSetBase::OnRep_ElementProficiency(const FGameplayAttributeData& OldElementProficiency)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, ElementProficiency, OldElementProficiency);
}

void UEnhoneyAttributeSetBase::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, ArmorPenetration, OldArmorPenetration);
}

void UEnhoneyAttributeSetBase::OnRep_PhysicalDamageBonus(const FGameplayAttributeData& OldPhysicalDamageBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, PhysicalDamageBonus, OldPhysicalDamageBonus);
}

void UEnhoneyAttributeSetBase::OnRep_FlameDamageBonus(const FGameplayAttributeData& OldFlameDamageBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, FlameDamageBonus, OldFlameDamageBonus);
}

void UEnhoneyAttributeSetBase::OnRep_FrostDamageBonus(const FGameplayAttributeData& OldFrostDamageBonus)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, FrostDamageBonus, OldFrostDamageBonus);
}

void UEnhoneyAttributeSetBase::OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, CriticalChance, OldCriticalChance);
}

void UEnhoneyAttributeSetBase::OnRep_CriticalRatio(const FGameplayAttributeData& OldCriticalRatio)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, CriticalRatio, OldCriticalRatio);
}

void UEnhoneyAttributeSetBase::OnRep_PhysicalDamageResistance(const FGameplayAttributeData& OldPhysicalDamageResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, PhysicalDamageResistance, OldPhysicalDamageResistance);
}

void UEnhoneyAttributeSetBase::OnRep_FlameDamageResistance(const FGameplayAttributeData& OldFlameDamageResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, FlameDamageResistance, OldFlameDamageResistance);
}

void UEnhoneyAttributeSetBase::OnRep_FrostDamageResistance(const FGameplayAttributeData& OldFrostDamageResistance)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, FrostDamageResistance, OldFrostDamageResistance);
}

void UEnhoneyAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UEnhoneyAttributeSetBase, Health, OldHealth);
}