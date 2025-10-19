// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "OffensiveAbility_Damage.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

#include "LittleDragonMelee.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API ULittleDragonMelee : public UOffensiveAbility_Damage
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// 结束技能
	UFUNCTION()
	void MeleeFinished();

	// 命中敌人的回调函数--造成伤害，并播放下一段攻击动画，播放三段之后结束技能
	UFUNCTION()
	void OnHitPlayerCharacter(FGameplayEventData Payload);

	// 播放下一段攻击动画蒙太奇
	UFUNCTION()
	void PlayNextMeleeMontage();

private:
	// 下一次攻击段数
	UPROPERTY()
	int32 NextComboSegment = 1;

	// 是否可以连击
	UPROPERTY()
	bool bCanExecuteComboAttack = false;

	// 攻击动画蒙太奇
	UPROPERTY(EditDefaultsOnly, Category = "LittleDragon|Melee")
	TObjectPtr<UAnimMontage> MeleeAttackMontage;

	// 三段攻击动画对应的SectionName
	UPROPERTY(EditDefaultsOnly, Category = "LittleDragon|Melee")
	TMap<int32 /** ComboSegment */, FName /** SectionName*/> MapComboSegmentToMontageSection;

	// 一开始播放第一段，之后就不断切换
	UPROPERTY()
	UAbilityTask_PlayMontageAndWait* CurrentMeleeMontageTask;

	// 等待造成伤害
	UPROPERTY()
	UAbilityTask_WaitGameplayEvent* WaitDamageTagEventTask;
	
};
