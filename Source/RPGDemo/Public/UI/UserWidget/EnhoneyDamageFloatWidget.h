// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyUserWidgetBase.h"
#include "EnhoneyGameplayTags.h"
#include "EnhoneyDamageFloatWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnhoneyDamageFloatWidget : public UEnhoneyUserWidgetBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "DamgeFloat")
	void StartDamageFloat(const FGameplayTag& DamageType, bool bIsCriticalHit, float InDamageValue);

};
