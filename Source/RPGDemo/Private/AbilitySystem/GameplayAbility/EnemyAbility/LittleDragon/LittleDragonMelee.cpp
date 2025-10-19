// Copyright Enhoney.


#include "EnemyAbility/LittleDragon/LittleDragonMelee.h"
#include "EnemyInterface.h"
#include "EnhoneyAbilityType.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "EnhoneyGameplayTags.h"

void ULittleDragonMelee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CheckCooldown(Handle, ActorInfo))
	{
		if (!ActorInfo->AvatarActor->Implements<UEnemyInterface>())
		{
			// 如果不是Enemy，就直接结束技能
			CommitAbility(Handle, ActorInfo, ActivationInfo);
			EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		}
		else
		{
			// 播放第一段攻击动画蒙太奇
			PlayNextMeleeMontage();

			// 命中敌人之后，继续播放下一段攻击动画
			WaitDamageTagEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this,
				FEnhoneyGameplayTags::Get().AbilityEventTag_CommonAttack_Melee);
			WaitDamageTagEventTask->EventReceived.AddDynamic(this, &ULittleDragonMelee::OnHitPlayerCharacter);
			WaitDamageTagEventTask->ReadyForActivation();
		}
	}
}

void ULittleDragonMelee::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{

	if (IsValid(CurrentMeleeMontageTask))
	{
		CurrentMeleeMontageTask->EndTask();
		CurrentMeleeMontageTask = nullptr;
	}

	if (IsValid(WaitDamageTagEventTask))
	{
		WaitDamageTagEventTask->EndTask();
		WaitDamageTagEventTask = nullptr;
	}
	NextComboSegment = 1;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULittleDragonMelee::MeleeFinished()
{
	CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void ULittleDragonMelee::OnHitPlayerCharacter(FGameplayEventData Payload)
{
	// 施加伤害
	FEnhoneyDamageEffectParams DamageEffectParams = MakeDamageEffectParams(Payload.Instigator, true);
	DamageEffectParams.ImpactResult = *(Payload.ContextHandle.Get()->GetHitResult());
	UEnhoneyAbilitySystemLibrary::CauseDamageByParams(DamageEffectParams);

	// 设置可以播放下一段动画
	bCanExecuteComboAttack = true;
}

void ULittleDragonMelee::PlayNextMeleeMontage()
{
	check(MeleeAttackMontage);
	check(!MapComboSegmentToMontageSection.IsEmpty());

	// 播放三段之后，或者攻击动画播放完成的时候，还没有命中敌人，就结束技能
	if (NextComboSegment > 3 || (!bCanExecuteComboAttack && NextComboSegment > 1))
	{
		MeleeFinished();
		return;
	}
	if (IsValid(CurrentMeleeMontageTask))
	{
		CurrentMeleeMontageTask->OnCompleted.Clear();
		CurrentMeleeMontageTask->OnCancelled.Clear();
		CurrentMeleeMontageTask->OnBlendOut.Clear();
		CurrentMeleeMontageTask->OnInterrupted.Clear();
		CurrentMeleeMontageTask->EndTask();
		CurrentMeleeMontageTask = nullptr;
	}

	// 设置MotionWarping
	IEnemyInterface::Execute_SetWarpingFacing(GetAvatarActorFromActorInfo());


	// 前三段攻击动画
	CurrentMeleeMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, 
			FName(), MeleeAttackMontage, 1.f, MapComboSegmentToMontageSection[NextComboSegment]);
	// 都绑定上，如果攻击动画蒙太奇被打断，依然可以结束技能
	CurrentMeleeMontageTask->OnCompleted.AddDynamic(this, &ULittleDragonMelee::PlayNextMeleeMontage);
	CurrentMeleeMontageTask->OnCancelled.AddDynamic(this, &ULittleDragonMelee::PlayNextMeleeMontage);
	CurrentMeleeMontageTask->OnBlendOut.AddDynamic(this, &ULittleDragonMelee::PlayNextMeleeMontage);
	CurrentMeleeMontageTask->OnInterrupted.AddDynamic(this, &ULittleDragonMelee::PlayNextMeleeMontage);

	CurrentMeleeMontageTask->ReadyForActivation();
	// 攻击段数加1
	NextComboSegment += 1;
	// 重置连击判定
	bCanExecuteComboAttack = false;

}
