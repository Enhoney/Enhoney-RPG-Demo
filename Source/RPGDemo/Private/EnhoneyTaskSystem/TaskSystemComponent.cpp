// Copyright Enhoney.


#include "EnhoneyTaskSystem/TaskSystemComponent.h"
#include "Net/UnrealNetwork.h"



UTaskSystemComponent::UTaskSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UTaskSystemComponent::IsTaskValid(const FGuid& InTaskId) const
{
	for (const FGuid& TaskId : CurrentTaskIds)
	{
		if(InTaskId == TaskId)
		{
			return true;
		}
	}
	return false;
}

bool UTaskSystemComponent::RequestNewTask(const FGameplayTag& InTaskType, FPlayerTask& OutPlayerTask)
{
	if (InTaskType.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("TaskType"))))
	{
		checkf(TaskInfoConfigDataAsset, TEXT("TaskInfoDataAsset is not valid, please check its config!"));
		if (TaskInfoConfigDataAsset->CreateNewTask(OutPlayerTask, InTaskType))
		{
			CurrentTaskIds.Add(OutPlayerTask.TaskId);
			return true;
		}
	}
	return false;
}

bool UTaskSystemComponent::CancelOrFinishTask(const FGuid& InTaskId)
{
	if(IsTaskValid(InTaskId))
	{
		CurrentTaskIds.Remove(InTaskId);
		return true;
	}
	return false;
}

void UTaskSystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UTaskSystemComponent, CurrentTaskIds);
}

void UTaskSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// TODO : 读档的时候，重新载入所有的任务Id
}

