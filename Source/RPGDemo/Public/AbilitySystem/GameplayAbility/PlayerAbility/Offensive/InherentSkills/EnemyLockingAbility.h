// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "OffensiveAbility_NoDamage.h"
#include "EnemyLockingAbility.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnemyLockingAbility : public UOffensiveAbility_NoDamage
{
	GENERATED_BODY()

public:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, 
		const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayTagContainer* SourceTags = nullptr, 
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;;

	UFUNCTION(BlueprintCallable)
	void UpdateLockingTargetEnemy(AActor* InEneny);

	UFUNCTION(BlueprintCallable)
	void HandleTargetEnemyUpdate();
	
};
