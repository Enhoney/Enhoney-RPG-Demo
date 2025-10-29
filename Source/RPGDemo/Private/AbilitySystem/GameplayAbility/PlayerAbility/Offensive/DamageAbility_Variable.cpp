// Copyright Enhoney.


#include "PlayerAbility/Offensive/DamageAbility_Variable.h"
#include "EnhoneyGameplayTags.h"

float UDamageAbility_Variable::GetDamageMultiplier(int32 InAbilityLevel) const
{
	// 从配置的FScalableFloat中获取数值
	if (SkillMultipliers.IsValid())
	{
		return SkillMultipliers.GetValueAtLevel(FMath::Clamp(InAbilityLevel, 1, MaxAbilityLevel));
	}
	return 0.0f;
}

float UDamageAbility_Variable::GetAbilityCooldownTime(int32 InAbilityLevel) const
{
	// 从配置的FScalableFloat中获取数值
	if (AbilityCooldownTime.IsValid())
	{
		return AbilityCooldownTime.GetValueAtLevel(FMath::Clamp(InAbilityLevel, 1, MaxAbilityLevel));
	}
	return 0.0f;
}

float UDamageAbility_Variable::GetAbilityManaOrManeCost(int32 InAbilityLevel) const
{
	// 从配置的FScalableFloat中获取数值
	if (AbilityManaOrManeCost.IsValid())
	{
		return AbilityManaOrManeCost.GetValueAtLevel(FMath::Clamp(InAbilityLevel, 1, MaxAbilityLevel));
	}
	return 0.0f;
}

FString UDamageAbility_Variable::GetDamageAbilityTypeDescription() const
{
	if(AbilityDamageType.IsValid())
	{
		// 根据不同的标签返回不同的描述
		FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();
		if (AbilityDamageType.MatchesTagExact(EnhoneyTags.AbilityDamageType_Flame))
		{
			return TEXT("火焰元素");
		}
		else if (AbilityDamageType.MatchesTagExact(EnhoneyTags.AbilityDamageType_Physic))
		{
			return TEXT("物理");
		}
		else if (AbilityDamageType.MatchesTagExact(EnhoneyTags.AbilityDamageType_Frost))
		{
			return TEXT("冰霜元素");
		}
	}

	return TEXT("");
}