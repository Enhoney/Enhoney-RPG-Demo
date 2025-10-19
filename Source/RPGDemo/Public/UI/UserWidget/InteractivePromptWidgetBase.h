// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractivePromptWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UInteractivePromptWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	// 设置文本内容
	UFUNCTION(BlueprintImplementableEvent, Category = "InteractivePrompt")
	void SetPromptText(const FText& NewText);
	
};
