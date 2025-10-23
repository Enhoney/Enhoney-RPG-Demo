// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "AbilityInputConfig.h"
#include "EnhoneyEnhancedInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnhoneyEnhancedInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename InputReleasedFuncType>
	void BindAbilityInputActions(const UAbilityInputConfig* InAbilityInputConfig,
		UserClass* UserObject, InputReleasedFuncType ReleasedFuncPtr);
	
};

template<class UserClass, typename InputReleasedFuncType>
void UEnhoneyEnhancedInputComponent::BindAbilityInputActions(const UAbilityInputConfig* InAbilityInputConfig,
	UserClass* UserObject, InputReleasedFuncType ReleasedFuncPtr)
{
	checkf(InAbilityInputConfig, TEXT("AbilityInputConfig is not valid!!!"));
	check(UserObject);

	for (const FAbilityInputAction& EnhoneyInputConfig : InAbilityInputConfig->AbilityInputConfig)
	{
		if (ReleasedFuncPtr)
		{
			BindAction(EnhoneyInputConfig.AbilityInputAction, ETriggerEvent::Completed, UserObject, ReleasedFuncPtr, EnhoneyInputConfig.InputTag);
		}
	}
}
