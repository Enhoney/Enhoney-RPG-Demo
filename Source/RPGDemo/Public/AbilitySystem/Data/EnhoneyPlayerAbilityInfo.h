// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EnhoneyPlayerAbilityInfo.generated.h"

class UEnhoneyAbilityBase;
class UMaterialInterface;
class UTexture2D;

USTRUCT(BlueprintType)
struct RPGDEMO_API FPlayerAbilityInfo
{
	GENERATED_BODY()

public:
	// 技能类
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAbility|Config")
	TSubclassOf<UEnhoneyAbilityBase> AbilityClass;

	// 这个也对应着技能树中的位置
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAbility|Config")
	FGameplayTag AbilityTag = FGameplayTag();

	// 技能冷却Tag，用于发起GameplayTagEvent，让UI中技能进入冷却并开始倒计时
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAbility|Config")
	FGameplayTag AbilityCooldownTag = FGameplayTag();

	// 在UI中显示的图标
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerAbility|Config")
	TObjectPtr<const UTexture2D> SkillIcon = nullptr;
};

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnhoneyPlayerAbilityInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	// 获取所有固有主动技能
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility|AbilityInfo")
	TArray<FPlayerAbilityInfo> GetInherentOffensiveAbilityInfo() const;
	// 获取所有固有被动技能
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility|AbilityInfo")
	TArray<FPlayerAbilityInfo> GetInherentPassiveAbilityInfo() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerAbility|AbilityInfo")
	TArray<FPlayerAbilityInfo> GetVariableOffensiveAbilityInfo() const;

	UFUNCTION(BlueprintCallable, Category = "PlayerAbility|AbilityInfo")
	TArray<FPlayerAbilityInfo> GetVariablePassiveAbilityInfo() const;

	// 查找可变主动技能信息
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility|AbilityInfo")
	bool GetOffensiveAbilityInfoByTag(const FGameplayTag& InAbilityTag, FPlayerAbilityInfo& OutAbilityInfo);
	// 查找可变被动技能信息
	UFUNCTION(BlueprintCallable, Category = "PlayerAbility|AbilityInfo")
	bool GetPassiveAbilityInfoByTag(const FGameplayTag& InAbilityTag, FPlayerAbilityInfo& OutAbilityInfo);

private:
	// 固有主动技能--无法装卸，常用技能
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAbility|Config")
	TArray<FPlayerAbilityInfo> OffensiveAbilityInfo_Inherent;
	// 可变主动技能
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAbility|Config")
	TArray<FPlayerAbilityInfo> OffensiveAbilityInfo_Variable;
	// 固有被动技能--无法装卸，默认激活
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAbility|Config")
	TArray<FPlayerAbilityInfo> PassiveAbilityInfo_Inherent;
	// 可变被动技能
	UPROPERTY(EditDefaultsOnly, Category = "PlayerAbility|Config")
	TArray<FPlayerAbilityInfo> PassiveAbilityInfo_Variable;
	
};
