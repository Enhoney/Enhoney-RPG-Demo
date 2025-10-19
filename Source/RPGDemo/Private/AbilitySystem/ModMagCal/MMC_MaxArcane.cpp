// Copyright Enhoney.


#include "MMC_MaxArcane.h"
#include "CombatInterface.h"

float UMMC_MaxArcane::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 获取玩家的等级
	int32 CharacterLevel = 0;
	UObject* SourceObject = Spec.GetContext().GetSourceObject();
	if (SourceObject->Implements<UCombatInterface>())
	{
		CharacterLevel = ICombatInterface::Execute_GetCharacterLevel(SourceObject);
	}

	return (500.f + 10.f * CharacterLevel);
}
