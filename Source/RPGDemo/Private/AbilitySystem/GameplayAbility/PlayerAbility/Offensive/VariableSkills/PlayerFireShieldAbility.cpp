// Copyright Enhoney.


#include "PlayerAbility/Offensive/VariableSkills/PlayerFireShieldAbility.h"

void UPlayerFireShieldAbility::GetAbilityDescOnLevel(UAbilitySystemComponent* InASC, int32 InAbilityLevel, FString& OutDescription) const
{
	if (InAbilityLevel >= MaxAbilityLevel)
	{
		return;
	}
	// 提供的物理伤害抗性值
	float PhysicalBuffValue = 0.20;
	// 增加的火焰伤害抗性值
	float BuffValue = GetBuffEffectValue(InAbilityLevel);
	// 技能冷却时间
	float CooldownTime = GetAbilityCooldownTime(InAbilityLevel);
	// 技能消耗魔法值
	float ManaCost = GetAbilityManaOrManeCost(InAbilityLevel);

	OutDescription = FString::Printf(TEXT(
		"<AbilityDesc>等级：Lv </><Golden>%d</>\t\
		<AbilityDesc>给玩家提供一个火焰护盾，效果如下：</>\
		<AbilityDesc>物理伤害抗性：</><Write>%.0f%%</>\
		<AbilityDesc>火焰伤害抗性：</><Orange>%.0f%%</>\
		<AbilityDesc>冷却时间：</><Green>%.1f秒</>\
		<AbilityDesc>魔法消耗：</><Pink>%.0f</>"),
		InAbilityLevel,
		PhysicalBuffValue * 100.0f,
		BuffValue * 100.0f,
		CooldownTime,
		ManaCost);
}
