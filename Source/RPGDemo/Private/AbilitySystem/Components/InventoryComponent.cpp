// Copyright Enhoney.


#include "InventoryComponent.h"

#include "Net/UnrealNetwork.h"
#include "EnhoneyGameStateBase.h"
#include "EnhoneyPlayerState.h"
#include "InventoryItemInfo.h"
#include "EnhoneyAbilitySystemComponent.h"
#include "EnhoneyAbilitySystemLibrary.h"

#include "EnhoneyLogChannel.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UInventoryComponent::BroadcastCurrentInventory()
{
	for (FInventoryItem FoodItem : FoodInventoryItems.Items)
	{
		if (FoodItem.ItemTag.IsValid())
		{
			OnNewItemAddDelegate.Broadcast(FoodItem.ItemTag, FoodItem.ItemNum);
		}
		else
		{
			break;
		}
	}
	for (FInventoryItem EconomicItem : EconomicInventoryItems.Items)
	{
		if (EconomicItem.ItemTag.IsValid())
		{
			OnNewItemAddDelegate.Broadcast(EconomicItem.ItemTag, EconomicItem.ItemNum);
		}
		else
		{
			break;
		}
	}
	for (FInventoryItem ValuableItem : ValuableInventoryItems.Items)
	{
		if (ValuableItem.ItemTag.IsValid())
		{
			OnNewItemAddDelegate.Broadcast(ValuableItem.ItemTag, ValuableItem.ItemNum);
		}
		else
		{
			break;
		}
	}
	for (FInventoryItem TaskItem : TaskInventoryItems.Items)
	{
		if (TaskItem.ItemTag.IsValid())
		{
			OnNewItemAddDelegate.Broadcast(TaskItem.ItemTag, TaskItem.ItemNum);
		}
		else
		{
			break;
		}
	}
}

void UInventoryComponent::ServerAddItems_Implementation(const TArray<FInventoryItem>& ItemsToAdd)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		for (const FInventoryItem& ItemToAdd : ItemsToAdd)
		{
			ServerAddItemByTag(ItemToAdd);
		}
	}
}

void UInventoryComponent::ServerConsumeItems_Implementation(const TArray<FInventoryItem>& ItemsToConsume)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		if (CheckItemsNum(ItemsToConsume))
		{
			for (const FInventoryItem& ItemToConsume : ItemsToConsume)
			{
				ServerConsumeItemByTag(ItemToConsume);
			}
		}
	}
}

bool UInventoryComponent::CheckItemsNum(TArray<FInventoryItem> ItemsToCheck)
{
	bool bOutNumsufficient = false;

	for (const FInventoryItem& Item : ItemsToCheck)
	{
		bOutNumsufficient = CheckItemNumByTag(Item);
		if (!bOutNumsufficient)
		{
			break;
		}
	}

	return bOutNumsufficient;
}

void UInventoryComponent::ServerAddItemByTag_Implementation(FInventoryItem ItemToAdd)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		// 执行客户端RPC，让UI显示获得物品的提示
		ClientBrocastItemGet(ItemToAdd);

		// 找到要查询的类别
		FInventoryItemArray* SubInventory = GetSubInventoryByTag(ItemToAdd.ItemTag);

		if (SubInventory->Items.Contains(ItemToAdd))
		{
			// 如果已经有了，就改变数量
			for (FInventoryItem& InventoryItem : SubInventory->Items)
			{
				if (InventoryItem.ItemTag.MatchesTagExact(ItemToAdd.ItemTag))
				{
					InventoryItem.ItemNum += ItemToAdd.ItemNum;
					SubInventory->MarkItemDirty(InventoryItem);
					ClientBrocastItemNumChanged(InventoryItem.ItemTag, InventoryItem.ItemNum);
					break;
				}
			}
		}
		else
		{
			// 如果没有，就新增
			SubInventory->Items.Add(ItemToAdd);
			SubInventory->MarkArrayDirty();
			ClientBrocastNewItemAdd(ItemToAdd.ItemTag, ItemToAdd.ItemNum);

		}
	}
	
}

