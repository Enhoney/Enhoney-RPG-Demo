// Copyright Enhoney.


#include "MMC_MaxHealth.h"

#include "PlayerAttributeSet.h"
#include "CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	ConstitutionDef.AttributeToCapture = UPlayerAttributeSet::GetConstitutionAttribute();
	ConstitutionDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ConstitutionDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(ConstitutionDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalutionParameters;
	EvalutionParameters.SourceTags = SourceTags;
	EvalutionParameters.TargetTags = TargetTags;

	// 获取体质属性
	float Constitution = 0.f;
	GetCapturedAttributeMagnitude(ConstitutionDef, Spec, EvalutionParameters, Constitution);
	Constitution = FMath::Max(0.f, Constitution);

	// 获取玩家的等级
	int32 CharacterLevel = 0;
	UObject* SourceObject = Spec.GetContext().GetSourceObject();
	if (SourceObject->Implements<UCombatInterface>())
	{
		CharacterLevel = ICombatInterface::Execute_GetCharacterLevel(SourceObject);
	}

	return (500.f + 50.f * CharacterLevel + 20.f * Constitution);
}
