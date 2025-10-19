// Copyright Enhoney.


#include "OffensiveAbilityBase.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

void UOffensiveAbilityBase::DisableAvatarCharacterMovement()
{
	if (!bMovementDisabled)
	{
		StoreOriginMovementMode();
		if (AvatarCharacterMovement.IsValid())
		{
			// 禁止移动
			AvatarCharacterMovement->DisableMovement();
			bMovementDisabled = true;
		}
	}
}

void UOffensiveAbilityBase::RecoveryAvatarCharacterMovement()
{
	if (bMovementDisabled)
	{
		if (AvatarCharacterMovement.IsValid())
		{
			AvatarCharacterMovement->SetMovementMode(OriginMovementMode);
			bMovementDisabled = false;
		}
	}
}

void UOffensiveAbilityBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	AvatarCharacter = Cast<ACharacter>(ActorInfo->AvatarActor);

	if (AvatarCharacter.IsValid())
	{
		AvatarCharacterMovement = AvatarCharacter->GetCharacterMovement();
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UOffensiveAbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// 确保技能结束的时候可以移动
	RecoveryAvatarCharacterMovement();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UOffensiveAbilityBase::StoreOriginMovementMode()
{
	if (!AvatarCharacterMovement.IsValid())
	{
		AvatarCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());

		if (AvatarCharacter.IsValid())
		{
			AvatarCharacterMovement = AvatarCharacter->GetCharacterMovement();
		}
	}
	check(AvatarCharacterMovement.IsValid());
	OriginMovementMode = AvatarCharacterMovement->MovementMode;
}
