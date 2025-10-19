// Copyright Enhoney.


#include "EnhoneyTaskSystem/PlayerTaskComponent.h"
#include "Net/UnrealNetwork.h"
#include "EnhoneyPlayerState.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "TaskSystemComponent.h"
#include "InventoryComponent.h"

// Sets default values for this component's properties
UPlayerTaskComponent::UPlayerTaskComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerTaskComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerTaskComponent, CurrentTask, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerTaskComponent, CommitItemCount, COND_None, REPNOTIFY_OnChanged);

}

void UPlayerTaskComponent::GetCurrentTask(FGameplayTag& OutCurrentTaskType) const
{
	if((HasTaskInProgress()))
	{
		OutCurrentTaskType = CurrentTask.TaskType;
	}
}

bool UPlayerTaskComponent::HasTaskInProgress() const
{
	return CurrentTask.TaskId.IsValid();
}


bool UPlayerTaskComponent::IsKillingTask() const
{
	return HasTaskInProgress() && CurrentTask.TaskType.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("TaskType.KillEnemy")));
}

bool UPlayerTaskComponent::IsCommittingTask() const
{
	return HasTaskInProgress() && CurrentTask.TaskType.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("TaskType.CommitMat")));
}

bool UPlayerTaskComponent::IsKillingTheTaskEnemy(const FGameplayTag& InEnemyType) const
{
	return HasTaskInProgress() && CurrentTask.ItemTypeToCommit.MatchesTagExact(InEnemyType);
}

bool UPlayerTaskComponent::IsGettingTheTaskItem(const FGameplayTag& InItemType) const
{
	return HasTaskInProgress() && CurrentTask.ItemTypeToCommit.MatchesTagExact(InItemType);
}

bool UPlayerTaskComponent::CanCurrentTaskCommit() const
{
	return HasTaskInProgress() && (CommitItemCount >= CurrentTask.ItemNumToCommit);
}

bool UPlayerTaskComponent::CanCurrentTaskCancel() const
{
	if (HasTaskInProgress())
	{
		if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetOwner()))
		{
			// 找到取消任务需要的钱
			FPlayerTaskConfigItem TaskConfig;
			if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(this, CurrentTask.TaskType, TaskConfig))
			{
				// 检查钱够不够
				return EnhoneyPlayerState->CanCoinNumConsumed(TaskConfig.CancelPenaltyCoinNum);
			}
		}
	}
	
	return false;
}

void UPlayerTaskComponent::TryRequestNewTask(const FGameplayTag& InTaskType)
{
	if (HasTaskInProgress())
	{
		// 已经有任务在执行，不能申请新任务
		return;
	}
	// 执行RPC，在服务器上申请新任务
	ServerTryRequestNewTask(InTaskType);
}

void UPlayerTaskComponent::TryCommitTask()
{
	if (!CanCurrentTaskCommit())
	{
		return;
	}

	// 执行RPC，在服务器上提交任务
	ServerTryCommitTask();
}

void UPlayerTaskComponent::TryCancelTask()
{
	if (!CanCurrentTaskCancel())
	{
		return;
	}

	// 执行RPC，在服务器上取消任务
	ServerTryCancelTask();
}

void UPlayerTaskComponent::ServerTryRequestNewTask_Implementation(const FGameplayTag& InTaskType)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	// 在服务器上再检查一遍
	if (HasTaskInProgress())
	{
		// 已经有任务在执行，不能申请新任务
		return;
	}
	// 找到任务系统组件
	if (UTaskSystemComponent* TaskSystemComponent = UEnhoneyAbilitySystemLibrary::GetTaskSystemComponent(this))
	{
		// 申请新任务
		if (TaskSystemComponent->RequestNewTask(InTaskType, CurrentTask))
		{
			// 初始化物品计数
			InitCommitItemCount();
			// 获取任务信息，执行广播，更新UI
			FPlayerTaskConfigItem TaskConfig;
			if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(this, CurrentTask.TaskType, TaskConfig))
			{
				OnNewTaskDelegate.Broadcast(TaskConfig, CommitItemCount, CurrentTask.ItemNumToCommit);
			}
			// 检查此时是否可以提交任务
			if (CanCurrentTaskCommit())
			{
				OnPlayerTaskCommitDelegate.Broadcast(true);
			}
			else
			{
				OnPlayerTaskCommitDelegate.Broadcast(false);
			}
		}
	}
}


