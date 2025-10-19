// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyUserWidgetBase.h"
#include "GameplayTagContainer.h"
#include "InventoryItemButtonBase.generated.h"

class UTexture2D;
class UInventoryWidgetController;

/**
 * 
 */
UCLASS(BlueprintType)
class RPGDEMO_API UInventoryItemButtonBase : public UEnhoneyUserWidgetBase
{
	GENERATED_BODY()

public:
	// 启用--会自行调用更新到UI的那个函数
	UFUNCTION(BlueprintCallable, Category = "InventoryItem")
	void EnableItem(UEnhoneyWidgetControllerBase* InInventoryWidgetController, const FGameplayTag& InItemTag, UTexture2D* InItemIcon, int32 InItemNum);

	// 回收
	UFUNCTION(BlueprintCallable, Category = "InventoryItem")
	void RecylingItem();

	// 更新数量--会自行调用更新到UI的函数
	UFUNCTION(BlueprintCallable, Category = "InventoryItem")
	void UpdateItemNum(int32 InNewItemNum);

	// Item被选中--显示边框
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryItem")
	void HandleItemSelected();

	// Item取消选中--隐藏边框
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InventoryItem")
	void HandleItemUnselected();

	// 获取ItemTag
	FORCEINLINE FGameplayTag GetItemTag() const
	{
		return InventoryItemTag;
	}

	// 是否在使用中
	FORCEINLINE bool IsItemButtonIsUse() const
	{
		return bInUse;
	}

	// 更新数据到UI
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InventoryItem")
	void UpdateInfoToWidget();

protected:
	// 对应的ItemTag
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|Common")
	FGameplayTag InventoryItemTag =  FGameplayTag();

	// 图标
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|Common")
	TObjectPtr<UTexture2D> InventoryItemIcon;

	// 数量
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|Common")
	int32 InventoryItemNum = 0;

	// 是否在使用
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|ObjectPool")
	bool bInUse = false;

	// 是否被选中
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|UI")
	bool bIsSelected = false;
};
