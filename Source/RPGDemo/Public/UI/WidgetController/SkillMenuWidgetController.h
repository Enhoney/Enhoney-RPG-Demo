// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/EnhoneyWidgetControllerBase.h"
#include "GameplayTagContainer.h"
#include "SkillMenuWidgetController.generated.h"

UENUM(BlueprintType)
enum class ESkillMenuState : uint8
{
	ESMS_None			UMETA(DisplayName = "None"),
	ESMS_Common			UMETA(DisplayName = "Common"),
	ESMS_View			UMETA(DisplayName = "View"),
	ESMS_PreEquip		UMETA(DisplayName = "PreEquip"),
	ESMS_Max			UMETA(DisplayName = "Max")
};

// 当前技能的信息
USTRUCT(BlueprintType)
struct FCurrentSkillInfo
{
	GENERATED_BODY()

public:
	FCurrentSkillInfo() {}

	FCurrentSkillInfo(const FName& InAbilityName, const UTexture2D* InAbilityIcn, const FString& InAbiliyDesc)
		: AilityName(InAbilityName), SkillIcon(InAbilityIcn), SkillDescription(InAbiliyDesc)
	{

	}
public:
	// 名称
	UPROPERTY(BlueprintReadOnly)
	FName AilityName = TEXT("");
	// 图标
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<const UTexture2D> SkillIcon = nullptr;
	// 描述信息
	UPROPERTY(BlueprintReadOnly)
	FString SkillDescription = TEXT("");

};

// 选中某个技能的委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAbilitySelectedSignature, const FGameplayTag&, InAbiltyTag, const FGameplayTag&, InAbilityStatusTag,
	const FCurrentSkillInfo&, InAbilityDescThisLevel, const FCurrentSkillInfo&, InAbilityDescNextLevel);

// 某个技能进入选中状态
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityEnterSelectedStateSignature, const FGameplayTag&, InAbilityTag);

// 技能输入变化的委托--技能装备到新的输入上，或者打开的时候广播
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnAbilityInputChangedOnSkillMenuSignature, const UTexture2D*, InAbilityIcon, const FGameplayTag&, InAbilityTag,
	const FGameplayTag&, InNewAbilityInputTag, const FGameplayTag&, InOldAbilityInputTag);

// 技能状态改变的委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAbilityStatusChangedOnSkillMenuSignature, const FGameplayTag&, InAbilityTag,
	const FGameplayTag&, InNewAbilityStatusTag);

// 进入PreEquip状态的委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterPreEquipStateSignature);

// 回到Common状态的委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnToCommonStateSignature);

// 技能点变化的委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillPointChangedOnSkillMenuSignature, int32, NewSkillPointValue);

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPGDEMO_API USkillMenuWidgetController : public UEnhoneyWidgetControllerBase
{
	GENERATED_BODY()
public:
	// 选择查看某个技能的信息
	UFUNCTION(BlueprintCallable, Category = "SkillMenuWidgetController")
	void SelectAbility(const FGameplayTag& InAbilityTag);

	// 解锁选中的技能
	UFUNCTION(BlueprintCallable, Category = "SkillMenuWidgetController")
	void UnlockAbility();

	// 升级选中的技能
	UFUNCTION(BlueprintCallable, Category = "SkillMenuWidgetController")
	void LevelupAbility();

	// 准备装备选中的技能
	UFUNCTION(BlueprintCallable, Category = "SkillMenuWidgetController")
	void PrepareToEquipAbility();

	// 装备选中的技能到指定输入
	UFUNCTION(BlueprintCallable, Category = "SkillMenuWidgetController")
	void EquipAbilityToInput(const FGameplayTag& InAbilityInputTag);

	virtual void BroadcastInitialValue() override;

	virtual void BindCallbacksToDependiencies() override;

private:
	// 回到Common状态
	void ReturnToCommonState();

public:
	// 选中某个技能之后，广播技能信息--进入View状态
	UPROPERTY(BlueprintAssignable, Category = "SkillMenuWidgetController|Delegate")
	FOnAbilitySelectedSignature OnAbilitySelectedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "SkillMenuWidgetController|Delegate")
	FOnAbilityEnterSelectedStateSignature OnAbilityEnterSelectedStateDelegate;
	// 输入变化的广播
	UPROPERTY(BlueprintAssignable, Category = "SkillMenuWidgetController|Delegate")
	FOnAbilityInputChangedOnSkillMenuSignature OnAbilityInputChangedOnSkillMenuDelegate;
	// 技能状态变化的广播
	UPROPERTY(BlueprintAssignable, Category = "SkillMenuWidgetController|Delegate")
	FOnAbilityStatusChangedOnSkillMenuSignature OnAbilityStatusChangedOnSkillMenuDelegate;
	// 进入PreEquip状态的广播--启用左下角按钮，不可选择其他技能
	UPROPERTY(BlueprintAssignable, Category = "SkillMenuWidgetController|Delegate")
	FOnEnterPreEquipStateSignature OnEnterPreEquipStateDelegate;
	// 回到Common状态的广播
	UPROPERTY(BlueprintAssignable, Category = "SkillMenuWidgetController|Delegate")
	FOnReturnToCommonStateSignature OnReturnToCommonStateDelegate;
	// 技能点变化的广播
	UPROPERTY(BlueprintAssignable, Category = "SkillMenuWidgetController|Delegate")
	FOnSkillPointChangedOnSkillMenuSignature OnSkillPointChangedOnSkillMenuDelegate;
private:
	// 当前技能菜单的状态
	ESkillMenuState CurrentSkillMenuState = ESkillMenuState::ESMS_Common;

	// 当前选中的技能标签
	FGameplayTag CurrentSelectedAbilityTag = FGameplayTag();
	
};
