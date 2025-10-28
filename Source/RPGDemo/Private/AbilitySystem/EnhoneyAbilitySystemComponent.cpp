// Copyright Enhoney.


#include "EnhoneyAbilitySystemComponent.h"

#include "PlayerInterface.h"
#include "GameplayEffect.h"

#include "PlayerAttributeSet.h"
#include "EnhoneyAbilityBase.h"
#include "EnhoneyGameplayTags.h"
#include "EnhoneyLogChannel.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "EnhoneyPlayerAbilityInfo.h"

void UEnhoneyAbilitySystemComponent::ServerUpgradeAttributePoint_Implementation(const FGameplayTag& InAttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_CanAttributeConsumed(GetAvatarActor(), 1))
		{
			// 增加属性值
			AddAttributeValue(InAttributeTag);

			// 消耗属性点
			IPlayerInterface::Execute_ConsumeAttributePoint(GetAvatarActor(), 1);
		}
	}
}

void UEnhoneyAbilitySystemComponent::ServerEquipSkill_Implementation(const FGameplayTag& InAbilityTag, const FGameplayTag& InTargetInputTag)
{
	// 先看看原来这个位置有没有装备技能
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();
	if (FGameplayAbilitySpec* LastInputAbilitySpec = GetAbilitySpecByInputTag(InTargetInputTag))
	{
		// 如果原来有装备，就把原来的技能卸载掉
		// 状态改为已解锁
		UpdateVariableAbilityStatus(LastInputAbilitySpec, EnhoneyTags.AbilityStatus_Unlocked);
		// 输入改为无
		UpdateVariableAbilityInputTag(LastInputAbilitySpec, FGameplayTag());
	}
	// 装备上现有技能
	if (FGameplayAbilitySpec* CurrentInputAbilitySpec = GetAbilitySpecByTag(InAbilityTag))
	{
		// 状态设为已装备
		UpdateVariableAbilityStatus(CurrentInputAbilitySpec, EnhoneyTags.AbilityStatus_Equipped);
		// 输入用设定的输入
		UpdateVariableAbilityInputTag(CurrentInputAbilitySpec, InTargetInputTag);
	}
}


void UEnhoneyAbilitySystemComponent::EquipSkill(const FGameplayTag& InAbilityTag, const FGameplayTag& InTargetInputTag)
{
	ServerEquipSkill(InAbilityTag, InTargetInputTag);
}

void UEnhoneyAbilitySystemComponent::UpgradeSkill(const FGameplayTag& InAbilityTag)
{
	// 第一次校验
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(InAbilityTag))
	{
		// 这里为了图省事，直接假设技能最高等级是5级
		if (AbilitySpec->Level < 5)
		{
			int32 SkillPointCost = 0;
			if (UEnhoneyAbilitySystemLibrary::GetVariableAbilityUpgradeCost(GetAvatarActor(), InAbilityTag, AbilitySpec->Level, SkillPointCost))
			{
				// 检查有没有这么多技能点可以消耗
				if (GetAvatarActor()->Implements<UPlayerInterface>())
				{
					if (IPlayerInterface::Execute_CanSkillPointConsumed(GetAvatarActor(), SkillPointCost))
					{
						ServerUpgradeSkill(InAbilityTag);
					}
				}

			}
		}
	}
}

void UEnhoneyAbilitySystemComponent::UnlockSkill(const FGameplayTag& InAbilityTag)
{
	// 查看这个技能是否处于可解锁状态
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(InAbilityTag))
	{
		FGameplayTag AbilityStatusTag;
		if (GetVariableAbilityStatusBySpec(AbilitySpec, AbilityStatusTag))
		{
			if (AbilityStatusTag.MatchesTagExact(FEnhoneyGameplayTags::Get().AbilityStatus_Eligible))
			{
				// 在服务器执行解锁操作
				ServerUnlockSkill(InAbilityTag);
			}
		}
	}
}

