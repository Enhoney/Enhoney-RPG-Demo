// Copyright Enhoney.


#include "PlayerAbility/Offensive/InherentSkills/EnemyLockingAbility.h"

#include "PlayerInterface.h"
#include "EnemyInterface.h"

#include "CommonAlgorithmLibrary.h"

bool UEnemyLockingAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayTagContainer* SourceTags, 
	const FGameplayTagContainer* TargetTags, 
	OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	bool bOutCanActivateAbility = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

	if (bOutCanActivateAbility)
	{
		if (GetAvatarActorFromActorInfo()->Implements<UPlayerInterface>())
		{
			AActor* TargetEnemy = nullptr;
			bOutCanActivateAbility = !IPlayerInterface::Execute_GetPlayerTargetEnemy(GetAvatarActorFromActorInfo(), TargetEnemy);
		}
	}

	return bOutCanActivateAbility;
}

void UEnemyLockingAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, 
	bool bWasCancelled)
{
	if (GetAvatarActorFromActorInfo()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_QuitTargetEnemyLocking(GetAvatarActorFromActorInfo());
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEnemyLockingAbility::UpdateLockingTargetEnemy(AActor* InEneny)
{
	if (IsValid(InEneny))
	{
		if (GetAvatarActorFromActorInfo()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_UpdateTargetEnemy(GetAvatarActorFromActorInfo(), InEneny);
			
			// 显示被锁定的敌人身上的图标，可以是一个WidgetComponent
			if (InEneny->Implements<UEnemyInterface>())
			{
				IEnemyInterface::Execute_SetAsTargetLocking(InEneny);
			}
		}
	}
}

void UEnemyLockingAbility::HandleTargetEnemyUpdate()
{
	AActor* NewTargetEnemy = UCommonAlgorithmLibrary::GetNearstEnemyInRadius(GetAvatarActorFromActorInfo(), 500.f, false);

	if (IsValid(NewTargetEnemy))
	{
		UpdateLockingTargetEnemy(NewTargetEnemy);
	}
}
