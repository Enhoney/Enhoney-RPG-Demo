// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyWidgetControllerBase.h"
#include "EnhoneyAttributeInfo.h"
#include "EnhoneyWeaponConfigInfo.h"
#include "WeaponUpgradeMatListWidget.h"
#include "AttributeMenuWidgetController.generated.h"

class UWeaponUpgradeMatListWidget;
class UEnhoneyWeaponComponent;
class UInventoryComponent;

struct FGameplayAttribute;

struct FInventoryItem_Valuable;
struct FEnhoneyWeaponLevelUpInfo;
struct FEnhoneyWeaponLevelUpMats;

USTRUCT(BlueprintType)
struct RPGDEMO_API FWeaponUpgradeMatListWidgetsInUse
{
	GENERATED_BODY()

public:
	FWeaponUpgradeMatListWidgetsInUse() 
	{
		MatListWidgets.Reserve(5);
	}

	// 新增
	bool Add(UWeaponUpgradeMatListWidget* InWidget)
	{
		if (InWidget->IsInUse())
		{
			MatListWidgets.Add(InWidget);
			return true;
		}
		return false;
	}

	void Clear()
	{
		for (UWeaponUpgradeMatListWidget*& Widget : MatListWidgets)
		{
			Widget->Recycle();
		}
		MatListWidgets.Empty();
	}

	UPROPERTY(BlueprintReadWrite)
	TArray<UWeaponUpgradeMatListWidget*> MatListWidgets;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributePointChangedSignture, int32, NewAttributePoint);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FEnhoneyAttributeInformation&, AttributeInfo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FWeaponChangedSignature, const FGameplayTag&, NewWeaponTag,
	int32, NewWeaponLevel, UTexture2D*, NewWeaponIcon, FEnhoneyWeaponLevelUpInfo, WeaponLevelUpItemRequired);

// 武器预升品弹窗信号
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FWeaponPreUpgradePopWindowSignature, bool, bCanUpgrade, FWeaponUpgradeMatListWidgetsInUse, MatListWidgets, int64, CoinNumNeed, int64, CurrentCoinNum);

// 关闭预升品弹窗信号
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCloseWeaponPreUpgradeWindowSignature);
/**
 * 
 */
UCLASS(Blueprintable)
class RPGDEMO_API UAttributeMenuWidgetController : public UEnhoneyWidgetControllerBase
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValue() override;

	// 监听属性变化
	virtual void BindCallbacksToDependiencies() override;

	// 属性加点
	UFUNCTION(BlueprintCallable)
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	// 更换武器
	UFUNCTION(BlueprintCallable)
	void SwitchEquippedWeapon();

private:
	// 广播单个属性信息
	void BroadcastSingleAttributeInfo(const FGameplayTag& TagToFind, const FGameplayAttribute& Attribute);

public:
	// 属性点变化
	UPROPERTY(BlueprintAssignable)
	FAttributePointChangedSignture OnAttributePointChangedDelegate;

	// 等级变化
	UPROPERTY(BlueprintAssignable)
	FCharacterLevelChangedSignature OnCharacterLevelChangedDelegate;

	// 属性变化--这个可以监听任何属性的变化
	UPROPERTY(BlueprintAssignable)
	FAttributeInfoSignature OnAttributeChangedDelegate;

	// 监听武器变化（切换武器或者武器升级）
	UPROPERTY(BlueprintAssignable)
	FWeaponChangedSignature OnWeaponChangedDelegate;

private:
	// AttributeInfo数据
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UEnhoneyAttributeInfo> EnhoneyAttribyteInfoData;

#pragma region WeaponUpgrade
public:
	// 预升品--查询背包物品，显示在UI上
	UFUNCTION(BlueprintCallable, Category = "CharacterMenuWidgetController|WeaponUpgrade")
	void PreUpgradeWeapon();

	// 升品--真正升品
	UFUNCTION(BlueprintCallable, Category = "CharacterMenuWidgetController|WeaponUpgrade")
	void UpgradeWeapon();

	// 关闭预升品弹窗
	UFUNCTION(BlueprintCallable, Category = "CharacterMenuWidgetController|WeaponUpgrade")
	void ClosePreUpgradeWindow();

private:
	// 查询材料图标和名称
	bool QueryMatIconAndName(const FGameplayTag& InMatType, UTexture2D*& OutMatIcon, FString& OutMatName);

	// 获取玩家的武器组件
	UEnhoneyWeaponComponent* GetPlayerWeaponComponent();

	// 获取玩家的背包组件
	UInventoryComponent* GetPlayerInventoryComponent();

	// 检查是否可以升品
	bool CanUpgradeWeapon();

public:
	UPROPERTY(BlueprintAssignable, Category = "WeaponUpgrade")
	FWeaponPreUpgradePopWindowSignature OnWeaponPreUpgradeDelegate;

	UPROPERTY(BlueprintAssignable, Category = "WeaponUpgrade")
	FCloseWeaponPreUpgradeWindowSignature OnCloseWeaponPreUpgradeWindowDelegate;

private:
	// 玩家的武器组件
	UPROPERTY()
	TWeakObjectPtr<UEnhoneyWeaponComponent> WeaponComponent;

	// 玩家的背包组件
	UPROPERTY()
	TWeakObjectPtr<UInventoryComponent> InventoryComponent;

	// 保存当前升品需要的配置
	UPROPERTY()
	FEnhoneyWeaponLevelUpInfo CurrentWeaponPreUpgradeConfig;

	// 记录使用中的MatListWidget
	UPROPERTY()
	FWeaponUpgradeMatListWidgetsInUse MatListWidgetsInUse = FWeaponUpgradeMatListWidgetsInUse();

public:

	/** 对象池*/
	// 初始化对象池
	void InitWeaponUpgradeMatListPool(int32 InPoolSize);


private:
	// 根据所需物品配置，启用对象
	bool EnableWeaponUpgradeMatWidgetsFromConfig(const TArray<FEnhoneyWeaponLevelUpMats>& InWeaponLevelUpMatConfig);

	// 从对象池中启用一个
	UWeaponUpgradeMatListWidget* EnableOneWeaponUpgradeMatList(UTexture2D* InMatIcon, const FString& InMatName, int32 InCurrentMatCount, int32 InNeedMatCount);

	// 回收所有使用的对象
	void RecycleAllUsingWeaponUpgradeMatList();

private:
	// 升阶材料清单类
	UPROPERTY(EditDefaultsOnly, Category = "CharacterMenuWidgetController|WeaponUpgradeMatListPool")
	TSubclassOf<UWeaponUpgradeMatListWidget> WeaponUpgradeMatListClass;

	// 升阶材料清单对象池
	UPROPERTY()
	TArray<TObjectPtr<UWeaponUpgradeMatListWidget>> WeaponUpgradeMatListPool;

#pragma endregion WeaponUpgrade
};
