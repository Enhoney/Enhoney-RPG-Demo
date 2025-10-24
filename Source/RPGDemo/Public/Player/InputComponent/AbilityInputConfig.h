// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnhoneyGameplayTags.h"
#include "AbilityInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType, Blueprintable)
struct RPGDEMO_API FAbilityInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "AbilityInputAction Config")
	const UInputAction* AbilityInputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AbilityInputAction Config")
	FGameplayTag InputTag = FGameplayTag();
};

/**
 * 
 */
UCLASS()
class RPGDEMO_API UAbilityInputConfig : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const UInputAction* GetInputActinoByTag(const FGameplayTag& InInputTag, bool bLogNotFound = false) const;

public:
	UPROPERTY(EditDefaultsOnly, Category = "AbilityInputAction Config")
	TArray<FAbilityInputAction> AbilityInputConfig;
};
