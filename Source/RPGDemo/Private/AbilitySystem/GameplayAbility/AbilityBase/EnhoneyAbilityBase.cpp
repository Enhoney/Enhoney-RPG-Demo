// Copyright Enhoney.


#include "EnhoneyAbilityBase.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "EnhoneyGameplayTags.h"
#include "EnhoneyPlayerAbilityInfo.h"
#include "EnhoneyAbilitySystemComponent.h"

void UEnhoneyAbilityBase::GetAbilityLevelDescription(UAbilitySystemComponent* InASC, FString& OutDescription, int32 InAbilityCurrentLevel) const
{
	// 找到这个技能的状态
	FGameplayTag AbilityStatsus;
	UEnhoneyAbilitySystemLibrary::GetVariableAbilityStatusByTag(InASC, AbilityTags.GetByIndex(0), AbilityStatsus);
	if (!AbilityStatsus.IsValid())
	{
		return;
	}
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();
	if (AbilityStatsus.MatchesTagExact(EnhoneyTags.AbilityStatus_Locked) ||
		AbilityStatsus.MatchesTagExact(EnhoneyTags.AbilityStatus_Eligible))
	{
		GetAbilityDescOnLockedOrEligible(InASC, OutDescription, AbilityStatsus);
		return;
	}

	// 走到这里说明技能已经解锁或装备
	GetAbilityDescOnLevel(InASC, InAbilityCurrentLevel, OutDescription);
	
}

void UEnhoneyAbilityBase::GetNextAbilityLevelDescription(UAbilitySystemComponent* InASC, FString& OutDescription, int32 InAbilityCurrentLevel) const
{
	// 找到这个技能的状态
	FGameplayTag AbilityStatsus;
	UEnhoneyAbilitySystemLibrary::GetVariableAbilityStatusByTag(InASC, AbilityTags.GetByIndex(0), AbilityStatsus);
	if (!AbilityStatsus.IsValid())
	{
		return;
	}
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();
	if (AbilityStatsus.MatchesTagExact(EnhoneyTags.AbilityStatus_Locked) ||
		AbilityStatsus.MatchesTagExact(EnhoneyTags.AbilityStatus_Eligible))
	{
		GetAbilityDescOnLockedOrEligible(InASC, OutDescription, AbilityStatsus);
		return;
	}

	// 看看现在是否满级
	if (InAbilityCurrentLevel >= MaxAbilityLevel)
	{
		OutDescription = FString::Printf(TEXT("<Golden>已满级</>"));
		return;
	}

	// 没有满级，就显示下一级的信息
	GetAbilityDescOnLevel(InASC, InAbilityCurrentLevel + 1, OutDescription);
}

void UEnhoneyAbilityBase::GetAbilityDescOnLevel(UAbilitySystemComponent* InASC, int32 InAbilityLevel, FString& OutDescription) const
{
}

void UEnhoneyAbilityBase::GetAbilityDescOnLockedOrEligible(UAbilitySystemComponent* InASC, FString& OutDescription, const FGameplayTag& InAbilityStatus) const
{
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();
	if (InAbilityStatus.MatchesTagExact(EnhoneyTags.AbilityStatus_Locked))
	{
		// 找到解锁的等级
		FPlayerAbilityInfo AbilityInfo;
		if(UEnhoneyAbilitySystemLibrary::GetVariableAbilityInfoByTag(InASC->GetAvatarActor(), AbilityTags.GetByIndex(0), AbilityInfo))
		{
			OutDescription = FString::Printf(TEXT("<Orange>%d</><AbilityDesc>级可解锁</>"), AbilityInfo.UnlockLevel);
		}
	}
	else if (InAbilityStatus.MatchesTagExact(EnhoneyTags.AbilityStatus_Eligible))
	{
		OutDescription = FString::Printf(TEXT("<AbilityDesc>可解锁</>"));
	}
}
