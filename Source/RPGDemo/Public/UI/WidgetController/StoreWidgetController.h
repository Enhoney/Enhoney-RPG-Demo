// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/EnhoneyWidgetControllerBase.h"
#include "GameplayTagContainer.h"
#include "StoreWidgetController.generated.h"

class APropMerchantCharacter;
class UEnhoneyUserWidgetBase;
class UInventoryItemButtonBase;

/** 要绑定到UI上的*/
// 更新商品描述信息--选中或者取消某个Item时，更新右侧描述信息
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FUpdateCommodityDescInfoSignature, bool, bSelected, UTexture2D*, ItemIcon, FString, ItemName, FString, ItemDescInfo, FString, ItemExtralInfo);
// 购物弹窗
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FShoppingPopupSignature, bool, bPopUp, int32, PopUpIndex, FGameplayTag, CommondityTag, int32, NewMaxPurchaseNum);
// 更新商品按钮的选中状态
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSwitchCommonditySelected, const FGameplayTag&, InSelectedItemTag);
// 更新金币数量
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerCoinNumChangedSignature, int64, NewCoinNum);
// 更新商店物品数量
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShopItemNumChangedSignature, const FGameplayTag&, InItemTag, int32, NewItemNum);

// 购买或者售卖
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPurchaseOrSaleSwitchSignature, int32, SwitcherIndex);
// 新增SaleItem
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewSaleItemAddSignature, const FGameplayTag&, ItemTypeTag, UInventoryItemButtonBase*, SaleItemButton);
// 售卖弹窗
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSalePopupSignature, bool, bPopUp, FGameplayTag, SaleItemTag, int32, NewMaxSaleNum);


/**
 * 
 */
UCLASS(Blueprintable)
class RPGDEMO_API UStoreWidgetController : public UEnhoneyWidgetControllerBase
{
	GENERATED_BODY()

public:
	// 初始化，我们要在这里找到地图上的商人
	void Initialize();

	// 广播初始值
	virtual void BroadcastInitialValue() override;

	// 监听
	virtual void BindCallbacksToDependiencies() override;

	// 打开商店的UI页面
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void OpenStoreUI();

	// 通过UI点击按钮调用，退出商店，此时我们解绑不需要的代理
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void QuitStore();

	// 选择商品--查看信息，显示边框
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void SelectCommodity(const FGameplayTag& InCommodityTag);

	// 双击时，预购买--购买物品弹窗
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void PrePurchase(const FGameplayTag& InCommodityTag);

	// 双击可售卖物品时--预售弹窗
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void PreSell(const FGameplayTag& InSaleItemTag);

	// 购买物品
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void PurchaseCommodity(const FGameplayTag& InCommodityTag, int32 InCommodityNum);

	// 出售物品
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void SellItem(const FGameplayTag& InSaleTag, int32 InSaleItemNum);

	// 关闭购买弹窗
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void ColsePopupWindow();

	// 购买或者出售
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void SwitchStorePurchaseOrSale(int32 SwitcherIndex);

	// 启用一个新的SaleItemButton
	UFUNCTION()
	void EnableNewSaleItem(FGameplayTag InNewSaleItemTag, int32 InSaleItemNum);

	// 更新对应物品的数量到UI
	UFUNCTION()
	void UpdateSaleItemNum(FGameplayTag InNewSaleItemTag, int32 InSaleItemNum);

	// 移除某个售卖的物品--背包中这个物品买完了
	UFUNCTION(BlueprintCallable)
	void RemoveSaleItem(FGameplayTag InSaleItem);

	// 选中某个SaleItemButton
	UFUNCTION(BlueprintCallable)
	void HandleSaleItemSelected(UInventoryItemButtonBase* InSaleItemSelected);
	// 取消某个SaleItemButton的选中
	UFUNCTION(BlueprintCallable)
	void HandleSaleItemUnselected(UInventoryItemButtonBase* InSaleItemUnselected);

private:
	// 查找已经在使用的和SaleItemTag对应的ItemButton
	UInventoryItemButtonBase* GetSaleItemButtonInUseByTag(FGameplayTag InSaleItemTag);

	// 查找未使用的SaltItemButton
	UInventoryItemButtonBase* GetSaleItemButtonCanUse();

	// 回收所有商品按钮对象
	void RecylingAllSaleButton();

public:
	// 更新物品描述信息的代理
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FUpdateCommodityDescInfoSignature OnUpdateCommondDescInfoDelegate;

	// 购物弹窗
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FShoppingPopupSignature OnShoppingPopupDelegate;

	// 更新物品选择边框
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FSwitchCommonditySelected OnSwitchCommonditySelectedDelegate;

	// 更新金币数量
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FPlayerCoinNumChangedSignature OnPlayerCoinNumChangedDelegate;

	// 更新商店单个物品数量
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FShopItemNumChangedSignature OnShopItemNumChangedDelegate;

	// 更新售卖的物品数量
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FShopItemNumChangedSignature OnSaleItemNumChangedDelegate;

	// 切换购买或者出售
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FPurchaseOrSaleSwitchSignature OnPurchaseOrSaleSwitchDelegate;

	// 增加可售卖物品
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FNewSaleItemAddSignature OnNewSaleItemAddDelegate;

	// 预售弹窗
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FSalePopupSignature OnSalePopupDelegate;

	// 当前选中要售卖的物品的SaleItem
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UInventoryItemButtonBase> SelectedItemToSale;


private:
	// 地图上的商人
	UPROPERTY()
	TWeakObjectPtr<APropMerchantCharacter> PropMerchant;

	// 商店页面UI类
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEnhoneyUserWidgetBase> StoreUIPageClass;

	// 商店页面UI对象
	UPROPERTY()
	TObjectPtr<UEnhoneyUserWidgetBase> StoreUI;

	// SaleItemButtonWidgetClass
	UPROPERTY(EditDefaultsOnly, Category = "SaleItemObjectPool")
	TSubclassOf<UInventoryItemButtonBase> SaleItemButtonWidgetClass;
	// SaleItemButtonWidget ObjectPool
	UPROPERTY()
	TArray<TObjectPtr<UInventoryItemButtonBase>> SaleItemButtonPool;
	// 对象池大小
	UPROPERTY(EditDefaultsOnly)
	int32 SaleObjectPoolSize = 20;
};
