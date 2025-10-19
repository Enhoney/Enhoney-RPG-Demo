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
	// 是否正在使用
	bool IsInUse() const { return bIsInUse; }

	// 启用--设置数据
	void Enable(UTexture2D* InMatIcon, const FString& InMatName, int32 InCurrentMatCount, int32 InNeedMatCount);

	// 回收
	void Recycle();

	// 更新数据到UI--蓝图实现，蓝图可以调用，显示之后调用
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "UWeaponUpgradeMatListWidget")
	void UpdateDataToUI();


protected:
	// 是否正在使用
	UPROPERTY()
	bool bIsInUse = false;

	// 物品图标
	UPROPERTY(BlueprintReadOnly, Category = "UWeaponUpgradeMatListWidget|MatIcon", meta = (DisplayName = "WeaponUpgradeMatIcon"))
	TObjectPtr<UTexture2D> MatIcon = nullptr;

	// 物品名字
	UPROPERTY(BlueprintReadOnly, Category = "UWeaponUpgradeMatListWidget|MatIcon", meta = (DisplayName = "WeaponUpgradeMatName"))
	FString MatName = TEXT("");

	// 当前物品数量
	UPROPERTY(BlueprintReadOnly, Category = "UWeaponUpgradeMatListWidget|MatIcon", meta = (DisplayName = "CurrentMatCount"))
	int32 CurrentMatCount = 0;

	// 需要的物品数量
	UPROPERTY(BlueprintReadOnly, Category = "UWeaponUpgradeMatListWidget|MatIcon", meta = (DisplayName = "NeedMatCount"))
	int32 NeedMatCount = 0;
	
};
