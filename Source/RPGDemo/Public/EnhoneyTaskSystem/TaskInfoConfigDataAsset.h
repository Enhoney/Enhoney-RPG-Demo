// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GameplayTagContainer.h"
#include "TaskInfoConfigDataAsset.generated.h"

class UTexture2D;

#pragma region PlayerTask
// 任务
USTRUCT(BlueprintType)
struct RPGDEMO_API FPlayerTask : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:
	FPlayerTask() {}

	FPlayerTask(FGuid InTaskId, const FGameplayTag& InItemTypeToCommit, int32 InItemNumToCommit)
		: TaskId(InTaskId), ItemTypeToCommit(InItemTypeToCommit), ItemNumToCommit(InItemNumToCommit)
	{

	}

	bool operator==(const FPlayerTask& InOtherPlayerTask) const
	{
		return TaskId == InOtherPlayerTask.TaskId;
	}
public:
	// 任务的Id号
	UPROPERTY(BlueprintReadOnly, Category = "PlayerTask")
	FGuid TaskId = FGuid();

	// 任务类型--（Task.Commit.xxx或者Task.Kill.xxx)
	UPROPERTY(BlueprintReadOnly, Category = "PlayerTask")
	FGameplayTag TaskType = FGameplayTag();

	// 要击杀的敌人类型或者要提交的任务物品
	UPROPERTY(BlueprintReadOnly, Category = "PlayerTask")
	FGameplayTag ItemTypeToCommit = FGameplayTag();

	// 要击杀的敌人数量或者要提交的物品数量
	UPROPERTY(BlueprintReadOnly, Category = "PlayerTask")
	int32 ItemNumToCommit = 0;

};

// 任务数组
USTRUCT(BlueprintType)
struct RPGDEMO_API FPlayerTaskContainer : public FFastArraySerializer
{
	GENERATED_BODY()
public:
	FPlayerTaskContainer() {}

	FPlayerTaskContainer(int32 InContainerCapacity)
	{
		Items.Reserve(InContainerCapacity);
	}

	// 必须实现的函数--网络复制
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPlayerTask, FPlayerTaskContainer>(Items, DeltaParms, *this);
	}

	/** 业务逻辑函数*/
	// 是否为空
	bool IsEmpty() const
	{
		return Items.IsEmpty();
	}
	// 添加任务
	bool AddTask(const FPlayerTask& PlayerTaskToAdd)
	{
		Items.AddUnique(PlayerTaskToAdd);
		MarkArrayDirty();
		return true;
	}
	// 查询任务
	bool GetTaskById(FGuid InTaskId, FPlayerTask& OutTask)
	{
		for (const FPlayerTask& TmpTask : Items)
		{
			if (InTaskId == TmpTask.TaskId)
			{
				OutTask = TmpTask;
				return true;
			}
		}
		return false;
	}
	// 移除任务--传递Guid
	bool RemoveTask(FGuid InTaskId)
	{
		FPlayerTask TaskToRemove;

		if (GetTaskById(InTaskId, TaskToRemove))
		{
			Items.Remove(TaskToRemove);
			MarkArrayDirty();
			return true;
		}
		return false;
	}
	// 移除任务--传递任务本身
	bool RemoveTask(const FPlayerTask& InTaskToRemove)
	{
		if (Items.Contains(InTaskToRemove))
		{
			Items.Remove(InTaskToRemove);
			MarkArrayDirty();
			return true;
		}
		return false;
	}
public:

	// 任务数组
	UPROPERTY(BlueprintReadOnly, Category = "PlayerTasks")
	TArray<FPlayerTask> Items;

};

template<>
struct TStructOpsTypeTraits<FPlayerTaskContainer> : public TStructOpsTypeTraitsBase2<FPlayerTaskContainer>
{
	enum
	{
		WithNetDeltaSerializer = true, // 启用自定义Delta序列化
	};
};

#pragma endregion PlayerTask



// 任务信息配置
USTRUCT(BlueprintType)
struct RPGDEMO_API FPlayerTaskConfigItem
{
	GENERATED_BODY()
public:
	// 创建一个新任务
	void CreatePlayerTask(FPlayerTask& OutPlayerTask) const
	{
		OutPlayerTask.TaskId = FGuid::NewGuid();
		OutPlayerTask.TaskType = TaskType;
		OutPlayerTask.ItemTypeToCommit = ItemTypeToCommit;
		// 提交数量为最小和最大值之间的随机值
		OutPlayerTask.ItemNumToCommit = FMath::RandRange(MinCommitNum, MaxCommitNum);
	}

	bool operator==(const FPlayerTaskConfigItem& InOtherTaskConfig) const
	{
		return TaskType.MatchesTagExact(InOtherTaskConfig.TaskType);
	}

public:
	// 任务类型--击杀任务或者提交类任务（Task.Commit.xxx或者Task.Kill.xxx)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	FGameplayTag TaskType = FGameplayTag();

	// 要击杀的敌人或者提交的物品类型
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	FGameplayTag ItemTypeToCommit = FGameplayTag();

	// 最小提交数量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	int32 MinCommitNum = 1;
	// 最大提交数量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	int32 MaxCommitNum = 1;

	// 任务名称
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	FName TaskName = TEXT("");

	// 任务描述
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	FString TaskDesc = TEXT("");

	// 任务图标
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	TObjectPtr<UTexture2D> TaskIcon = nullptr;

	/** 任务奖励*/
	// 奖励的金币数量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	int64 RewardCoinNum = 0;
	// 奖励的经验值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	int32 RewardExp = 0;
	// 奖励的物品及其数量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	FGameplayTag RewardItemTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	int32 RewardItemNum = 0;
	// 取消惩罚的金币数量
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	int64 CancelPenaltyCoinNum = 0;
};

/**
 * 
 */
UCLASS()
class RPGDEMO_API UTaskInfoConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	// 获取任务配置信息
	UFUNCTION(BlueprintCallable, Category = "PlayerTaskConfigAsset")
	bool GetPlayerTaskConfigInfo(const FGameplayTag& InTaskType, FPlayerTaskConfigItem& OutTaskConfig) const;

	// 生成一个新任务
	UFUNCTION(BlueprintCallable, Category = "PlayerTaskConfigAsset")
	bool CreateNewTask(FPlayerTask& OutPlayerTask, const FGameplayTag& InTaskName) const;
private:
	UPROPERTY(EditDefaultsOnly, Category = "PlayerTaskConfigAsset")
	TArray<FPlayerTaskConfigItem> PlayerTaskConfig;
	
};
