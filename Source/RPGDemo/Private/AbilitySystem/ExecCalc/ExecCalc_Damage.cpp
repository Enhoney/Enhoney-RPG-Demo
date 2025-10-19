// Copyright Enhoney.


#include "ExecCalc/ExecCalc_Damage.h"

#include "EnhoneyLogChannel.h"
#include "EnhoneyGameplayTags.h"
#include "EnhoneyAttributeSetBase.h"
#include "EnhoneyAbilityType.h"

#include "AbilitySystemBlueprintLibrary.h"

struct FEnhoneyDamageStatic
{
	// SourceAttribute
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ElementProficiency);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDamageBonus);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FlameDamageBonus);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FrostDamageBonus);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalRatio);

	// TargetAttribute
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefenseCapacity);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalDamageResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FlameDamageResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(FrostDamageResistance);


	FEnhoneyDamageStatic()
	{
		// SourceAttribute
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, AttackPower,			Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, ElementProficiency,	Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, ArmorPenetration,		Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, PhysicalDamageBonus,	Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, FlameDamageBonus,		Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, FrostDamageBonus,		Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, CriticalChance,		Source, true);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, CriticalRatio,		Source, true);

		// TargetAttribute
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, DefenseCapacity,			Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, PhysicalDamageResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, FlameDamageResistance,	Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UEnhoneyAttributeSetBase, FrostDamageResistance,	Target, false);
	}
};

