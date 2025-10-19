// Copyright Enhoney.


#include "MMC_DefenseCapacity.h"

#include "PlayerAttributeSet.h"
#include "CombatInterface.h"

UMMC_DefenseCapacity::UMMC_DefenseCapacity()
{
	DefenseDef.AttributeToCapture = UPlayerAttributeSet::GetDefenseAttribute();
	DefenseDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	DefenseDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(DefenseDef);
}

float UMMC_DefenseCapacity::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalutionParameters;
	EvalutionParameters.SourceTags = SourceTags;
	EvalutionParameters.TargetTags = TargetTags;

	// 获取防御属性
	float Defense = 0.f;
	GetCapturedAttributeMagnitude(DefenseDef, Spec, EvalutionParameters, Defense);
	Defense = FMath::Max(0.f, Defense);

	// 获取玩家的等级
	int32 CharacterLevel = 0;
	UObject* SourceObject = Spec.GetContext().GetSourceObject();
	if (SourceObject->Implements<UCombatInterface>())
	{
		CharacterLevel = ICombatInterface::Execute_GetCharacterLevel(SourceObject);
	}

	return (50.f + 10.f * CharacterLevel + 5.f * Defense);
}
