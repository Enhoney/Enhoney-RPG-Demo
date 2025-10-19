// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "GameplayTagContainer.h"
#include "TaskInfoConfigDataAsset.generated.h"

class UTexture2D;

#pragma region PlayerTask
// ����
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
	// �����Id��
	UPROPERTY(BlueprintReadOnly, Category = "PlayerTask")
	FGuid TaskId = FGuid();

	// ��������--��Task.Commit.xxx����Task.Kill.xxx)
	UPROPERTY(BlueprintReadOnly, Category = "PlayerTask")
	FGameplayTag TaskType = FGameplayTag();

	// Ҫ��ɱ�ĵ������ͻ���Ҫ�ύ��������Ʒ
	UPROPERTY(BlueprintReadOnly, Category = "PlayerTask")
	FGameplayTag ItemTypeToCommit = FGameplayTag();

	// Ҫ��ɱ�ĵ�����������Ҫ�ύ����Ʒ����
	UPROPERTY(BlueprintReadOnly, Category = "PlayerTask")
	int32 ItemNumToCommit = 0;

};

// ��������
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

	// ����ʵ�ֵĺ���--���縴��
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FPlayerTask, FPlayerTaskContainer>(Items, DeltaParms, *this);
	}

	/** ҵ���߼�����*/
	// �Ƿ�Ϊ��
	bool IsEmpty() const
	{
		return Items.IsEmpty();
	}
	// �������
	bool AddTask(const FPlayerTask& PlayerTaskToAdd)
	{
		Items.AddUnique(PlayerTaskToAdd);
		MarkArrayDirty();
		return true;
	}
	// ��ѯ����
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
	// �Ƴ�����--����Guid
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
	// �Ƴ�����--����������
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

	// ��������
	UPROPERTY(BlueprintReadOnly, Category = "PlayerTasks")
	TArray<FPlayerTask> Items;

};

template<>
struct TStructOpsTypeTraits<FPlayerTaskContainer> : public TStructOpsTypeTraitsBase2<FPlayerTaskContainer>
{
	enum
	{
		WithNetDeltaSerializer = true, // �����Զ���Delta���л�
	};
};

#pragma endregion PlayerTask



// ������Ϣ����
USTRUCT(BlueprintType)
struct RPGDEMO_API FPlayerTaskConfigItem
{
	GENERATED_BODY()
public:
	// ����һ��������
	void CreatePlayerTask(FPlayerTask& OutPlayerTask) const
	{
		OutPlayerTask.TaskId = FGuid::NewGuid();
		OutPlayerTask.TaskType = TaskType;
		OutPlayerTask.ItemTypeToCommit = ItemTypeToCommit;
		// �ύ����Ϊ��С�����ֵ֮������ֵ
		OutPlayerTask.ItemNumToCommit = FMath::RandRange(MinCommitNum, MaxCommitNum);
	}

	bool operator==(const FPlayerTaskConfigItem& InOtherTaskConfig) const
	{
		return TaskType.MatchesTagExact(InOtherTaskConfig.TaskType);
	}

public:
	// ��������--��ɱ��������ύ������Task.Commit.xxx����Task.Kill.xxx)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	FGameplayTag TaskType = FGameplayTag();

	// Ҫ��ɱ�ĵ��˻����ύ����Ʒ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	FGameplayTag ItemTypeToCommit = FGameplayTag();

	// ��С�ύ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	int32 MinCommitNum = 1;
	// ����ύ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	int32 MaxCommitNum = 1;

	// ��������
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	FName TaskName = TEXT("");

	// ��������
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	FString TaskDesc = TEXT("");

	// ����ͼ��
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	TObjectPtr<UTexture2D> TaskIcon = nullptr;

	/** ������*/
	// �����Ľ������
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	int64 RewardCoinNum = 0;
	// �����ľ���ֵ
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	int32 RewardExp = 0;
	// ��������Ʒ��������
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	FGameplayTag RewardItemTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PlayerTaskConfig")
	int32 RewardItemNum = 0;
	// ȡ���ͷ��Ľ������
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
	// ��ȡ����������Ϣ
	UFUNCTION(BlueprintCallable, Category = "PlayerTaskConfigAsset")
	bool GetPlayerTaskConfigInfo(const FGameplayTag& InTaskType, FPlayerTaskConfigItem& OutTaskConfig) const;

	// ����һ��������
	UFUNCTION(BlueprintCallable, Category = "PlayerTaskConfigAsset")
	bool CreateNewTask(FPlayerTask& OutPlayerTask, const FGameplayTag& InTaskName) const;
private:
	UPROPERTY(EditDefaultsOnly, Category = "PlayerTaskConfigAsset")
	TArray<FPlayerTaskConfigItem> PlayerTaskConfig;
	
};
