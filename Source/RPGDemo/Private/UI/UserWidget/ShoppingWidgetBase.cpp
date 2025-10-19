// Copyright Enhoney.


#include "ShoppingWidgetBase.h"

void UShoppingWidgetBase::PopWindow_SelectShopNum_Implementation(const FGameplayTag& InItemTag, int32 InMaxShopNum)
{
	ItemToShop = InItemTag;
	ShoppingMaxNum = InMaxShopNum;
}

void UShoppingWidgetBase::AddShoppingNum_Implementation()
{
	// �ܹ�����������ܸ������������
	CurrentShoppingNum = FMath::Min(CurrentShoppingNum + 1, ShoppingMaxNum);
}

void UShoppingWidgetBase::ReduceShoppingNum_Implementation()
{
	// ��С������������С��1
	CurrentShoppingNum = FMath::Max(1, CurrentShoppingNum - 1);
}

void UShoppingWidgetBase::PopWindow_SelectSaleNum_Implementation(const FGameplayTag& InItemTag, int32 InMaxShopNum)
{
	ItemToSale = InItemTag;
	SaleMaxNum = InMaxShopNum;
}

void UShoppingWidgetBase::AddSaleNum_Implementation()
{
	CurrentSaleNum = FMath::Min(CurrentSaleNum + 1, SaleMaxNum);
}

void UShoppingWidgetBase::ReduceSaleNum_Implementation()
{
	CurrentSaleNum = FMath::Max(1, CurrentSaleNum - 1);
}
