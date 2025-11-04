// Copyright Enhoney.


#include "EnemyAbility/Boss/BossDamageAbilityBase.h"
#include "EnhoneyAbilityType.h"
#include "BossInterface.h"

FEnhoneyDamageEffectParams UBossDamageAbilityBase::MakeDamageEffectParamsByPhase(const AActor* InTargetActor, bool bIsMeleeAbility)
{
	FEnhoneyDamageEffectParams OutParams = Super::MakeDamageEffectParams(InTargetActor, bIsMeleeAbility);

	if (GetAvatarActorFromActorInfo()->Implements<UBossInterface>())
	{
		if (IBossInterface::Execute_GetBossPhase(GetAvatarActorFromActorInfo()) == EBossPhase::EBP_Phase2)
		{
			OutParams.AbilityLevel += 1;
			OutParams.SkillMultiplier = SkillMultipliers.GetValueAtLevel(OutParams.AbilityLevel);
		}
	}

	// 这个给子类实现
	ModifyDamageTypeByPhase(OutParams);

	return OutParams;
}

void UBossDamageAbilityBase::ModifyDamageTypeByPhase(FEnhoneyDamageEffectParams& OutDamageEffectParams)
{
}
