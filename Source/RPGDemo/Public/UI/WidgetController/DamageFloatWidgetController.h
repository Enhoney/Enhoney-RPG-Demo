// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyWidgetControllerBase.h"
#include "DamageFloatWidgetController.generated.h"

class UDamageFloatComponent;

/**
 * 
 */
UCLASS(Blueprintable)
class RPGDEMO_API UDamageFloatWidgetController : public UEnhoneyWidgetControllerBase
{
	GENERATED_BODY()

public:
	// 初始化对象池
	void Initialize();

	// 从对象池中启用一个对象
	UFUNCTION(BlueprintCallable)
	void EnableDamageFloatComponent(AActor* TargetEnemy, const FGameplayTag& DamageType, bool bIsCriticalHit, float InDamageValue);

private:
	UPROPERTY(EditDefaultsOnly, Category = "DamageFloatComponent")
	TSubclassOf<UDamageFloatComponent> DamageFloat;

	UPROPERTY()
	TArray<TObjectPtr<UDamageFloatComponent>> DamageFloatComponentPool;

	UPROPERTY(EditDefaultsOnly, Category = "DamageFloatComponent")
	int32 ObjectPoolSize = 50;
	
};
