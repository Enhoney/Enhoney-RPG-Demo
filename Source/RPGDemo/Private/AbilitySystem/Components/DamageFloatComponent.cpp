// Copyright Enhoney.


#include "DamageFloatComponent.h"

#include "EnhoneyDamageFloatWidget.h"
#include "DamageFloatWidgetController.h"
#include "TimerManager.h"

void UDamageFloatComponent::EnableComponent(const FGameplayTag& DamageType, bool bIsCriticalHit, float InDamageValue)
{
	bInUse = true;

	DamageFloatUI = Cast<UEnhoneyDamageFloatWidget>(GetUserWidgetObject());

	if (IsValid(DamageFloatUI))
	{
		DamageFloatUI->SetVisibility(ESlateVisibility::Visible);
		DamageFloatUI->StartDamageFloat(DamageType, bIsCriticalHit, InDamageValue);

		// 开启定时器
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindWeakLambda(this, [this]() {
			RecyclingComponent();
			});
		FTimerManagerTimerParameters TimerParams;
		TimerParams.bLoop = false;
		TimerParams.bMaxOncePerFrame = true;
		GetWorld()->GetTimerManager().SetTimer(DamageFloatTimerHandle, TimerDelegate, DamageFloatLifeTime, false);
	}
}

void UDamageFloatComponent::RecyclingComponent()
{
	bInUse = false;

	if (IsValid(DamageFloatUI))
	{
		GetWorld()->GetTimerManager().ClearTimer(this->DamageFloatTimerHandle);

		DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

		DamageFloatUI->SetVisibility(ESlateVisibility::Hidden);
	}
}
