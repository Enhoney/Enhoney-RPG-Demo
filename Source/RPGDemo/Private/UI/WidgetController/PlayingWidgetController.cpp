// Copyright Enhoney.


#include "PlayingWidgetController.h"

#include "EnhoneyPlayerState.h"
#include "EnhoneyAbilitySystemComponent.h"
#include "PlayerAttributeSet.h"
#include "PlayerInterface.h"
#include "EnhoneyPlayerController.h"
#include "EnhoneyPlayingHUD.h"
#include "InventoryWidgetController.h"
#include "PlayerTaskPanelWidget.h"
#include "TaskWidgetController.h"

#include "GameplayEffectTypes.h"
#include "Kismet/KismetSystemLibrary.h"

void UPlayingWidgetController::BroadcastInitialValue()
{
	if (IsValid(PlayerState))
	{
		OnCharacterLevelChangedDelegate.Broadcast(PlayerState->GetCharacterLevel());

		OnExpChangedDelegate.Broadcast(PlayerState->GetCurrentExp(), PlayerState->GetExpForLevelUp(PlayerState->GetCharacterLevel()));

		if (IsValid(AttributeSet))
		{
			OnMaxHealthChangedDelegate.Broadcast(AttributeSet->GetMaxHealth());
			OnHealthChangedDelegate.Broadcast(AttributeSet->GetHealth());
			
			OnMaxManaChangedDelegate.Broadcast(AttributeSet->GetMaxMana());
			OnManaChangedDelegate.Broadcast(AttributeSet->GetMana());

			OnMaxArcaneChangedDelegate.Broadcast(AttributeSet->GetMaxArcane());
			OnArcaneChangedDelegate.Broadcast(AttributeSet->GetArcane());

			OnExpChangedDelegate.Broadcast(PlayerState->GetCurrentExp(), PlayerState->GetExpForLevelUp(PlayerState->GetCharacterLevel()));
		}
	}
}

void UPlayingWidgetController::BindCallbacksToDependiencies()
{
	if (IsValid(PlayerState))
	{
		// 相比较与AddLambda，这个更安全
		PlayerState->OnCharacterLevelChanged.AddWeakLambda(this, [this](int32 NewCharacterLevel)
			{
				OnCharacterLevelChangedDelegate.Broadcast(NewCharacterLevel);
			});

		PlayerState->OnCurrentExpChanged.AddWeakLambda(this, [this](int32 NewExp, int32 NewExpNeededForLevelUp)
			{
				OnExpChangedDelegate.Broadcast(NewExp, NewExpNeededForLevelUp);
			});

		if (IsValid(PlayerState->GetPawn()) && PlayerState->GetPawn()->Implements<UPlayerInterface>())
		{
			if (UInventoryComponent* PlayerInventoryComponent = IPlayerInterface::Execute_GetPlayerInventoryComponent(PlayerState->GetPawn()))
			{
				PlayerInventoryComponent->OnPlayerGetItemDelegate.AddWeakLambda(this, [this](const FInventoryItem& NewGetItem)
					{
						OnShowGetItemOnUIDelegate.Broadcast(NewGetItem);
					});
			}

		}
	}

	if (IsValid(AbilitySystemComponent) && IsValid(AttributeSet))
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetMaxHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& NewMaxHealth)
				{
					OnMaxHealthChangedDelegate.Broadcast(NewMaxHealth.NewValue);
				});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetHealthAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& NewHealth)
				{
					OnHealthChangedDelegate.Broadcast(NewHealth.NewValue);
				});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetMaxManaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& NewMaxMana)
				{
					OnMaxManaChangedDelegate.Broadcast(NewMaxMana.NewValue);
				});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetMaxManaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& NewMaxMana)
				{
					OnMaxManaChangedDelegate.Broadcast(NewMaxMana.NewValue);
				});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetManaAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& NewMana)
				{
					OnManaChangedDelegate.Broadcast(NewMana.NewValue);
				});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetMaxArcaneAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& NewMaxArcane)
				{
					OnMaxArcaneChangedDelegate.Broadcast(NewMaxArcane.NewValue);
				});

		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			AttributeSet->GetArcaneAttribute()).AddWeakLambda(this, [this](const FOnAttributeChangeData& NewArcane)
				{
					OnArcaneChangedDelegate.Broadcast(NewArcane.NewValue);
				});
	}
}

UInventoryWidgetController* UPlayingWidgetController::GetInventoryWidgetController()
{
	if (IsValid(PlayerController))
	{
		if (AEnhoneyPlayingHUD* HUD = Cast<AEnhoneyPlayingHUD>(PlayerController->GetHUD()))
		{
			FWidgetControllerParam WCParam(PlayerController, PlayerState, AbilitySystemComponent, AttributeSet);
			return Cast<UInventoryWidgetController>(HUD->GetInventoryWidgetController(WCParam));
		}
	}
	return nullptr;
}

UTaskWidgetController* UPlayingWidgetController::GetPlayerTaskWidgetController()
{
	if (IsValid(PlayerController))
	{
		if (AEnhoneyPlayingHUD* HUD = Cast<AEnhoneyPlayingHUD>(PlayerController->GetHUD()))
		{
			FWidgetControllerParam WCParam(PlayerController, PlayerState, AbilitySystemComponent, AttributeSet);
			return Cast<UTaskWidgetController>(HUD->GetPlayerTaskWidgetController(WCParam));
		}
	}
	return nullptr;
}

void UPlayingWidgetController::SetPlayerTaskWidgetOnController(UPlayerTaskPanelWidget* InPlayerTaskWidget)
{
	if(IsValid(InPlayerTaskWidget) )
	{
		if (UTaskWidgetController* TaskWidgetController = GetPlayerTaskWidgetController())
		{
			TaskWidgetController->SetPlayerTaskWidget(InPlayerTaskWidget);
		}
	}
}

void UPlayingWidgetController::OpenPauseMenu()
{
	if (PlayerController)
	{
		// 如果是单机游戏，就暂停
		PlayerController->SetPause(true);
		// 输入仅对UI有效
		PlayerController->SetInputMode(FInputModeUIOnly());
		// 显示鼠标光标
		PlayerController->SetShowMouseCursor(true);
	}
	
	// 显示暂停菜单
	OnGamePauseDelegate.Broadcast(true);
}

void UPlayingWidgetController::ClosePuaseMenu()
{
	if (PlayerController)
	{
		// 如果是单机游戏，就取消暂停
		PlayerController->SetPause(false);
		// 输入对UI和游戏有效
		PlayerController->SetInputMode(FInputModeGameAndUI());
		// 隐藏鼠标光标
		PlayerController->SetShowMouseCursor(false);
	}
	
	// 隐藏暂停菜单
	OnGamePauseDelegate.Broadcast(false);
}

void UPlayingWidgetController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(PlayerController, PlayerController, EQuitPreference::Quit, false);
}

