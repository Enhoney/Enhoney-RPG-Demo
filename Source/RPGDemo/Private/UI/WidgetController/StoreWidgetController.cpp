// Copyright Enhoney.


#include "StoreWidgetController.h"
#include "Kismet/GameplayStatics.h"
#include "PropMerchantCharacter.h"
#include "EnhoneyPlayerController.h"
#include "EnhoneyUserWidgetBase.h"
#include "InventoryComponent.h"
#include "InventoryItemInfo.h"
#include "EnhoneyPlayerState.h"
#include "EnhoneyGameStateBase.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "PlayerInterface.h"
#include "InventoryItemButtonBase.h"

void UStoreWidgetController::Initialize()
{
	AActor* PropMerchantFound = UGameplayStatics::GetActorOfClass(PlayerController, APropMerchantCharacter::StaticClass());
	if (IsValid(PropMerchantFound))
	{
		PropMerchant = Cast<APropMerchantCharacter>(PropMerchantFound);

		// 创建商店页面的UI
		StoreUI = CreateWidget<UEnhoneyUserWidgetBase>(GetWorld(), StoreUIPageClass);

		// 初始化售卖物品UI对象池
		SaleItemButtonPool.Reserve(SaleObjectPoolSize);
		for (int32 Index = 0; Index < SaleObjectPoolSize; ++Index)
		{
			UInventoryItemButtonBase* SaleItemButton =
				CreateWidget<UInventoryItemButtonBase>(GetWorld(), SaleItemButtonWidgetClass);
			SaleItemButtonPool.Add(SaleItemButton);
		}
	}
}

void UStoreWidgetController::BroadcastInitialValue()
{
	// 更新商人库存
	if (PropMerchant.IsValid())
	{
		if (AEnhoneyGameStateBase* GameState = UEnhoneyAbilitySystemLibrary::GetEnhoneyGameState(PropMerchant.Get()))
		{
			const FInventoryItemArray& PropMerchantInventory = GameState->GetInventoryItems();
			for (const FInventoryItem& PropMerchantItem : PropMerchantInventory.Items)
			{
				OnShopItemNumChangedDelegate.Broadcast(PropMerchantItem.ItemTag, PropMerchantItem.ItemNum);
			}
		}
		
	}

	
	if (IsValid(PlayerState))
	{
		// 更新金币
		OnPlayerCoinNumChangedDelegate.Broadcast(PlayerState->GetCoinNum());

		// 更新库存中的所有可交易物
		PlayerState->GetInventoryComponent()->BroadcastCurrentInventory();
	}

	

	// 刚打开的时候，清空原来的描述信息
	OnUpdateCommondDescInfoDelegate.Broadcast(false, nullptr, TEXT(""), TEXT(""), TEXT(""));
	// 刚打开的时候，默认为购买
	OnPurchaseOrSaleSwitchDelegate.Broadcast(0);
}

void UStoreWidgetController::BindCallbacksToDependiencies()
{
	// 更新商人库存
	if (PropMerchant.IsValid())
	{
		if (AEnhoneyGameStateBase* GameState = UEnhoneyAbilitySystemLibrary::GetEnhoneyGameState(PropMerchant.Get()))
		{
			GameState->InventoryChangedDelegate.BindWeakLambda(this,
				[this](const FInventoryItemArray& PropMerchantInventory)
				{
					for (const FInventoryItem& PropMerchantItem : PropMerchantInventory.Items)
					{
						OnShopItemNumChangedDelegate.Broadcast(PropMerchantItem.ItemTag, PropMerchantItem.ItemNum);
					}
				});
		}
	}

	// 更新金币
	if (IsValid(PlayerState))
	{
		PlayerState->OnCoinNumChanged.AddWeakLambda(this, [this](int64 NewCoinNum)
			{
				OnPlayerCoinNumChangedDelegate.Broadcast(NewCoinNum);
			});

		PlayerState->GetInventoryComponent()->OnNewItemAddDelegate.AddUObject(this, &UStoreWidgetController::EnableNewSaleItem);
		PlayerState->GetInventoryComponent()->OnItemNumChangedDelegate.AddUObject(this, &UStoreWidgetController::UpdateSaleItemNum);
		PlayerState->GetInventoryComponent()->OnItemExhaustedDelegate.AddUObject(this, &UStoreWidgetController::RemoveSaleItem);
	}
}

