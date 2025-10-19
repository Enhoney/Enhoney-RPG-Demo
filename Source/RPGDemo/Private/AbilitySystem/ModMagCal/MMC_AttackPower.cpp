// Copyright Enhoney.


#include "MMC_AttackPower.h"

#include "PlayerAttributeSet.h"
#include "CombatInterface.h"

UMMC_AttackPower::UMMC_AttackPower()
{
	BasicAttackPowerDef.AttributeToCapture = UPlayerAttributeSet::GetBasicAttackPowerAttribute();
	BasicAttackPowerDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	BasicAttackPowerDef.bSnapshot = false;
	ExtraAttackPowerDef.AttributeToCapture = UPlayerAttributeSet::GetExtraAttackPowerAttribute();
	ExtraAttackPowerDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ExtraAttackPowerDef.bSnapshot = false;
	ExtraPercentageAttackDef.AttributeToCapture = UPlayerAttributeSet::GetExtraPercentageAttackAttribute();
	ExtraPercentageAttackDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	ExtraPercentageAttackDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(BasicAttackPowerDef);
	RelevantAttributesToCapture.Add(ExtraAttackPowerDef);
	RelevantAttributesToCapture.Add(ExtraPercentageAttackDef);
}

float UMMC_AttackPower::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalutionParameters;
	EvalutionParameters.SourceTags = SourceTags;
	EvalutionParameters.TargetTags = TargetTags;

	// 获取玩家基础攻击属性
	float BasicAttackPower = 0.f;
	GetCapturedAttributeMagnitude(BasicAttackPowerDef, Spec, EvalutionParameters, BasicAttackPower);
	BasicAttackPower = FMath::Max(0.f, BasicAttackPower);

	// 获取额外基础攻击属性
	float ExtraAttackPower = 0.f;
	GetCapturedAttributeMagnitude(ExtraAttackPowerDef, Spec, EvalutionParameters, ExtraAttackPower);
	ExtraAttackPower = FMath::Max(0.f, ExtraAttackPower);

	// 获取额外百分比攻击属性
	float ExtraPercentageAttack = 0.f;
	GetCapturedAttributeMagnitude(ExtraPercentageAttackDef, Spec, EvalutionParameters, ExtraPercentageAttack);
	ExtraPercentageAttack = FMath::Max(0.f, ExtraPercentageAttack);

	// 获取玩家的等级
	int32 CharacterLevel = 0;
	UObject* SourceObject = Spec.GetContext().GetSourceObject();
	if (SourceObject->Implements<UCombatInterface>())
	{
		CharacterLevel = ICombatInterface::Execute_GetCharacterLevel(SourceObject);
	}

	return (BasicAttackPower * (1 + ExtraPercentageAttack) + ExtraAttackPower);
}
