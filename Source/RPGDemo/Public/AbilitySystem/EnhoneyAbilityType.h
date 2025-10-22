// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"

#include "EnhoneyAbilityType.generated.h"

class UEnhoneyAbilitySystemComponent;
class UGameplayEffect;


USTRUCT(BlueprintType)
struct RPGDEMO_API FEnhoneyDamageEffectParams
{
	GENERATED_BODY()

public:
	FEnhoneyDamageEffectParams() {}

	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	TObjectPtr<UObject> WorldContextObject;

	// SourceASC
	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	TObjectPtr<UEnhoneyAbilitySystemComponent> SourceASC;

	// TargetASC
	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	TObjectPtr<UEnhoneyAbilitySystemComponent> TargetASC;

	// 使用的DamageEffect
	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass;

	// 恢复Arcane的GE类
	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	TSubclassOf<UGameplayEffect> RecoveryArcaneEffectClass;

	// 技能伤害类型
	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	FGameplayTag AbilityDamageType = FGameplayTag();

	// AbilityTag
	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	FGameplayTagContainer AbilityTags = FGameplayTagContainer();

	// 技能倍率
	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	float SkillMultiplier = 0.f;

	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	int32 AbilityLevel = 1;

	// 击退几率
	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	float KnockbackChance = 0.f;

	// 击退力度
	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	float KnockbackForceMagnitude = 0.f;

	// 击退向量
	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	FVector KnockbackVector = FVector::ZeroVector;

	// 受击位置
	UPROPERTY(BlueprintReadWrite, Category = "DamageEffectParams")
	FHitResult ImpactResult = FHitResult();

};

USTRUCT(BlueprintType)
struct RPGDEMO_API FEnhoneyGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

public:
	/** Returns the actual struct used for serialization, subclasses must override this! */
	virtual UScriptStruct* GetScriptStruct() const
	{
		return FEnhoneyGameplayEffectContext::StaticStruct();
	}

	/** Custom serialization, subclasses must override this */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/** Creates a copy of this context, used to duplicate for later modifications */
	virtual FEnhoneyGameplayEffectContext* Duplicate() const
	{
		FEnhoneyGameplayEffectContext* NewContext = new FEnhoneyGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

public:
	// Custom Function
	/** Get*/
	bool GetIsCriticalHit() const { return bIsCriticalHit; }

	FVector GetKnockbackVector() const { return KnockbackVector; }

	FGameplayTag GetAbilityDamageType() const
	{
		return AbilityDamageType;
	}

	/** Set*/
	void SetIsCriticalHit(bool bInIsCriticalHit) {
		bIsCriticalHit = bInIsCriticalHit;
	}

	void SetKnockbackVector(const FVector& InKnockbackVector)
	{
		KnockbackVector = InKnockbackVector;
	}

	void SetAbilityDamageType(const FGameplayTag& InAbilityDamageType)
	{
		AbilityDamageType = InAbilityDamageType;
	}
protected:
	// 是否暴击
	UPROPERTY()
	bool bIsCriticalHit = false;

	// 击退向量
	UPROPERTY()
	FVector KnockbackVector = FVector::ZeroVector;

	// 伤害类型
	UPROPERTY()
	FGameplayTag AbilityDamageType = FGameplayTag();

};

// USTRUCT特化
template<>
struct TStructOpsTypeTraits<FEnhoneyGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FEnhoneyGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};

