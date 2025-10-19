// Copyright Enhoney.


#include "ObtainItemsWidgetBase.h"
#include "InventoryWidgetController.h"

void UObtainItemsWidgetBase::EnableItem(FName InItemName, UTexture2D* InItemIcon, int32 InItemNum)
{
	bInUse = true;

	ItemName = InItemName;
	ItemIcon = InItemIcon;
	ItemNum = InItemNum;
}

void UObtainItemsWidgetBase::RecylingItem()
{
	ItemName = TEXT("");
	ItemIcon = nullptr;
	ItemNum = 0;

	bInUse = false;
	// ���ӿ����Ƴ�����������Ȼ���ڴ���
	RemoveFromParent();
}