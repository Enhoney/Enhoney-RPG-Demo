// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "AbilityBase/OffensiveAbility_Damage.h"
#include "BossDamageAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UBossDamageAbilityBase : public UOffensiveAbility_Damage
{
	GENERATED_BODY()
	

public:
	// 根据Boss阶段生成DamageEffectParams
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "MakeDamageEffectParamsByBossPhase"))
	FEnhoneyDamageEffectParams MakeDamageEffectParamsByPhase(const AActor* InTargetActor = nullptr, bool bIsMeleeAbility = true);

	// 根据阶段修改伤害类型
	virtual void ModifyDamageTypeByPhase(FEnhoneyDamageEffectParams& OutDamageEffectParams);
};
