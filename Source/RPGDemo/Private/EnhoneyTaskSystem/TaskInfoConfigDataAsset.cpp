// Copyright Enhoney.


#include "TaskInfoConfigDataAsset.h"
#include "EnhoneyLogChannel.h"

bool UTaskInfoConfigDataAsset::GetPlayerTaskConfigInfo(const FGameplayTag& InTaskType, FPlayerTaskConfigItem& OutTaskConfig) const
{
	for (const FPlayerTaskConfigItem& TmpTaskConfig : PlayerTaskConfig)
	{
		if (InTaskType.MatchesTagExact(TmpTaskConfig.TaskType))
		{
			OutTaskConfig = TmpTaskConfig;
			return true;
		}
	}

	UE_LOG(LogEnhoney, Error, TEXT("[%s]: Taks with Tag--[%s] is not valid, please check your asset config!!!"), 
		*FString(__FUNCTION__), *(InTaskType.ToString()));
	return false;
}

bool UTaskInfoConfigDataAsset::CreateNewTask(FPlayerTask& OutPlayerTask, const FGameplayTag& InTaskName) const
{
	if (!PlayerTaskConfig.IsEmpty())
	{
		for (const FPlayerTaskConfigItem& TaskConfig : PlayerTaskConfig)
		{
			if (InTaskName.MatchesTagExact(TaskConfig.TaskType))
			{
				// ’“µΩTaskConfig
				TaskConfig.CreatePlayerTask(OutPlayerTask);
				return true;
			}
		}
	}
	return false;
}
