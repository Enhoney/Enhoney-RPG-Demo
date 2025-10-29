// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "EnhoneyAbilityBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class RPGDEMO_API UEnhoneyAbilityBase : public UGameplayAbility
{
	GENERATED_BODY()

public:
	// 获取当前等级的描述信息
	virtual void GetAbilityLevelDescription(UAbilitySystemComponent* InASC, FString& OutDescription, int32 InAbilityCurrentLevel) const;

	// 获取下一等级的描述信息
	virtual void GetNextAbilityLevelDescription(UAbilitySystemComponent* InASC, FString& OutDescription, int32 InAbilityCurrentLevel) const;

protected:
	// 获取某一等级的描述信息--已解锁或者已装备状态
	virtual void GetAbilityDescOnLevel(UAbilitySystemComponent* InASC, int32 InAbilityLevel, FString& OutDescription) const;

private:
	void GetAbilityDescOnLockedOrEligible(UAbilitySystemComponent* InASC, FString& OutDescription, const FGameplayTag& InAbilityStatus) const;

protected:
	// 最大等级约束
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	int32 MaxAbilityLevel = 5;
	
};
