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
	NetUpdateFrequency = 5.0f;  // ÿ�볢�Ը��µ�������
	MinNetUpdateFrequency = 2.0f; // ÿ�볢�Ը��µ���С����
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
	// ������Ʒ�ܼۣ������ҵ�Ǯ������
	// ��������Ϳ�Ǯ������ҷ������������˱���������Ʒ
	// ���ҵ���Ʒ���ۣ�������۴���0����ʾ�����Ʒ��Ч���Ϳ��Թ���
	if (TransactionPlayer.IsValid() && TransactionPlayer->Implements<UPlayerInterface>())
	{
		int64 ItemUibtPrice = GetItemPrice(PurchasedItem.ItemTag);
		if ((ItemUibtPrice > 0) && (PurchasedItem.ItemNum > 0))
		{
			// ����Ҷ���
			UInventoryComponent* PlayerInventoryComp = IPlayerInterface::Execute_GetPlayerInventoryComponent(TransactionPlayer.Get());
			check(PlayerInventoryComp);
			PlayerInventoryComp->ServerAddItemByTag(PurchasedItem);
			// ��Ǯ
			IPlayerInterface::Execute_ConsumeCoinNum(TransactionPlayer.Get(), ItemUibtPrice * PurchasedItem.ItemNum);

			{
				// �߳���
				FScopeLock ScopeLock(&CriticalSection);
				// ����������
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
		// �ҵ���Ʒ������Ϣ
		FInventoryItem_Food InventoryItemInfo_Food;
		if (UEnhoneyAbilitySystemLibrary::GetInventoryItemConfig_Food(this, ItemTag, InventoryItemInfo_Food))
		{
			// �ҵ�����۸�
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
	// ��ѯ��ǰ����������Ʒ������
	int32 ItemNumInInventory = QueryFoodItemNum(InFoodItemTag);
	// ��ѯ��Щ��ҿɹ�����������
	int64 ItemUintPrice = GetItemPrice(InFoodItemTag);	// ��Ʒ����
	int32 MaxNumPurchase = (ItemUintPrice > 0) ? (InPlayerCoinNum / ItemUintPrice) : 0;


	return FMath::Min<int32>(ItemUintPrice, MaxNumPurchase);
}

