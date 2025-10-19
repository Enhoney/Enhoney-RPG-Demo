// Copyright Enhoney.


#include "EnhoneyPlayerAbilityInfo.h"

TArray<FPlayerAbilityInfo> UEnhoneyPlayerAbilityInfo::GetInherentOffensiveAbilityInfo() const
{
	return OffensiveAbilityInfo_Inherent;
}

TArray<FPlayerAbilityInfo> UEnhoneyPlayerAbilityInfo::GetInherentPassiveAbilityInfo() const
{
	return PassiveAbilityInfo_Inherent;
}

bool UEnhoneyPlayerAbilityInfo::GetOffensiveAbilityInfoByTag(const FGameplayTag& InAbilityTag, FPlayerAbilityInfo& OutAbilityInfo)
{
	bool OutResult = false;

	for (const FPlayerAbilityInfo& AbilityInfo : OffensiveAbilityInfo_Variable)
	{
		if (AbilityInfo.AbilityTag.MatchesTagExact(InAbilityTag))
		{
			OutAbilityInfo = AbilityInfo;
			OutResult = true;
			break;
		}
	}

	return OutResult;
}

bool UEnhoneyPlayerAbilityInfo::GetPassiveAbilityInfoByTag(const FGameplayTag& InAbilityTag, FPlayerAbilityInfo& OutAbilityInfo)
{
	bool OutResult = false;

	for (const FPlayerAbilityInfo& AbilityInfo : PassiveAbilityInfo_Variable)
	{
		if (AbilityInfo.AbilityTag.MatchesTagExact(InAbilityTag))
		{
			OutAbilityInfo = AbilityInfo;
			OutResult = true;
			break;
		}
	}

	return OutResult;
}


