// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyUserWidgetBase.h"
#include "WeaponUpgradeMatListWidget.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPGDEMO_API UWeaponUpgradeMatListWidget : public UEnhoneyUserWidgetBase
{
	GENERATED_BODY()

public:
	// �Ƿ�����ʹ��
	bool IsInUse() const { return bIsInUse; }

	// ����--��������
	void Enable(UTexture2D* InMatIcon, const FString& InMatName, int32 InCurrentMatCount, int32 InNeedMatCount);

	// ����
	void Recycle();

	// �������ݵ�UI--��ͼʵ�֣���ͼ���Ե��ã���ʾ֮�����
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UWeaponUpgradeMatListWidget")
	void UpdateDataToUI();


protected:
	// �Ƿ�����ʹ��
	UPROPERTY()
	bool bIsInUse = false;

	// ��Ʒͼ��
	UPROPERTY(BlueprintReadOnly, Category = "UWeaponUpgradeMatListWidget|MatIcon", meta = (DisplayName = "WeaponUpgradeMatIcon"))
	TObjectPtr<UTexture2D> MatIcon = nullptr;

	// ��Ʒ����
	UPROPERTY(BlueprintReadOnly, Category = "UWeaponUpgradeMatListWidget|MatIcon", meta = (DisplayName = "WeaponUpgradeMatName"))
	FString MatName = TEXT("");

	// ��ǰ��Ʒ����
	UPROPERTY(BlueprintReadOnly, Category = "UWeaponUpgradeMatListWidget|MatIcon", meta = (DisplayName = "CurrentMatCount"))
	int32 CurrentMatCount = 0;

	// ��Ҫ����Ʒ����
	UPROPERTY(BlueprintReadOnly, Category = "UWeaponUpgradeMatListWidget|MatIcon", meta = (DisplayName = "NeedMatCount"))
	int32 NeedMatCount = 0;
	
};