void UStoreWidgetController::OpenStoreUI()
{
	// 将商店页面添加到视口
	StoreUI->AddToViewport();

	StoreUI->SetWidgetController(this);
}

void UStoreWidgetController::QuitStore()
{
	if (PropMerchant.IsValid())
	{
		// 退出交互状态
		PropMerchant->ExitInteractState();

		// 解绑这些绑定到UI上的代理
		OnUpdateCommondDescInfoDelegate.Clear();
		OnShoppingPopupDelegate.Clear();
		OnSwitchCommonditySelectedDelegate.Clear();
		OnPlayerCoinNumChangedDelegate.Clear();
		OnShopItemNumChangedDelegate.Clear();
		OnPurchaseOrSaleSwitchDelegate.Clear();
		OnNewSaleItemAddDelegate.Clear();
		OnSalePopupDelegate.Clear();
		OnSaleItemNumChangedDelegate.Clear();

		// 回收所有商品按钮
		RecylingAllSaleButton();

		// 移除商店UI
		StoreUI->RemoveFromParent();
		StoreUI->WidgetController = nullptr;

	}
}

void UStoreWidgetController::SelectCommodity(const FGameplayTag& InCommodityTag)
{
	// 显示详细信息
	FInventoryItem_Food ItemDescInfo;
	UEnhoneyAbilitySystemLibrary::GetInventoryItemConfig_Food(PropMerchant.Get(), InCommodityTag, ItemDescInfo);
	OnUpdateCommondDescInfoDelegate.Broadcast(true,
		ItemDescInfo.InventoryItemBaseInfo.InventoryItemIcon,
		ItemDescInfo.InventoryItemBaseInfo.InventoryItemName,
		ItemDescInfo.InventoryItemBaseInfo.InventoryItemDesc,
		FString::Printf(TEXT("%d"), ItemDescInfo.InventoryItemBaseInfo.PurchasePrice));


	// 物品边框显示和切换
	OnSwitchCommonditySelectedDelegate.Broadcast(InCommodityTag);
}

void UStoreWidgetController::PrePurchase(const FGameplayTag& InCommodityTag)
{
	if (PropMerchant.IsValid())
	{
		if (AEnhoneyGameStateBase* GameState = UEnhoneyAbilitySystemLibrary::GetEnhoneyGameState(PropMerchant.Get()))
		{
			/** 先看看还有没有库存*/
			int32 CommodityInventoryNum = GameState->QueryFoodItemNum(InCommodityTag);
			if (CommodityInventoryNum == 0)
			{
				// 售罄
				OnShoppingPopupDelegate.Broadcast(true, 0, InCommodityTag, 0);
			}
			else
			{
				/** 然后看看钱够不够买一个*/
				// 找到最大的可购买数量
				int32 MaxPurchaseNum = GameState->QueryMaxNumPurchase(InCommodityTag, PlayerState->GetCoinNum());
				if (MaxPurchaseNum == 0)
				{
					// 一个都买不起
					OnShoppingPopupDelegate.Broadcast(true, 1, InCommodityTag, 0);
				}
				else
				{
					// 选择购买数量
					OnShoppingPopupDelegate.Broadcast(true, 2, InCommodityTag, MaxPurchaseNum);
				}
			}
		}
	}
	
	
}

void UStoreWidgetController::PreSell(const FGameplayTag& InSaleItemTag)
{
	// 弹出预售的弹窗
	OnSalePopupDelegate.Broadcast(true, InSaleItemTag, PlayerState->GetInventoryComponent()->GetItemNum(InSaleItemTag));
}

void UStoreWidgetController::PurchaseCommodity(const FGameplayTag& InCommodityTag, int32 InCommodityNum)
{
	if (PropMerchant.IsValid())
	{
		if (AEnhoneyGameStateBase* GameState = UEnhoneyAbilitySystemLibrary::GetEnhoneyGameState(PropMerchant.Get()))
		{
			// 购买物品
			PlayerController->ServerBuyItem(InCommodityTag, InCommodityNum);

			// 关闭弹窗
			ColsePopupWindow();
		}
	}
}

