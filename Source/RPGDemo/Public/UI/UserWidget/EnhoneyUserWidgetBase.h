// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnhoneyUserWidgetBase.generated.h"

class UEnhoneyWidgetControllerBase;


/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnhoneyUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UEnhoneyWidgetControllerBase* InWidgetController);

	UFUNCTION(BlueprintImplementableEvent)
	void OnWidgetControllerSet();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void HandleEnhoneyWidgetVisibility(bool bIsVisibile);


public:
	// WidgetController，可以使用TWeakObjectPtr，但是考虑到性能问题，用TObjectPtr更好
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UEnhoneyWidgetControllerBase> WidgetController;
};