void UEnhoneyAbilitySystemComponent::UpdateVariableAbilityStatusOnLevelUp(int32 NewCharacterLevel)
{
	// 获取所有VariableAbility的信息
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (UEnhoneyPlayerAbilityInfo* AbilityConfigInfo = IPlayerInterface::Execute_GetPlayerAbilityInfoAsset(GetAvatarActor()))
		{
			TArray<FPlayerAbilityInfo> VariableAbilitiesInfo = AbilityConfigInfo->GetVariableOffensiveAbilityInfo();
			if (!VariableAbilitiesInfo.IsEmpty())
			{
				for (const FPlayerAbilityInfo& VariableAbilityInfo : VariableAbilitiesInfo)
				{
					// 找到解锁等级，判断现在能否进行解锁
					FGameplayTag AbilityTag = VariableAbilityInfo.AbilityTag;
					int32 UnlockLevel = VariableAbilityInfo.UnlockLevel;
					bool bCanUnlockThisAbility = (UnlockLevel <= NewCharacterLevel);

					// 查看这个技能是否处于状态
					if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(AbilityTag))
					{
						FGameplayTag AbilityStatus;
						GetVariableAbilityStatusBySpec(AbilitySpec, AbilityStatus);
						bool bIsAbilityLocked = (AbilityStatus.IsValid() && AbilityStatus.MatchesTagExact(FEnhoneyGameplayTags::Get().AbilityStatus_Locked));

						// 如果可以解锁，并且这个技能处于锁定状态，就将状态更新为可解锁状态
						if (bCanUnlockThisAbility && bIsAbilityLocked)
						{
							UpdateVariableAbilityStatus(AbilitySpec, FEnhoneyGameplayTags::Get().AbilityStatus_Eligible);
						}
					}
					
				}
			}

		}
	}
}

void UEnhoneyAbilitySystemComponent::AddAttributeValue(const FGameplayTag& InAttributeTag)
{
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		FName EffectName = FName(FString::Printf(TEXT("EffectToAdd_%s"), *InAttributeTag.ToString()));
		UGameplayEffect* EffectToUpgradeAttributePoint = NewObject<UGameplayEffect>(GetTransientPackage(), EffectName);

		EffectToUpgradeAttributePoint->DurationPolicy = EGameplayEffectDurationType::Instant;
		// Modifier
		FGameplayModifierInfo& EffectModifier = EffectToUpgradeAttributePoint->Modifiers.AddDefaulted_GetRef();
		auto AttributePtr = IPlayerInterface::Execute_GetPlayerAttributeSet(GetAvatarActor())->MapGameplayTagToAttribute.Find(InAttributeTag);
		EffectModifier.Attribute = (*AttributePtr)();
		EffectModifier.ModifierMagnitude = FGameplayEffectModifierMagnitude(1.0f);
		EffectModifier.ModifierOp = EGameplayModOp::Additive;

		FGameplayEffectContextHandle EffectHandle = MakeEffectContext();
		EffectHandle.AddSourceObject(GetAvatarActor());
		FGameplayEffectSpec* EffectSpec = new FGameplayEffectSpec(EffectToUpgradeAttributePoint, EffectHandle, 1.f);
		ApplyGameplayEffectSpecToSelf(*EffectSpec);

	}
}

void UEnhoneyAbilitySystemComponent::EndowInherentAbility(TSubclassOf<UEnhoneyAbilityBase> AbilityClassToEndow, bool bIsPassiveSkill)
{
	check(AbilityClassToEndow);

	FGameplayAbilitySpec InherentAbilitySpec = FGameplayAbilitySpec(AbilityClassToEndow, 1);

	if (bIsPassiveSkill)
	{
		// 如果是固有被动技能，赋予之后就直接激活
		GiveAbilityAndActivateOnce(InherentAbilitySpec);
	}
	else
	{
		// 如果是固有主动技能，赋予之后等待玩家手动激活
		GiveAbility(InherentAbilitySpec);
	}

}

