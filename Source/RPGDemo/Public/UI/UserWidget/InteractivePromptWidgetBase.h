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
	// �����ı�����
	UFUNCTION(BlueprintImplementableEvent, Category = "InteractivePrompt")
	void SetPromptText(const FText& NewText);
	
};
