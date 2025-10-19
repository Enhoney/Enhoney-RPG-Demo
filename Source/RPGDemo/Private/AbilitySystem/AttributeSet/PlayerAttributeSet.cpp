// Copyright Enhoney.


#include "PlayerAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"

#include "EnhoneyGameplayTags.h"

#include "EnhoneyAbilityType.h"
#include "CombatInterface.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
	const FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();

	/** 要显示到AttributeMenu中的属性*/
	// PrimaryAttribute
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Primary_Strength, GetStrengthAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Primary_Constitution, GetConstitutionAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Primary_Intelligence, GetIntelligenceAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Primary_Defense, GetDefenseAttribute);

	// SecondaryAttribute
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_AttackPower, GetAttackPowerAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_DefenseCapacity, GetDefenseCapacityAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_ElementProficiency, GetElementProficiencyAttribute);

	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_MaxHealth, GetMaxHealthAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_MaxMana, GetMaxManaAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_MaxArcane, GetMaxArcaneAttribute);

	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_CriticalChance, GetCriticalChanceAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_CriticalRatio, GetCriticalRatioAttribute);

	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_PhysicalDamageBonus, GetPhysicalDamageBonusAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_FlameDamageBonus, GetFlameDamageBonusAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_FrostDamageBonus, GetFrostDamageBonusAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_PhysicalDamageResistance, GetPhysicalDamageResistanceAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_FlameDamageResistance, GetFlameDamageResistanceAttribute);
	MapGameplayTagToAttribute.Add(EnhoneyTags.Attribute_Secondary_FrostDamageResistance, GetFrostDamageResistanceAttribute);
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Constitution, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Defense, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, BasicAttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxArcane, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Arcane, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, ExtraAttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, ExtraPercentageAttack, COND_None, REPNOTIFY_Always);
}

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	/** 属性约束*/
	// 魔法值
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}

	// 最大魔法值最小不能小于1
	if (Attribute == GetMaxManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.f, INT32_MAX);
	}

	// 奥义值
	if (Attribute == GetArcaneAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxArcane());
	}

	// 最大奥义值最小不能小于1
	if (Attribute == GetMaxArcaneAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.f, INT32_MAX);
	}
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties EffectProps;
	GetEffectProps(EffectProps, Data);

	/** 属性约束*/
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Clamp(GetMaxHealth(), 1.f, INT32_MAX));
	}

	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(), 0, GetMaxMana()));
	}

	if (Data.EvaluatedData.Attribute == GetMaxManaAttribute())
	{
		SetMaxMana(FMath::Clamp(GetMaxMana(), 1.f, INT32_MAX));
	}

	if (Data.EvaluatedData.Attribute == GetArcaneAttribute())
	{
		SetArcane(FMath::Clamp(GetArcane(), 0, GetMaxArcane()));
	}

	if (Data.EvaluatedData.Attribute == GetMaxArcaneAttribute())
	{
		SetMaxArcane(FMath::Clamp(GetMaxArcane(), 1.f, INT32_MAX));
	}

	// 接收到的伤害--最终伤害
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(EffectProps);
	}

	// 吸收的经验值
	if (Data.EvaluatedData.Attribute == GetIncomingExpAttribute())
	{
		HandleIncomingExp(EffectProps);
	}
}

void UPlayerAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UPlayerAttributeSet::HandleIncomingDamage(const FEffectProperties& EffectProps)
{
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();

	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);

	if (LocalIncomingDamage > 0.f)
	{
		const float NewHealth = FMath::Clamp(GetHealth() - LocalIncomingDamage, 0.f, GetMaxHealth());
		SetHealth(NewHealth);
		// 是否为致命伤害
		const bool bDamageFatal = FMath::IsNearlyZero(GetHealth());
		if (bDamageFatal)
		{
			// 死亡
			if (EffectProps.TargetAvatarActor->Implements<UCombatInterface>())
			{
				ICombatInterface::Execute_CharacterDie(EffectProps.TargetAvatarActor);
			}
		}
		else
		{
			const FEnhoneyGameplayEffectContext* EnhoneyEffectContext =
				static_cast<const FEnhoneyGameplayEffectContext*>(EffectProps.GEContectHandle.Get());

			// 击退
			if (EnhoneyEffectContext->GetKnockbackVector() != FVector::ZeroVector)
			{
				EffectProps.TargetCharacter->LaunchCharacter(EnhoneyEffectContext->GetKnockbackVector(), true, true);
			}
			else
			{
				// 受击--暴击时触发
				// 激活受击能力
				if (EffectProps.TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(EnhoneyTags.Ability_Offensive_Inherent_HitReaction)))
				{
					// 播放受击动画
					ICombatInterface::Execute_PlayHitReactionAnim(EffectProps.TargetAvatarActor, *(EnhoneyEffectContext->GetHitResult()));
				}

			}
		}
	}
}

void UPlayerAttributeSet::HandleIncomingExp(const FEffectProperties& EffectProps)
{
}

void UPlayerAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Strength, OldStrength);
}

void UPlayerAttributeSet::OnRep_Constitution(const FGameplayAttributeData& OldConstitution)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Constitution, OldConstitution);
}

void UPlayerAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Intelligence, OldIntelligence);
}

void UPlayerAttributeSet::OnRep_Defense(const FGameplayAttributeData& OldDefense)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Defense, OldDefense);
}

void UPlayerAttributeSet::OnRep_BasicAttackPower(const FGameplayAttributeData& OldBasicAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, BasicAttackPower, OldBasicAttackPower);
}

void UPlayerAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxMana, OldMaxMana);
}

void UPlayerAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Mana, OldMana);
}

void UPlayerAttributeSet::OnRep_MaxArcane(const FGameplayAttributeData& OldMaxArcane)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxArcane, OldMaxArcane);
}

void UPlayerAttributeSet::OnRep_Arcane(const FGameplayAttributeData& OldArcane)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Arcane, OldArcane);
}

void UPlayerAttributeSet::OnRep_ExtraAttackPower(const FGameplayAttributeData& OldExtraAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, ExtraAttackPower, OldExtraAttackPower);
}

void UPlayerAttributeSet::OnRep_ExtraPercentageAttack(const FGameplayAttributeData& OldExtraPercentageAttack)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, ExtraPercentageAttack, OldExtraPercentageAttack);
}