void UStoreWidgetController::SellItem(const FGameplayTag& InSaleItemTag, int32 InSaleItemNum)
{
	if (SelectedItemToSale.IsValid() && SelectedItemToSale->GetItemTag().MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"))))
	{
		if (IsValid(PlayerState))
		{
			// 扣除背包中的物品
			PlayerState->GetInventoryComponent()->ServerConsumeItemByTag(FInventoryItem(SelectedItemToSale->GetItemTag(), InSaleItemNum));
		}

		// 给钱
		// 先找到单价
		UInventoryItemInfo* InventoryItemInfo = UEnhoneyAbilitySystemLibrary::GetInventoryItemInfoDataAsset(PlayerController->GetPawn());
		check(InventoryItemInfo);
		FInventoryItem_Economic EconomicItemInfo = InventoryItemInfo->FindItemInfoByTag_Economic(SelectedItemToSale->GetItemTag());
		PlayerState->ServerAddCoinNum(EconomicItemInfo.SellingPrice * InSaleItemNum);

		// 取消选中
		SelectedItemToSale->HandleItemUnselected();

		// 隐藏弹窗
		ColsePopupWindow();
	}
}

void UStoreWidgetController::ColsePopupWindow()
{
	// 关闭弹窗
	OnShoppingPopupDelegate.Broadcast(false, 0, FGameplayTag(), 0);
}

void UStoreWidgetController::SwitchStorePurchaseOrSale(int32 SwitcherIndex)
{
	OnPurchaseOrSaleSwitchDelegate.Broadcast(SwitcherIndex);
}

void UStoreWidgetController::EnableNewSaleItem(FGameplayTag InNewSaleItemTag, int32 InSaleItemNum)
{
	if (!InNewSaleItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic")))) return;

	UInventoryItemButtonBase* SaleItem = nullptr;
	// 图标
	UTexture2D* NewSaleItemIcon = nullptr;

	// 找到物品配置信息
	UInventoryItemInfo* InventoryItemInfo = UEnhoneyAbilitySystemLibrary::GetInventoryItemInfoDataAsset(PlayerController->GetPawn());
	check(InventoryItemInfo);

	// 先找到可用的InvenytoryItemButton，以及配置信息--主要是图标
	SaleItem = GetSaleItemButtonCanUse();

	FInventoryItem_Economic EconomicItemInfo = InventoryItemInfo->FindItemInfoByTag_Economic(InNewSaleItemTag);
	NewSaleItemIcon = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemIcon;

	if (IsValid(SaleItem))
	{
		// 启用
		SaleItem->EnableItem(this, InNewSaleItemTag, NewSaleItemIcon, InSaleItemNum);
		// 用于将其添加到视口上
		OnNewSaleItemAddDelegate.Broadcast(InNewSaleItemTag, SaleItem);
		// 更新数据到UI上
		SaleItem->UpdateInfoToWidget();
	}
}

void UStoreWidgetController::UpdateSaleItemNum(FGameplayTag InNewSaleItemTag, int32 InSaleItemNum)
{
	if (!InNewSaleItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic")))) return;

	UInventoryItemButtonBase* SaleItem = GetSaleItemButtonInUseByTag(InNewSaleItemTag);

	if (IsValid(SaleItem))
	{
		SaleItem->UpdateItemNum(InSaleItemNum);
	}
}

void UStoreWidgetController::RemoveSaleItem(FGameplayTag InSaleItem)
{
	if (!InSaleItem.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic")))) return;

	UInventoryItemButtonBase* SaleItemToRemove = nullptr;
	if (SelectedItemToSale.IsValid() && InSaleItem.MatchesTagExact(SelectedItemToSale.Get()->GetItemTag()))
	{
		// 取消选中
		SelectedItemToSale->HandleItemUnselected();

		// 更新描述信息
		OnUpdateCommondDescInfoDelegate.Broadcast(false, nullptr, TEXT(""), TEXT(""), TEXT(""));

		SaleItemToRemove = SelectedItemToSale.Get();
	}
	else
	{
		SaleItemToRemove = GetSaleItemButtonInUseByTag(InSaleItem);
	}

	if (IsValid(SaleItemToRemove))
	{
		// 从背包中移除
		SaleItemToRemove->RemoveFromParent();

		// 回收
		SaleItemToRemove->RecylingItem();
	}
}

void UStoreWidgetController::HandleSaleItemSelected(UInventoryItemButtonBase* InSaleItemSelected)
{
	// 先取消原来InventoryItem的选中
	if (SelectedItemToSale.IsValid())
	{
		SelectedItemToSale->HandleItemUnselected();
	}

	// 更新选中的InventoryItem
	SelectedItemToSale = InSaleItemSelected;
	SelectedItemToSale->HandleItemSelected();

	// 获取新的信息
	FGameplayTag SelectedSaleItemTag = SelectedItemToSale->GetItemTag();
	// 图标
	UTexture2D* SelectedSaleItemIcon = nullptr;
	// 名称
	FString SelectedSaleItemName = TEXT("");
	// 描述信息
	FString SelectedSaleItemDescInfo = TEXT("");
	// 售卖价格
	FString SelectedSaleItemPrice = TEXT("");
	if (GetWorld())
	{

		// 找到物品配置信息
		UInventoryItemInfo* InventoryItemInfo = UEnhoneyAbilitySystemLibrary::GetInventoryItemInfoDataAsset(PlayerController->GetPawn());
		check(InventoryItemInfo);

		// 先找到可用的InvenytoryItemButton，以及配置信息--主要是图标
		if (SelectedSaleItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"))))
		{

			FInventoryItem_Economic EconomicItemInfo = InventoryItemInfo->FindItemInfoByTag_Economic(SelectedSaleItemTag);
			SelectedSaleItemIcon = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
			SelectedSaleItemName = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemName;
			SelectedSaleItemDescInfo = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemDesc;
			SelectedSaleItemPrice = FString::Printf(TEXT("%d"), EconomicItemInfo.SellingPrice);

			// 更新信息
			OnUpdateCommondDescInfoDelegate.Broadcast(true, SelectedSaleItemIcon, SelectedSaleItemName, SelectedSaleItemDescInfo, SelectedSaleItemPrice);
		}
	}
}

void UStoreWidgetController::HandleSaleItemUnselected(UInventoryItemButtonBase * InSaleItemUnselected)
{
	if (SelectedItemToSale->GetItemTag().MatchesTagExact(InSaleItemUnselected->GetItemTag()))
	{
		// 取消选中
		SelectedItemToSale->HandleItemUnselected();
		SelectedItemToSale = nullptr;

		OnUpdateCommondDescInfoDelegate.Broadcast(false, nullptr, TEXT(""), TEXT(""), TEXT(""));
	}
}


UInventoryItemButtonBase* UStoreWidgetController::GetSaleItemButtonInUseByTag(FGameplayTag InSaleItemTag)
{
	for (UInventoryItemButtonBase* SaleItem : SaleItemButtonPool)
	{
		if (SaleItem->IsItemButtonIsUse() && SaleItem->GetItemTag().MatchesTagExact(InSaleItemTag))
		{
			return SaleItem;
		}
	}

	return nullptr;
}

UInventoryItemButtonBase* UStoreWidgetController::GetSaleItemButtonCanUse()
{
	for (UInventoryItemButtonBase* SaleItem : SaleItemButtonPool)
	{
		if (!SaleItem->IsItemButtonIsUse())
		{
			return SaleItem;
		}
	}
	return nullptr;
}

void UStoreWidgetController::RecylingAllSaleButton()
{
	for (UInventoryItemButtonBase* SaleItemButton : SaleItemButtonPool)
	{
		if (SaleItemButton->IsItemButtonIsUse())
		{
			SaleItemButton->RemoveFromParent();
			SaleItemButton->RecylingItem();
		}
	}
}
