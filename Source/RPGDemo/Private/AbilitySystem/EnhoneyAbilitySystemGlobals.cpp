// Copyright Enhoney.


#include "EnhoneyAbilitySystemGlobals.h"

#include "EnhoneyAbilityType.h"

FGameplayEffectContext* UEnhoneyAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FEnhoneyGameplayEffectContext();
}