void UEnhoneyAbilitySystemComponent::EndowVariableAbility(TSubclassOf<UEnhoneyAbilityBase> AbilityClassToEndow)
{
	check(AbilityClassToEndow);

	FGameplayAbilitySpec InherentAbilitySpec = FGameplayAbilitySpec(AbilityClassToEndow, 1);
	// 默认锁定
	InherentAbilitySpec.DynamicAbilityTags.AddTag(FEnhoneyGameplayTags::Get().AbilityStatus_Locked);

	GiveAbility(InherentAbilitySpec);
}

FGameplayAbilitySpec* UEnhoneyAbilitySystemComponent::GetAbilitySpecByTag(const FGameplayTag& InAbilityTag)
{
	// 避免有能力被移除或者添加
	FScopedAbilityListLock AbilityLock(*this);

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag AbilityTag : AbilitySpec.Ability->AbilityTags)
		{
			if (AbilityTag.MatchesTagExact(InAbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}

	return nullptr;
}

FGameplayAbilitySpec* UEnhoneyAbilitySystemComponent::GetAbilitySpecByInputTag(const FGameplayTag& InAbilityInputTag)
{
	// 避免有能力被移除或者添加
	FScopedAbilityListLock AbilityLock(*this);

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for (FGameplayTag DynamicTag : AbilitySpec.DynamicAbilityTags)
		{
			if (DynamicTag.MatchesTagExact(InAbilityInputTag))
			{
				return &AbilitySpec;
			}
		}
	}

	return nullptr;
}

bool UEnhoneyAbilitySystemComponent::GetVariableAbilityStatusBySpec(const FGameplayAbilitySpec* InAbilitySpec, FGameplayTag& OutAbilityStatusTag)
{
	for (FGameplayTag DynamicTag : InAbilitySpec->DynamicAbilityTags)
	{
		if (DynamicTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityStatus"))))
		{
			OutAbilityStatusTag = DynamicTag;
			return true;
		}
	}

	return false;
}

bool UEnhoneyAbilitySystemComponent::GetVariableAbilityInputTagBySpec(const FGameplayAbilitySpec* InAbilitySpec, FGameplayTag& OutAbilityInputTag)
{
	// 避免有能力被移除或者添加
	FScopedAbilityListLock AbilityLock(*this);

	for (FGameplayTag DynamicTag : InAbilitySpec->DynamicAbilityTags)
	{
		if (DynamicTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("InputTag"))))
		{
			OutAbilityInputTag = DynamicTag;
			return true;
		}
	}
	return false;
}

void UEnhoneyAbilitySystemComponent::AbilitySpecInputPressed(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputPressed(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

void UEnhoneyAbilitySystemComponent::AbilitySpecInputReleased(FGameplayAbilitySpec& Spec)
{
	Super::AbilitySpecInputReleased(Spec);

	if (Spec.IsActive())
	{
		InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());
	}
}

bool UEnhoneyAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InAbilityInputTag)
{
	if (!InAbilityInputTag.IsValid())
	{
		return false;
	}

	// 尝试激活技能
	if (FGameplayAbilitySpec* AbilitySpecToActive = GetAbilitySpecByInputTag(InAbilityInputTag))
	{
		if (!AbilitySpecToActive->IsActive())
		{
			AbilitySpecInputReleased(*AbilitySpecToActive);
			TryActivateAbility(AbilitySpecToActive->Handle);
			return true;
		}
		
	}

	return false;
}

bool UEnhoneyAbilitySystemComponent::GetVariableAbilityCurrentDescriptionByTag(const FGameplayTag& InAbilityTag, FString& OutAbilityDescription)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(InAbilityTag))
	{
		if (UEnhoneyAbilityBase* Ability = Cast<UEnhoneyAbilityBase>(AbilitySpec->Ability))
		{
			Ability->GetAbilityLevelDescription(this, OutAbilityDescription);
			return true;
		}
	}

	return false;
}

