// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EnhoneyAbilitySystemComponent.generated.h"

class UEnhoneyAbilityBase;

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnhoneyAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// 服务器RPC
	UFUNCTION(Server, Reliable, Category = "AttributeMenu|UpgradeAttributePoint")
	void ServerUpgradeAttributePoint(const FGameplayTag& InAttributeTag);

	// 使用动态Effect来增加属性--仅在服务器有效
	void AddAttributeValue(const FGameplayTag& InAttributeTag);

	// 赋予玩家固有技能--这个函数只会在服务器上调用
	void EndowInherentAbility(TSubclassOf<UEnhoneyAbilityBase> AbilityClassToEndow, bool bIsPassiveSkill);

	// 赋予玩家可变技能--需要初始化技能状态为锁定
	void EndowVariableAbility(TSubclassOf<UEnhoneyAbilityBase> AbilityClassToEndow);

	// 获取指定AbilityTag对应的AbilitySpec
	FGameplayAbilitySpec* GetAbilitySpecByTag(const FGameplayTag& InAbilityTag);

	// 根据InputTag找到对应的AbilitySpec
	FGameplayAbilitySpec* GetAbilitySpecByInputTag(const FGameplayTag& InAbilityInputTag);

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;

	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	// 尝试激活对应的技能
	bool AbilityInputTagReleased(const FGameplayTag& InAbilityInputTag);

private:
	
	
};
