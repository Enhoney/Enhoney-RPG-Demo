// Copyright Enhoney.


#include "AI/EnhoneyAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BrainComponent.h"

AEnhoneyAIController::AEnhoneyAIController()
{

	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));

	BrainComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTree"));
}

UBehaviorTreeComponent* AEnhoneyAIController::GetBehaviorTreeComponent() const
{
	return BrainComponent->IsA<UBehaviorTreeComponent>() ? Cast<UBehaviorTreeComponent>(BrainComponent) : nullptr;
}
