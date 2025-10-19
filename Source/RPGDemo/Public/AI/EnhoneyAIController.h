// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnhoneyAIController.generated.h"

class UBehaviorTreeComponent;

/**
 * 
 */
UCLASS()
class RPGDEMO_API AEnhoneyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnhoneyAIController();

	UBehaviorTreeComponent* GetBehaviorTreeComponent() const;
};
