// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhoneyGameplayTags.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryComponent.generated.h"

// 背包物品--单个
USTRUCT(BlueprintType)
struct RPGDEMO_API FInventoryItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FInventoryItem() {}

	FInventoryItem(FGameplayTag InItemTag, int32 InItemNum)
		: ItemTag(InItemTag), ItemNum(InItemNum)
	{

	}

	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	FGameplayTag ItemTag = FGameplayTag();

	UPROPERTY(BlueprintReadOnly, Category = "InventoryItem")
	int32 ItemNum = 0;

	bool operator==(const FInventoryItem& InInventoryItem) const
	{
		return ItemTag.MatchesTagExact(InInventoryItem.ItemTag);
	}


};

// 库存物品数组--专门用于网络复制
USTRUCT(BlueprintType)
struct RPGDEMO_API FInventoryItemArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FInventoryItemArray() {}

	FInventoryItemArray(int32 InventoryArrayCapacity)
	{
		Items.Reserve(InventoryArrayCapacity);
	}

	UPROPERTY()
	TArray<FInventoryItem> Items; // 必须命名为 Items

	// 必须实现 NetDeltaSerialize 函数
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FInventoryItem, FInventoryItemArray>(Items, DeltaParms, *this);
	}

	// 自己的业务逻辑函数
	bool IsEmpty() const
	{
		return Items.IsEmpty();
	}
};

template<>
struct TStructOpsTypeTraits<FInventoryItemArray> : public TStructOpsTypeTraitsBase2<FInventoryItemArray>
{
	enum
	{
		WithNetDeltaSerializer = true, // 启用自定义Delta序列化
	};
};

// 物品数量改变（不是从有到无也不是从无到有）
DECLARE_MULTICAST_DELEGATE_TwoParams(FItemNumChangedSignature, FGameplayTag /** ItemTag*/, int32 /** NewNum*/);
// 新增物品
DECLARE_MULTICAST_DELEGATE_TwoParams(FNewItemAddSignature, FGameplayTag /** ItemTag*/, int32 /** NewNum*/);
// 物品耗尽
DECLARE_MULTICAST_DELEGATE_OneParam(FItemExhaustedSignature, FGameplayTag /** ItemTag*/);

// 获得了物品，广播给UI，让玩家能够看到获得的物品
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerGetItemSignature, FInventoryItem /** GetItemInfo*/)


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGDEMO_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	// 打开背包的时候，广播现有物品
	void BroadcastCurrentInventory();

	/** 多种物品的操作*/
	// 增加一定数量的物品
	UFUNCTION(Server, Reliable,BlueprintCallable, Category = "Inventory|AddItem")
	void ServerAddItems(const TArray<FInventoryItem>& ItemsToAdd);

	// 消费一定数量的物品
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory|ConsumeItem")
	void ServerConsumeItems(const TArray<FInventoryItem>& ItemsToConsume);

	// 查询物品数量是否满足消耗
	UFUNCTION(BlueprintCallable, Category = "Inventory|CheckItemNum")
	bool CheckItemsNum(TArray<FInventoryItem> ItemsToCheck);

	/** 单个物品的操作*/
	// 增加一定数量的物品
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory|AddItem")
	void ServerAddItemByTag(FInventoryItem ItemToAdd);

	// 消费一定数量的物品
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Inventory|ConsumeItem")
	void ServerConsumeItemByTag(FInventoryItem ItemToConsume);

	// 查询物品数量是否满足消耗
	UFUNCTION(BlueprintCallable, Category = "Inventory|CheckItemNum")
	bool CheckItemNumByTag(FInventoryItem ItemToCheck);

	// 查询物品数量
	UFUNCTION(BlueprintCallable, Category = "Inventory|GetItem")
	int32 GetItemNum(FGameplayTag ItemTag);


	// 分配库存
	void AllocateInventorySpace();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	FInventoryItemArray* GetSubInventoryByTag(FGameplayTag InItemTag);

	// 客户端RPC执行广播
	UFUNCTION(Client, Reliable)
	void ClientBrocastItemNumChanged(FGameplayTag ItemTag, int32 NewNum);
	UFUNCTION(Client, Reliable)
	void ClientBrocastNewItemAdd(FGameplayTag ItemTag, int32 NewNum);
	UFUNCTION(Client, Reliable)
	void ClientBrocastItemExhausted(FGameplayTag ItemTag);

	// 获得物品时，客户端RPC执行广播
	UFUNCTION(Client, Reliable)
	void ClientBrocastItemGet(FInventoryItem ItemGet);

public:
	// 物品数量改变
	FItemNumChangedSignature OnItemNumChangedDelegate;
	// 新增物品
	FNewItemAddSignature OnNewItemAddDelegate;
	// 物品耗尽
	FItemExhaustedSignature OnItemExhaustedDelegate;

	FPlayerGetItemSignature OnPlayerGetItemDelegate;

protected:
	virtual void BeginPlay() override;

private:
	// FoodItems
	UPROPERTY(Replicated)
	FInventoryItemArray FoodInventoryItems;
	// ValuableItems
	UPROPERTY(Replicated)
	FInventoryItemArray ValuableInventoryItems;
	// EconomicItems
	UPROPERTY(Replicated)
	FInventoryItemArray EconomicInventoryItems;
	// TaskItems
	UPROPERTY(Replicated)
	FInventoryItemArray TaskInventoryItems;

	// 每种背包的容量
	UPROPERTY(EditDefaultsOnly, Category = "InventoryCapacity")
	int32 InventoryCapacity = 50;

};
