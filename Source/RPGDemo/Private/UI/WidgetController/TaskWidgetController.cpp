// Copyright Enhoney.


#include "TaskWidgetController.h"
#include "PlayerTaskPanelWidget.h"
#include "PlayerTaskComponent.h"
#include "EnhoneyPlayerState.h"
#include "PlayerInterface.h"
#include "TaskIntreactPanelWidget.h"
#include "EnhoneyPlayerController.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "EnhoneyLogChannel.h"
#include "InventoryItemInfo.h"
#include "Kismet/GameplayStatics.h"
#include "VillageHeadCharacter.h"
#include "EnhoneyGameplayTags.h"



void UTaskWidgetController::SetPlayerTaskWidget(UPlayerTaskPanelWidget* InPlayerTaskWidget)
{
	if (IsValid(InPlayerTaskWidget) && !IsValid(PlayerTaskWidget))
	{
		PlayerTaskWidget = InPlayerTaskWidget;

		// ����һ���ִ���
		// ��ȡ����������
		if (PlayerState->GetPawn()->Implements<UPlayerInterface>())
		{
			if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
			{
				// ��������ɴ���
				PlayerTaskComponent->OnPlayerTaskCommitDelegate.AddWeakLambda(this, [this](bool bCanCommit)
					{
						if (IsValid(PlayerTaskWidget))
						{
							PlayerTaskWidget->OnTaskCanCommit(bCanCommit);
						}
						// ����������Ϣ
						BroadcastInitialValue();
					});
				// �������ȡ����
				PlayerTaskComponent->OnNewTaskDelegate.AddWeakLambda(this, [this](const FPlayerTaskConfigItem& TaskConfigInfo, int32 CurrentCount, int32 ItemNumToCommit)
					{
						if (IsValid(PlayerTaskWidget))
						{
							PlayerTaskWidget->OnNewTaskReceived(TaskConfigInfo, CurrentCount, ItemNumToCommit);
						}

						// ����������Ϣ
						BroadcastInitialValue();

						// ���ڻ��ܷ��ȡ������
						CheckIfCanAcceptTask();
					});
				// ������ȡ��/�ύ����
				PlayerTaskComponent->OnFinishOrCancelTaskDelegate.AddWeakLambda(this, [this]()
					{
						if (IsValid(PlayerTaskWidget))
						{
							PlayerTaskWidget->OnTaskCommittedOrCanceled();
						}

						// ����������Ϣ
						BroadcastInitialValue();

						// ���ڻ��ܷ��ȡ������
						CheckIfCanAcceptTask();
					});
				// ����Ʒ�������´���
				PlayerTaskComponent->OnPlayerTaskItemCountChangedDelegate.AddWeakLambda(this, [this](int32 NewCount)
					{
						if (IsValid(PlayerTaskWidget))
						{
							PlayerTaskWidget->OnTaskItemCountChanged(NewCount);
						}

						// ����������Ϣ
						BroadcastInitialValue();
					});
				// �㲥��ʼ����״̬
				PlayerTaskComponent->BroadcastInitialTaskState();
			}
		}
	}
}

void UTaskWidgetController::InitializeForTaskNPC()
{
	if (!VillageHead.IsValid())
	{
		VillageHead = Cast<AVillageHeadCharacter>(UGameplayStatics::GetActorOfClass(
			PlayerController, AVillageHeadCharacter::StaticClass()));
	}
}

void UTaskWidgetController::OpenTaskInteractPanel()
{
	// �����ǰû�����񽻻�����࣬�ʹ���һ��
	if (!IsValid(TaskInteractPanelWidget))
	{
		if (IsValid(TaskInteractPanelWidgetClass) && IsValid(PlayerController))
		{
			TaskInteractPanelWidget = CreateWidget<UTaskIntreactPanelWidget>(GetWorld(), TaskInteractPanelWidgetClass);
		}
	}

	// ������񽻻��������Ч������ӵ��ӿ�
	if (IsValid(TaskInteractPanelWidget))
	{
		TaskInteractPanelWidget->AddToViewport();
		TaskInteractPanelWidget->SetWidgetController(this);
	}
}

void UTaskWidgetController::ShowOrHidePlayerTaskPanel()
{
	if (IsValid(PlayerTaskWidget))
	{
		PlayerTaskWidget->ShowOrHideTaskPanel();
	}
}

void UTaskWidgetController::ViewSelectedTask(const FGameplayTag& InSelectedTaskType)
{
	// ����ѡ�п�
	CurrentSelectedTaskType = InSelectedTaskType;
	OnTaskSelectedChangedDelegate.Broadcast(CurrentSelectedTaskType);
	
	// ��ѯ������Ϣ���㲥
	FPlayerTaskConfigItem OutTaskConfigItem;
	// ��ѯ�����Ĺ�����Ʒ��Ϣ

	// �ҵ�����ϵͳ��������������Ϣ
	if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(PlayerState, InSelectedTaskType, OutTaskConfigItem))
	{
		FInventoryItem_Valuable RewardItemConfig;
		if (UEnhoneyAbilitySystemLibrary::GetInventoryItemConfig_Valuable(PlayerState, OutTaskConfigItem.RewardItemTag, RewardItemConfig))
		{
			OnTaskDetailInfoChangedDelegate.Broadcast(OutTaskConfigItem, RewardItemConfig.InventoryItemBaseInfo.InventoryItemIcon);
		}
		else
		{
			// ���û�ҵ����;���
			UE_LOG(LogEnhoney, Warning, TEXT("UTaskWidgetController::ViewSelectedTask: Cannot find inventory item config info for item type %s"), *OutTaskConfigItem.RewardItemTag.ToString());
		}
		
	}
	else
	{
		// ���û���ҵ����;���
		UE_LOG(LogEnhoney, Warning, TEXT("UTaskWidgetController::ViewSelectedTask: Cannot find task config info for task type %s"), *InSelectedTaskType.ToString());
	}

	// ������ǰ�Ƿ��������ڽ��У������Ƿ���Խ�������
	CheckIfCanAcceptTask();

}

