// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/AbilityBase/OffensiveAbility_Damage.h"
#include "DamageAbility_Variable.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UDamageAbility_Variable : public UOffensiveAbility_Damage
{
	GENERATED_BODY()

protected:
	// 获取伤害倍率
	float GetDamageMultiplier(int32 InAbilityLevel) const;
	// 获取技能冷却时间
	float GetAbilityCooldownTime(int32 InAbilityLevel) const;
	// 获取技能消耗魔法或奥义值
	float GetAbilityManaOrManeCost(int32 InAbilityLevel) const;
	// 获取技能伤害类型描述
	FString GetDamageAbilityTypeDescription() const;
	

private:
	// 技能冷却时间配置（查询使用）
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Damage")
	FScalableFloat AbilityCooldownTime;
	// 技能消耗魔法或奥义值配置（查询使用）
	UPROPERTY(EditDefaultsOnly, Category = "Ability|Damage")
	FScalableFloat AbilityManaOrManeCost;
	
};
