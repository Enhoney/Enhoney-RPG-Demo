// Copyright Enhoney.


#include "AbilitySystem/Components/BuffNiagaraComponent.h"
#include "CombatInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

void UBuffNiagaraComponent::InitBuffNiagaraComponent(const FGameplayTag& InBuffTypeTag)
{
	BuffTypeTag = InBuffTypeTag;

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		if (UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
		{
			OwnerASC->RegisterGameplayTagEvent(BuffTypeTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UBuffNiagaraComponent::HandleBuffNiagara);
		}

		CombatInterface->GetOnActorDeathDelegate().AddDynamic(this, &UBuffNiagaraComponent::OnOwnerDeath);
	}
}

void UBuffNiagaraComponent::OnOwnerDeath(AActor* OwnerActor)
{
	// 角色死亡，隐藏组件
	Deactivate();
}

void UBuffNiagaraComponent::HandleBuffNiagara(const FGameplayTag InBuffTypeTag, int32 InTagCount)
{
	if (InBuffTypeTag.MatchesTagExact(BuffTypeTag))
	{
		if (InTagCount > 0)
		{
			Activate();
		}
		else
		{
			Deactivate();
			DestroyInstanceNotComponent();

		}
	}
}
