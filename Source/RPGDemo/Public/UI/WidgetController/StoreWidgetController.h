// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/EnhoneyWidgetControllerBase.h"
#include "GameplayTagContainer.h"
#include "StoreWidgetController.generated.h"

class APropMerchantCharacter;
class UEnhoneyUserWidgetBase;
class UInventoryItemButtonBase;

/** Ҫ�󶨵�UI�ϵ�*/
// ������Ʒ������Ϣ--ѡ�л���ȡ��ĳ��Itemʱ�������Ҳ�������Ϣ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FUpdateCommodityDescInfoSignature, bool, bSelected, UTexture2D*, ItemIcon, FString, ItemName, FString, ItemDescInfo, FString, ItemExtralInfo);
// ���ﵯ��
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FShoppingPopupSignature, bool, bPopUp, int32, PopUpIndex, FGameplayTag, CommondityTag, int32, NewMaxPurchaseNum);
// ������Ʒ��ť��ѡ��״̬
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSwitchCommonditySelected, const FGameplayTag&, InSelectedItemTag);
// ���½������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerCoinNumChangedSignature, int64, NewCoinNum);
// �����̵���Ʒ����
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShopItemNumChangedSignature, const FGameplayTag&, InItemTag, int32, NewItemNum);

// �����������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPurchaseOrSaleSwitchSignature, int32, SwitcherIndex);
// ����SaleItem
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNewSaleItemAddSignature, const FGameplayTag&, ItemTypeTag, UInventoryItemButtonBase*, SaleItemButton);
// ��������
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSalePopupSignature, bool, bPopUp, FGameplayTag, SaleItemTag, int32, NewMaxSaleNum);


/**
 * 
 */
UCLASS(Blueprintable)
class RPGDEMO_API UStoreWidgetController : public UEnhoneyWidgetControllerBase
{
	GENERATED_BODY()

public:
	// ��ʼ��������Ҫ�������ҵ���ͼ�ϵ�����
	void Initialize();

	// �㲥��ʼֵ
	virtual void BroadcastInitialValue() override;

	// ����
	virtual void BindCallbacksToDependiencies() override;

	// ���̵��UIҳ��
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void OpenStoreUI();

	// ͨ��UI�����ť���ã��˳��̵꣬��ʱ���ǽ����Ҫ�Ĵ���
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void QuitStore();

	// ѡ����Ʒ--�鿴��Ϣ����ʾ�߿�
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void SelectCommodity(const FGameplayTag& InCommodityTag);

	// ˫��ʱ��Ԥ����--������Ʒ����
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void PrePurchase(const FGameplayTag& InCommodityTag);

	// ˫����������Ʒʱ--Ԥ�۵���
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void PreSell(const FGameplayTag& InSaleItemTag);

	// ������Ʒ
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void PurchaseCommodity(const FGameplayTag& InCommodityTag, int32 InCommodityNum);

	// ������Ʒ
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void SellItem(const FGameplayTag& InSaleTag, int32 InSaleItemNum);

	// �رչ��򵯴�
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void ColsePopupWindow();

	// ������߳���
	UFUNCTION(BlueprintCallable, Category = "StoreWidgetController")
	void SwitchStorePurchaseOrSale(int32 SwitcherIndex);

	// ����һ���µ�SaleItemButton
	UFUNCTION()
	void EnableNewSaleItem(FGameplayTag InNewSaleItemTag, int32 InSaleItemNum);

	// ���¶�Ӧ��Ʒ��������UI
	UFUNCTION()
	void UpdateSaleItemNum(FGameplayTag InNewSaleItemTag, int32 InSaleItemNum);

	// �Ƴ�ĳ����������Ʒ--�����������Ʒ������
	UFUNCTION(BlueprintCallable)
	void RemoveSaleItem(FGameplayTag InSaleItem);

	// ѡ��ĳ��SaleItemButton
	UFUNCTION(BlueprintCallable)
	void HandleSaleItemSelected(UInventoryItemButtonBase* InSaleItemSelected);
	// ȡ��ĳ��SaleItemButton��ѡ��
	UFUNCTION(BlueprintCallable)
	void HandleSaleItemUnselected(UInventoryItemButtonBase* InSaleItemUnselected);

private:
	// �����Ѿ���ʹ�õĺ�SaleItemTag��Ӧ��ItemButton
	UInventoryItemButtonBase* GetSaleItemButtonInUseByTag(FGameplayTag InSaleItemTag);

	// ����δʹ�õ�SaltItemButton
	UInventoryItemButtonBase* GetSaleItemButtonCanUse();

	// ����������Ʒ��ť����
	void RecylingAllSaleButton();

public:
	// ������Ʒ������Ϣ�Ĵ���
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FUpdateCommodityDescInfoSignature OnUpdateCommondDescInfoDelegate;

	// ���ﵯ��
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FShoppingPopupSignature OnShoppingPopupDelegate;

	// ������Ʒѡ��߿�
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FSwitchCommonditySelected OnSwitchCommonditySelectedDelegate;

	// ���½������
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FPlayerCoinNumChangedSignature OnPlayerCoinNumChangedDelegate;

	// �����̵굥����Ʒ����
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FShopItemNumChangedSignature OnShopItemNumChangedDelegate;

	// ������������Ʒ����
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FShopItemNumChangedSignature OnSaleItemNumChangedDelegate;

	// �л�������߳���
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FPurchaseOrSaleSwitchSignature OnPurchaseOrSaleSwitchDelegate;

	// ���ӿ�������Ʒ
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FNewSaleItemAddSignature OnNewSaleItemAddDelegate;

	// Ԥ�۵���
	UPROPERTY(BlueprintAssignable, Category = "StoreWidgetController")
	FSalePopupSignature OnSalePopupDelegate;

	// ��ǰѡ��Ҫ��������Ʒ��SaleItem
	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<UInventoryItemButtonBase> SelectedItemToSale;


private:
	// ��ͼ�ϵ�����
	UPROPERTY()
	TWeakObjectPtr<APropMerchantCharacter> PropMerchant;

	// �̵�ҳ��UI��
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UEnhoneyUserWidgetBase> StoreUIPageClass;

	// �̵�ҳ��UI����
	UPROPERTY()
	TObjectPtr<UEnhoneyUserWidgetBase> StoreUI;

	// SaleItemButtonWidgetClass
	UPROPERTY(EditDefaultsOnly, Category = "SaleItemObjectPool")
	TSubclassOf<UInventoryItemButtonBase> SaleItemButtonWidgetClass;
	// SaleItemButtonWidget ObjectPool
	UPROPERTY()
	TArray<TObjectPtr<UInventoryItemButtonBase>> SaleItemButtonPool;
	// ����ش�С
	UPROPERTY(EditDefaultsOnly)
	int32 SaleObjectPoolSize = 20;
};
