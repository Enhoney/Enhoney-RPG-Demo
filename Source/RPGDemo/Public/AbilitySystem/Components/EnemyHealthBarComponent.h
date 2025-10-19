// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "EnemyHealthBarComponent.generated.h"

class UEnemyHealthBarWidget;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPGDEMO_API UEnemyHealthBarComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void OnCommonEnenmyHealthChanged(float NewCurrentHealth, float NewCurrentMaxHealth);

protected:
	void BeginPlay() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UEnemyHealthBarWidget> HealthBarWidget;

	UPROPERTY()
	FTimerHandle HealthBarTimerHandle;

	UPROPERTY(EditDefaultsOnly)
	float HealthBarLifeTime = 5.f;
	
};
