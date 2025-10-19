// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyAbilityBase.h"
#include "OffensiveAbilityBase.generated.h"

class UCharacterMovementComponent;

/**
 * 
 */
UCLASS(Abstract)
class RPGDEMO_API UOffensiveAbilityBase : public UEnhoneyAbilityBase
{
	GENERATED_BODY()

public:
	// 禁止移动
	UFUNCTION(BlueprintCallable, Category = "Ability|Movement Allow")
	void DisableAvatarCharacterMovement();

	// 恢复移动
	UFUNCTION(BlueprintCallable, Category = "Ability|Movement Allow")
	void RecoveryAvatarCharacterMovement();
protected:
	// 先保存AvatarCharacter和AvatarCharacterMovement
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData);

	// 确保在能力结束的时候恢复原来的移动模式
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled);

	// 存储原来的移动模式
	void StoreOriginMovementMode();
protected:
	// 是否处于移动禁止状态
	UPROPERTY(BlueprintReadOnly, Category = "Ability|Movement Allow")
	bool bMovementDisabled = false;

	// 存储Avatar
	UPROPERTY(BlueprintReadOnly, Category = "Ability|Avatar Character")
	TWeakObjectPtr<ACharacter> AvatarCharacter;

	// 存储移动组件
	UPROPERTY(BlueprintReadOnly, Category = "Ability|Avatar Character")
	TWeakObjectPtr<UCharacterMovementComponent> AvatarCharacterMovement;

	// 存储原来的移动模式
	UPROPERTY()
	TEnumAsByte<EMovementMode> OriginMovementMode;
	
};
