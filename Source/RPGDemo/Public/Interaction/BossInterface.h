// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BossInterface.generated.h"

// BOSS的阶段
UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	EBP_None		UMETA(DisplayName = "None"),

	EBP_Phase1		UMETA(DisplayName = "Phase1"),
	EBP_Phase2		UMETA(DisplayName = "Phase2"),
	EBP_Phase3		UMETA(DisplayName = "Phase3"),	// 可以根据需要继续添加阶段，这个项目我们只有两个阶段

	EBP_Max			UMETA(DisplayName = "Max")
};

UINTERFACE(MinimalAPI)
class UBossInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGDEMO_API IBossInterface
{
	GENERATED_BODY()

	
public:
	// 获取BOSS当前阶段
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	EBossPhase GetBossPhase() const;

	// 切换到二阶段--激活切换阶段的技能
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SwitchToPhaseTwoForAbility();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SwitchToPhaseTwoForBuff();
	// 切换到二阶段的视觉表现
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SwitchToPhaseTwoVisual();

	// 战斗
	// 设置近战攻击的DamageBox的Collision
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponSystem")
	void SetMeleeCollisionEnabled(bool bIsEnabled);
	// 设置武器攻击的DamageBox的Collision--战狂专用，巨龙没有武器，都是用嘴攻击
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WeaponSystem")
	void SetWeaponCollisionEnabled(bool bIsEnabled);
};
