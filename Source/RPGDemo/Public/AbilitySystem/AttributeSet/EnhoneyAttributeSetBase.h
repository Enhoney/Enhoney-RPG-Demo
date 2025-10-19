// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "EnhoneyAttributeSetBase.generated.h"

USTRUCT()
struct RPGDEMO_API FEffectProperties
{
	GENERATED_BODY()

public:
	FEffectProperties() {}
public:
	FGameplayEffectContextHandle GEContectHandle;

	/** Source*/
	UPROPERTY()
	UAbilitySystemComponent* SourceASC = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	/** Target*/
	UPROPERTY()
	UAbilitySystemComponent* TargetASC = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
  	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName);

/**
 * 
 */
UCLASS(Abstract)
class RPGDEMO_API UEnhoneyAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	// 为那些需要复制的属性注册属性同步
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 即时Clamp
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

protected:
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower);

	UFUNCTION()
	void OnRep_DefenseCapacity(const FGameplayAttributeData& OldDefenseCapacity);

	UFUNCTION()
	void OnRep_ElementProficiency(const FGameplayAttributeData& OldElementProficiency);

	UFUNCTION()
	void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration);

	UFUNCTION()
	void OnRep_PhysicalDamageBonus(const FGameplayAttributeData& OldPhysicalDamageBonus);

	UFUNCTION()
	void OnRep_FlameDamageBonus(const FGameplayAttributeData& OldFlameDamageBonus);

	UFUNCTION()
	void OnRep_FrostDamageBonus(const FGameplayAttributeData& OldFrostDamageBonus);

	UFUNCTION()
	void OnRep_CriticalChance(const FGameplayAttributeData& OldCriticalChance);

	UFUNCTION()
	void OnRep_CriticalRatio(const FGameplayAttributeData& OldCriticalRatio);

	UFUNCTION()
	void OnRep_PhysicalDamageResistance(const FGameplayAttributeData& OldPhysicalDamageResistance);

	UFUNCTION()
	void OnRep_FlameDamageResistance(const FGameplayAttributeData& OldFlameDamageResistance);

	UFUNCTION()
	void OnRep_FrostDamageResistance(const FGameplayAttributeData& OldFrostDamageResistance);

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

protected:
	void GetEffectProps(FEffectProperties& OutEffectProps, const FGameplayEffectModCallbackData& InData);

	virtual void HandleIncomingDamage(const FEffectProperties& EffectProps);

public:
	// 最大生命值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Common Attribute")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, MaxHealth);

	// 最终攻击力
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackPower, Category = "Enemy Attribute")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, AttackPower);

	// 防御力
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DefenseCapacity, Category = "Common Attribute")
	FGameplayAttributeData DefenseCapacity;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, DefenseCapacity);

	// 元素精通
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ElementProficiency, Category = "Common Attribute")
	FGameplayAttributeData ElementProficiency;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, ElementProficiency);

	// 护甲穿透
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ArmorPenetration, Category = "Common Attribute")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, ArmorPenetration);

	// 物理伤害加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalDamageBonus, Category = "Common Attribute")
	FGameplayAttributeData PhysicalDamageBonus;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, PhysicalDamageBonus);

	// 火焰伤害加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FlameDamageBonus, Category = "Common Attribute")
	FGameplayAttributeData FlameDamageBonus;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, FlameDamageBonus);

	// 冰霜伤害加成
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FrostDamageBonus, Category = "Common Attribute")
	FGameplayAttributeData FrostDamageBonus;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, FrostDamageBonus);

	// 暴击率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalChance, Category = "Common Attribute")
	FGameplayAttributeData CriticalChance;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, CriticalChance);

	// 暴击伤害倍率
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CriticalRatio, Category = "Common Attribute")
	FGameplayAttributeData CriticalRatio;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, CriticalRatio);

	// 物理伤害抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_PhysicalDamageResistance, Category = "Common Attribute")
	FGameplayAttributeData PhysicalDamageResistance;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, PhysicalDamageResistance);

	// 火焰伤害抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FlameDamageResistance, Category = "Common Attribute")
	FGameplayAttributeData FlameDamageResistance;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, FlameDamageResistance);

	// 冰霜伤害抗性
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FrostDamageResistance, Category = "Common Attribute")
	FGameplayAttributeData FrostDamageResistance;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, FrostDamageResistance);

	// 实际生命值
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Common Attribute")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, Health);

	// 元属性--接收到的伤害
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attribute")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UEnhoneyAttributeSetBase, IncomingDamage);
	
};
