// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxArcane.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UMMC_MaxArcane : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

};
