// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_AttackPower.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UMMC_AttackPower : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_AttackPower();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	// 捕获的属性
	FGameplayEffectAttributeCaptureDefinition BasicAttackPowerDef;
	FGameplayEffectAttributeCaptureDefinition ExtraAttackPowerDef;
	FGameplayEffectAttributeCaptureDefinition ExtraPercentageAttackDef;
	
};