void UTaskWidgetController::CheckIfCanAcceptTask()
{
	if (PlayerState->GetPawn()->Implements<UPlayerInterface>() && CurrentSelectedTaskType.IsValid())
	{
		if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
		{
			// �Ƿ���Խ�ȡ������
			OnTaskCanAcceptDelegate.Broadcast(!PlayerTaskComponent->HasTaskInProgress());
		}
	}
}

void UTaskWidgetController::AcceptCurrentTask()
{
	if (CurrentSelectedTaskType.IsValid() && 
		PlayerState->GetPawn()->Implements<UPlayerInterface>())
	{
		if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
		{
			// ��������������
			PlayerTaskComponent->TryRequestNewTask(CurrentSelectedTaskType);

			//// ����������Ϣ
			//BroadcastInitialValue();

			//// ���ڻ��ܷ��ȡ������
			//CheckIfCanAcceptTask();
		}
	}
}

void UTaskWidgetController::SubmitCurrentTask()
{
	// ��ȡ����������
	if (PlayerState->GetPawn()->Implements<UPlayerInterface>())
	{
		if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
		{
			PlayerTaskComponent->TryCommitTask();

			//// �鿴��ǰ�����Ƿ����ȡ�����ύ
			//BroadcastInitialValue();

			//// ���ڻ��ܷ��ȡ������
			//CheckIfCanAcceptTask();
		}
	}
}

void UTaskWidgetController::CloseTaskInteractPanel()
{
	// ������񽻻��������Ч�����Ƴ��ӿ�
	if (IsValid(TaskInteractPanelWidget))
	{
		TaskInteractPanelWidget->RemoveFromParent();
		TaskInteractPanelWidget->SetWidgetController(nullptr);

		// ������
		OnTaskSelectedChangedDelegate.Clear();
		OnTaskDetailInfoChangedDelegate.Clear();
		OnTaskCanSubmitDelegate.Clear();
		OnTaskCanCancelDelegate.Clear();
		OnTaskCanAcceptDelegate.Clear();
		OnCurrentTaskChangedDelegate.Clear();
		CurrentSelectedTaskType = FGameplayTag();

		// ����˳�����״̬
		if (VillageHead.IsValid())
		{
			VillageHead->ExitInteractState();

			// ��������Լ��Ľ�������
			if (PlayerState->GetPawn())
			{
				UEnhoneyAbilitySystemLibrary::CancelAbilityWithAbilityTag(PlayerState->GetPawn(), FEnhoneyGameplayTags::Get().Ability_Offensive_Inherent_InteractWithNPC);
			}
		}
	}
}

void UTaskWidgetController::CancelCurrentTask()
{
	// ��ȡ����������
	if (PlayerState->GetPawn()->Implements<UPlayerInterface>())
	{
		if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
		{
			PlayerTaskComponent->TryCancelTask();

		//	// �鿴��ǰ�����Ƿ����ȡ�����ύ
		//	BroadcastInitialValue();

		//	// ���ڻ��ܷ��ȡ������
		//	CheckIfCanAcceptTask();
		}
	}
}

void UTaskWidgetController::BroadcastInitialValue()
{
	if (IsValid(PlayerTaskWidget))
	{
		if (PlayerState->GetPawn()->Implements<UPlayerInterface>())
		{
			if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
			{
				PlayerTaskComponent->BroadcastInitialTaskState();
			}
		}
	}

	// �㲥��ǰ����״̬
	if (PlayerState->GetPawn()->Implements<UPlayerInterface>())
	{
		if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
		{
			if (PlayerTaskComponent->HasTaskInProgress())
			{
				// ��ѯ������Ϣ���㲥
				FPlayerTaskConfigItem TaskConfigItem;
				// �ҵ�����ϵͳ��������������Ϣ
				FGameplayTag CurrentTaskType = FGameplayTag();
				PlayerTaskComponent->GetCurrentTask(CurrentTaskType);
				// �㲥����ͼ��
				if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(PlayerState, CurrentTaskType, TaskConfigItem))
				{
					OnCurrentTaskChangedDelegate.Broadcast(TaskConfigItem.TaskIcon);
				}
			}
			else
			{
				OnCurrentTaskChangedDelegate.Broadcast(nullptr);
			}

			// ÿ�δ����ù�ѡ���Լ��Ҳ��������Ϣ
			if (!CurrentSelectedTaskType.IsValid())
			{
				OnTaskSelectedChangedDelegate.Broadcast(FGameplayTag());
				OnTaskDetailInfoChangedDelegate.Broadcast(FPlayerTaskConfigItem(), nullptr);
			}
			
			// �㲥�����Ƿ�����ύ��ȡ��
			OnTaskCanSubmitDelegate.Broadcast(PlayerTaskComponent->CanCurrentTaskCommit());
			OnTaskCanCancelDelegate.Broadcast(PlayerTaskComponent->CanCurrentTaskCancel());
		}
	}

}

void UTaskWidgetController::BindCallbacksToDependiencies()
{
}
