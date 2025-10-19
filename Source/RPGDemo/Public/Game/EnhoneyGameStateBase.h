// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "EnhoneyGameplayTags.h"
#include "InventoryComponent.h"
#include "EnhoneyGameStateBase.generated.h"

class UInventoryItemInfo;
class UTaskSystemComponent;

struct FInventoryItem_Food;

DECLARE_DELEGATE_OneParam(FPropMerchantInventoryChangedSignature, const FInventoryItemArray&);

/**
 * 
 */
UCLASS()
class RPGDEMO_API AEnhoneyGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	AEnhoneyGameStateBase();

	/** �̳�ϵͳ*/
#pragma region Store

	UFUNCTION(BlueprintCallable)
	UInventoryItemInfo* GetInventoryItemConfigInfo() const;

	// ����ĳ����Ʒ
	UFUNCTION(BlueprintCallable, Category = "Merchant")
	void BuyItem(FInventoryItem PurchasedItem);

	// ��ȡ������Ʒ�ļ۸�--ͨ��GameState�е�������Ϣ����ȡ
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Merchant")
	int64 GetItemPrice(const FGameplayTag& ItemTag) const;

	// ��ѯ������Ʒ����
	UFUNCTION(BlueprintCallable, Category = "Merchant")
	int32 QueryFoodItemNum(const FGameplayTag& InFoodItemTag) const;

	// ��ѯ���������
	UFUNCTION(BlueprintCallable, Category = "Merchant")
	int32 QueryMaxNumPurchase(const FGameplayTag& InFoodItemTag, int64 InPlayerCoinNum) const;

	// ���Ը���
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ��ȡ���
	FORCEINLINE const FInventoryItemArray& GetInventoryItems() const
	{
		return MerchantInventoryItems;
	}

	// ��ʼ�����
	void InitInventory();

	// ���ý��׶���
	UFUNCTION(BlueprintCallable, Category = "Merchant")
	void SetTransactionPlayer(AActor* InPlayerCharacter);

protected:
	virtual void BeginPlay() override;

	// �㲥���п��
	UFUNCTION()
	void OnRep_PropMerchantInventory(const FInventoryItemArray& OldInventory);

public:
	FPropMerchantInventoryChangedSignature InventoryChangedDelegate;

protected:
	// ���ڽ��׵���ҽ�ɫ
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> TransactionPlayer;

private:
	
	// ����NPC����Ʒ�б�����ø��ƣ���Ϊ�ͻ���Ҫ��ʾ
	UPROPERTY(ReplicatedUsing = OnRep_PropMerchantInventory)
	FInventoryItemArray MerchantInventoryItems;

#pragma endregion Store

	/** ����ϵͳ*/
#pragma region Task
public:
	// ��ȡ����ϵͳ���
	FORCEINLINE UTaskSystemComponent* GetTaskSystemComponent()
	{
		return TaskSystemComponent;
	}
	

private:
	// ����ϵͳ���
	UPROPERTY(EditDefaultsOnly, Category = "TaskSystem")
	TObjectPtr<UTaskSystemComponent> TaskSystemComponent;

#pragma endregion Task

private:
	// InventoryItemInfo
	UPROPERTY(EditDefaultsOnly, Category = "InventoryItemInfo")
	TObjectPtr<UInventoryItemInfo> InventoryItemInfo;

	// �ٽ���
	FCriticalSection CriticalSection;

	
};
