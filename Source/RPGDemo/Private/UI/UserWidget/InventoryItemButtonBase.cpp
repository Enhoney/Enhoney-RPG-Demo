// Copyright Enhoney.


#include "InventoryItemButtonBase.h"

#include "InventoryWidgetController.h"
#include "ObtainItemsWidgetBase.h"

void UInventoryItemButtonBase::EnableItem(UEnhoneyWidgetControllerBase* InInventoryWidgetController, const FGameplayTag& InItemTag, UTexture2D* InItemIcon, int32 InItemNum)
{
	bInUse = true;

	// 设置WidgetController
	SetWidgetController(InInventoryWidgetController);

	InventoryItemTag = InItemTag;
	InventoryItemIcon = InItemIcon;
	InventoryItemNum = InItemNum;
}

void UInventoryItemButtonBase::RecylingItem()
{
	WidgetController = nullptr;

	InventoryItemTag = FGameplayTag();
	InventoryItemIcon = nullptr;
	InventoryItemNum = 0;

	bIsSelected = false;
	bInUse = false;
}

void UInventoryItemButtonBase::UpdateItemNum(int32 InNewItemNum)
{
	InventoryItemNum = InNewItemNum;

	UpdateInfoToWidget();
}

void UInventoryItemButtonBase::HandleItemSelected_Implementation()
{
	bIsSelected = true;
}

void UInventoryItemButtonBase::HandleItemUnselected_Implementation()
{
	bIsSelected = false;
}
