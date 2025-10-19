// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_DefenseCapacity.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UMMC_DefenseCapacity : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_DefenseCapacity();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	// 捕获的属性
	FGameplayEffectAttributeCaptureDefinition DefenseDef;
	
};
