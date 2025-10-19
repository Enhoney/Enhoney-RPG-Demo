// Copyright Enhoney.


#include "CharacterMenuWidgetController.h"

#include "EnhoneyPlayerController.h"

#include "EnhoneyPlayingHUD.h"

void UCharacterMenuWidgetController::HandleCharacterMenuClosed()
{
	// UI
	OnCharacterMenuClosedDelegate.ExecuteIfBound();

	// 如果是单机，需要取消暂停
	if (IsValid(PlayerController))
	{
		PlayerController->CloseCharacterMenu();
	}
}

void UCharacterMenuWidgetController::HandleCharacterMenuStateSwitch(ECharacterMenuState NewCharacterMenuState)
{
	OnCharacterMenuStateChangedDelegate.Broadcast(NewCharacterMenuState);
}

UEnhoneyWidgetControllerBase* UCharacterMenuWidgetController::GetAttributeMenuWidgetController()
{
	if (IsValid(PlayerController))
	{
		if (AEnhoneyPlayingHUD* HUD = Cast<AEnhoneyPlayingHUD>(PlayerController->GetHUD()))
		{
			FWidgetControllerParam WCParam(PlayerController, PlayerState, AbilitySystemComponent, AttributeSet);
			 return HUD->GetAttributeMenuWidgetController(WCParam);
		}
	}
	return nullptr;
}

UEnhoneyWidgetControllerBase* UCharacterMenuWidgetController::GetSkillMenuWidgetController()
{
	return nullptr;
}

UEnhoneyWidgetControllerBase* UCharacterMenuWidgetController::GetInventoryWidgetController()
{
	if (IsValid(PlayerController))
	{
		if (AEnhoneyPlayingHUD* HUD = Cast<AEnhoneyPlayingHUD>(PlayerController->GetHUD()))
		{
			FWidgetControllerParam WCParam(PlayerController, PlayerState, AbilitySystemComponent, AttributeSet);
			return HUD->GetInventoryWidgetController(WCParam);
		}
	}
	return nullptr;
}
