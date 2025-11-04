// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAbility/Boss/BossDamageAbilityBase.h"
#include "GiantDragonDamageAbilityBase.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UGiantDragonDamageAbilityBase : public UBossDamageAbilityBase
{
	GENERATED_BODY()

	virtual void ModifyDamageTypeByPhase(FEnhoneyDamageEffectParams& OutDamageEffectParams) override;
	
};
