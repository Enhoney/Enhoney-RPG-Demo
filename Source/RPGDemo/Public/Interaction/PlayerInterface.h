// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "PlayerInterface.generated.h"

class UPlayerAttributeSet;
class UEnhoneyWeaponComponent;
class UEnhoneyWidgetControllerBase;
class AEnhoneyPlayerController;
class UInventoryComponent;
class UPlayerTaskComponent;
class UTaskWidgetController;

// NPC交互委托--传递玩家自己的InventoryComponent
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractWithNPCSignature, AActor*);

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGDEMO_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsPlayerLocallyControlled() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AEnhoneyPlayerController* GetEnhneyPlayerController();


#pragma region PlayerStateAttributes
	/** PlayerState相关操作*/
	// 是否可以消耗属性点
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanAttributeConsumed(int32 AttributePointToComsume) const;
	// 消耗属性点
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ConsumeAttributePoint(int32 AttributePointToComsume);
	// 增加属性点
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddAttributePoint(int32 AttributePointToComsume);

	// 技能点
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanSkillPointConsumed(int32 SkillPointToComsume) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddSkillPoint(int32 SkillPointToAdd);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ConsumeSkillPoint(int32 SkillPointToComsume);

	// 金币
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool CanCoinNumConsumed(int64 CoinNumToComsume) const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddCoinNum(int64 CoinNumToAdd);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ConsumeCoinNum(int64 CoinNumToComsume);


	// 经验值
	// 获取升级所需要的经验值
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetExpToLevelUp() const;
	// 获取当前经验值
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetCurrentExp() const;
	// 增加经验值
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddCurrentExp(int32 ExpToAdd);

	// 等级
	// 升级
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void PlayerLevelUp(int32 TargetLevel);

#pragma endregion PlayerStateAttributes

	// 获取AttributeSet
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UPlayerAttributeSet* GetPlayerAttributeSet() const;

	/** TargetEnemy Locking*/
	// 是否处于目标锁定状态
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TargetEnemy Locking")
	bool IsInTargetEnemyLocing() const;
	// 是否真正锁定到敌人
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TargetEnemy Locking")
	bool IsExactEnenyLocking() const;
	// 获取锁定到的敌人
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TargetEnemy Locking")
	bool GetPlayerTargetEnemy(AActor*& OutTargetEnemy);
	// 进入锁敌状态
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TargetEnemy Locking")
	void EnterTargetEnemyLocking();
	// 退出目标锁定状态
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "TargetEnemy Locking")
	void QuitTargetEnemyLocking();

	/** 武器系统*/
	// 切换武器
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponSystem")
	void SwitchEquippedWeapon(const FGameplayTag& NewWeaponTag);
	// 获取武器组件
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponSystem")
	UEnhoneyWeaponComponent* GetPlayerWeaponComponent();
	// 近战攻击--用于动画通知，启用/关闭盒体的碰撞
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponSystem")
	void SetMeleeCollisionEnabled(bool bIsEnabled);
	// 获取普通攻击动画蒙太奇
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponSystem")
	UAnimMontage* GetCurrentCommontAttackMontage();
	// 找到当前普通攻击要使用的AbilityTag
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponSystem")
	FGameplayTag GetCommonAttackAbilityTag() const;

	// 获取背包组件
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Inventory")
	UInventoryComponent* GetPlayerInventoryComponent();

	/** NPC交互*/
	// 获取NPC交互委托--这个函数只会在C++调用
	virtual FOnInteractWithNPCSignature* GetOnInteractWithNPCDelegate();

	// 设置是否可以交互
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionWithNPC")
	void SetCanInteractWithNPC(bool bInCanInteract);

	// 获取StoreWidgetControlelr
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "InteractionWithNPC")
	UEnhoneyWidgetControllerBase* GetStoreWidgetController();

	// 修改InputMode-不暂停游戏
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Common")
	void SwitchPlayerInputMode(bool bShowMouseCursor, bool bIsUIOnly);

	// 获取任务组件
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Task")
	UPlayerTaskComponent* GetPlayerTaskComponent();

	// 获取任务面板WidgetController
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Task")
	UTaskWidgetController* GetPlayerTaskWidgetController();

};
