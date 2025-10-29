// Copyright Enhoney.


#include "PlayerAbility/Offensive/NoDamageAbility_Variable.h"
#include "AbilitySystemComponent.h"

void UNoDamageAbility_Variable::ApplyBuffEffectToSelf()
{
	if (BuffEffectClass)
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo())
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);
			FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(BuffEffectClass, GetAbilityLevel(), EffectContext);
			if (SpecHandle.IsValid())
			{
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
		
	}
}

float UNoDamageAbility_Variable::GetBuffEffectValue(int32 InAbilityLevel) const
{
	// 从配置的FScalableFloat中获取数值
	if (BuffEffect.IsValid())
	{
		return BuffEffect.GetValueAtLevel(FMath::Clamp(InAbilityLevel, 1, MaxAbilityLevel));
	}
	return 0.0f;
}

float UNoDamageAbility_Variable::GetAbilityCooldownTime(int32 InAbilityLevel) const
{
	// 从配置的FScalableFloat中获取数值
	if (AbilityCooldownTime.IsValid())
	{
		return AbilityCooldownTime.GetValueAtLevel(FMath::Clamp(InAbilityLevel, 1, MaxAbilityLevel));
	}
	return 0.0f;
}

float UNoDamageAbility_Variable::GetAbilityManaOrManeCost(int32 InAbilityLevel) const
{
	// 从配置的FScalableFloat中获取数值
	if (AbilityManaOrManeCost.IsValid())
	{
		return AbilityManaOrManeCost.GetValueAtLevel(FMath::Clamp(InAbilityLevel, 1, MaxAbilityLevel));
	}
	return 0.0f;
}
