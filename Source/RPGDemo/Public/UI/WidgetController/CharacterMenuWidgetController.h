// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyWidgetControllerBase.h"
#include "CharacterMenuWidgetController.generated.h"



// CharacterMenu的状态--标识当前使用的页面
UENUM(BlueprintType)
enum class ECharacterMenuState : uint8
{
	AttributeMenu,
	SkillMenu,
	BackpackMenu,
	Max
};

// 切换页面按钮点击
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterMehuStateChangedSignature, ECharacterMenuState, NewCharacterState);

// 关闭CharacterMenu--在HUD中绑定，用于移除CharacterMenu和显示PlayingWidget
DECLARE_DELEGATE(FCharacterMenuClosedSignature);

/**
 * 
 */
UCLASS(Blueprintable)
class RPGDEMO_API UCharacterMenuWidgetController : public UEnhoneyWidgetControllerBase
{
	GENERATED_BODY()

public:
	/** CharacterMenu关闭回调
	 *	--移除CharacterMenu，并调用Controller中相关函数
	 *	如果是单机，就取消暂停，如果是多人游戏，就仅仅移除，显示PlayingWidget
	 */
	UFUNCTION(BlueprintCallable)
	void HandleCharacterMenuClosed();

	UFUNCTION(BlueprintCallable)
	void HandleCharacterMenuStateSwitch(ECharacterMenuState NewCharacterMenuState);

	/** 获取Switcher中三种Widget的WidgetController*/
	// 获取AttributeMenuWidgetController
	UFUNCTION(BlueprintCallable, Category = "CharacterMenuWidgetController")
	UEnhoneyWidgetControllerBase* GetAttributeMenuWidgetController();
	// 获取SkillMenuWidgetController
	UFUNCTION(BlueprintCallable, Category = "CharacterMenuWidgetController")
	UEnhoneyWidgetControllerBase* GetSkillMenuWidgetController();
	// 获取InventoryWidgetController
	UFUNCTION(BlueprintCallable, Category = "CharacterMenuWidgetController")
	UEnhoneyWidgetControllerBase* GetInventoryWidgetController();

public:
	// 页面切换
	UPROPERTY(BlueprintAssignable, BlueprintReadWrite);
	FCharacterMehuStateChangedSignature OnCharacterMenuStateChangedDelegate;

	// 关闭页面
	FCharacterMenuClosedSignature OnCharacterMenuClosedDelegate;
};
