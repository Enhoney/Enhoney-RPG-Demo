// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UI/UserWidget/EnhoneyUserWidgetBase.h"
#include "GameplayTagContainer.h"
#include "ShoppingWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UShoppingWidgetBase : public UEnhoneyUserWidgetBase
{
	GENERATED_BODY()


public:
	/** 这些弹窗函数都是要在添加到视口之后调用的，需要在蓝图自行更新数据到UI*/

	// 弹窗--警告钱不够
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void PopWindow_WarningForLackOfMoney();

	// 弹窗--警告物品售罄
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void PopWindow_WaringForSoldOut();

	// 弹窗--选择购买数量，在这里设置最大购买数量
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void PopWindow_SelectShopNum(const FGameplayTag& InItemTag, int32 InMaxShopNum);
	// 服务于选择购买数量的弹窗--增加购买数量
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void AddShoppingNum();
	// 服务于选择购买数量的弹窗--减少购买数量
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void ReduceShoppingNum();

	// 弹窗--选择售卖数量，在这里设置最大售卖数量
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void PopWindow_SelectSaleNum(const FGameplayTag& InItemTag, int32 InMaxSaleNum);
	// 服务于选择售卖数量的弹窗--增加售卖数量
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void AddSaleNum();
	// 服务于选择售卖数量的弹窗--减少售卖数量
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void ReduceSaleNum();

protected:
	// 最大购买数量
	UPROPERTY(BlueprintReadOnly)
	int32 ShoppingMaxNum = 0;
	// 当前购买数量
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentShoppingNum = 1;
	// 当前选中要购买的物品
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemToShop = FGameplayTag();

	// 最大售卖数量
	UPROPERTY(BlueprintReadOnly)
	int32 SaleMaxNum = 0;
	// 当前售卖数量
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentSaleNum = 1;
	// 当前选中要出售的物品
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemToSale = FGameplayTag();

	
};
