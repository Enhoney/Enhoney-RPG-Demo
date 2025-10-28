// Copyright Enhoney.


#include "UI/WidgetController/SkillMenuWidgetController.h"
#include "EnhoneyAbilitySystemComponent.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "EnhoneyPlayerAbilityInfo.h"
#include "EnhoneyPlayerState.h"

void USkillMenuWidgetController::SelectAbility(const FGameplayTag& InAbilityTag)
{
	if (CurrentSkillMenuState == ESkillMenuState::ESMS_Common ||
		CurrentSkillMenuState == ESkillMenuState::ESMS_View)
	{
		// 查询技能信息
		FPlayerAbilityInfo AbilityInfo;
		if (UEnhoneyAbilitySystemLibrary::GetVariableAbilityInfoByTag(PlayerState->GetPawn(), InAbilityTag, AbilityInfo))
		{
			// 获取当前等级和下一级的描述信息
			FString AbilityDescThisLevel;
			AbilitySystemComponent->GetVariableAbilityCurrentDescriptionByTag(InAbilityTag, AbilityDescThisLevel);
			FCurrentSkillInfo CurrentSkillInfoThisLevel(AbilityInfo.AbilityName, AbilityInfo.SkillIcon, AbilityDescThisLevel);
			FString AbilityDescNextLevel;
			AbilitySystemComponent->GetVariableAbilityNextLevelDescriptionByTag(InAbilityTag, AbilityDescNextLevel);
			FCurrentSkillInfo CurrentSkillInfoNextLevel(AbilityInfo.AbilityName, AbilityInfo.SkillIcon, AbilityDescNextLevel);

			FGameplayTag CurrentSkillStatusTag;
			UEnhoneyAbilitySystemLibrary::GetVariableAbilityStatusByTag(AbilitySystemComponent, InAbilityTag, CurrentSkillStatusTag);
			OnAbilitySelectedDelegate.Broadcast(InAbilityTag, CurrentSkillStatusTag,
				CurrentSkillInfoThisLevel, CurrentSkillInfoNextLevel);
			// 进入View状态
			CurrentSkillMenuState = ESkillMenuState::ESMS_View;
			CurrentSelectedAbilityTag = InAbilityTag;
			// 让选中的按钮边框高亮
			OnAbilityEnterSelectedStateDelegate.Broadcast(CurrentSelectedAbilityTag);
		}
	}
}

void USkillMenuWidgetController::UnlockAbility()
{
	if (CurrentSkillMenuState == ESkillMenuState::ESMS_View)
	{
		if (CurrentSelectedAbilityTag.IsValid())
		{
			// 尝试解锁这个技能
			AbilitySystemComponent->UnlockSkill(CurrentSelectedAbilityTag);

			// 回到Common状态
			ReturnToCommonState();
		}
	}
}

void USkillMenuWidgetController::LevelupAbility()
{
	if (CurrentSkillMenuState == ESkillMenuState::ESMS_View)
	{
		if (CurrentSelectedAbilityTag.IsValid())
		{
			// 尝试升级这个技能
			AbilitySystemComponent->UpgradeSkill(CurrentSelectedAbilityTag);

			// 回到Common状态
			ReturnToCommonState();
		}
	}
}

void USkillMenuWidgetController::PrepareToEquipAbility()
{
	if (CurrentSkillMenuState == ESkillMenuState::ESMS_View)
	{
		if (CurrentSelectedAbilityTag.IsValid())
		{
			// 进入PreEquip状态
			CurrentSkillMenuState = ESkillMenuState::ESMS_PreEquip;
			OnEnterPreEquipStateDelegate.Broadcast();
		}
	}
}

void USkillMenuWidgetController::EquipAbilityToInput(const FGameplayTag& InAbilityInputTag)
{
	if (CurrentSkillMenuState == ESkillMenuState::ESMS_PreEquip)
	{
		if (CurrentSelectedAbilityTag.IsValid())
		{
			// 尝试装备这个技能到指定输入
			AbilitySystemComponent->EquipSkill(CurrentSelectedAbilityTag, InAbilityInputTag);
			// 回到Common状态
			ReturnToCommonState();
		}
	}
}

void USkillMenuWidgetController::BroadcastInitialValue()
{
	// 广播初始技能点数
	if (IsValid(PlayerState))
	{
		OnSkillPointChangedOnSkillMenuDelegate.Broadcast(PlayerState->GetSkillPoint());
	}

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->BroadcastAllVariableAbility();
	}
}

void USkillMenuWidgetController::BindCallbacksToDependiencies()
{
	// 监听技能点变化
	if (IsValid(PlayerState))
	{
		PlayerState->OnSkillPointChanged.AddWeakLambda(this, [this](int32 NewSkillPointValue)
			{
				OnSkillPointChangedOnSkillMenuDelegate.Broadcast(NewSkillPointValue);
			});
	}

	if (AbilitySystemComponent)
	{
		// 技能输入改变的监听
		AbilitySystemComponent->OnVariabieAbilityInputChangedDelegate.AddWeakLambda(this, [this](const FGameplayTag& InAbilityTag,
			const FGameplayTag& InNewAbilityInputTag, const FGameplayTag& InOldAbilityInputTag)
			{
				// 查询技能信息--图标
				FPlayerAbilityInfo AbilityInfo;
				if (UEnhoneyAbilitySystemLibrary::GetVariableAbilityInfoByTag(PlayerState->GetPawn(), InAbilityTag, AbilityInfo))
				{
					OnAbilityInputChangedOnSkillMenuDelegate.Broadcast(AbilityInfo.SkillIcon, InAbilityTag, InNewAbilityInputTag, InOldAbilityInputTag);
				}
			});
		// 技能状态改变的监听
		AbilitySystemComponent->OnVariableAbilityStatusChangedDelegate.AddWeakLambda(this, [this](const FGameplayTag& InAbilityTag,
			const FGameplayTag& InNewStatus)
			{
				OnAbilityStatusChangedOnSkillMenuDelegate.Broadcast(InAbilityTag, InNewStatus);
			});
	}
}

void USkillMenuWidgetController::ReturnToCommonState()
{
	CurrentSkillMenuState = ESkillMenuState::ESMS_Common;
	CurrentSelectedAbilityTag = FGameplayTag();
	OnReturnToCommonStateDelegate.Broadcast();
}
