// Copyright Enhoney.


#include "Actor/DamageProjectileActor.h"

#include "GameFramework/ProjectileMovementComponent.h"

ADamageProjectileActor::ADamageProjectileActor()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}
