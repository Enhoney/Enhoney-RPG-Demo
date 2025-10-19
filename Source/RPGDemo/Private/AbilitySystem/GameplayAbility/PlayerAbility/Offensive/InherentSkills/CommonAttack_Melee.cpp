// Copyright Enhoney.


#include "PlayerAbility/Offensive/InherentSkills/CommonAttack_Melee.h"

#include "EnhoneyGameplayTags.h"
#include "EnhoneyAbilityType.h"
#include "EnhoneyAbilitySystemLibrary.h"

#include "PlayerInterface.h"

void UCommonAttack_Melee::ActivateAbility(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 冷却时间满足
	if (CheckCooldown(Handle, ActorInfo))
	{
		// 播放第一段攻击动画
		UAnimMontage* FirstAttackMontage = IPlayerInterface::Execute_GetCurrentCommontAttackMontage(GetAvatarActorFromActorInfo());
		CurrentCommonAttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), FirstAttackMontage, 1.f, GetAttackMontageSection());
		CurrentCommonAttackMontageTask->OnCompleted.AddDynamic(this, &UCommonAttack_Melee::CommonAttackFinished);	// 如果动画播放完成，就结束能力
		CurrentCommonAttackMontageTask->ReadyForActivation();	// 启动任务

		FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();
		// 监听--进入连击监听状态
		WaitEnterComboListenEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EnhoneyTags.AbilityEventTag_ListenCombat_Enter);
		WaitEnterComboListenEventTask->EventReceived.AddDynamic(this, &UCommonAttack_Melee::EnterComboListen);
		WaitEnterComboListenEventTask->ReadyForActivation();
		// 监听--退出连击监听状态
		WaitQuitComboListenEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EnhoneyTags.AbilityEventTag_ListenCombat_Enter);
		WaitQuitComboListenEventTask->EventReceived.AddDynamic(this, &UCommonAttack_Melee::QuitComboListen);
		WaitQuitComboListenEventTask->ReadyForActivation();

		// 监听--输入--设置可以执行连击
		SetUpInputPressedListen();

		// 监听--命中--造成伤害
		WaitDamageTagEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EnhoneyTags.AbilityEventTag_CommonAttack_Melee);
		WaitDamageTagEventTask->EventReceived.AddDynamic(this, &UCommonAttack_Melee::CauseDamageToTargetEnemy);
		WaitDamageTagEventTask->ReadyForActivation();

		// 监听--连击--重置执行连击状态
		WaitResetComboAttackEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EnhoneyTags.AbilityEventTag_ComboAttack_Reset);
		WaitResetComboAttackEventTask->EventReceived.AddDynamic(this, &UCommonAttack_Melee::ResetExecuteComboAttackState);
		WaitResetComboAttackEventTask->ReadyForActivation();

		// 监听--连击--真正的执行连击
		WaitExecuteComboAttackEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EnhoneyTags.AbilityEventTag_ComboAttack_ExecuteCombo);
		WaitExecuteComboAttackEventTask->EventReceived.AddDynamic(this, &UCommonAttack_Melee::ExecuteComboAttack);
		WaitExecuteComboAttackEventTask->ReadyForActivation();
	}
}

void UCommonAttack_Melee::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 为了安全起见，手动清除一下
	if (IsValid(CurrentInputPressTask))
	{
		CurrentInputPressTask->EndTask();
		CurrentInputPressTask = nullptr;
	}
	if (IsValid(CurrentCommonAttackMontageTask))
	{
		CurrentCommonAttackMontageTask->EndTask();
		CurrentCommonAttackMontageTask = nullptr;
	}
	if (IsValid(WaitDamageTagEventTask))
	{
		WaitDamageTagEventTask->EndTask();
		WaitDamageTagEventTask = nullptr;
	}
	if (IsValid(WaitEnterComboListenEventTask))
	{
		WaitEnterComboListenEventTask->EndTask();
		WaitEnterComboListenEventTask = nullptr;
	}
	if (IsValid(WaitQuitComboListenEventTask))
	{
		WaitQuitComboListenEventTask->EndTask();
		WaitQuitComboListenEventTask = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

}

void UCommonAttack_Melee::SetUpInputPressedListen()
{
	CurrentInputPressTask = UAbilityTask_WaitInputPress::WaitInputPress(this);

	CurrentInputPressTask->OnPress.AddDynamic(this, &UCommonAttack_Melee::HandleInputPressedForComboAttack);
	CurrentInputPressTask->ReadyForActivation();	// 启动任务
}

void UCommonAttack_Melee::HandleInputPressedForComboAttack(float TimeWaited)
{
	// 如果处于连击监听状态
	if (bIsComboListen)
	{
		if (!bCanExecuteComboAttack)
		{
			// 切换连击段数
			SwitchComboSegment();
			// 设置为可以执行连击状态，等待真正的连击时刻到来
			bCanExecuteComboAttack = true;
		}

	}

	if (IsValid(CurrentInputPressTask))
	{
		CurrentInputPressTask->EndTask();
		CurrentInputPressTask = nullptr;
	}

	// 开始监听下一次输入
	SetUpInputPressedListen();
}

void UCommonAttack_Melee::CauseDamageToTargetEnemy(FGameplayEventData Payload)
{
	// 对目标施加上伤害
	FEnhoneyDamageEffectParams DamageEffectParams = MakeDamageEffectParams(Payload.Instigator, true);
	DamageEffectParams.ImpactResult = *(Payload.ContextHandle.Get()->GetHitResult());
	UEnhoneyAbilitySystemLibrary::CauseDamageByParams(DamageEffectParams);
}

void UCommonAttack_Melee::CommonAttackFinished()
{
	CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo);

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UCommonAttack_Melee::EnterComboListen(FGameplayEventData Payload)
{
	bIsComboListen = true;
}

void UCommonAttack_Melee::QuitComboListen(FGameplayEventData Payload)
{
	bIsComboListen = false;
}

void UCommonAttack_Melee::ResetExecuteComboAttackState(FGameplayEventData Payload)
{
	bCanExecuteComboAttack = false;
}

void UCommonAttack_Melee::ExecuteComboAttack(FGameplayEventData Payload)
{
	if (bCanExecuteComboAttack)
	{
		// 执行连击，播放下一段攻击动画
		UAnimMontage* NextAttackMontage = IPlayerInterface::Execute_GetCurrentCommontAttackMontage(GetAvatarActorFromActorInfo());
		CurrentCommonAttackMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, FName(), NextAttackMontage, 1.f, GetAttackMontageSection());
		CurrentCommonAttackMontageTask->OnCompleted.AddDynamic(this, &UCommonAttack_Melee::CommonAttackFinished);	// 如果动画播放完成，就结束能力
		CurrentCommonAttackMontageTask->ReadyForActivation();
	}
}

void UCommonAttack_Melee::SwitchComboSegment()
{
	ComboSegment = (ComboSegment == 1) ? 2 : 1;
}

FName UCommonAttack_Melee::GetAttackMontageSection()
{
	if (ComboSegment == 1)
	{
		return FName(TEXT("Melee_1"));
	}
	return FName(TEXT("Melee_2"));
}
