// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyUserWidgetBase.h"
#include "EnemyHealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnemyHealthBarWidget : public UEnhoneyUserWidgetBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void UpdateHealthBarValue(float NewCurrentHealth, float NewCurrentMaxHealth, FName EnemyName = TEXT(""));

};
