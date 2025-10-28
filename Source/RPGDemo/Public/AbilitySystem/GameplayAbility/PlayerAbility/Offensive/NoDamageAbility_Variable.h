// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase/OffensiveAbility_NoDamage.h"
#include "NoDamageAbility_Variable.generated.h"

/**
 * 预设一个不造成伤害的进攻型能力类，可以在此基础上扩展不同的无伤害效果
 * 通常是给自己施加一个增益效果，或者给敌人施加一个负面效果
 */
UCLASS()
class RPGDEMO_API UNoDamageAbility_Variable : public UOffensiveAbility_NoDamage
{
	GENERATED_BODY()
public:
	// 在激活能力时，应用指定的增益效果
	UFUNCTION(BlueprintCallable, Category = "Ability|NoDamage")
	void ApplyBuffEffectToSelf();
protected:
	// 获取增益效果的数值
	float GetBuffEffectValue(int32 InAbilityLevel) const;
	// 获取技能冷却时间
	float GetAbilityCooldownTime(int32 InAbilityLevel) const;
	// 获取技能消耗魔法或奥义值
	float GetAbilityManaOrManeCost(int32 InAbilityLevel) const;


private:
	// 应用的增益效果类，可以在蓝图中指定具体的效果
	UPROPERTY(EditDefaultsOnly, Category = "Ability|NoDamage")
	TSubclassOf<UGameplayEffect> BuffEffectClass;

	// 技能冷却时间配置（查询使用）
	UPROPERTY(EditDefaultsOnly, Category = "Ability|NoDamage")
	FScalableFloat AbilityCooldownTime;
	// 技能消耗魔法或奥义值配置（查询使用）
	UPROPERTY(EditDefaultsOnly, Category = "Ability|NoDamage")
	FScalableFloat AbilityManaOrManeCost;
	// 技能提供的增益效果配置（查询使用）--提供的伤害减免百分比
	UPROPERTY(EditDefaultsOnly, Category = "Ability|NoDamage")
	FScalableFloat BuffEffect;
	
};
