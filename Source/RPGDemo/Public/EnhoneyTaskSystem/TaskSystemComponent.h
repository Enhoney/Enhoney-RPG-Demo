// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TaskInfoConfigDataAsset.h"
#include "EnhoneyGameplayTags.h"
#include "TaskSystemComponent.generated.h"

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGDEMO_API UTaskSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTaskSystemComponent();

	// 任务校验，查看任务是否已存在，提交任务的时候只需要在这里校验一下
	bool IsTaskValid(const FGuid& InTaskId) const;

	// 请求新任务，并记录任务Id
	bool RequestNewTask(const FGameplayTag& InTaskType, FPlayerTask& OutPlayerTask);

	// 取消任务，并移除任务Id
	bool CancelOrFinishTask(const FGuid& InTaskId);

	// 获取任务信息配置
	FORCEINLINE UTaskInfoConfigDataAsset* GetTaskInfoConfigDataAsset() const { return TaskInfoConfigDataAsset; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// 任务信息配置数据资产
	UPROPERTY(EditDefaultsOnly, Category = "TaskConfig")
	TObjectPtr<UTaskInfoConfigDataAsset> TaskInfoConfigDataAsset = nullptr;

	// 存储玩家当前的任务Id，用于校验任务是否已存在
	UPROPERTY(Replicated)
	TArray<FGuid> CurrentTaskIds;

		
};
