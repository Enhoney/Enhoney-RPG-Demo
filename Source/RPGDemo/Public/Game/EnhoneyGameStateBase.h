// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EnhoneyGameplayTags.h"
#include "InventoryComponent.h"
#include "EnhoneyGameStateBase.generated.h"

class UInventoryItemInfo;
class UTaskSystemComponent;

struct FInventoryItem_Food;

DECLARE_DELEGATE_OneParam(FPropMerchantInventoryChangedSignature, const FInventoryItemArray&);

/**
 * 
 */
UCLASS()
class RPGDEMO_API AEnhoneyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	AEnhoneyGameStateBase();

	/** 商城系统*/
#pragma region Store

	UFUNCTION(BlueprintCallable)
	UInventoryItemInfo* GetInventoryItemConfigInfo() const;

	// 购买某个物品
	UFUNCTION(BlueprintCallable, Category = "Merchant")
	void BuyItem(FInventoryItem PurchasedItem);

	// 获取单个物品的价格--通过GameState中的配置信息来获取
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Merchant")
	int64 GetItemPrice(const FGameplayTag& ItemTag) const;

	// 查询单个物品数量
	UFUNCTION(BlueprintCallable, Category = "Merchant")
	int32 QueryFoodItemNum(const FGameplayTag& InFoodItemTag) const;

	// 查询最大购买数量
	UFUNCTION(BlueprintCallable, Category = "Merchant")
	int32 QueryMaxNumPurchase(const FGameplayTag& InFoodItemTag, int64 InPlayerCoinNum) const;

	// 属性复制
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 获取库存
	FORCEINLINE const FInventoryItemArray& GetInventoryItems() const
	{
		return MerchantInventoryItems;
	}

	// 初始化库存
	void InitInventory();

	// 设置交易对象
	UFUNCTION(BlueprintCallable, Category = "Merchant")
	void SetTransactionPlayer(AActor* InPlayerCharacter);

protected:
	virtual void BeginPlay() override;

	// 广播现有库存
	UFUNCTION()
	void OnRep_PropMerchantInventory(const FInventoryItemArray& OldInventory);

public:
	FPropMerchantInventoryChangedSignature InventoryChangedDelegate;

protected:
	// 正在交易的玩家角色
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> TransactionPlayer;

private:
	
	// 商人NPC的物品列表，这个得复制，因为客户端要显示
	UPROPERTY(ReplicatedUsing = OnRep_PropMerchantInventory)
	FInventoryItemArray MerchantInventoryItems;

#pragma endregion Store

	/** 任务系统*/
#pragma region Task
public:
	// 获取任务系统组件
	FORCEINLINE UTaskSystemComponent* GetTaskSystemComponent()
	{
		return TaskSystemComponent;
	}
	

private:
	// 任务系统组件
	UPROPERTY(EditDefaultsOnly, Category = "TaskSystem")
	TObjectPtr<UTaskSystemComponent> TaskSystemComponent;

#pragma endregion Task

private:
	// InventoryItemInfo
	UPROPERTY(EditDefaultsOnly, Category = "InventoryItemInfo")
	TObjectPtr<UInventoryItemInfo> InventoryItemInfo;

	// 临界区
	FCriticalSection CriticalSection;

	
};
