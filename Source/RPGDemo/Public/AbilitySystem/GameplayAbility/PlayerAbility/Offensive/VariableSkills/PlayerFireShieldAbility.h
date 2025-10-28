// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GameplayAbility/PlayerAbility/Offensive/NoDamageAbility_Variable.h"
#include "PlayerFireShieldAbility.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UPlayerFireShieldAbility : public UNoDamageAbility_Variable
{
	GENERATED_BODY()
	
protected:
	virtual void GetAbilityDescOnLevel(UAbilitySystemComponent* InASC, int32 InAbilityLevel, FString& OutDescription) const override;
};
