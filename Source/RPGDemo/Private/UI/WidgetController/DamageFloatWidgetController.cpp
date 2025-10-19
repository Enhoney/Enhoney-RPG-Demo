// Copyright Enhoney.


#include "DamageFloatWidgetController.h"

#include "DamageFloatComponent.h"

#include "EnhoneyPlayerController.h"

void UDamageFloatWidgetController::Initialize()
{
	DamageFloatComponentPool.Reserve(ObjectPoolSize);
	// 初始化对象池
	for (int32 Index = 0; Index < ObjectPoolSize; ++Index)
	{
		UDamageFloatComponent* DamageFloatComponent =
			NewObject<UDamageFloatComponent>(PlayerController->GetPawn(), DamageFloat, TEXT("DamageFloatComponent"));
		DamageFloatComponent->RegisterComponent();

		DamageFloatComponentPool.Add(DamageFloatComponent);
	}
}

void UDamageFloatWidgetController::EnableDamageFloatComponent(AActor* TargetEnemy, const FGameplayTag& DamageType, bool bIsCriticalHit, float InDamageValue)
{
	check(TargetEnemy);

	UDamageFloatComponent* DamageFloatComponent = nullptr;

	for (UDamageFloatComponent* Component : DamageFloatComponentPool)
	{
		if (!Component->GetComponentInUse())
		{
			DamageFloatComponent = Component;
			break;
		}
	}

	if (IsValid(DamageFloatComponent))
	{
		DamageFloatComponent->AttachToComponent(TargetEnemy->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		DamageFloatComponent->EnableComponent(DamageType, bIsCriticalHit, InDamageValue);
	}
}
