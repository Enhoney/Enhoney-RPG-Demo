// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyWidgetControllerBase.h"
#include "InventoryComponent.h"
#include "PlayingWidgetController.generated.h"

class UInventoryWidgetController;
class UTaskWidgetController;
class UPlayerTaskPanelWidget;

// 显示或者隐藏BOSS血条以及更新血条
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FBossHealthChangedSignature, bool, bIsVisibile, FName, BossName, float, NewCurrentHealth, float, NewCurretnMaxHealth);

// 获得物品弹窗
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowGetItemOnUISignature, FInventoryItem, GetInvnetoryItemInfo);

// 经验值改变
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCharacterExpChangedSignature, float, NewExpValue, float, NewExpNeededForLevelup);

// 暂定菜单打开或者关闭
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGamePauseOpenOrCloseSignature, bool, bOpen);

/**
 * 
 */
UCLASS(Blueprintable)
class RPGDEMO_API UPlayingWidgetController : public UEnhoneyWidgetControllerBase
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValue() override;

	virtual void BindCallbacksToDependiencies() override;

	// 获取InventoryWidgetController
	UFUNCTION(BlueprintCallable)
	UInventoryWidgetController* GetInventoryWidgetController();

	// 获取玩家任务面板WidgetController
	UFUNCTION(BlueprintCallable)
	UTaskWidgetController* GetPlayerTaskWidgetController();

	// 设置PlayerTaskWidgetController中的PlayerTaskWidget
	UFUNCTION(BlueprintCallable)
	void SetPlayerTaskWidgetOnController(UPlayerTaskPanelWidget* InPlayerTaskWidget);

	// 打开暂停菜单
	void OpenPauseMenu();

	// 关闭暂停菜单继续游戏
	UFUNCTION(BlueprintCallable)
	void ClosePuaseMenu();

	// 退出游戏
	UFUNCTION(BlueprintCallable)
	void QuitGame();


public:
	UPROPERTY(BlueprintAssignable)
	FCharacterLevelChangedSignature OnCharacterLevelChangedDelegate;

	// 经验值改变
	UPROPERTY(BlueprintAssignable)
	FCharacterExpChangedSignature OnExpChangedDelegate;

	// 最大生命值
	UPROPERTY(BlueprintAssignable)
	FAttributeValueChangedSignature OnMaxHealthChangedDelegate;
	// 当前生命值
	UPROPERTY(BlueprintAssignable)
	FAttributeValueChangedSignature OnHealthChangedDelegate;

	// 最大魔法值
	UPROPERTY(BlueprintAssignable)
	FAttributeValueChangedSignature OnMaxManaChangedDelegate;
	// 当前魔法值
	UPROPERTY(BlueprintAssignable)
	FAttributeValueChangedSignature OnManaChangedDelegate;

	// 最大奥义值
	UPROPERTY(BlueprintAssignable)
	FAttributeValueChangedSignature OnMaxArcaneChangedDelegate;
	// 当前奥义值
	UPROPERTY(BlueprintAssignable)
	FAttributeValueChangedSignature OnArcaneChangedDelegate;

	// 显示或者隐藏BOSS血条以及更新血条
	UPROPERTY(BlueprintAssignable)
	FBossHealthChangedSignature OnBossHealthChangedDelegate;

	// 获得物品弹窗
	UPROPERTY(BlueprintAssignable)
	FShowGetItemOnUISignature OnShowGetItemOnUIDelegate;

	// 暂停菜单打开或者关闭
	UPROPERTY(BlueprintAssignable)
	FGamePauseOpenOrCloseSignature OnGamePauseDelegate;
};
