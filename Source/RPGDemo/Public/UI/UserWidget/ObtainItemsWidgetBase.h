// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyUserWidgetBase.h"
#include "ObtainItemsWidgetBase.generated.h"

class UTexture2D;
class UInventoryWidgetController;
class UVerticalBox;


/**
 * 
 */
UCLASS(BlueprintType)
class RPGDEMO_API UObtainItemsWidgetBase : public UEnhoneyUserWidgetBase
{
	GENERATED_BODY()

public:
	// 启用--会自行调用更新到UI的那个函数
	UFUNCTION(BlueprintCallable, Category = "ObtainItem")
	void EnableItem(FName InItemName, UTexture2D* InItemIcon, int32 InItemNum);

	// 回收
	UFUNCTION(BlueprintCallable, Category = "ObtainItem")
	void RecylingItem();

	// 更新数据到UI
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InventoryItem")
	void UpdateInfoToWidget(UVerticalBox* LootUIBox);

	// 是否在使用中
	FORCEINLINE bool IsItemWidgetInUse() const
	{
		return bInUse;
	}

protected:
	// 对应的ItemTag
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|Common")
	FName ItemName = TEXT("");

	// 图标
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|Common")
	TObjectPtr<UTexture2D> ItemIcon;

	// 数量
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|Common")
	int32 ItemNum = 0;

	// 是否在使用
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|ObjectPool")
	bool bInUse = false;
	
};
