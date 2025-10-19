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
			// �ҵ�ȡ��������Ҫ��Ǯ
			FPlayerTaskConfigItem TaskConfig;
			if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(this, CurrentTask.TaskType, TaskConfig))
			{
				// ���Ǯ������
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
		// �Ѿ���������ִ�У���������������
		return;
	}
	// ִ��RPC���ڷ�����������������
	ServerTryRequestNewTask(InTaskType);
}

void UPlayerTaskComponent::TryCommitTask()
{
	if (!CanCurrentTaskCommit())
	{
		return;
	}

	// ִ��RPC���ڷ��������ύ����
	ServerTryCommitTask();
}

void UPlayerTaskComponent::TryCancelTask()
{
	if (!CanCurrentTaskCancel())
	{
		return;
	}

	// ִ��RPC���ڷ�������ȡ������
	ServerTryCancelTask();
}

void UPlayerTaskComponent::ServerTryRequestNewTask_Implementation(const FGameplayTag& InTaskType)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	// �ڷ��������ټ��һ��
	if (HasTaskInProgress())
	{
		// �Ѿ���������ִ�У���������������
		return;
	}
	// �ҵ�����ϵͳ���
	if (UTaskSystemComponent* TaskSystemComponent = UEnhoneyAbilitySystemLibrary::GetTaskSystemComponent(this))
	{
		// ����������
		if (TaskSystemComponent->RequestNewTask(InTaskType, CurrentTask))
		{
			// ��ʼ����Ʒ����
			InitCommitItemCount();
			// ��ȡ������Ϣ��ִ�й㲥������UI
			FPlayerTaskConfigItem TaskConfig;
			if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(this, CurrentTask.TaskType, TaskConfig))
			{
				OnNewTaskDelegate.Broadcast(TaskConfig, CommitItemCount, CurrentTask.ItemNumToCommit);
			}
			// ����ʱ�Ƿ�����ύ����
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
	// �ڷ��������ټ��һ��
	if (!CanCurrentTaskCommit())
	{
		return;
	}

	// �ҵ�����ϵͳ���
	if (UTaskSystemComponent * TaskSystemComponent = UEnhoneyAbilitySystemLibrary::GetTaskSystemComponent(this))
	{
		// ����IdУ��
		if (TaskSystemComponent->IsTaskValid(CurrentTask.TaskId))
		{
			// ���Ž���--���顢��ҡ���Ʒ���۳������ж�Ӧ��������Ʒ
			if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetOwner()))
			{
				FPlayerTaskConfigItem TaskConfig;
				if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(this, CurrentTask.TaskType,TaskConfig))
				{
					// ����
					EnhoneyPlayerState->AddCurrentExp(TaskConfig.RewardExp);
					// ���
					EnhoneyPlayerState->AddCoinNum(TaskConfig.RewardCoinNum);
					// ��Ʒ--������������Ʒ
					if (UInventoryComponent* PlayerInventoryComp = EnhoneyPlayerState->GetInventoryComponent())
					{
						// �۳�������Ʒ
						if (IsCommittingTask())
						{
							PlayerInventoryComp->ServerConsumeItemByTag(FInventoryItem(CurrentTask.ItemTypeToCommit, CurrentTask.ItemNumToCommit));
						}

						PlayerInventoryComp->ServerAddItemByTag(FInventoryItem(TaskConfig.RewardItemTag, TaskConfig.RewardItemNum));
					}
					// ֪ͨ����ϵͳ������������
					TaskSystemComponent->CancelOrFinishTask(CurrentTask.TaskId);
					// ��յ�ǰ����
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
	// �ڷ��������ټ��һ��
	if (!CanCurrentTaskCancel())
	{
		return;
	}

	if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetOwner()))
	{
		FPlayerTaskConfigItem TaskConfig;
		if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(this, CurrentTask.TaskType, TaskConfig))
		{
			// �ҵ�����ϵͳ���
			if (UTaskSystemComponent* TaskSystemComponent = UEnhoneyAbilitySystemLibrary::GetTaskSystemComponent(this))
			{
				// ����IdУ��
				if (TaskSystemComponent->IsTaskValid(CurrentTask.TaskId))
				{
					// ��Ǯ
					EnhoneyPlayerState->ConsumeCoinNum(TaskConfig.CancelPenaltyCoinNum);
					
					// ֪ͨ����ϵͳ�����ȡ������
					TaskSystemComponent->CancelOrFinishTask(CurrentTask.TaskId);
					// ��յ�ǰ����
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
		// �����Ӽ�����Ȼ�����Ƿ�����ύ����
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
		// �ӵ��������񣬾ͻ�ȡ������Ϣ��ִ�й㲥������UI
		FPlayerTaskConfigItem TaskConfig;
		if (UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(this, CurrentTask.TaskType, TaskConfig))
		{
			OnNewTaskDelegate.Broadcast(TaskConfig, CommitItemCount, CurrentTask.ItemNumToCommit);
		}
	}
	else
	{
		// �����ύ����ȡ��֮��Ļص�
		OnFinishOrCancelTaskDelegate.Broadcast();
	}
}

void UPlayerTaskComponent::OnRep_CommitItemCount(int32 OldCommitItemCount)
{
	OnPlayerTaskItemCountChangedDelegate.Broadcast(CommitItemCount);
	// ����Ƿ�����ύ����
	if (CanCurrentTaskCommit())
	{
		OnPlayerTaskCommitDelegate.Broadcast(true);
	}
}

void UPlayerTaskComponent::InitCommitItemCount()
{
	// ֻ���ڽ��ܵ�����֮�����

	if (IsKillingTask())
	{
		CommitItemCount = 0;
	}
	else if (IsCommittingTask())
	{
		// �ύ���񣬼����Ʒ����������û������Ҫ�����Ʒ
		if (AEnhoneyPlayerState* EnhoneyPlayerState = Cast<AEnhoneyPlayerState>(GetOwner()))
		{
			if (UInventoryComponent* PlayerInventoryComp = EnhoneyPlayerState->GetInventoryComponent())
			{
				CommitItemCount = PlayerInventoryComp->GetItemNum(CurrentTask.ItemTypeToCommit);
			}
		}
	}

}


