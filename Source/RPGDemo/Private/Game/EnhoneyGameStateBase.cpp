// Copyright Enhoney.


#include "EnhoneyGameStateBase.h"
#include "InventoryItemInfo.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PlayerInterface.h"
#include "TaskSystemComponent.h"
#include "StoreWidgetController.h"

AEnhoneyGameStateBase::AEnhoneyGameStateBase()
{
	bReplicates = true;
	NetUpdateFrequency = 5.0f;  // 每秒尝试更新的最大次数
	MinNetUpdateFrequency = 2.0f; // 每秒尝试更新的最小次数
	MerchantInventoryItems = FInventoryItemArray(20);

	TaskSystemComponent = CreateDefaultSubobject<UTaskSystemComponent>(TEXT("TaskSystemComponent"));
	TaskSystemComponent->SetIsReplicated(true);
}

UInventoryItemInfo* AEnhoneyGameStateBase::GetInventoryItemConfigInfo() const
{
	return InventoryItemInfo;
}

void AEnhoneyGameStateBase::BuyItem(FInventoryItem PurchasedItem)
{
	// 计算物品总价，检查玩家的钱够不够
	// 如果够，就扣钱、给玩家发东西，从商人背包减少物品
	// 先找到物品单价，如果单价大于0，表示这个物品有效，就可以购买
	if (TransactionPlayer.IsValid() && TransactionPlayer->Implements<UPlayerInterface>())
	{
		int64 ItemUibtPrice = GetItemPrice(PurchasedItem.ItemTag);
		if ((ItemUibtPrice > 0) && (PurchasedItem.ItemNum > 0))
		{
			// 给玩家东西
			UInventoryComponent* PlayerInventoryComp = IPlayerInterface::Execute_GetPlayerInventoryComponent(TransactionPlayer.Get());
			check(PlayerInventoryComp);
			PlayerInventoryComp->ServerAddItemByTag(PurchasedItem);
			// 扣钱
			IPlayerInterface::Execute_ConsumeCoinNum(TransactionPlayer.Get(), ItemUibtPrice * PurchasedItem.ItemNum);

			{
				// 线程锁
				FScopeLock ScopeLock(&CriticalSection);
				// 减少自身库存
				for (FInventoryItem& FoodItem : MerchantInventoryItems.Items)
				{
					if (FoodItem.ItemTag.MatchesTagExact(PurchasedItem.ItemTag))
					{
						FoodItem.ItemNum -= PurchasedItem.ItemNum;
						MerchantInventoryItems.MarkItemDirty(FoodItem);

						break;
					}
				}
				MerchantInventoryItems.MarkArrayDirty();
				InventoryChangedDelegate.ExecuteIfBound(MerchantInventoryItems);
			}
			
		}
	}

}


void AEnhoneyGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AEnhoneyGameStateBase, MerchantInventoryItems, COND_None, REPNOTIFY_Always);
}

void AEnhoneyGameStateBase::InitInventory()
{
	if (HasAuthority())
	{
		FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();

		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_BlueMushroom, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_CompoundPotion, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_ForbiddenPotion, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_GrilledBirdLegs, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_GrilledDragonMeat, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_HoneyPotion, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_JinGua, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_LifePotion, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_ManaPotion, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_Morels, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_Pumpkin, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_RoastedPork, 99));
		MerchantInventoryItems.Items.Add(FInventoryItem(EnhoneyTags.Material_Food_WoodMushroom, 99));

		MerchantInventoryItems.MarkArrayDirty();
	}

}

void AEnhoneyGameStateBase::SetTransactionPlayer(AActor* InPlayerCharacter)
{
	TransactionPlayer = InPlayerCharacter;
}

void AEnhoneyGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	InitInventory();
}

void AEnhoneyGameStateBase::OnRep_PropMerchantInventory(const FInventoryItemArray& OldInventory)
{
	InventoryChangedDelegate.ExecuteIfBound(MerchantInventoryItems);
}

int64 AEnhoneyGameStateBase::GetItemPrice(const FGameplayTag& ItemTag) const
{
	if (ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Food"))))
	{
		// 找到物品配置信息
		FInventoryItem_Food InventoryItemInfo_Food;
		if (UEnhoneyAbilitySystemLibrary::GetInventoryItemConfig_Food(this, ItemTag, InventoryItemInfo_Food))
		{
			// 找到购买价格
			return InventoryItemInfo_Food.InventoryItemBaseInfo.PurchasePrice;
		}
	}

	return 0;
}

int32 AEnhoneyGameStateBase::QueryFoodItemNum(const FGameplayTag& InFoodItemTag) const
{
	int32 OutNum = 0;
	if (InFoodItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Food"))))
	{

		for (const FInventoryItem& FoodItem : MerchantInventoryItems.Items)
		{
			if (FoodItem.ItemTag.MatchesTagExact(InFoodItemTag))
			{
				OutNum = FoodItem.ItemNum;
				break;
			}
		}

	}
	return OutNum;
}

int32 AEnhoneyGameStateBase::QueryMaxNumPurchase(const FGameplayTag& InFoodItemTag, int64 InPlayerCoinNum) const
{
	// 查询当前库存中这个物品的数量
	int32 ItemNumInInventory = QueryFoodItemNum(InFoodItemTag);
	// 查询这些金币可购买的最大数量
	int64 ItemUintPrice = GetItemPrice(InFoodItemTag);	// 物品单价
	int32 MaxNumPurchase = (ItemUintPrice > 0) ? (InPlayerCoinNum / ItemUintPrice) : 0;


	return FMath::Min<int32>(ItemUintPrice, MaxNumPurchase);
}

