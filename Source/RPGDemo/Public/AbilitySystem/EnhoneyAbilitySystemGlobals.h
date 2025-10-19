// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "EnhoneyAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnhoneyAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

public:
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	
};
