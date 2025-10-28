// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "WaitForCooldownChanged.generated.h"

// 输出引脚
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldwonChangedSignature, float, TimeRemaining);

/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class RPGDEMO_API UWaitForCooldownChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	// 监听某个ASC上某个技能的冷却时间
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UWaitForCooldownChanged* WaitForCooldownChange(UAbilitySystemComponent* InASC, const FGameplayTag& InCooldownTag);

	// 蓝图可调用，在结束之后手动进行资源释放
	UFUNCTION(BlueprintCallable)
	void EndTask();
	

protected:
	// 冷却时间变化监听
	void CooldownChanged(const FGameplayTag InCooldownTag, int32 NewCount);

	// 进入冷却监听
	void OnActiveAdded(UAbilitySystemComponent* InASC, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveEffectHandle);

public:
	UPROPERTY(BlueprintAssignable)
	FCooldwonChangedSignature CooldownStart;

	UPROPERTY(BlueprintAssignable)
	FCooldwonChangedSignature CooldownEnd;

protected:
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayTag CooldownTag;

};
