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

		// �����̵�ҳ���UI
		StoreUI = CreateWidget<UEnhoneyUserWidgetBase>(GetWorld(), StoreUIPageClass);

		// ��ʼ��������ƷUI�����
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
	// �������˿��
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
		// ���½��
		OnPlayerCoinNumChangedDelegate.Broadcast(PlayerState->GetCoinNum());

		// ���¿���е����пɽ�����
		PlayerState->GetInventoryComponent()->BroadcastCurrentInventory();
	}

	

	// �մ򿪵�ʱ�����ԭ����������Ϣ
	OnUpdateCommondDescInfoDelegate.Broadcast(false, nullptr, TEXT(""), TEXT(""), TEXT(""));
	// �մ򿪵�ʱ��Ĭ��Ϊ����
	OnPurchaseOrSaleSwitchDelegate.Broadcast(0);
}

void UStoreWidgetController::BindCallbacksToDependiencies()
{
	// �������˿��
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

	// ���½��
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
	// ���̵�ҳ����ӵ��ӿ�
	StoreUI->AddToViewport();

	StoreUI->SetWidgetController(this);
}

void UStoreWidgetController::QuitStore()
{
	if (PropMerchant.IsValid())
	{
		// �˳�����״̬
		PropMerchant->ExitInteractState();

		// �����Щ�󶨵�UI�ϵĴ���
		OnUpdateCommondDescInfoDelegate.Clear();
		OnShoppingPopupDelegate.Clear();
		OnSwitchCommonditySelectedDelegate.Clear();
		OnPlayerCoinNumChangedDelegate.Clear();
		OnShopItemNumChangedDelegate.Clear();
		OnPurchaseOrSaleSwitchDelegate.Clear();
		OnNewSaleItemAddDelegate.Clear();
		OnSalePopupDelegate.Clear();
		OnSaleItemNumChangedDelegate.Clear();

		// ����������Ʒ��ť
		RecylingAllSaleButton();

		// �Ƴ��̵�UI
		StoreUI->RemoveFromParent();
		StoreUI->WidgetController = nullptr;

	}
}

void UStoreWidgetController::SelectCommodity(const FGameplayTag& InCommodityTag)
{
	// ��ʾ��ϸ��Ϣ
	FInventoryItem_Food ItemDescInfo;
	UEnhoneyAbilitySystemLibrary::GetInventoryItemConfig_Food(PropMerchant.Get(), InCommodityTag, ItemDescInfo);
	OnUpdateCommondDescInfoDelegate.Broadcast(true,
		ItemDescInfo.InventoryItemBaseInfo.InventoryItemIcon,
		ItemDescInfo.InventoryItemBaseInfo.InventoryItemName,
		ItemDescInfo.InventoryItemBaseInfo.InventoryItemDesc,
		FString::Printf(TEXT("%d"), ItemDescInfo.InventoryItemBaseInfo.PurchasePrice));


	// ��Ʒ�߿���ʾ���л�
	OnSwitchCommonditySelectedDelegate.Broadcast(InCommodityTag);
}

void UStoreWidgetController::PrePurchase(const FGameplayTag& InCommodityTag)
{
	if (PropMerchant.IsValid())
	{
		if (AEnhoneyGameStateBase* GameState = UEnhoneyAbilitySystemLibrary::GetEnhoneyGameState(PropMerchant.Get()))
		{
			/** �ȿ�������û�п��*/
			int32 CommodityInventoryNum = GameState->QueryFoodItemNum(InCommodityTag);
			if (CommodityInventoryNum == 0)
			{
				// ����
				OnShoppingPopupDelegate.Broadcast(true, 0, InCommodityTag, 0);
			}
			else
			{
				/** Ȼ�󿴿�Ǯ��������һ��*/
				// �ҵ����Ŀɹ�������
				int32 MaxPurchaseNum = GameState->QueryMaxNumPurchase(InCommodityTag, PlayerState->GetCoinNum());
				if (MaxPurchaseNum == 0)
				{
					// һ��������
					OnShoppingPopupDelegate.Broadcast(true, 1, InCommodityTag, 0);
				}
				else
				{
					// ѡ��������
					OnShoppingPopupDelegate.Broadcast(true, 2, InCommodityTag, MaxPurchaseNum);
				}
			}
		}
	}
	
	
}

void UStoreWidgetController::PreSell(const FGameplayTag& InSaleItemTag)
{
	// ����Ԥ�۵ĵ���
	OnSalePopupDelegate.Broadcast(true, InSaleItemTag, PlayerState->GetInventoryComponent()->GetItemNum(InSaleItemTag));
}

