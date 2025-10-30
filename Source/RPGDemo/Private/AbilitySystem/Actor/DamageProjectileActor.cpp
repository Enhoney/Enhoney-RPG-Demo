// Copyright Enhoney.


#include "Actor/DamageProjectileActor.h"
#include "CombatInterface.h"
#include "GameFramework/ProjectileMovementComponent.h"

ADamageProjectileActor::ADamageProjectileActor()
{
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = 500.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void ADamageProjectileActor::SetHomingTargetComponent(USceneComponent* InHomingTargetComponent)
{
	if (IsValid(ProjectileMovement))
	{
		ProjectileMovement->HomingTargetComponent = InHomingTargetComponent;
		ProjectileMovement->HomingAccelerationMagnitude = 200.f;
		ProjectileMovement->bIsHomingProjectile = true;
	}
}

void ADamageProjectileActor::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(ProjectileMovement) && ProjectileMovement->bIsHomingProjectile && ProjectileMovement->HomingTargetComponent.IsValid())
	{
		if (ICombatInterface* EnemyComabt = Cast<ICombatInterface>(ProjectileMovement->HomingTargetComponent->GetOwner()))
		{
			FOnActorDeathSignature& OnActorDeathDelegate = EnemyComabt->GetOnActorDeathDelegate();
			OnActorDeathDelegate.AddDynamic(this, &ADamageProjectileActor::OnHomingTargetDead);
		}
	}
}

void ADamageProjectileActor::OnHomingTargetDead(AActor* DeadActor)
{
	Destroy();
}
