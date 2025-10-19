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

		// 绑定这一部分代理
		// 获取玩家任务组件
		if (PlayerState->GetPawn()->Implements<UPlayerInterface>())
		{
			if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
			{
				// 绑定任务完成代理
				PlayerTaskComponent->OnPlayerTaskCommitDelegate.AddWeakLambda(this, [this](bool bCanCommit)
					{
						if (IsValid(PlayerTaskWidget))
						{
							PlayerTaskWidget->OnTaskCanCommit(bCanCommit);
						}
						// 更新任务信息
						BroadcastInitialValue();
					});
				// 绑定任务获取代理
				PlayerTaskComponent->OnNewTaskDelegate.AddWeakLambda(this, [this](const FPlayerTaskConfigItem& TaskConfigInfo, int32 CurrentCount, int32 ItemNumToCommit)
					{
						if (IsValid(PlayerTaskWidget))
						{
							PlayerTaskWidget->OnNewTaskReceived(TaskConfigInfo, CurrentCount, ItemNumToCommit);
						}

						// 更新任务信息
						BroadcastInitialValue();

						// 现在还能否接取新任务
						CheckIfCanAcceptTask();
					});
				// 绑定任务取消/提交代理
				PlayerTaskComponent->OnFinishOrCancelTaskDelegate.AddWeakLambda(this, [this]()
					{
						if (IsValid(PlayerTaskWidget))
						{
							PlayerTaskWidget->OnTaskCommittedOrCanceled();
						}

						// 更新任务信息
						BroadcastInitialValue();

						// 现在还能否接取新任务
						CheckIfCanAcceptTask();
					});
				// 绑定物品数量更新代理
				PlayerTaskComponent->OnPlayerTaskItemCountChangedDelegate.AddWeakLambda(this, [this](int32 NewCount)
					{
						if (IsValid(PlayerTaskWidget))
						{
							PlayerTaskWidget->OnTaskItemCountChanged(NewCount);
						}

						// 更新任务信息
						BroadcastInitialValue();
					});
				// 广播初始任务状态
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
	// 如果当前没有任务交互面板类，就创建一个
	if (!IsValid(TaskInteractPanelWidget))
	{
		if (IsValid(TaskInteractPanelWidgetClass) && IsValid(PlayerController))
		{
			TaskInteractPanelWidget = CreateWidget<UTaskIntreactPanelWidget>(GetWorld(), TaskInteractPanelWidgetClass);
		}
	}

	// 如果任务交互面板类有效，就添加到视口
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
	// 处理选中框
	CurrentSelectedTaskType = InSelectedTaskType;
	OnTaskSelectedChangedDelegate.Broadcast(CurrentSelectedTaskType);
	
	// 查询任务信息并广播
	FPlayerTaskConfigItem OutTaskConfigItem;
	// 查询奖励的贵重物品信息

	// 找到任务系统组件上面的配置信息
	if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(PlayerState, InSelectedTaskType, OutTaskConfigItem))
	{
		FInventoryItem_Valuable RewardItemConfig;
		if (UEnhoneyAbilitySystemLibrary::GetInventoryItemConfig_Valuable(PlayerState, OutTaskConfigItem.RewardItemTag, RewardItemConfig))
		{
			OnTaskDetailInfoChangedDelegate.Broadcast(OutTaskConfigItem, RewardItemConfig.InventoryItemBaseInfo.InventoryItemIcon);
		}
		else
		{
			// 如果没找到，就警告
			UE_LOG(LogEnhoney, Warning, TEXT("UTaskWidgetController::ViewSelectedTask: Cannot find inventory item config info for item type %s"), *OutTaskConfigItem.RewardItemTag.ToString());
		}
		
	}
	else
	{
		// 如果没有找到，就警告
		UE_LOG(LogEnhoney, Warning, TEXT("UTaskWidgetController::ViewSelectedTask: Cannot find task config info for task type %s"), *InSelectedTaskType.ToString());
	}

	// 看看当前是否有任务在进行，决定是否可以接受任务
	CheckIfCanAcceptTask();

}

