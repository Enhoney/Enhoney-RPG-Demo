// Copyright Enhoney.


#include "EnemyAbility/Boss/GiantDragonDamageAbilityBase.h"
#include "BossInterface.h"
#include "EnhoneyAbilityType.h"
#include "EnhoneyGameplayTags.h"

void UGiantDragonDamageAbilityBase::ModifyDamageTypeByPhase(FEnhoneyDamageEffectParams& OutDamageEffectParams)
{
	// 一阶段火伤，二阶段，冰伤害
	if (GetAvatarActorFromActorInfo()->Implements<UBossInterface>())
	{
		if (IBossInterface::Execute_GetBossPhase(GetAvatarActorFromActorInfo()) == EBossPhase::EBP_Phase1)
		{
			OutDamageEffectParams.AbilityDamageType = FEnhoneyGameplayTags::Get().AbilityDamageType_Flame;
		}
		else if (IBossInterface::Execute_GetBossPhase(GetAvatarActorFromActorInfo()) == EBossPhase::EBP_Phase2)
		{
			OutDamageEffectParams.AbilityDamageType = FEnhoneyGameplayTags::Get().AbilityDamageType_Frost;
		}
	}
}
