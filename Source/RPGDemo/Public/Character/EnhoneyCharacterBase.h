// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhoneyCharacterBase.generated.h"

class UGameplayEffect;
class UAbilitySystemComponent;

UCLASS()
class RPGDEMO_API AEnhoneyCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnhoneyCharacterBase();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