bool UEnhoneyAbilitySystemComponent::GetVariableAbilityNextLevelDescriptionByTag(const FGameplayTag& InAbilityTag, FString& OutAbilityDescription)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(InAbilityTag))
	{
		if (UEnhoneyAbilityBase* Ability = Cast<UEnhoneyAbilityBase>(AbilitySpec->Ability))
		{
			Ability->GetNextAbilityLevelDescription(this, OutAbilityDescription);
			return true;
		}
	}

	return false;
}

void UEnhoneyAbilitySystemComponent::BroadcastAllVariableAbility()
{
	// 获取技能配置信息
	FScopedAbilityListLock AbilityLock(*this);
	// 获取所有VariableAbility的信息
	if (GetAvatarActor()->Implements<UPlayerInterface>())
	{
		if (UEnhoneyPlayerAbilityInfo* AbilityConfigInfo = IPlayerInterface::Execute_GetPlayerAbilityInfoAsset(GetAvatarActor()))
		{
			TArray<FPlayerAbilityInfo> VariableAbilitiesInfo = AbilityConfigInfo->GetVariableOffensiveAbilityInfo();
			if (!VariableAbilitiesInfo.IsEmpty())
			{
				for (const FPlayerAbilityInfo& VariableAbilityInfo : VariableAbilitiesInfo)
				{
					if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(VariableAbilityInfo.AbilityTag))
					{
						// 状态
						FGameplayTag AbilityStatusTag;
						GetVariableAbilityStatusBySpec(AbilitySpec, AbilityStatusTag);
						OnVariableAbilityStatusChangedDelegate.Broadcast(VariableAbilityInfo.AbilityTag, AbilityStatusTag);

						// 输入
						FGameplayTag AbilityInputTag;
						GetVariableAbilityInputTagBySpec(AbilitySpec, AbilityInputTag);
						OnVariabieAbilityInputChangedDelegate.Broadcast(VariableAbilityInfo.AbilityTag, AbilityInputTag, AbilityInputTag);	// 这里我们传一样的，是为了告诉主页面的技能面板，不要刷新
					}
				}
			}
		}
	}
}

bool UEnhoneyAbilitySystemComponent::UpdateVariableAbilityStatus(FGameplayAbilitySpec* InAbilitySpec, const FGameplayTag& InNewStatus)
{
	// 先找到原来的状态
	FGameplayTag LastStatus = FGameplayTag();
	for (FGameplayTag DynamicTag : InAbilitySpec->DynamicAbilityTags)
	{
		if (DynamicTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("AbilityStatus"))))
		{
			LastStatus = DynamicTag;
		}
	}
	// 移除原来的状态，添加新的状态
	if (LastStatus.IsValid())
	{
		InAbilitySpec->DynamicAbilityTags.RemoveTag(LastStatus);
		InAbilitySpec->DynamicAbilityTags.AddTag(InNewStatus);
	}
	ClientNotifyAbilityStatusChanged(InAbilitySpec->Ability->AbilityTags.GetByIndex(0), InNewStatus);

	// 强制更新GA到客户端
	MarkAbilitySpecDirty(*InAbilitySpec);

	return false;
}

bool UEnhoneyAbilitySystemComponent::UpdateVariableAbilityInputTag(FGameplayAbilitySpec* InAbilitySpec, const FGameplayTag& InNewInputTag)
{
	// 先找到原来的输入
	FGameplayTag LastInputTag = FGameplayTag();
	for (FGameplayTag DynamicTag : InAbilitySpec->DynamicAbilityTags)
	{
		if (DynamicTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("InputTag"))))
		{
			LastInputTag = DynamicTag;
		}
	}
	// 移除原来的输入配置，添加新的输入配置
	if (LastInputTag.IsValid())
	{
		InAbilitySpec->DynamicAbilityTags.RemoveTag(LastInputTag);

	}
	if (InNewInputTag.IsValid())
	{
		InAbilitySpec->DynamicAbilityTags.AddTag(InNewInputTag);
	}
	
	ClientNotifyAbilityInputChanged(InAbilitySpec->Ability->AbilityTags.GetByIndex(0), InNewInputTag, LastInputTag);

	// 强制更新GA到客户端
	MarkAbilitySpecDirty(*InAbilitySpec);

	return true;
}

