// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyUserWidgetBase.h"
#include "ObtainItemsWidgetBase.generated.h"

class UTexture2D;
class UInventoryWidgetController;
class UVerticalBox;


/**
 * 
 */
UCLASS(BlueprintType)
class RPGDEMO_API UObtainItemsWidgetBase : public UEnhoneyUserWidgetBase
{
	GENERATED_BODY()

public:
	// ����--�����е��ø��µ�UI���Ǹ�����
	UFUNCTION(BlueprintCallable, Category = "ObtainItem")
	void EnableItem(FName InItemName, UTexture2D* InItemIcon, int32 InItemNum);

	// ����
	UFUNCTION(BlueprintCallable, Category = "ObtainItem")
	void RecylingItem();

	// �������ݵ�UI
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "InventoryItem")
	void UpdateInfoToWidget(UVerticalBox* LootUIBox);

	// �Ƿ���ʹ����
	FORCEINLINE bool IsItemWidgetInUse() const
	{
		return bInUse;
	}

protected:
	// ��Ӧ��ItemTag
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|Common")
	FName ItemName = TEXT("");

	// ͼ��
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|Common")
	TObjectPtr<UTexture2D> ItemIcon;

	// ����
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|Common")
	int32 ItemNum = 0;

	// �Ƿ���ʹ��
	UPROPERTY(BlueprintReadWrite, Category = "InventoryItem|ObjectPool")
	bool bInUse = false;
	
};