void UPlayerTaskComponent::ServerTryCommitTask_Implementation()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	// 在服务器上再检查一遍
	if (!CanCurrentTaskCommit())
	{
		return;
	}

	// 找到任务系统组件
	if (UTaskSystemComponent * TaskSystemComponent = UEnhoneyAbilitySystemLibrary::GetTaskSystemComponent(this))
	{
		// 任务Id校验
		if (TaskSystemComponent->IsTaskValid(CurrentTask.TaskId))
		{
			// 发放奖励--经验、金币、物品，扣除背包中对应的任务物品
			if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetOwner()))
			{
				FPlayerTaskConfigItem TaskConfig;
				if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(this, CurrentTask.TaskType,TaskConfig))
				{
					// 经验
					EnhoneyPlayerState->AddCurrentExp(TaskConfig.RewardExp);
					// 金币
					EnhoneyPlayerState->AddCoinNum(TaskConfig.RewardCoinNum);
					// 物品--库存组件增加物品
					if (UInventoryComponent* PlayerInventoryComp = EnhoneyPlayerState->GetInventoryComponent())
					{
						// 扣除任务物品
						if (IsCommittingTask())
						{
							PlayerInventoryComp->ServerConsumeItemByTag(FInventoryItem(CurrentTask.ItemTypeToCommit, CurrentTask.ItemNumToCommit));
						}

						PlayerInventoryComp->ServerAddItemByTag(FInventoryItem(TaskConfig.RewardItemTag, TaskConfig.RewardItemNum));
					}
					// 通知任务系统组件，完成任务
					TaskSystemComponent->CancelOrFinishTask(CurrentTask.TaskId);
					// 清空当前任务
					CurrentTask = FPlayerTask();
					CommitItemCount = 0;
					OnFinishOrCancelTaskDelegate.Broadcast();
				}
			}
		}
	}
	
}

void UPlayerTaskComponent::ServerTryCancelTask_Implementation()
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	// 在服务器上再检查一遍
	if (!CanCurrentTaskCancel())
	{
		return;
	}

	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetOwner()))
	{
		FPlayerTaskConfigItem TaskConfig;
		if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(this, CurrentTask.TaskType, TaskConfig))
		{
			// 找到任务系统组件
			if (UTaskSystemComponent* TaskSystemComponent = UEnhoneyAbilitySystemLibrary::GetTaskSystemComponent(this))
			{
				// 任务Id校验
				if (TaskSystemComponent->IsTaskValid(CurrentTask.TaskId))
				{
					// 扣钱
					EnhoneyPlayerState->ConsumeCoinNum(TaskConfig.CancelPenaltyCoinNum);
					
					// 通知任务系统组件，取消任务
					TaskSystemComponent->CancelOrFinishTask(CurrentTask.TaskId);
					// 清空当前任务
					CurrentTask = FPlayerTask();
					CommitItemCount = 0;
					OnFinishOrCancelTaskDelegate.Broadcast();
				}
			}

		}
	}
}

void UPlayerTaskComponent::TryAddCommitItemCount(const FGameplayTag& InItemType, int32 InKillingNum)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	if (HasTaskInProgress())
	{
		// 先增加计数，然后检查是否可以提交任务
		if (IsGettingTheTaskItem(InItemType))
		{
			CommitItemCount += InKillingNum;
			if (CommitItemCount > CurrentTask.ItemNumToCommit)
			{
				CommitItemCount = CurrentTask.ItemNumToCommit;
			}
			OnPlayerTaskItemCountChangedDelegate.Broadcast(CommitItemCount);
			if (CanCurrentTaskCommit())
			{
				OnPlayerTaskCommitDelegate.Broadcast(true);
			}
		}
	}
}

void UPlayerTaskComponent::BroadcastInitialTaskState()
{

}


// Called when the game starts
void UPlayerTaskComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerTaskComponent::OnRep_CurrentTask(FPlayerTask OldPlayerTask)
{
	if (HasTaskInProgress())
	{
		// 接到了新任务，就获取任务信息，执行广播，更新UI
		FPlayerTaskConfigItem TaskConfig;
		if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(this, CurrentTask.TaskType, TaskConfig))
		{
			OnNewTaskDelegate.Broadcast(TaskConfig, CommitItemCount, CurrentTask.ItemNumToCommit);
		}
	}
	else
	{
		// 任务提交或者取消之后的回调
		OnFinishOrCancelTaskDelegate.Broadcast();
	}
}

void UPlayerTaskComponent::OnRep_CommitItemCount(int32 OldCommitItemCount)
{
	OnPlayerTaskItemCountChangedDelegate.Broadcast(CommitItemCount);
	// 检查是否可以提交任务
	if (CanCurrentTaskCommit())
	{
		OnPlayerTaskCommitDelegate.Broadcast(true);
	}
}

void UPlayerTaskComponent::InitCommitItemCount()
{
	// 只会在接受到任务之后调用

	if (IsKillingTask())
	{
		CommitItemCount = 0;
	}
	else if (IsCommittingTask())
	{
		// 提交任务，检查物品栏，看看有没有任务要求的物品
		if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetOwner()))
		{
			if (UInventoryComponent* PlayerInventoryComp = EnhoneyPlayerState->GetInventoryComponent())
			{
				CommitItemCount = PlayerInventoryComp->GetItemNum(CurrentTask.ItemTypeToCommit);
			}
		}
	}

}


