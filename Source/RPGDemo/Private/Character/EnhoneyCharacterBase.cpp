// Copyright Enhoney.


#include "EnhoneyCharacterBase.h"
#include "RPGDemo.h"

#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnhoneyCharacterBase::AEnhoneyCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	ReplicatedMovementMode = true;

	// 伤害相关碰撞都由Mesh来处理
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	GetMesh()->SetCollisionObjectType(ECC_CombatMesh);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_DamageObject, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetCollisionResponseToChannel(ECC_MonitoringCollision, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetGenerateOverlapEvents(true);
}

void AEnhoneyCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

void AEnhoneyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



