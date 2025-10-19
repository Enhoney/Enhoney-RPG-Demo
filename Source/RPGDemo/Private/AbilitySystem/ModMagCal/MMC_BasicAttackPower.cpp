// Copyright Enhoney.


#include "MMC_BasicAttackPower.h"

#include "PlayerAttributeSet.h"
#include "CombatInterface.h"

UMMC_BasicAttackPower::UMMC_BasicAttackPower()
{
	StrengthDef.AttributeToCapture = UPlayerAttributeSet::GetStrengthAttribute();
	StrengthDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	StrengthDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(StrengthDef);
}

float UMMC_BasicAttackPower::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalutionParameters;
	EvalutionParameters.SourceTags = SourceTags;
	EvalutionParameters.TargetTags = TargetTags;

	// 获取力量属性
	float Strength = 0.f;
	GetCapturedAttributeMagnitude(StrengthDef, Spec, EvalutionParameters, Strength);
	Strength = FMath::Max(0.f, Strength);

	// 获取玩家的等级
	int32 CharacterLevel = 0;
	UObject* SourceObject = Spec.GetContext().GetSourceObject();
	if (SourceObject->Implements<UCombatInterface>())
	{
		CharacterLevel = ICombatInterface::Execute_GetCharacterLevel(SourceObject);
	}

	return (100.f + 2.f * CharacterLevel + 5.f * Strength);
}
