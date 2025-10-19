// Copyright Enhoney.


#include "InventoryWidgetController.h"

#include "EnhoneyPlayerState.h"
#include "EnhoneyPlayerController.h"
#include "InventoryItemInfo.h"
#include "EnhoneyPlayingHUD.h"
#include "InventoryComponent.h"

#include "InventoryItemButtonBase.h"
#include "EnhoneyGameStateBase.h"
#include "ObtainItemsWidgetBase.h"

#include "EnhoneyLogChannel.h"
#include "StoreWidgetController.h"

void UInventoryWidgetController::HandleInventorySwitch(const FGameplayTag& InItemTypeTag)
{
	int32 OutSwitcherIndex = 0;
	if (InItemTypeTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Food"))))
	{
		OutSwitcherIndex = 0;
	}
	else if (InItemTypeTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Valuable"))))
	{
		OutSwitcherIndex = 1;
	}
	else if (InItemTypeTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"))))
	{
		OutSwitcherIndex = 2;
	}
	else if (InItemTypeTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Task"))))
	{
		OutSwitcherIndex = 3;
	}

	InventorySwitchDelegate.Broadcast(InItemTypeTag, OutSwitcherIndex);
}

void UInventoryWidgetController::HandleInventoryItemSelected(UInventoryItemButtonBase* InInventoryItemSelected)
{
	// 先取消原来InventoryItem的选中
	if (SelectedInventoryItem.IsValid())
	{
		SelectedInventoryItem->HandleItemUnselected();
	}

	// 更新选中的InventoryItem
	SelectedInventoryItem = InInventoryItemSelected;
	SelectedInventoryItem->HandleItemSelected();

	// 获取新的信息
	FGameplayTag SelectedInventoryItemTag = SelectedInventoryItem->GetItemTag();
	// 类型Tag
	FGameplayTag SelectedItemTypeTag = FGameplayTag();
	// 图标
	UTexture2D* SelectedItemIcon = nullptr;
	// 名称
	FString SelectedItemName = TEXT("");
	// 描述信息
	FString SelectedItemDescInfo = TEXT("");
	// 来源
	FString SelectedItemSource = TEXT("");
	if (GetWorld())
	{
		// 拿到GameState
		AEnhoneyGameStateBase* EnhoneyGameState = GetWorld()->GetGameState<AEnhoneyGameStateBase>();
		check(EnhoneyGameState);
		
		// 找到物品配置信息
		UInventoryItemInfo* InventoryItemInfo = EnhoneyGameState->GetInventoryItemConfigInfo();

		// 先找到可用的InvenytoryItemButton，以及配置信息--主要是图标
		if (SelectedInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Food"))))
		{
			SelectedItemTypeTag = FGameplayTag::RequestGameplayTag(TEXT("Material.Food"));

			FInventoryItem_Food FoodItemInfo = InventoryItemInfo->FindItemInfoByTag_Food(SelectedInventoryItemTag);
			SelectedItemIcon = FoodItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
			SelectedItemName = FoodItemInfo.InventoryItemBaseInfo.InventoryItemName;
			SelectedItemDescInfo = FoodItemInfo.InventoryItemBaseInfo.InventoryItemDesc;
			SelectedItemSource = FoodItemInfo.InventoryItemBaseInfo.InventoryItemSource;
		}
		else if (SelectedInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"))))
		{
			SelectedItemTypeTag = FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"));

			FInventoryItem_Economic EconomicItemInfo = InventoryItemInfo->FindItemInfoByTag_Economic(SelectedInventoryItemTag);
			SelectedItemIcon = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
			SelectedItemName = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemName;
			SelectedItemDescInfo = FString::Printf(TEXT("<Write>售卖价格：</><Pink> %d金币</>"), EconomicItemInfo.SellingPrice);
			SelectedItemSource = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemSource;
		}
		else if (SelectedInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Valuable"))))
		{
			SelectedItemTypeTag = FGameplayTag::RequestGameplayTag(TEXT("Material.Valuable"));

			FInventoryItem_Valuable ValuableItemInfo = InventoryItemInfo->FindItemInfoByTag_Valuable(SelectedInventoryItemTag);
			SelectedItemIcon = ValuableItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
			SelectedItemName = ValuableItemInfo.InventoryItemBaseInfo.InventoryItemName;
			SelectedItemDescInfo = ValuableItemInfo.InventoryItemBaseInfo.InventoryItemDesc;
			SelectedItemSource = ValuableItemInfo.InventoryItemBaseInfo.InventoryItemSource;
		}
		else if (SelectedInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Task"))))
		{
			SelectedItemTypeTag = FGameplayTag::RequestGameplayTag(TEXT("Material.Task"));

			FInventoryItem_Task TaskItemInfo = InventoryItemInfo->FindItemInfoByTag_Task(SelectedInventoryItemTag);
			SelectedItemIcon = TaskItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
			SelectedItemName = TaskItemInfo.InventoryItemBaseInfo.InventoryItemName;
			SelectedItemDescInfo = TaskItemInfo.InventoryItemBaseInfo.InventoryItemDesc;
			SelectedItemSource = TaskItemInfo.InventoryItemBaseInfo.InventoryItemSource;
		}

		// 更新信息
		OnUpdateDescInfoDelegate.Broadcast(true, SelectedItemIcon, SelectedItemName, SelectedItemDescInfo, SelectedItemSource);
	}
}

void UInventoryWidgetController::HandleInventoryItemUnselected(UInventoryItemButtonBase* InInventoryItemUnselected)
{
	if (SelectedInventoryItem->GetItemTag().MatchesTagExact(InInventoryItemUnselected->GetItemTag()))
	{
		// 取消选中
		SelectedInventoryItem->HandleItemUnselected();
		SelectedInventoryItem = nullptr;

		OnUpdateDescInfoDelegate.Broadcast(false, nullptr, TEXT(""), TEXT(""), TEXT(""));
	}
}

void UInventoryWidgetController::HandleFoodBarPopup(bool bPopUp)
{
	OnShowOrHideFoodBarPopupDelegate.Broadcast(bPopUp);
}

void UInventoryWidgetController::UseFoodItem()
{
	if (SelectedInventoryItem.IsValid() && SelectedInventoryItem->GetItemTag().MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Food"))))
	{
		if (IsValid(PlayerState))
		{
			// 使用一个食物
			PlayerState->GetInventoryComponent()->ServerConsumeItemByTag(FInventoryItem(SelectedInventoryItem->GetItemTag(), 1));
		}

		// 取消选中
		SelectedInventoryItem->HandleItemUnselected();

		// 隐藏弹窗
		HandleFoodBarPopup(false);
	}
}

void UInventoryWidgetController::CancelUseFoodItem()
{
	if (SelectedInventoryItem.IsValid() && SelectedInventoryItem->GetItemTag().MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Food"))))
	{
		// 取消选中
		SelectedInventoryItem->HandleItemUnselected();
		// 隐藏弹窗
		HandleFoodBarPopup(false);
	}
	else
	{
		UE_LOG(LogEnhoney, Error, TEXT("CancelUseFoodItem: SelectedInventoryItem is not valid or not a food item!"));
	}
}

void UInventoryWidgetController::BroadcastInitialValue()
{
	if (IsValid(PlayerState))
	{
		OnCoinNumChangedDelegate.Broadcast(PlayerState->GetCoinNum());

		// 初始情况下描述信息栏为空
		OnUpdateDescInfoDelegate.Broadcast(false, nullptr, TEXT(""), TEXT(""), TEXT(""));

		// 广播背包中所有物品
		PlayerState->GetInventoryComponent()->BroadcastCurrentInventory();
	}
}

void UInventoryWidgetController::BindCallbacksToDependiencies()
{
	if (IsValid(PlayerState))
	{
		PlayerState->OnCoinNumChanged.AddWeakLambda(this, [this](int64 NewCoinNum)
			{
				OnCoinNumChangedDelegate.Broadcast(NewCoinNum);
			});

		PlayerState->GetInventoryComponent()->OnNewItemAddDelegate.AddUObject(this, &UInventoryWidgetController::EnableNewInventoryItem);
		PlayerState->GetInventoryComponent()->OnItemNumChangedDelegate.AddUObject(this, &UInventoryWidgetController::UpdateInventoryItemNum);
		PlayerState->GetInventoryComponent()->OnItemExhaustedDelegate.AddUObject(this, &UInventoryWidgetController::RemoveInventoryItem);
	}
}

void UInventoryWidgetController::Initialize()
{
	FoodItemButtonPool.Reserve(ObjectPoolSize);
	for (int32 Index = 0; Index < ObjectPoolSize; ++Index)
	{
		UInventoryItemButtonBase* FoodInventoryItemButton =
			CreateWidget<UInventoryItemButtonBase>(GetWorld(), FoodItemButtonWidgetClass);
		FoodItemButtonPool.Add(FoodInventoryItemButton);
	}

	ValuableItemButtonPool.Reserve(ObjectPoolSize);
	for (int32 Index = 0; Index < ObjectPoolSize; ++Index)
	{
		UInventoryItemButtonBase* ValuableInventoryItemButton =
			CreateWidget<UInventoryItemButtonBase>(GetWorld(), ValuableItemButtonWidgetClass);
		ValuableItemButtonPool.Add(ValuableInventoryItemButton);
	}

	EconomicItemButtonPool.Reserve(ObjectPoolSize);
	for (int32 Index = 0; Index < ObjectPoolSize; ++Index)
	{
		UInventoryItemButtonBase* EconomicInventoryItemButton =
			CreateWidget<UInventoryItemButtonBase>(GetWorld(), EconomicItemButtonWidgetClass);
		EconomicItemButtonPool.Add(EconomicInventoryItemButton);
	}

	TaskItemButtonPool.Reserve(ObjectPoolSize);
	for (int32 Index = 0; Index < ObjectPoolSize; ++Index)
	{
		UInventoryItemButtonBase* TaskInventoryItemButton =
			CreateWidget<UInventoryItemButtonBase>(GetWorld(), TaskItemButtonWidgetClass);
		TaskItemButtonPool.Add(TaskInventoryItemButton);
	}

	// 初始化ObatinItemsWidget对象池
	ObatinItemsWidgetPool.Reserve(ObjectPoolSize);
	for (int32 Index = 0; Index < ObjectPoolSize; ++Index)
	{
		UObtainItemsWidgetBase* ObatinItemsWidget =
			CreateWidget<UObtainItemsWidgetBase>(GetWorld(), ObatinItemsWidgetClass);
		ObatinItemsWidgetPool.Add(ObatinItemsWidget);
	}

}

void UInventoryWidgetController::EnableNewInventoryItem(FGameplayTag InNewInventoryItemTag, int32 InInventoryItemNum)
{
	UInventoryItemButtonBase* InventoryItem = nullptr;
	// 物品类型Tag
	FGameplayTag NewItemTypeTag = FGameplayTag();
	// 图标
	UTexture2D* NewItemIcon = nullptr;
	if (GetWorld())
	{
		// 拿到GameState
		AEnhoneyGameStateBase* EnhoneyGameState = GetWorld()->GetGameState<AEnhoneyGameStateBase>();
		check(EnhoneyGameState);

		// 找到物品配置信息
		UInventoryItemInfo* InventoryItemInfo = EnhoneyGameState->GetInventoryItemConfigInfo();;

		// 先找到可用的InvenytoryItemButton，以及配置信息--主要是图标
		if (InNewInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Food"))))
		{
			NewItemTypeTag = FGameplayTag::RequestGameplayTag(TEXT("Material.Food"));
			InventoryItem = GetInventoryItemButtonCanUseOrdered(FoodItemButtonPool);

			FInventoryItem_Food FoodItemInfo = InventoryItemInfo->FindItemInfoByTag_Food(InNewInventoryItemTag);
			NewItemIcon = FoodItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
		}
		else if (InNewInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"))))
		{
			NewItemTypeTag = FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"));
			InventoryItem = GetInventoryItemButtonCanUseOrdered(EconomicItemButtonPool);

			FInventoryItem_Economic EconomicItemInfo = InventoryItemInfo->FindItemInfoByTag_Economic(InNewInventoryItemTag);
			NewItemIcon = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
		}
		else if (InNewInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Valuable"))))
		{
			NewItemTypeTag = FGameplayTag::RequestGameplayTag(TEXT("Material.Valuable"));
			InventoryItem = GetInventoryItemButtonCanUseOrdered(ValuableItemButtonPool);
			
			FInventoryItem_Valuable ValuableItemInfo = InventoryItemInfo->FindItemInfoByTag_Valuable(InNewInventoryItemTag);
			NewItemIcon = ValuableItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
		}
		else if (InNewInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Task"))))
		{
			NewItemTypeTag = FGameplayTag::RequestGameplayTag(TEXT("Material.Task"));
			InventoryItem = GetInventoryItemButtonCanUseOrdered(TaskItemButtonPool);

			FInventoryItem_Task TaskItemInfo = InventoryItemInfo->FindItemInfoByTag_Task(InNewInventoryItemTag);
			NewItemIcon = TaskItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
		}

		if (IsValid(InventoryItem))
		{
			// 启用
			InventoryItem->EnableItem(this, InNewInventoryItemTag, NewItemIcon, InInventoryItemNum);
			// 用于将其添加到视口上
			OnNewInventoryItemAddDelegate.Broadcast(NewItemTypeTag, InventoryItem);
			// 更新数据到UI上
			InventoryItem->UpdateInfoToWidget();
		}
	}
}

void UInventoryWidgetController::UpdateInventoryItemNum(FGameplayTag InNewInventoryItemTag, int32 InInventoryItemNum)
{
	UInventoryItemButtonBase* InventoryItem = GetInventoryItemButtonByTag(InNewInventoryItemTag);

	if (IsValid(InventoryItem))
	{
		InventoryItem->UpdateItemNum(InInventoryItemNum);
	}

}

void UInventoryWidgetController::RemoveInventoryItem(FGameplayTag InInventoryItem)
{
	UInventoryItemButtonBase* InventoryItemToRemove = nullptr;
	if (SelectedInventoryItem.IsValid() && InInventoryItem.MatchesTagExact(SelectedInventoryItem.Get()->GetItemTag()))
	{
		// 取消选中
		SelectedInventoryItem->HandleItemUnselected();

		// 更新描述信息
		OnUpdateDescInfoDelegate.Broadcast(false, nullptr, TEXT(""), TEXT(""), TEXT(""));

		InventoryItemToRemove = SelectedInventoryItem.Get();
	}
	else
	{
		InventoryItemToRemove = GetInventoryItemButtonByTag(InInventoryItem);
	}

	if (IsValid(InventoryItemToRemove))
	{
		// 从背包中移除
		InventoryItemToRemove->RemoveFromParent();

		// 回收
		InventoryItemToRemove->RecylingItem();
	}
	
}

void UInventoryWidgetController::EnableObatinItemsWidget(FInventoryItem InItemInfo, UVerticalBox* LootUIBox)
{
	// 首先找到不在使用的ObatinItemsWidget
	UObtainItemsWidgetBase* ObtainItemWidget = GetObatinItemsWidgetCanUse();
	if (IsValid(ObtainItemWidget))
	{
		
		// 拿到GameState
		AEnhoneyGameStateBase* EnhoneyGameState = GetWorld()->GetGameState<AEnhoneyGameStateBase>();
		check(EnhoneyGameState);

		// 找到物品配置信息
		UInventoryItemInfo* InventoryItemInfo = EnhoneyGameState->GetInventoryItemConfigInfo();
		// 找到物品图标
		UTexture2D* ItemIcon = nullptr;
		FName ItemName = TEXT("");
		if (IsValid(InventoryItemInfo))
		{
			// 先找到可用的InvenytoryItemButton，以及配置信息--主要是图标
			if (InItemInfo.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Food"))))
			{

				FInventoryItem_Food FoodItemInfo = InventoryItemInfo->FindItemInfoByTag_Food(InItemInfo.ItemTag);
				ItemIcon = FoodItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
				ItemName = FName(FoodItemInfo.InventoryItemBaseInfo.InventoryItemName);
			}
			else if (InItemInfo.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"))))
			{
				FInventoryItem_Economic EconomicItemInfo = InventoryItemInfo->FindItemInfoByTag_Economic(InItemInfo.ItemTag);
				ItemIcon = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
				ItemName = FName(EconomicItemInfo.InventoryItemBaseInfo.InventoryItemName);
			}
			else if (InItemInfo.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Valuable"))))
			{
				FInventoryItem_Valuable ValuableItemInfo = InventoryItemInfo->FindItemInfoByTag_Valuable(InItemInfo.ItemTag);
				ItemIcon = ValuableItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
				ItemName = FName(ValuableItemInfo.InventoryItemBaseInfo.InventoryItemName);
			}
			else if (InItemInfo.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Task"))))
			{
				FInventoryItem_Task TaskItemInfo = InventoryItemInfo->FindItemInfoByTag_Task(InItemInfo.ItemTag);
				ItemIcon = TaskItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
				ItemName = FName(TaskItemInfo.InventoryItemBaseInfo.InventoryItemName);
			}
		}

		if (IsValid(ItemIcon))
		{
			// 启用
			ObtainItemWidget->EnableItem(ItemName, ItemIcon, InItemInfo.ItemNum);

			// 添加到视口，更新数据，并播放动画，用定时器回收
			ObtainItemWidget->UpdateInfoToWidget(LootUIBox);
		}
		else
		{
			UE_LOG(LogEnhoney, Warning, TEXT("EnableObatinItemsWidget: Cannot find ItemIcon for ItemTag %s"), *InItemInfo.ItemTag.GetTagName().ToString());
		}
	}
	else
	{
		UE_LOG(LogEnhoney, Warning, TEXT("EnableObatinItemsWidget: No available ObatinItemsWidget in pool!"));
	}
}

UInventoryItemButtonBase* UInventoryWidgetController::GetInventoryItemButtonByTag(FGameplayTag InInventoryItemTag)
{
	if (InInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Food"))))
	{
		return GetInventoryItemButtonOrdered(InInventoryItemTag, FoodItemButtonPool);
	}
	else if (InInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"))))
	{
		return GetInventoryItemButtonOrdered(InInventoryItemTag, EconomicItemButtonPool);
	}
	else if (InInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Valuable"))))
	{

		return GetInventoryItemButtonOrdered(InInventoryItemTag, ValuableItemButtonPool);
	}
	else if (InInventoryItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Task"))))
	{
		return GetInventoryItemButtonOrdered(InInventoryItemTag, TaskItemButtonPool);
	}
	else
	{
		checkf(false, TEXT("InventoryError: ItemTag is Not Valid!!!"));
		return nullptr;
	}

}

UInventoryItemButtonBase* UInventoryWidgetController::GetInventoryItemButtonOrdered(FGameplayTag InInventoryItemTag, TArray<TObjectPtr<UInventoryItemButtonBase>> OrderedPool)
{
	for (UInventoryItemButtonBase* InventoryItem : OrderedPool)
	{
		if (InventoryItem->IsItemButtonIsUse() && InventoryItem->GetItemTag().MatchesTagExact(InInventoryItemTag))
		{
			return InventoryItem;
		}
	}

	return nullptr;
}

UInventoryItemButtonBase* UInventoryWidgetController::GetInventoryItemButtonCanUseOrdered(TArray<TObjectPtr<UInventoryItemButtonBase>> OrderedPool)
{
	for (UInventoryItemButtonBase* InventoryItem : OrderedPool)
	{
		if (!InventoryItem->IsItemButtonIsUse())
		{
			return InventoryItem;
		}
	}
	return nullptr;
}

UObtainItemsWidgetBase* UInventoryWidgetController::GetObatinItemsWidgetCanUse()
{
	for(UObtainItemsWidgetBase* ObtainItemWidget : ObatinItemsWidgetPool)
	{
		if (!ObtainItemWidget->IsItemWidgetInUse())
		{
			return ObtainItemWidget;
		}
	}
	return nullptr;
}
