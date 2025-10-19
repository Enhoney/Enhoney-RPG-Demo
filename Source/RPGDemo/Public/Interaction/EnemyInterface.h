// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "EnemyInterface.generated.h"

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_None			UMETA(DisplayName = "None"),

	EES_Rest			UMETA(DisplayName = "Boss Rest"),
	EES_Inspection		UMETA(DisplayName = "Enemy Inspection"),
	EES_Combat			UMETA(DisplayName = "Enemy Combat"),
	EES_Flee			UMETA(DisplayName = "Enemy Flee"),
	EES_Dead			UMETA(DisplayName = "Enemy Dead"),

	EES_Max				UMETA(DisplayName = "Max")

};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGDEMO_API IEnemyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
	void SetAsTargetLocking();

	UFUNCTION(BlueprintNativeEvent)
	void QuitTargetLocking();

	// 伤害飘字
	UFUNCTION(BlueprintNativeEvent)
	void MulticastShowDamageFloat(const FGameplayTag& DamageType, bool bIsCriticalHit, float InDamageValue);

	// 更新血条
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void MulticastUpdateHealthBar(float NewCurrentHealth, float NewCurrentMaxHealth);

	// 获取攻击目标
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetCurrentCombatTarget() const;

	// 设置攻击目标
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetCombatTarget(AActor* NewCombatTarget);

	// 获取巡逻点
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetCurrentPatrolPoint() const;

	// 切换巡逻点
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SwitchPatrolPoint();

	// 获取敌人状态
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EEnemyState GetEnemyState() const;

	// 进入战斗状态
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void EnemyEnterCombat(APawn* NewCombatTarget);

	// 启用或者关闭指定位置的DamageBox的碰撞
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
	void SetMeleeCollisionEnabled(const FGameplayTag DamageLocationTag, bool bIsEnabled);

	// 设置MotinoWarping
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetWarpingFacing();

	// 设置击杀者
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetKillerPawn(APawn* InKillerPawn);
};