void UStoreWidgetController::PurchaseCommodity(const FGameplayTag& InCommodityTag, int32 InCommodityNum)
{
	if (PropMerchant.IsValid())
	{
		if (AEnhoneyGameStateBase* GameState = UEnhoneyAbilitySystemLibrary::GetEnhoneyGameState(PropMerchant.Get()))
		{
			// ������Ʒ
			PlayerController->ServerBuyItem(InCommodityTag, InCommodityNum);

			// �رյ���
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
			// �۳������е���Ʒ
			PlayerState->GetInventoryComponent()->ServerConsumeItemByTag(FInventoryItem(SelectedItemToSale->GetItemTag(), InSaleItemNum));
		}

		// ��Ǯ
		// ���ҵ�����
		UInventoryItemInfo* InventoryItemInfo = UEnhoneyAbilitySystemLibrary::GetInventoryItemInfoDataAsset(PlayerController->GetPawn());
		check(InventoryItemInfo);
		FInventoryItem_Economic EconomicItemInfo = InventoryItemInfo->FindItemInfoByTag_Economic(SelectedItemToSale->GetItemTag());
		PlayerState->ServerAddCoinNum(EconomicItemInfo.SellingPrice * InSaleItemNum);

		// ȡ��ѡ��
		SelectedItemToSale->HandleItemUnselected();

		// ���ص���
		ColsePopupWindow();
	}
}

void UStoreWidgetController::ColsePopupWindow()
{
	// �رյ���
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
	// ͼ��
	UTexture2D* NewSaleItemIcon = nullptr;

	// �ҵ���Ʒ������Ϣ
	UInventoryItemInfo* InventoryItemInfo = UEnhoneyAbilitySystemLibrary::GetInventoryItemInfoDataAsset(PlayerController->GetPawn());
	check(InventoryItemInfo);

	// ���ҵ����õ�InvenytoryItemButton���Լ�������Ϣ--��Ҫ��ͼ��
	SaleItem = GetSaleItemButtonCanUse();

	FInventoryItem_Economic EconomicItemInfo = InventoryItemInfo->FindItemInfoByTag_Economic(InNewSaleItemTag);
	NewSaleItemIcon = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemIcon;

	if (IsValid(SaleItem))
	{
		// ����
		SaleItem->EnableItem(this, InNewSaleItemTag, NewSaleItemIcon, InSaleItemNum);
		// ���ڽ�����ӵ��ӿ���
		OnNewSaleItemAddDelegate.Broadcast(InNewSaleItemTag, SaleItem);
		// �������ݵ�UI��
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
		// ȡ��ѡ��
		SelectedItemToSale->HandleItemUnselected();

		// ����������Ϣ
		OnUpdateCommondDescInfoDelegate.Broadcast(false, nullptr, TEXT(""), TEXT(""), TEXT(""));

		SaleItemToRemove = SelectedItemToSale.Get();
	}
	else
	{
		SaleItemToRemove = GetSaleItemButtonInUseByTag(InSaleItem);
	}

	if (IsValid(SaleItemToRemove))
	{
		// �ӱ������Ƴ�
		SaleItemToRemove->RemoveFromParent();

		// ����
		SaleItemToRemove->RecylingItem();
	}
}

void UStoreWidgetController::HandleSaleItemSelected(UInventoryItemButtonBase* InSaleItemSelected)
{
	// ��ȡ��ԭ��InventoryItem��ѡ��
	if (SelectedItemToSale.IsValid())
	{
		SelectedItemToSale->HandleItemUnselected();
	}

	// ����ѡ�е�InventoryItem
	SelectedItemToSale = InSaleItemSelected;
	SelectedItemToSale->HandleItemSelected();

	// ��ȡ�µ���Ϣ
	FGameplayTag SelectedSaleItemTag = SelectedItemToSale->GetItemTag();
	// ͼ��
	UTexture2D* SelectedSaleItemIcon = nullptr;
	// ����
	FString SelectedSaleItemName = TEXT("");
	// ������Ϣ
	FString SelectedSaleItemDescInfo = TEXT("");
	// �����۸�
	FString SelectedSaleItemPrice = TEXT("");
	if (GetWorld())
	{

		// �ҵ���Ʒ������Ϣ
		UInventoryItemInfo* InventoryItemInfo = UEnhoneyAbilitySystemLibrary::GetInventoryItemInfoDataAsset(PlayerController->GetPawn());
		check(InventoryItemInfo);

		// ���ҵ����õ�InvenytoryItemButton���Լ�������Ϣ--��Ҫ��ͼ��
		if (SelectedSaleItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"))))
		{

			FInventoryItem_Economic EconomicItemInfo = InventoryItemInfo->FindItemInfoByTag_Economic(SelectedSaleItemTag);
			SelectedSaleItemIcon = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemIcon;
			SelectedSaleItemName = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemName;
			SelectedSaleItemDescInfo = EconomicItemInfo.InventoryItemBaseInfo.InventoryItemDesc;
			SelectedSaleItemPrice = FString::Printf(TEXT("%d"), EconomicItemInfo.SellingPrice);

			// ������Ϣ
			OnUpdateCommondDescInfoDelegate.Broadcast(true, SelectedSaleItemIcon, SelectedSaleItemName, SelectedSaleItemDescInfo, SelectedSaleItemPrice);
		}
	}
}

void UStoreWidgetController::HandleSaleItemUnselected(UInventoryItemButtonBase * InSaleItemUnselected)
{
	if (SelectedItemToSale->GetItemTag().MatchesTagExact(InSaleItemUnselected->GetItemTag()))
	{
		// ȡ��ѡ��
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
