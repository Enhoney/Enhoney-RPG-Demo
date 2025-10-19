// Copyright Enhoney.


#include "Character/VillageHeadCharacter.h"
#include "PlayerInterface.h"
#include "Components/WidgetComponent.h"
#include "TaskWidgetController.h"

void AVillageHeadCharacter::OnInteract(AActor* InInteractivePlayer)
{
	// 这里就不需要调Super了，任务我们允许所有都可以同时进行提交
	// 这里只需要打开任务交互的UI即可
	if (InInteractivePlayer->Implements<UPlayerInterface>() &&
		IPlayerInterface::Execute_IsPlayerLocallyControlled(InInteractivePlayer))
	{
		// 隐藏名字和交互提示
		NPCNameWidgetComponent->SetVisibility(false);
		NPCInteractWidgetComponent->SetVisibility(false);

		// 打开任务交互UI
		if (UTaskWidgetController* TaskWidgetController = IPlayerInterface::Execute_GetPlayerTaskWidgetController(InInteractivePlayer))
		{
			TaskWidgetController->OpenTaskInteractPanel();
		}

	}
}