void UTaskWidgetController::CheckIfCanAcceptTask()
{
	if (PlayerState->GetPawn()->Implements<UPlayerInterface>() && CurrentSelectedTaskType.IsValid())
	{
		if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
		{
			// 是否可以接取新任务
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
			// 尝试请求新任务
			PlayerTaskComponent->TryRequestNewTask(CurrentSelectedTaskType);

			//// 更新任务信息
			//BroadcastInitialValue();

			//// 现在还能否接取新任务
			//CheckIfCanAcceptTask();
		}
	}
}

void UTaskWidgetController::SubmitCurrentTask()
{
	// 获取玩家任务组件
	if (PlayerState->GetPawn()->Implements<UPlayerInterface>())
	{
		if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
		{
			PlayerTaskComponent->TryCommitTask();

			//// 查看当前任务是否可以取消和提交
			//BroadcastInitialValue();

			//// 现在还能否接取新任务
			//CheckIfCanAcceptTask();
		}
	}
}

void UTaskWidgetController::CloseTaskInteractPanel()
{
	// 如果任务交互面板类有效，就移出视口
	if (IsValid(TaskInteractPanelWidget))
	{
		TaskInteractPanelWidget->RemoveFromParent();
		TaskInteractPanelWidget->SetWidgetController(nullptr);

		// 解绑代理
		OnTaskSelectedChangedDelegate.Clear();
		OnTaskDetailInfoChangedDelegate.Clear();
		OnTaskCanSubmitDelegate.Clear();
		OnTaskCanCancelDelegate.Clear();
		OnTaskCanAcceptDelegate.Clear();
		OnCurrentTaskChangedDelegate.Clear();
		CurrentSelectedTaskType = FGameplayTag();

		// 玩家退出交互状态
		if (VillageHead.IsValid())
		{
			VillageHead->ExitInteractState();

			// 结束玩家自己的交互能力
			if (PlayerState->GetPawn())
			{
				UEnhoneyAbilitySystemLibrary::CancelAbilityWithAbilityTag(PlayerState->GetPawn(), FEnhoneyGameplayTags::Get().Ability_Offensive_Inherent_InteractWithNPC);
			}
		}
	}
}

void UTaskWidgetController::CancelCurrentTask()
{
	// 获取玩家任务组件
	if (PlayerState->GetPawn()->Implements<UPlayerInterface>())
	{
		if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
		{
			PlayerTaskComponent->TryCancelTask();

		//	// 查看当前任务是否可以取消和提交
		//	BroadcastInitialValue();

		//	// 现在还能否接取新任务
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

	// 广播当前任务状态
	if (PlayerState->GetPawn()->Implements<UPlayerInterface>())
	{
		if (UPlayerTaskComponent* PlayerTaskComponent = IPlayerInterface::Execute_GetPlayerTaskComponent(PlayerState->GetPawn()))
		{
			if (PlayerTaskComponent->HasTaskInProgress())
			{
				// 查询任务信息并广播
				FPlayerTaskConfigItem TaskConfigItem;
				// 找到任务系统组件上面的配置信息
				FGameplayTag CurrentTaskType = FGameplayTag();
				PlayerTaskComponent->GetCurrentTask(CurrentTaskType);
				// 广播任务图标
				if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(PlayerState, CurrentTaskType, TaskConfigItem))
				{
					OnCurrentTaskChangedDelegate.Broadcast(TaskConfigItem.TaskIcon);
				}
			}
			else
			{
				OnCurrentTaskChangedDelegate.Broadcast(nullptr);
			}

			// 每次打开重置勾选框，以及右侧的描述信息
			if (!CurrentSelectedTaskType.IsValid())
			{
				OnTaskSelectedChangedDelegate.Broadcast(FGameplayTag());
				OnTaskDetailInfoChangedDelegate.Broadcast(FPlayerTaskConfigItem(), nullptr);
			}
			
			// 广播任务是否可以提交和取消
			OnTaskCanSubmitDelegate.Broadcast(PlayerTaskComponent->CanCurrentTaskCommit());
			OnTaskCanCancelDelegate.Broadcast(PlayerTaskComponent->CanCurrentTaskCancel());
		}
	}

}

void UTaskWidgetController::BindCallbacksToDependiencies()
{
}
