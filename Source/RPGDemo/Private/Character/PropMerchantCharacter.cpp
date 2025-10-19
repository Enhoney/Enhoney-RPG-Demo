// Copyright Enhoney.


#include "Character/PropMerchantCharacter.h"
#include "PlayerInterface.h"
#include "StoreWidgetController.h"
#include "EnhoneyGameStateBase.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "EnhoneyPlayerController.h"

void APropMerchantCharacter::ExitInteractState_Implementation()
{

	if (AEnhoneyPlayerController* PlayerController = IPlayerInterface::Execute_GetEnhneyPlayerController(InteractivePlayer.Get()))
	{
		PlayerController->ServerSetTransactionPlayer(nullptr);
	}

	Super::ExitInteractState_Implementation();
}

void APropMerchantCharacter::OnInteract(AActor* InInteractivePlayer)
{
	Super::OnInteract(InInteractivePlayer);

	if (!bIsNPCIdle)
	{
		// 广播现有库存，通过WidgetController传递显示到UI上
		if (IsValid(InInteractivePlayer) && InInteractivePlayer->Implements<UPlayerInterface>())
		{
			if (AEnhoneyPlayerController* PlayerController = IPlayerInterface::Execute_GetEnhneyPlayerController(InInteractivePlayer))
			{
				PlayerController->ServerSetTransactionPlayer(InInteractivePlayer);
			}

			if (UEnhoneyWidgetControllerBase* StoreWidgetControlelr = IPlayerInterface::Execute_GetStoreWidgetController(InInteractivePlayer))
			{
				// 打开商店页面
				CastChecked<UStoreWidgetController>(StoreWidgetControlelr)->OpenStoreUI();
			}
		}
	}

}