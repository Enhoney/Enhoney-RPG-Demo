// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorBase.h"
#include "DamageProjectileActor.generated.h"

class UProjectileMovementComponent;

/**
 * 
 */
UCLASS()
class RPGDEMO_API ADamageProjectileActor : public ADamageActorBase
{
	GENERATED_BODY()

public:
	ADamageProjectileActor();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	
};
