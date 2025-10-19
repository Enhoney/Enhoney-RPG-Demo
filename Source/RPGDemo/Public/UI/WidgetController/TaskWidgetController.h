// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyWidgetControllerBase.h"
#include "GameplayTagContainer.h"
#include "TaskWidgetController.generated.h"

class UPlayerTaskPanelWidget;
class UTaskIntreactPanelWidget;
class AVillageHeadCharacter;

struct FPlayerTaskConfigItem;

// 查看选中的任务--任务是否选中
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskSelectedChangedSignature, const FGameplayTag&, SelectedTaskType);

// 查看选中的任务--任务详细信息
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTaskDetailInfoChangedSignature, const FPlayerTaskConfigItem&, SelectedTaskInfo, UTexture2D*, RewardItemIcon);

// 当前是否有任务--关系到三个按钮的显示和可用状态
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerTaskStatusChangedSignature, bool, bAvailableOrSubmitted);

// 更新当前任务图标
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentTaskChangedSignature, UTexture2D*, NewTaskIcon);



/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class RPGDEMO_API UTaskWidgetController : public UEnhoneyWidgetControllerBase
{
	GENERATED_BODY()

public:
	// 设置玩家任务面板，这一步非常重要，在主UI设置完WidgetController后，一定要设置PlayerTaskWidget
	void SetPlayerTaskWidget(UPlayerTaskPanelWidget* InPlayerTaskWidget);

	// 初始化任务NPC
	void InitializeForTaskNPC();

	// 打开任务交互面板
	void OpenTaskInteractPanel();

	// 关闭任务交互面板
	UFUNCTION(BlueprintCallable, Category = "TaskWidgetController")
	void CloseTaskInteractPanel();

	// 显示或者隐藏任务面板--要绑定到PlayerController的输入事件上
	void ShowOrHidePlayerTaskPanel();

	// 查看选中的任务
	UFUNCTION(BlueprintCallable, Category = "TaskWidgetController")
	void ViewSelectedTask(const FGameplayTag& InSelectedTaskType);

	// 接受任务
	UFUNCTION(BlueprintCallable, Category = "TaskWidgetController")
	void AcceptCurrentTask();

	// 提交任务
	UFUNCTION(BlueprintCallable, Category = "TaskWidgetController")
	void SubmitCurrentTask();

	// 取消任务
	UFUNCTION(BlueprintCallable, Category = "TaskWidgetController")
	void CancelCurrentTask();

	virtual void BroadcastInitialValue() override;

	virtual void BindCallbacksToDependiencies() override;

private:
	void CheckIfCanAcceptTask();

public:
	// 选中要查看的任务--处理选中框
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnTaskSelectedChangedSignature OnTaskSelectedChangedDelegate;

	// 选中要查看的任务--处理任务详细信息
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnTaskDetailInfoChangedSignature OnTaskDetailInfoChangedDelegate;

	// 当前是否有任务--当前任务是否可以提交
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnPlayerTaskStatusChangedSignature OnTaskCanSubmitDelegate;

	// 当前是否有任务--当前任务是否可以取消
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnPlayerTaskStatusChangedSignature OnTaskCanCancelDelegate;

	// 当前是否有任务--是否可以接取新任务
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnPlayerTaskStatusChangedSignature OnTaskCanAcceptDelegate;

	// 当前任务改变--更新任务图标（用于在打开任务交互、接受新任务以及取消之后，实时更新当前任务图标）
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnCurrentTaskChangedSignature OnCurrentTaskChangedDelegate;

private:
	// 玩家任务面板 实例
	UPROPERTY()
	TObjectPtr<UPlayerTaskPanelWidget> PlayerTaskWidget;

	// 当前查看的任务
	UPROPERTY()
	FGameplayTag CurrentSelectedTaskType = FGameplayTag();

	// 任务交互面板类
	UPROPERTY(EditDefaultsOnly, Category = "TaskWidgetController")
	TSubclassOf<UTaskIntreactPanelWidget> TaskInteractPanelWidgetClass;

	// 任务交互面板对象实例
	UPROPERTY()
	TObjectPtr<UTaskIntreactPanelWidget> TaskInteractPanelWidget;

	// 任务NPC实例
	UPROPERTY()
	TWeakObjectPtr<AVillageHeadCharacter> VillageHead;
};
