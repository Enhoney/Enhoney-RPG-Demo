// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyAttributeSetBase.h"
#include "PlayerAttributeSet.generated.h"

template<class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<T, FDefaultDelegateUserPolicy>::FFuncPtr;

/**
 * 
 */
UCLASS()
class RPGDEMO_API UPlayerAttributeSet : public UEnhoneyAttributeSetBase
{
	GENERATED_BODY()

public:
	UPlayerAttributeSet();

	// 为那些需要复制的属性注册属性同步
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 即时Clamp
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// 发送
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;

	// 当CurrentValue被修改时触发--我们在这里处理升级时血条填不满的问题
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;

protected:
	virtual void HandleIncomingDamage(const FEffectProperties& EffectProps) override;

	void HandleIncomingExp(const FEffectProperties& EffectProps);

protected:
	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldStrength);

	UFUNCTION()
	void OnRep_Constitution(const FGameplayAttributeData& OldConstitution);

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence);

	UFUNCTION()
	void OnRep_Defense(const FGameplayAttributeData& OldDefense);

	UFUNCTION()
	void OnRep_BasicAttackPower(const FGameplayAttributeData& OldBasicAttackPower);

	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);

	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana);

	UFUNCTION()
	void OnRep_MaxArcane(const FGameplayAttributeData& OldMaxArcane);

	UFUNCTION()
	void OnRep_Arcane(const FGameplayAttributeData& OldArcane);

	UFUNCTION()
	void OnRep_ExtraAttackPower(const FGameplayAttributeData& OldExtraAttackPower);

	UFUNCTION()
	void OnRep_ExtraPercentageAttack(const FGameplayAttributeData& OldExtraPercentageAttack);

public:

	/** Primary 属性--可以加点的*/ 
	// 力量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Strength, Category = "PlayerCustom Attribute")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Strength);

	// 体质
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Constitution, Category = "PlayerCustom Attribute")
	FGameplayAttributeData Constitution;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Constitution);

	// 智力
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Intelligence, Category = "PlayerCustom Attribute")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Intelligence);

	// 防御
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Defense, Category = "PlayerCustom Attribute")
	FGameplayAttributeData Defense;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Defense);

	
	// 玩家基础攻击力--随等级成长
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BasicAttackPower, Category = "PlayerCustom Attribute")
	FGameplayAttributeData BasicAttackPower;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, BasicAttackPower);

	/** Vital属性*/
	// 最大蓝量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "PlayerCustom Attribute")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxMana);

	// 蓝量
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "PlayerCustom Attribute")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Mana);

	// 最大奥义值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxArcane, Category = "PlayerCustom Attribute")
	FGameplayAttributeData MaxArcane;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, MaxArcane);

	// 奥义值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Arcane, Category = "PlayerCustom Attribute")
	FGameplayAttributeData Arcane;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, Arcane);

	/** 武器增益*/
	// 额外基础攻击力--由武器赋予
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ExtraAttackPower,  Category = "PlayerCustom Attribute")
	FGameplayAttributeData ExtraAttackPower;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, ExtraAttackPower);

	// 额外百分比攻击力--由武器赋予
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ExtraPercentageAttack, Category = "PlayerCustom Attribute")
	FGameplayAttributeData ExtraPercentageAttack;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, ExtraPercentageAttack);

	/** 元属性*/
	// 获得的经验值
	UPROPERTY(BlueprintReadOnly, Category = "PlayerCustom Attribute|Meta")
	FGameplayAttributeData IncomingExp;
	ATTRIBUTE_ACCESSORS(UPlayerAttributeSet, IncomingExp);

public:
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> MapGameplayTagToAttribute;
};
