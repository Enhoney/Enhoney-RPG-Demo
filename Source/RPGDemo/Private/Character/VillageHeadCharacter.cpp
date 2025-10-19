// Copyright Enhoney.


#include "Character/VillageHeadCharacter.h"
#include "PlayerInterface.h"
#include "Components/WidgetComponent.h"
#include "TaskWidgetController.h"

void AVillageHeadCharacter::OnInteract(AActor* InInteractivePlayer)
{
	// ����Ͳ���Ҫ��Super�ˣ����������������ж�����ͬʱ�����ύ
	// ����ֻ��Ҫ�����񽻻���UI����
	if (InInteractivePlayer->Implements<UPlayerInterface>() &&
		IPlayerInterface::Execute_IsPlayerLocallyControlled(InInteractivePlayer))
	{
		// �������ֺͽ�����ʾ
		NPCNameWidgetComponent->SetVisibility(false);
		NPCInteractWidgetComponent->SetVisibility(false);

		// �����񽻻�UI
		if (UTaskWidgetController* TaskWidgetController = IPlayerInterface::Execute_GetPlayerTaskWidgetController(InInteractivePlayer))
		{
			TaskWidgetController->OpenTaskInteractPanel();
		}

	}
}
