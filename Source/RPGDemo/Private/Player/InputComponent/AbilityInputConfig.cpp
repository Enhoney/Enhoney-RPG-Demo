// Copyright Enhoney.

#include "Player/InputComponent/AbilityInputConfig.h"
#include "EnhoneyLogChannel.h"


const UInputAction* UAbilityInputConfig::GetInputActinoByTag(const FGameplayTag& InInputTag, bool bLogNotFound) const
{
	if (!InInputTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("InputTag"))))
	{
		if (bLogNotFound)
		{
			UE_LOG(LogEnhoney, Error, TEXT("[%s]: InputTag--%s is not valid, please check your config"), *FString(__FUNCTION__), *InInputTag.ToString());
		}
		return nullptr;
	}
	
	for (const FAbilityInputAction& InputConfig : AbilityInputConfig)
	{
		if (InputConfig.InputTag.MatchesTagExact(InInputTag))
		{
			return InputConfig.AbilityInputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogEnhoney, Error, TEXT("[%s]: No InputAction matching the Tag: [%s]"), *FString(__FUNCTION__), *InInputTag.ToString());
	}
	return nullptr;
}