static FEnhoneyDamageStatic& EnhoneyDamageStatic()
{
	static FEnhoneyDamageStatic EnhoneyDamageStatic;

	return EnhoneyDamageStatic;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// SourceAttribute
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().AttackPowerDef);
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().ElementProficiencyDef);
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().PhysicalDamageBonusDef);
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().FlameDamageBonusDef);
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().FrostDamageBonusDef);
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().CriticalChanceDef);
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().CriticalRatioDef);

	// TargetAttribute
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().DefenseCapacityDef);
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().PhysicalDamageResistanceDef);
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().FlameDamageResistanceDef);
	RelevantAttributesToCapture.Add(EnhoneyDamageStatic().FrostDamageResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();

	// 获取ASC
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	// 拿到对应的AvatorActor，一般就是Pawn
	AActor* SourceAvatorActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatortActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvalutionParameters;
	EvalutionParameters.SourceTags = SourceTags;
	EvalutionParameters.TargetTags = TargetTags;

	// 根据Spec拿到Context
	FGameplayEffectContextHandle ContextHandle = Spec.GetEffectContext();
	// 拿到EnhoneyEffectContext
	FEnhoneyGameplayEffectContext* EnhoneyEffectContext = static_cast<FEnhoneyGameplayEffectContext*>((ContextHandle.Get()));

	// 拿到伤害类型
	FGameplayTag AbilityDamageTag = EnhoneyEffectContext->GetAbilityDamageType();
	// 拿到技能倍率
	float SkillMultiplier = 0.f;
	SkillMultiplier = Spec.GetSetByCallerMagnitude(AbilityDamageTag, false);

	// 源攻击力
	float SourceAttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		EnhoneyDamageStatic().AttackPowerDef, EvalutionParameters, SourceAttackPower);
	// 源元素精通
	float SourceElementProficiency = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		EnhoneyDamageStatic().ElementProficiencyDef, EvalutionParameters, SourceElementProficiency);
	// 源穿透
	float SourceArmorPenetration = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		EnhoneyDamageStatic().ArmorPenetrationDef, EvalutionParameters, SourceArmorPenetration);
	// 源暴击率
	float SourceCriticalChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		EnhoneyDamageStatic().CriticalChanceDef, EvalutionParameters, SourceCriticalChance);
	// 源暴击伤害倍率
	float SourceCriticalRatio = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		EnhoneyDamageStatic().CriticalRatioDef, EvalutionParameters, SourceCriticalRatio);

	// 目标防御力
	float TargetDefenseCapacity = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		EnhoneyDamageStatic().DefenseCapacityDef, EvalutionParameters, TargetDefenseCapacity);

	// 是否暴击
	bool bIsCirticalHit = CalculCriticalChance(SourceCriticalChance);
	// 将是否暴击设置到EffectContext
	EnhoneyEffectContext->SetIsCriticalHit(bIsCirticalHit);

	float FinalDamage = 0.f;
	/** 处理不同类型的伤害*/
	if (AbilityDamageTag.MatchesTagExact(EnhoneyTags.AbilityDamageType_Physic))
	{
		/** 物理伤害*/
		// 源物理伤害加成
		float SourcePhysicalDamageBonus = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			EnhoneyDamageStatic().PhysicalDamageBonusDef, EvalutionParameters, SourcePhysicalDamageBonus);

		// 目标物理伤害抗性
		float TargetPhysicalDamageResistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			EnhoneyDamageStatic().PhysicalDamageResistanceDef, EvalutionParameters, TargetPhysicalDamageResistance);

		// 物理最终伤害=((1+物理伤害加成(S)) * 暴击倍率(S) * 技能倍率(S) * 攻击力(S) - 防御力(T) * (1-穿透(S))) * (1-物理伤害抗性(T))

		if (bIsCirticalHit)
		{
			FinalDamage = (1 + SourcePhysicalDamageBonus) * SourceCriticalRatio * SkillMultiplier * SourceAttackPower
				- TargetDefenseCapacity * (1 - SourceArmorPenetration) * (1 - TargetPhysicalDamageResistance);
		}
		else
		{
			FinalDamage = (1 + SourcePhysicalDamageBonus) * SkillMultiplier * SourceAttackPower
				- TargetDefenseCapacity * (1 - SourceArmorPenetration) * (1 - TargetPhysicalDamageResistance);
		}
	}
	else if (AbilityDamageTag.MatchesTagExact(EnhoneyTags.AbilityDamageType_Flame))
	{
		/** 火焰伤害*/
		// 源火焰伤害加成
		float SourceFlameDamageBonus = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			EnhoneyDamageStatic().FlameDamageBonusDef, EvalutionParameters, SourceFlameDamageBonus);

		// 目标火焰伤害抗性
		float TargetFlameDamageResistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			EnhoneyDamageStatic().FlameDamageResistanceDef, EvalutionParameters, TargetFlameDamageResistance);

		// 火焰最终伤害 =((1+火焰伤害加成(S)) * 暴击倍率(S) * (技能倍率(S)*元素精通(S)) - 防御力(T) * (1-穿透(S))) * (1-火焰伤害抗性(T))

		if (bIsCirticalHit)
		{
			FinalDamage = (1 + SourceFlameDamageBonus) * SourceCriticalRatio * SkillMultiplier * SourceElementProficiency
				- TargetDefenseCapacity * (1 - SourceArmorPenetration) * (1 - TargetFlameDamageResistance);
		}
		else
		{
			FinalDamage = (1 + SourceFlameDamageBonus) * SkillMultiplier * SourceAttackPower
				- TargetDefenseCapacity * (1 - SourceArmorPenetration) * (1 - TargetFlameDamageResistance);
		}
	}
	else if(AbilityDamageTag.MatchesTagExact(EnhoneyTags.AbilityDamageType_Frost))
	{
		/** 冰霜伤害*/
		// 源冰霜伤害加成
		float SourceFrostDamageBonus = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			EnhoneyDamageStatic().FlameDamageBonusDef, EvalutionParameters, SourceFrostDamageBonus);

		// 目标冰霜伤害抗性
		float TargetFrostDamageResistance = 0.f;
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
			EnhoneyDamageStatic().FlameDamageResistanceDef, EvalutionParameters, TargetFrostDamageResistance);

		// 冰霜最终伤害 =((1+冰霜伤害加成(S)) * 暴击倍率(S) * (技能倍率(S)*元素精通(S)) - 防御力(T) * (1-穿透(S))) * (1-冰霜伤害抗性(T))

		if (bIsCirticalHit)
		{
			FinalDamage = (1 + SourceFrostDamageBonus) * SourceCriticalRatio * SkillMultiplier * SourceElementProficiency
				- TargetDefenseCapacity * (1 - SourceArmorPenetration) * (1 - TargetFrostDamageResistance);
		}
		else
		{
			FinalDamage = (1 + SourceFrostDamageBonus) * SkillMultiplier * SourceAttackPower
				- TargetDefenseCapacity * (1 - SourceArmorPenetration) * (1 - TargetFrostDamageResistance);
		}
	}

	FinalDamage = FMath::Max(0.f, FinalDamage);
	FGameplayModifierEvaluatedData ModifierEvaluatedData(UEnhoneyAttributeSetBase::GetIncomingDamageAttribute(),
		EGameplayModOp::Additive, FinalDamage);
	OutExecutionOutput.AddOutputModifier(ModifierEvaluatedData);
}

bool UExecCalc_Damage::CalculCriticalChance(float InCriticalChance) const
{
	return FMath::RandRange(1, 1000) < InCriticalChance * 1000;
}
