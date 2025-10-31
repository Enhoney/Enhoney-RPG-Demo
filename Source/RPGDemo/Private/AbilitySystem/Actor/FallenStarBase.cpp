// Copyright Enhoney.


#include "AbilitySystem/Actor/FallenStarBase.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraComponent.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "RPGDemo.h"
#include "EnhoneyAbilitySystemComponent.h"
#include "CombatInterface.h"

AFallenStarBase::AFallenStarBase()
{
 
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	FallenStarNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("FallenStarNiagaraComponent"));
	SetRootComponent(FallenStarNiagaraComponent);

	// 生命周期为3秒
	SetLifeSpan(5.f);

}

void AFallenStarBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFallenStarBase, TmpDamageEffectParams);
}

void AFallenStarBase::BeginPlay()
{
	Super::BeginPlay();

	// 执行定时器，定期向下发射多球体追踪，对命中敌人造成伤害
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &AFallenStarBase::EmitFallenStarProjectiles);
	TimerManager.SetTimer(EmitFallenStarTimerHandle, TimerDelegate, TraceFrequency, true, TraceInitialDelay);
}

void AFallenStarBase::EmitFallenStarProjectiles()
{
	// 设置查询对象类型为CombatMesh
	TArray<TEnumAsByte<EObjectTypeQuery>> QueryObjectTypes;
	QueryObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_CombatMesh));
	// 忽略伤害来源
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(TmpDamageEffectParams.SourceASC->GetOwner());

	TArray<FHitResult> HitResults;

	// 多球体追踪，对命中敌人造成伤害
	UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		GetActorLocation(),
		GetActorLocation() + FVector(0.f, 0.f, MinTraceHeight),
		TraceRadius,
		QueryObjectTypes, // CombatMesh类型
		false,
		IgnoreActors,
		bIsDrawDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResults,
		true
	);

	// 去重
	TMap<AActor*, FHitResult> UniqueHitActors;
	if (!HitResults.IsEmpty())
	{
		for (const FHitResult& HitResult : HitResults)
		{
			UniqueHitActors.Emplace(HitResult.GetActor(), HitResult);
		}
	}

	// 对命中的敌人，造成伤害
	if (!UniqueHitActors.IsEmpty())
	{
		for(auto HitActorInfo : UniqueHitActors)
		{
			FEnhoneyDamageEffectParams DamageEffectParams = TmpDamageEffectParams;

			DamageEffectParams.TargetASC = Cast<UEnhoneyAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActorInfo.Key));
			DamageEffectParams.ImpactResult = HitActorInfo.Value;
			// 造成伤害
			UEnhoneyAbilitySystemLibrary::CauseDamageByParams(DamageEffectParams);
		}
	}
}

void AFallenStarBase::SetFallenStarNiagaraIfOnMaxLevel(bool bIsOnMaxLevel)
{
	if (bIsOnMaxLevel)
	{
		FallenStarNiagaraComponent->SetAsset(MaxLevelFallenStarNiagaraSystem);
	}
}


