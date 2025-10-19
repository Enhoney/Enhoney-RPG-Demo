// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyAttributeSetBase.h"
#include "EnemyAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnemyAttributeSet : public UEnhoneyAttributeSetBase
{
	GENERATED_BODY()

public:
	UEnemyAttributeSet();

	// 发送
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
protected:

	virtual void HandleIncomingDamage(const FEffectProperties& EffectProps) override;
	
	
};
