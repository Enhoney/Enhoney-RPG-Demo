// Copyright Enhoney.


#include "PlayerAbility/Offensive/VariableSkills/StarFallenAbility.h"

void UStarFallenAbility::GetAbilityDescOnLevel(UAbilitySystemComponent* InASC, int32 InAbilityLevel, FString& OutDescription) const
{
	if (InAbilityLevel >= MaxAbilityLevel)
	{
		return;
	}

	// 技能伤害类型
	FString DamageTypeDescription = GetDamageAbilityTypeDescription();
	// 技能伤害倍率
	float DamageMultiplier = GetDamageMultiplier(InAbilityLevel);
	// 技能冷却时间
	float CooldownTime = GetAbilityCooldownTime(InAbilityLevel);
	// 技能消耗魔法值
	float ArcaneCost = GetAbilityManaOrManeCost(InAbilityLevel);

	OutDescription = FString::Printf(TEXT(
		"<AbilityDesc>等级：Lv </><Golden>%d</>\t\
		<AbilityDesc>在敌人头顶召唤星尘坠落，持续向下方敌人造成大量：</><Orange>%s</><AbilityDesc>元素伤害</>\t\
		<AbilityDesc>技能倍率：</><Orange>%.0f%%</>\
		<AbilityDesc>冷却时间：</><Green>%.1f秒</>\
		<AbilityDesc>奥义消耗：</><Pink>%.0f</>"),
		InAbilityLevel,
		*DamageTypeDescription,
		DamageMultiplier * 100.0f,
		CooldownTime,
		ArcaneCost);
}
