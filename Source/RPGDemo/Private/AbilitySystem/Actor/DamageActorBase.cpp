// Copyright Enhoney.


#include "Actor/DamageActorBase.h"

#include "DamageBoxComponent.h"
#include "Net/UnrealNetwork.h"

ADamageActorBase::ADamageActorBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	ProjectileBox = CreateDefaultSubobject<UDamageBoxComponent>(TEXT("ProjectileBox"));
	SetRootComponent(ProjectileBox);
	ProjectileBox->bIsMeleeAttack = false;
	ProjectileBox->SetIsReplicated(true);

	SetLifeSpan(3.f);
}

void ADamageActorBase::BeginPlay()
{
	Super::BeginPlay();

	ProjectileBox->SetCombatBoxCollisionEnable(true);
	ProjectileBox->DamageEffectParams = TmpDamageEffectParams;
	
}

void ADamageActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADamageActorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADamageActorBase, TmpDamageEffectParams);
}

