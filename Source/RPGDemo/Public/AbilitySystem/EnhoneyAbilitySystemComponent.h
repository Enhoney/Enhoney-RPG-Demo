// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "EnhoneyAbilitySystemComponent.generated.h"

class UEnhoneyAbilityBase;

// 技能输入变化
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnVariableAbilityInputChanged, const FGameplayTag& /** AbilityTag*/,
	const FGameplayTag& /** NewAbilityInputTag*/, const FGameplayTag& /** OldAbilityInputTag*/);

// 技能状态变化
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnVariableAbilityStatusChanged, const FGameplayTag& /** AbilityTag*/, const FGameplayTag& /** NewAbilityStatus*/);


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

	// 调用RPC来执行改键操作
	void EquipSkill(const FGameplayTag& InAbilityTag, const FGameplayTag& InTargetInputTag);

	// 执行RPC来升级技能
	void UpgradeSkill(const FGameplayTag& InAbilityTag);

	// 执行RPC来解锁技能
	void UnlockSkill(const FGameplayTag& InAbilityTag);

	// 监测技能是否可以解锁--升级的时候--在服务器调用
	void UpdateVariableAbilityStatusOnLevelUp(int32 NewCharacterLevel);


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

	// 获取可变技能的状态
	bool GetVariableAbilityStatusBySpec(const FGameplayAbilitySpec* InAbilitySpec, FGameplayTag& OutAbilityStatusTag);

	// 获取可变技能的输入Tag
	bool GetVariableAbilityInputTagBySpec(const FGameplayAbilitySpec* InAbilitySpec, FGameplayTag& OutAbilityInputTag);

	// 获取某个可变技能的状态
	bool GetVariableAbilityStatusByTag(const FGameplayTag& InAbilityTag, FGameplayTag& OutAbilityStatusTag);

	virtual void AbilitySpecInputPressed(FGameplayAbilitySpec& Spec) override;

	virtual void AbilitySpecInputReleased(FGameplayAbilitySpec& Spec) override;

	// 尝试激活对应的技能
	bool AbilityInputTagReleased(const FGameplayTag& InAbilityInputTag);

	// 获取某个技能当前的描述信息
	bool GetVariableAbilityCurrentDescriptionByTag(const FGameplayTag& InAbilityTag, FString& OutAbilityDescription);

	// 获取某个技能下一等级的描述信息
	bool GetVariableAbilityNextLevelDescriptionByTag(const FGameplayTag& InAbilityTag, FString& OutAbilityDescription);


	// 广播一次所有可变技能的状态和输入--打开技能菜单用的，给客户端调用的
	void BroadcastAllVariableAbility();
	

private:
	// 更新某个技能的状态--在服务器调用
	bool UpdateVariableAbilityStatus(FGameplayAbilitySpec* InAbilitySpec, const FGameplayTag& InNewStatus);
	// 更新某个技能对应的输入--在服务器调用
	bool UpdateVariableAbilityInputTag(FGameplayAbilitySpec* InAbilitySpec, const FGameplayTag& InNewInputTag);

	// 客户端RPC，广播技能状态改变
	UFUNCTION(Client, Reliable, Category = "SkillMenu")
	void ClientNotifyAbilityStatusChanged(const FGameplayTag& AbilityTag, const FGameplayTag& NewAbilityStatus);
	// 客户端RPC，广播技能输入改变
	UFUNCTION(Client, Reliable, Category = "SkillMenu")
	void ClientNotifyAbilityInputChanged(const FGameplayTag& AbilityTag, const FGameplayTag& NewAbilityInputTag, const FGameplayTag& OldAbilityInputTag);

	// 服务器RPC，客户端调用，装备某个技能到某个输入上
	UFUNCTION(Server, Reliable, Category = "SkillMenu|Skill Equip")
	void ServerEquipSkill(const FGameplayTag& InAbilityTag, const FGameplayTag& InTargetInputTag);

	// 服务器RPC，客户端调用，升级某个技能
	UFUNCTION(Server, Reliable, Category = "SkillMenu|Skill Upgrade")
	void ServerUpgradeSkill(const FGameplayTag& InAbilityTag);

	// 服务器RPC，客户端调用，解锁某个技能
	UFUNCTION(Server, Reliable, Category = "SkillMenu|Skill Unlock")
	void ServerUnlockSkill(const FGameplayTag& InAbilityTag);


public:
	FOnVariableAbilityInputChanged OnVariabieAbilityInputChangedDelegate;

	FOnVariableAbilityStatusChanged OnVariableAbilityStatusChangedDelegate;
};
