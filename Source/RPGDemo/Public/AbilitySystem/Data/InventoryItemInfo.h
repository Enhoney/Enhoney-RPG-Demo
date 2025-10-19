// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "InventoryItemInfo.generated.h"

class UGameplayEffect;

// 库存物品的基类
USTRUCT(BlueprintType)
struct RPGDEMO_API FInventoryItemBase
{
	GENERATED_BODY()

public:
	// 类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem")
	FGameplayTag InventoryItemType = FGameplayTag();

	// 名称的Tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem")
	FGameplayTag InventoryItemNameTag = FGameplayTag();

	// 购入价格
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem")
	int64 PurchasePrice = 0;

	// 名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem")
	FString InventoryItemName = TEXT("未知");

	// 来源
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem")
	FString InventoryItemSource = TEXT("未知");

	// 描述信息
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem")
	FString InventoryItemDesc = TEXT("无");

	// 图标
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem")
	TObjectPtr<UTexture2D> InventoryItemIcon;
};

// 任务物品
USTRUCT(BlueprintType)
struct RPGDEMO_API FInventoryItem_Task
{
	GENERATED_BODY()

public:
	// 基础信息
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem|Task")
	FInventoryItemBase InventoryItemBaseInfo;
};

// 交易物品
USTRUCT(BlueprintType)
struct RPGDEMO_API FInventoryItem_Economic
{
	GENERATED_BODY()

public:
	// 基础信息
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem|Economic")
	FInventoryItemBase InventoryItemBaseInfo;

	// 售卖价格
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem|Economic")
	int64 SellingPrice = 0;
};

// 可食用物品
USTRUCT(BlueprintType)
struct RPGDEMO_API FInventoryItem_Food
{
	GENERATED_BODY()

public:
	// 基础信息
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem|Food")
	FInventoryItemBase InventoryItemBaseInfo;

	// 效果
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem|Food")
	TArray<TSubclassOf<UGameplayEffect>> FoodEffectClasses;
};

// 贵重物品
USTRUCT(BlueprintType)
struct RPGDEMO_API FInventoryItem_Valuable
{
	GENERATED_BODY()

public:
	// 基础信息
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "InventoryItem|Valuable")
	FInventoryItemBase InventoryItemBaseInfo;
};

/**
 * 
 */
UCLASS()
class RPGDEMO_API UInventoryItemInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "InventoryItemInfo")
	FInventoryItem_Task FindItemInfoByTag_Task(FGameplayTag InItemTag) const;

	UFUNCTION(BlueprintCallable, Category = "InventoryItemInfo")
	FInventoryItem_Economic FindItemInfoByTag_Economic(FGameplayTag InItemTag) const;

	UFUNCTION(BlueprintCallable, Category = "InventoryItemInfo")
	FInventoryItem_Food FindItemInfoByTag_Food(FGameplayTag InItemTag) const;

	UFUNCTION(BlueprintCallable, Category = "InventoryItemInfo")
	FInventoryItem_Valuable FindItemInfoByTag_Valuable(FGameplayTag InItemTag) const;

	// 获取Food对应的EffectClass
	UFUNCTION(BlueprintCallable, Category = "InventoryItemInfo")
	TSubclassOf<UGameplayEffect> GetFoodEffectClassByTag(FGameplayTag InItemTag) const;

private:
	// 任务物品
	UPROPERTY(EditDefaultsOnly, Category = "InventoryItemInfo|Task")
	TArray<FInventoryItem_Task> TaskItemsInfo;

	// 交易物品
	UPROPERTY(EditDefaultsOnly, Category = "InventoryItemInfo|Economic")
	TArray<FInventoryItem_Economic> EconomicItemsInfo;

	// 可食用物品
	UPROPERTY(EditDefaultsOnly, Category = "InventoryItemInfo|Food")
	TArray<FInventoryItem_Food> FoodItemsInfo;

	UPROPERTY(EditDefaultsOnly, Category = "InventoryItemInfo|Valuable")
	TArray<FInventoryItem_Valuable> ValuableItemsInfo;
	
};
