// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "OffensiveAbilityBase.h"
#include "GameplayTagContainer.h"
#include "OffensiveAbility_Damage.generated.h"

struct FScalableFloat;
struct FEnhoneyDamageEffectParams;

class UGameplayEffect;

/**
 * 
 */
UCLASS()
class RPGDEMO_API UOffensiveAbility_Damage : public UOffensiveAbilityBase
{
	GENERATED_BODY()

public:
	// 生成DamageEffectParams
	UFUNCTION(BlueprintCallable)
	FEnhoneyDamageEffectParams MakeDamageEffectParams(const AActor* InTargetActor = nullptr, bool bIsMeleeAbility = true);

protected:
	// 获取技能伤害倍率
	float GetSkillMultiplierByLevel() const;

protected:
	// 伤害使用的GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DamageAbilityParams")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 技能伤害类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DamageAbilityParams")
	FGameplayTag AbilityDamageType = FGameplayTag();

	// 技能倍率
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DamageAbilityParams")
	FScalableFloat SkillMultipliers;

	// 击退几率
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DamageAbilityParams")
	float KnockbackChance = 0.f;
	// 击退力度
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DamageAbilityParams")
	float KnockbackForceMagnitude = 1000.f;
	// 击退俯仰角
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "DamageAbilityParams")
	float KnockbackPitch = 30.f;
};