void UInventoryComponent::ServerConsumeItemByTag_Implementation(FInventoryItem ItemToConsume)
{
	if (GetOwnerRole() == ENetRole::ROLE_Authority)
	{
		if (CheckItemNumByTag(ItemToConsume))
		{
			// 找到要查询的类别
			FInventoryItemArray* SubInventory = GetSubInventoryByTag(ItemToConsume.ItemTag);

			if (!SubInventory->IsEmpty())
			{
				// 是否耗尽
				bool bExhausted = false;
				// 要移除的ItemIndex
				int32 ItemIndexToRemove = 0;

				for (FInventoryItem& InventoryItem : SubInventory->Items)
				{
					if (InventoryItem.ItemTag.MatchesTagExact(ItemToConsume.ItemTag))
					{
						if (InventoryItem.ItemNum == ItemToConsume.ItemNum)
						{
							// 刚好消耗完
							bExhausted = true;
							// 通知UI移除这个Item
							ClientBrocastItemExhausted(ItemToConsume.ItemTag);
							break;
						}
						else
						{
							// 消耗之后数量不是0
							InventoryItem.ItemNum -= ItemToConsume.ItemNum;
							// 通知UI修改数量
							SubInventory->MarkItemDirty(InventoryItem);
							ClientBrocastItemNumChanged(InventoryItem.ItemTag, InventoryItem.ItemNum);
							break;
						}
					}

					++ItemIndexToRemove;
				}

				// 移除消耗完的物品
				if (bExhausted)
				{
					SubInventory->Items.RemoveAt(ItemIndexToRemove);
					SubInventory->MarkArrayDirty();
				}

				// 施加效果到这个玩家身上
				if (GetWorld())
				{
					AEnhoneyGameStateBase* EnhoneyGameState = GetWorld()->GetGameState<AEnhoneyGameStateBase>();
					check(EnhoneyGameState);
					UInventoryItemInfo* InventoryItemInfo = EnhoneyGameState->GetInventoryItemConfigInfo();
					if (IsValid(InventoryItemInfo))
					{
						// 找到PlayerState
						AEnhoneyPlayerState* PlayerState = Cast<AEnhoneyPlayerState>(GetOwner());
						if (ItemToConsume.ItemTag.MatchesTagExact(FEnhoneyGameplayTags::Get().Material_Food_DragonVeinFruit))
						{
							// 如果是龙脉果，就增加5点属性点
							if (IsValid(PlayerState))
							{
								for (int32 ItemIndex = 0; ItemIndex < ItemToConsume.ItemNum; ++ItemIndex)
								{
									// 每消耗一个龙脉果就增加5点属性点
									PlayerState->AddAttributePoint(5);
								}
							}
						}
						else if (ItemToConsume.ItemTag.MatchesTagExact(FEnhoneyGameplayTags::Get().Material_Food_ElfFruit))
						{
							// 如果是精灵果，就增加点技能点
							if (IsValid(PlayerState))
							{
								for (int32 ItemIndex = 0; ItemIndex < ItemToConsume.ItemNum; ++ItemIndex)
								{
									PlayerState->AddSkillPoint(2);
								}
							}
						}
						else
						{
							// 除此之外，根据ItemTag找到对应的FoodEffectClass
							TSubclassOf<UGameplayEffect> FoodEffectClass = InventoryItemInfo->GetFoodEffectClassByTag(ItemToConsume.ItemTag);
							// 如果FoodEffectClass有效，就施加效果
							if (FoodEffectClass)
							{
								for (int32 ItemIndex = 0; ItemIndex < ItemToConsume.ItemNum; ++ItemIndex)
								{
									UEnhoneyAbilitySystemLibrary::ApplyBuffEffectToTarget(GetWorld(), FoodEffectClass, GetOwner());
								}
							}
							else
							{
								UE_LOG(LogEnhoney, Warning, TEXT("FoodEffectClass is not valid for ItemTag: %s"), *ItemToConsume.ItemTag.ToString());
							}
						}


					}
				}
			}
		}
	}
}

bool UInventoryComponent::CheckItemNumByTag(FInventoryItem ItemToCheck)
{
	return (GetItemNum(ItemToCheck.ItemTag) >= ItemToCheck.ItemNum);
}

