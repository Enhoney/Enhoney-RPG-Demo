// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhoneyTaskSystem/TaskInfoConfigDataAsset.h"
#include "PlayerTaskComponent.generated.h"

// 任务可提交或者不可提交状态变化，用于提示玩家当前任务进度和提交按钮的可用控制
DECLARE_MULTICAST_DELEGATE_OneParam(FCanPlayerTaskCommitSignature, bool /** bCanCommit*/);

// 接到了新的任务
DECLARE_MULTICAST_DELEGATE_ThreeParams(FNewTaskSignature, const FPlayerTaskConfigItem& /** TaskConfigInfo*/, int32 /** CurrentCount*/, int32 /** ItemNumToCommit*/);

// 提交或者取消了当前任务（或者是一开始啥任务都没有的时候）
DECLARE_MULTICAST_DELEGATE(FFinishOrCancelTaskSignature);

// 任务物品数量改变
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerTaskItemCountChangedSignature, int32 /**NewCount*/);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGDEMO_API UPlayerTaskComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerTaskComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 获取当前任务
	void GetCurrentTask(FGameplayTag& OutCurrentTaskType) const;

	// 当前是否有任务在执行
	bool HasTaskInProgress() const;
	
	// 是否为击杀任务
	bool IsKillingTask() const;

	// 是否为提交任务
	bool IsCommittingTask() const;

	// 击杀的敌人是否为任务要求的敌人
	bool IsKillingTheTaskEnemy(const FGameplayTag& InEnemyType) const;

	// 获取的物品是否为任务要求的物品
	bool IsGettingTheTaskItem(const FGameplayTag& InItemType) const;

	// 检查当前任务是否可以提交
	bool CanCurrentTaskCommit() const;

	// 检查当前任务是否可以取消--看看钱够不够
	bool CanCurrentTaskCancel() const;

	// 尝试申请新任务
	void TryRequestNewTask(const FGameplayTag& InTaskType);

	// 尝试提交任务--方便客户端调用的
	void TryCommitTask();

	// 尝试取消任务--方便客户端调用的
	void TryCancelTask();

	// 尝试增加提交物品或者击杀计数--这个只会在服务端调用
	void TryAddCommitItemCount(const FGameplayTag& InItemType, int32 InKillingNum = 1);

	// 广播一次初始状态
	void BroadcastInitialTaskState();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_CurrentTask(FPlayerTask OldPlayerTask);

	UFUNCTION()
	void OnRep_CommitItemCount(int32 OldCommitItemCount);

	// 尝试申请新任务
	UFUNCTION(Server, Reliable)
	void ServerTryRequestNewTask(const FGameplayTag& InTaskType);

	// 尝试提交任务
	UFUNCTION(Server, Reliable)
	void ServerTryCommitTask();

	// 尝试取消任务
	UFUNCTION(Server, Reliable)
	void ServerTryCancelTask();

	// 初始化物品计数--申请到新任务时调用
	void InitCommitItemCount();

public:
	// 当前任务是否可以提交
	FCanPlayerTaskCommitSignature OnPlayerTaskCommitDelegate;

	// 申请了新的任务
	FNewTaskSignature OnNewTaskDelegate;

	// 任务提交或者取消之后的回调
	FFinishOrCancelTaskSignature OnFinishOrCancelTaskDelegate;

	// 击杀数变化或者可提交物品数变化
	FPlayerTaskItemCountChangedSignature OnPlayerTaskItemCountChangedDelegate;


private:
	// 当前执行的任务，它只会在申请、提交和取消的时候
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentTask, Category = "PlayerTask")
	FPlayerTask CurrentTask;

	// 击杀计数或者提交的物品计数
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CommitItemCount, Category = "PlayerTask")
	int32 CommitItemCount = 0;

		
};
