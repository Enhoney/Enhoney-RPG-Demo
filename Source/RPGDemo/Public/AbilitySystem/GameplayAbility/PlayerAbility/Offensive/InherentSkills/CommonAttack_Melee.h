// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "OffensiveAbility_Damage.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "CommonAttack_Melee.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UCommonAttack_Melee : public UOffensiveAbility_Damage
{
	GENERATED_BODY()

public:


protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	void SetUpInputPressedListen();

	// 按键按下回调
	UFUNCTION()
	void HandleInputPressedForComboAttack(float TimeWaited);

	// 造成伤害
	UFUNCTION()
	void CauseDamageToTargetEnemy(FGameplayEventData Payload);

	UFUNCTION()
	void CommonAttackFinished();

	// 进入连击监听状态
	UFUNCTION()
	void EnterComboListen(FGameplayEventData Payload);
	// 退出连击监听状态
	UFUNCTION()
	void QuitComboListen(FGameplayEventData Payload);

	// 重置是否可执行连击状态
	UFUNCTION()
	void ResetExecuteComboAttackState(FGameplayEventData Payload);
	// 执行连击，播放下一段攻击动画
	UFUNCTION()
	void ExecuteComboAttack(FGameplayEventData Payload);

private:
	// 切换连击段数
	void SwitchComboSegment();

	// 获取AttackMontageSection Name
	FName GetAttackMontageSection();

private:
	UPROPERTY()
	UAbilityTask_WaitInputPress* CurrentInputPressTask;

	// 一开始播放第一段，之后就不断切换
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* CurrentCommonAttackMontageTask;

	// 等待造成伤害
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitDamageTagEventTask;
	// 等待进入连击监听状态
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitEnterComboListenEventTask;
	// 等待退出连击监听状态
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitQuitComboListenEventTask;

	// 重置连击状态
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitResetComboAttackEventTask;
	// 执行连击，播放下一个攻击动画
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitExecuteComboAttackEventTask;

private:
	// 是否处于连击监听状态
	UPROPERTY()
	bool bIsComboListen = false;

	// 是否可以执行连击
	UPROPERTY()
	bool bCanExecuteComboAttack = false;

	UPROPERTY()
	int32 ComboSegment = 1;
	
};
