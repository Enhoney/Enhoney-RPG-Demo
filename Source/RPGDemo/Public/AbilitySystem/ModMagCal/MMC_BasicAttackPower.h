// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_BasicAttackPower.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UMMC_BasicAttackPower : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_BasicAttackPower();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	// 捕获的属性
	FGameplayEffectAttributeCaptureDefinition StrengthDef;
	
};
