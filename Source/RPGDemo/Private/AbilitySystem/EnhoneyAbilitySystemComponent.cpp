// Copyright Enhoney.


#include "EnhoneyAbilitySystemComponent.h"

#include "PlayerInterface.h"
#include "GameplayEffect.h"

#include "PlayerAttributeSet.h"
#include "EnhoneyAbilityBase.h"

#include "EnhoneyLogChannel.h"

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
