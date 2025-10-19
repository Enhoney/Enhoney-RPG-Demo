// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageFloatComponent.generated.h"

class UEnhoneyDamageFloatWidget;
class UDamageFloatWidgetController;

/**
 * 
 */
UCLASS()
class RPGDEMO_API UDamageFloatComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	// 启用组件
	UFUNCTION(BlueprintCallable, Category = "DamageFloatComponent")
	void EnableComponent(const FGameplayTag& DamageType, bool bIsCriticalHit, float InDamageValue);

	// 回收组件
	UFUNCTION(BlueprintCallable, Category = "DamageFloatComponent")
	void RecyclingComponent();

	// 查看组件是否可用
	UFUNCTION(BlueprintCallable, Category = "DamageFloatComponent")
	FORCEINLINE bool GetComponentInUse() const { return bInUse; }



private:
	UPROPERTY()
	bool bInUse = false;

	UPROPERTY(EditDefaultsOnly)
	float DamageFloatLifeTime = 1.0f;

	// 使用的UI
	UPROPERTY()
	TObjectPtr<UEnhoneyDamageFloatWidget> DamageFloatUI;

	UPROPERTY()
	TWeakObjectPtr<UDamageFloatWidgetController> DamageFloatWidgetController;

	UPROPERTY()
	FTimerHandle DamageFloatTimerHandle;
};
