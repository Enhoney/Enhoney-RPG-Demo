// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/EnhoneyWidgetControllerBase.h"
#include "EnhoneyGameplayTags.h"
#include "InventoryWidgetController.generated.h"

class UInventoryItemButtonBase;
class UObtainItemsWidgetBase;
class UVerticalBox;

struct FInventoryItem;

// 子背包切换
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInventorySwitchSignature, const FGameplayTag&, ItemTypeTag, int32, InventorySwitcherIndex);

// 更新金币
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCoinNumChangedSignature, int64, NewCoinNum);

// 新增InventoryItem
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewInventoryItemAddSignature, const FGameplayTag&, ItemTypeTag, UInventoryItemButtonBase*, InventoryItemButton);

// 更新描述信息--选中或者取消某个Item时，更新右侧描述信息
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FUpdateItemDescInfoSignature, bool, bSelected, UTexture2D*, ItemIcon, FString, ItemName, FString, ItemDescInfo, FString, ItemExtralInfo);

// 显示或者隐藏食物栏弹窗
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShowOrHideFoodBarPopupSignature, bool, bPopUp);

/**
 * 
 */
UCLASS(Blueprintable)
class RPGDEMO_API UInventoryWidgetController : public UEnhoneyWidgetControllerBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void HandleInventorySwitch(const FGameplayTag& InItemTypeTag);

	// 选中某个InventoryItemButton
	UFUNCTION(BlueprintCallable)
	void HandleInventoryItemSelected(UInventoryItemButtonBase* InInventoryItemSelected);
	// 取消某个InventoryItemButton的选中
	UFUNCTION(BlueprintCallable)
	void HandleInventoryItemUnselected(UInventoryItemButtonBase* InInventoryItemUnselected);

	// 处理食物栏弹窗
	UFUNCTION(BlueprintCallable)
	void HandleFoodBarPopup(bool bPopUp);

	// 食用食物
	UFUNCTION(BlueprintCallable)
	void UseFoodItem();

	// 取消食用
	UFUNCTION(BlueprintCallable)
	void CancelUseFoodItem();


	// 广播初始值
	virtual void BroadcastInitialValue() override;

	// 监听
	virtual void BindCallbacksToDependiencies() override;

public:
	UPROPERTY(BlueprintAssignable, Category = "InventoryWidgetController|InventorySwitch")
	FInventorySwitchSignature InventorySwitchDelegate;
	
	UPROPERTY(BlueprintAssignable, Category = "InventoryWidgetController|InventorySwitch")
	FCoinNumChangedSignature OnCoinNumChangedDelegate;

	// 新增物品
	UPROPERTY(BlueprintAssignable, Category = "InventoryWidgetController|InventoryItems")
	FNewInventoryItemAddSignature OnNewInventoryItemAddDelegate;

	// 更新物品描述信息
	UPROPERTY(BlueprintAssignable, Category = "InventoryWidgetController|InventoryItems")
	FUpdateItemDescInfoSignature OnUpdateDescInfoDelegate;

	// 显示或者隐藏食物栏弹窗
	UPROPERTY(BlueprintAssignable, Category = "InventoryWidgetController|InventoryItems")
	FShowOrHideFoodBarPopupSignature OnShowOrHideFoodBarPopupDelegate;

	// 当前选中的InventoryItem
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UInventoryItemButtonBase> SelectedInventoryItem;


#pragma region InventoryItemObjectPool
public:
	// 初始化对象池
	UFUNCTION(BlueprintCallable, Category = "InventoryItemObjectPool")
	void Initialize();

	// 启用一个新的InventoryItemButton
	UFUNCTION()
	void EnableNewInventoryItem(FGameplayTag InNewInventoryItemTag, int32 InInventoryItemNum);

	// 更新对应物品的数量到UI
	UFUNCTION()
	void UpdateInventoryItemNum(FGameplayTag InNewInventoryItemTag, int32 InInventoryItemNum);


	// 移除某个物品
	UFUNCTION(BlueprintCallable)
	void RemoveInventoryItem(FGameplayTag InInventoryItem);

	// 启用一个ObatinItemsWidget
	UFUNCTION(BlueprintCallable)
	void EnableObatinItemsWidget(FInventoryItem InItemInfo, UVerticalBox* LootUIBox);

private:
	// 查找已经在使用的和InventoryItemTag对应的ItemButton
	UInventoryItemButtonBase* GetInventoryItemButtonByTag(FGameplayTag InInventoryItemTag);

	// 查找指定对象池中的指定InventoryItemButton
	UInventoryItemButtonBase* GetInventoryItemButtonOrdered(FGameplayTag InInventoryItemTag, TArray<TObjectPtr<UInventoryItemButtonBase>> OrderedPool);

	// 获取指定对象池中未使用的InventoryItemButton
	UInventoryItemButtonBase* GetInventoryItemButtonCanUseOrdered(TArray<TObjectPtr<UInventoryItemButtonBase>> OrderedPool);

	// 获取未使用的ObatinItemsWidget
	UObtainItemsWidgetBase* GetObatinItemsWidgetCanUse();
	

private:
	/** Button类*/
	// FoodItemButtonWidgetClass
	UPROPERTY(EditDefaultsOnly, Category = "InventoryItemObjectPool")
	TSubclassOf<UInventoryItemButtonBase> FoodItemButtonWidgetClass;
	// ValuableItemButtonWidgetClass
	UPROPERTY(EditDefaultsOnly, Category = "InventoryItemObjectPool")
	TSubclassOf<UInventoryItemButtonBase> ValuableItemButtonWidgetClass;
	// EconomicItemButtonWidgetClass
	UPROPERTY(EditDefaultsOnly, Category = "InventoryItemObjectPool")
	TSubclassOf<UInventoryItemButtonBase> EconomicItemButtonWidgetClass;
	// TaskItemButtonWidgetClass
	UPROPERTY(EditDefaultsOnly, Category = "InventoryItemObjectPool")
	TSubclassOf<UInventoryItemButtonBase> TaskItemButtonWidgetClass;

	// ObatinItemsWidgetClass
	UPROPERTY(EditDefaultsOnly, Category = "ObatinItemsWidgetObjectPool")
	TSubclassOf<UObtainItemsWidgetBase> ObatinItemsWidgetClass;

	/** 对象池*/
	// 大小
	UPROPERTY(EditDefaultsOnly, Category = "InventoryItemObjectPool")
	int32 ObjectPoolSize = 50;
	// FoodItemButton对象池
	UPROPERTY()
	TArray<TObjectPtr<UInventoryItemButtonBase>> FoodItemButtonPool;
	// ValuableItemButton对象池
	UPROPERTY()
	TArray<TObjectPtr<UInventoryItemButtonBase>> ValuableItemButtonPool;
	// EconomicItemButton对象池
	UPROPERTY()
	TArray<TObjectPtr<UInventoryItemButtonBase>> EconomicItemButtonPool;
	// TaskItemButton对象池
	UPROPERTY()
	TArray<TObjectPtr<UInventoryItemButtonBase>> TaskItemButtonPool;

	// ObatinItemsWidget对象池
	UPROPERTY()
	TArray<TObjectPtr<UObtainItemsWidgetBase>> ObatinItemsWidgetPool;


#pragma endregion InventoryItemObjectPool
};
