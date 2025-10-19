// Copyright Enhoney.


#include "EnemyHealthBarComponent.h"
#include "EnemyHealthBarWidget.h"

#include "TimerManager.h"

void UEnemyHealthBarComponent::OnCommonEnenmyHealthChanged(float NewCurrentHealth, float NewCurrentMaxHealth)
{

	if (!HealthBarWidget.IsValid())
	{
		HealthBarWidget = Cast<UEnemyHealthBarWidget>(GetUserWidgetObject());
	}

	if (HealthBarWidget.IsValid())
	{
		HealthBarWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		// 更新血条
		HealthBarWidget->UpdateHealthBarValue(NewCurrentHealth, NewCurrentMaxHealth);

		// 重置定时器
		GetWorld()->GetTimerManager().ClearTimer(HealthBarTimerHandle);

		/** 定时器--指定秒数之后，隐藏血条，下次更新血量的时候，再显示，除非更新间隔小于指定秒数*/
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindWeakLambda(this, [this]() {
			HealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
			});
		FTimerManagerTimerParameters TimerParams;
		TimerParams.bLoop = false;
		TimerParams.bMaxOncePerFrame = true;
		GetWorld()->GetTimerManager().SetTimer(HealthBarTimerHandle, TimerDelegate, HealthBarLifeTime, false);
	}
}

void UEnemyHealthBarComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!HealthBarWidget.IsValid())
	{
		HealthBarWidget = Cast<UEnemyHealthBarWidget>(GetUserWidgetObject());
	}

	if (HealthBarWidget.IsValid())
	{
		HealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