int32 UInventoryComponent::GetItemNum(FGameplayTag ItemTag)
{
	int32 OutItemNum = 0;

	// 找到要查询的类别
	FInventoryItemArray* SubInventory = GetSubInventoryByTag(ItemTag);

	if (!SubInventory->IsEmpty())
	{
		for (FInventoryItem InventoryItem : SubInventory->Items)
		{
			if (InventoryItem.ItemTag.MatchesTagExact(ItemTag))
			{
				OutItemNum = InventoryItem.ItemNum;
				break;
			}
		}
	}

	return OutItemNum;
}

void UInventoryComponent::AllocateInventorySpace()
{
	if (IsValid(GetOwner()) && GetOwner()->HasAuthority())
	{
		if (FoodInventoryItems.IsEmpty())
		{
			FoodInventoryItems = FInventoryItemArray(InventoryCapacity);
		}
		if (ValuableInventoryItems.IsEmpty())
		{
			ValuableInventoryItems = FInventoryItemArray(InventoryCapacity);
		}
		if (EconomicInventoryItems.IsEmpty())
		{
			EconomicInventoryItems = FInventoryItemArray(InventoryCapacity);
		}
		if (TaskInventoryItems.IsEmpty())
		{
			TaskInventoryItems = FInventoryItemArray(InventoryCapacity);
		}
	}
	
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, FoodInventoryItems);
	DOREPLIFETIME(UInventoryComponent, ValuableInventoryItems);
	DOREPLIFETIME(UInventoryComponent, EconomicInventoryItems);
	DOREPLIFETIME(UInventoryComponent, TaskInventoryItems);
}

FInventoryItemArray* UInventoryComponent::GetSubInventoryByTag(FGameplayTag InItemTag)
{
	if (InItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Food"))))
	{
		return &FoodInventoryItems;
	}
	else if (InItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"))))
	{
		return &EconomicInventoryItems;
	}
	else if (InItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Valuable"))))
	{
		
		return &ValuableInventoryItems;
	}
	else if (InItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Task"))))
	{
		return &TaskInventoryItems;
	}
	else
	{
		checkf(false, TEXT("InventoryError: ItemTag is Not Valid!!!"));
		return nullptr;
	}
}

void UInventoryComponent::ClientBrocastItemGet_Implementation(FInventoryItem ItemGet)
{
	OnPlayerGetItemDelegate.Broadcast(ItemGet);
}

void UInventoryComponent::ClientBrocastItemNumChanged_Implementation(FGameplayTag ItemTag, int32 NewNum)
{
	OnItemNumChangedDelegate.Broadcast(ItemTag, NewNum);
}

void UInventoryComponent::ClientBrocastNewItemAdd_Implementation(FGameplayTag ItemTag, int32 NewNum)
{
	OnNewItemAddDelegate.Broadcast(ItemTag, NewNum);
}

void UInventoryComponent::ClientBrocastItemExhausted_Implementation(FGameplayTag ItemTag)
{
	OnItemExhaustedDelegate.Broadcast(ItemTag);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();

	// 为四种背包分配内存
	AllocateInventorySpace();

	if (IsValid(GetOwner()) && GetOwner()->HasAuthority())
	{
		// 测试代码，添加Item
		ServerAddItemByTag(FInventoryItem(EnhoneyTags.Material_Food_WoodMushroom, 2));
		ServerAddItemByTag(FInventoryItem(EnhoneyTags.Material_Food_HoneyPotion, 2));
		ServerAddItemByTag(FInventoryItem(EnhoneyTags.Material_Food_LifePotion, 3));
		ServerAddItemByTag(FInventoryItem(EnhoneyTags.Material_Valuable_DragonBone, 3));
		ServerAddItemByTag(FInventoryItem(EnhoneyTags.Material_Valuable_DragonTooth, 5));

		ServerAddItemByTag(FInventoryItem(EnhoneyTags.Material_Economic_Slag, 5));
		ServerAddItemByTag(FInventoryItem(EnhoneyTags.Material_Economic_GoldBar, 5));

		ServerAddItemByTag(FInventoryItem(EnhoneyTags.Material_Task_Chalcedony, 5));
		ServerAddItemByTag(FInventoryItem(EnhoneyTags.Material_Task_RoundRuneStone, 5));
		ServerAddItemByTag(FInventoryItem(EnhoneyTags.Material_Task_ElfFeather, 5));
	}

}