void UEnhoneyAbilitySystemComponent::ServerUnlockSkill_Implementation(const FGameplayTag& InAbilityTag)
{
	bool bIsStandalone = (GetNetMode() == NM_Standalone);
	bool bIsAbilityEligible = false;

	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(InAbilityTag))
	{
		if (!bIsStandalone)
		{
			FGameplayTag AbilityStatusTag;
			if (GetVariableAbilityStatusBySpec(AbilitySpec, AbilityStatusTag))
			{
				bIsAbilityEligible = AbilityStatusTag.MatchesTagExact(FEnhoneyGameplayTags::Get().AbilityStatus_Eligible);
			}
		}
		
		if (bIsStandalone || bIsAbilityEligible)
		{
			UpdateVariableAbilityStatus(AbilitySpec, FEnhoneyGameplayTags::Get().AbilityStatus_Unlocked);
		}

	}
}

void UEnhoneyAbilitySystemComponent::ServerUpgradeSkill_Implementation(const FGameplayTag& InAbilityTag)
{
	// 服务器二次校验，技能点是否满足升级要求（如果是单机模式，就不需要二次校验了）
	bool bIsStandalone = (GetNetMode() == NM_Standalone);
	bool bIsSkillPointEnough = false;
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(InAbilityTag))
	{
		// 这里为了图省事，直接假设技能最高等级是5级
		if (AbilitySpec->Level < 5)
		{
			int32 SkillPointCost = 0;
			if (UEnhoneyAbilitySystemLibrary::GetVariableAbilityUpgradeCost(GetAvatarActor(), InAbilityTag, AbilitySpec->Level, SkillPointCost))
			{
				// 检查有没有这么多技能点可以消耗
				if (GetAvatarActor()->Implements<UPlayerInterface>())
				{
					if (!bIsStandalone)
					{
						if (IPlayerInterface::Execute_CanSkillPointConsumed(GetAvatarActor(), SkillPointCost))
						{
							bIsSkillPointEnough = true;
						}
					}
					// 能走到这里，要么是单机模式，那就走过了第一遍校验，没问题；
					// 要么是多人模式，并且通过了二次校验，也没问题
					// 只有在多人模式，并且二次校验没通过（作弊）的情况下才会失败
					if (bIsStandalone || bIsSkillPointEnough)
					{
						// 消耗技能点
						IPlayerInterface::Execute_ConsumeSkillPoint(GetAvatarActor(), SkillPointCost);
						// 技能升级
						AbilitySpec->Level += 1;

						// 强制更新GA到客户端
						MarkAbilitySpecDirty(*AbilitySpec);
					}
				}
			}
		}
	}

}


void UEnhoneyAbilitySystemComponent::ClientNotifyAbilityInputChanged_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& NewAbilityInputTag, const FGameplayTag& OldAbilityInputTag)
{
	OnVariabieAbilityInputChangedDelegate.Broadcast(AbilityTag, NewAbilityInputTag, OldAbilityInputTag);
}

void UEnhoneyAbilitySystemComponent::ClientNotifyAbilityStatusChanged_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& NewAbilityStatus)
{
	OnVariableAbilityStatusChangedDelegate.Broadcast(AbilityTag, NewAbilityStatus);
}

bool UEnhoneyAbilitySystemComponent::GetVariableAbilityStatusByTag(const FGameplayTag& InAbilityTag, FGameplayTag& OutAbilityStatusTag)
{
	// 先找找这个技能有没有被赋予
	if (FGameplayAbilitySpec* AbilitySpec = GetAbilitySpecByTag(InAbilityTag))
	{
		if (GetVariableAbilityStatusBySpec(AbilitySpec, OutAbilityStatusTag))
		{
			return true;
		}
	}

	return false;
}
