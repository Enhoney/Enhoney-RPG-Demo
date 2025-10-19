// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UI/UserWidget/EnhoneyUserWidgetBase.h"
#include "GameplayTagContainer.h"
#include "ShoppingWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UShoppingWidgetBase : public UEnhoneyUserWidgetBase
{
	GENERATED_BODY()


public:
	/** ��Щ������������Ҫ����ӵ��ӿ�֮����õģ���Ҫ����ͼ���и������ݵ�UI*/

	// ����--����Ǯ����
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void PopWindow_WarningForLackOfMoney();

	// ����--������Ʒ����
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void PopWindow_WaringForSoldOut();

	// ����--ѡ�����������������������������
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void PopWindow_SelectShopNum(const FGameplayTag& InItemTag, int32 InMaxShopNum);
	// ������ѡ���������ĵ���--���ӹ�������
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void AddShoppingNum();
	// ������ѡ���������ĵ���--���ٹ�������
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void ReduceShoppingNum();

	// ����--ѡ���������������������������������
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void PopWindow_SelectSaleNum(const FGameplayTag& InItemTag, int32 InMaxSaleNum);
	// ������ѡ�����������ĵ���--������������
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void AddSaleNum();
	// ������ѡ�����������ĵ���--������������
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Shopping Pop-Up Window")
	void ReduceSaleNum();

protected:
	// ���������
	UPROPERTY(BlueprintReadOnly)
	int32 ShoppingMaxNum = 0;
	// ��ǰ��������
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentShoppingNum = 1;
	// ��ǰѡ��Ҫ�������Ʒ
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemToShop = FGameplayTag();

	// �����������
	UPROPERTY(BlueprintReadOnly)
	int32 SaleMaxNum = 0;
	// ��ǰ��������
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentSaleNum = 1;
	// ��ǰѡ��Ҫ���۵���Ʒ
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag ItemToSale = FGameplayTag();

	
};
