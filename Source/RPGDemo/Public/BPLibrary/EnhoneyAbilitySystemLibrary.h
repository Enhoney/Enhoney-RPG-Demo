// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EnhoneyAbilitySystemLibrary.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;

class UInventoryItemInfo;

struct FInventoryItem_Food;
struct FInventoryItem_Task;
struct FInventoryItem_Valuable;
struct FInventoryItem_Economic;

struct FEnhoneyDamageEffectParams;
struct FPlayerTaskConfigItem;

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnhoneyAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Initialize Character Attribute")
	static void ApplyAttributeEffectToSelf(const UObject* InWorldContextObject, const TSubclassOf<UGameplayEffect> EffectToApply, UAbilitySystemComponent* AbilitySystemComponent, float CharacterLevel = 1);

	// 根据AbilityTag找到这个AbilitySpec
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|GetAbilitySpec By AbilityTag")
	static FGameplayAbilitySpec GetAbilitySpecByTag(UAbilitySystemComponent* InASC, const FGameplayTag& InAbilityTag);

	// 施加伤害
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Cause Damage")
	static FGameplayEffectContextHandle CauseDamageByParams(const FEnhoneyDamageEffectParams& InDamageParams);

	// 施加Buff--嗑药
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Use Effect")
	static void ApplyBuffEffectToTarget(const UObject* InWorldContextObject, const TSubclassOf<UGameplayEffect> EffectToApply, AActor* TargetActor, float CharacterLevel = 1);

	// 在外部手动结束某个能力
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Cancel Ability")
	static void CancelAbilityWithAbilityTag(AActor* InTargetActor, const FGameplayTag& InAbilityTag);

	// 获取GameState
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Get InventoryItemInfo")
	static AEnhoneyGameStateBase* GetEnhoneyGameState(const UObject* InWorldContextObject);

	// 获取GameState中InventoryItemInfo数据资产
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Get InventoryItemInfo")
	static UInventoryItemInfo* GetInventoryItemInfoDataAsset(const UObject* InWorldContextObject);

	// 获取食物物品的配置信息
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Get Food Item Config")
	static bool GetInventoryItemConfig_Food(const UObject* InWorldContextObject, const FGameplayTag& InItemTag, FInventoryItem_Food& OutFoodItemConfig);

	// 获取任务物品的配置信息
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Get Task Item Config")
	static bool GetInventoryItemConfig_Task(const UObject* InWorldContextObject, const FGameplayTag& InItemTag, FInventoryItem_Task& OutTaskItemConfig);

	// 获取贵重物品的配置信息
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Get Valuable Item Config")
	static bool GetInventoryItemConfig_Valuable(const UObject* InWorldContextObject, const FGameplayTag& InItemTag, FInventoryItem_Valuable& OutValuableItemConfig);

	// 获取交易物品的配置信息
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Get Trade Item Config")
	static bool GetInventoryItemConfig_Economic(const UObject* InWorldContextObject, const FGameplayTag& InItemTag, FInventoryItem_Economic& OutEconomicItemConfig);

	// 获取任务系统组件
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Get TaskSystem Component")
	static class UTaskSystemComponent* GetTaskSystemComponent(const UObject* InWorldContextObject);

	// 获取单个任务的配置信息
	UFUNCTION(BlueprintCallable, Category = "EnhoneryAbilitySystemLibrary|Get Single Task Config")
	static bool GetSingleTaskInfoConfig(const UObject* InWorldContextObject, const FGameplayTag& InTaskTag, FPlayerTaskConfigItem& OutTaskInfo);


	
};
