// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnhoneyUserWidgetBase.h"
#include "PlayerTaskPanelWidget.generated.h"

struct FPlayerTaskConfigItem;

/**
 * 
 */
UCLASS(BlueprintType)
class RPGDEMO_API UPlayerTaskPanelWidget : public UEnhoneyUserWidgetBase
{
	GENERATED_BODY()

public:
	// 切换任务面板显示/隐藏
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowOrHideTaskPanel();

	// 接到了新任务
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnNewTaskReceived(const FPlayerTaskConfigItem& TaskConfigInfo, int32 CurrentCount, int32 ItemNumToCommit);

	// 任务提交或者取消
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTaskCommittedOrCanceled();

	// 任务物品数量改变
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTaskItemCountChanged(int32 NewCount);

	// 任务可提交--给玩家提示的（红点，以及提示信息）
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnTaskCanCommit(bool bCanCommit);


protected:
	// 隐藏还是显示任务面板
	UPROPERTY(BlueprintReadOnly)
	bool bIsTaskPanelVisible = false;

	// 当前是否有任务
	UPROPERTY(BlueprintReadOnly)
	bool bHasTask = false;

	// 任务名称
	UPROPERTY(BlueprintReadOnly)
	FName CurrentTaskName = TEXT("");

	// 任务图标
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTexture2D> CurrentTaskIcon = nullptr;

	// 任务描述
	UPROPERTY(BlueprintReadOnly)
	FString CurrentTaskDesc = TEXT("");

	// 需要的物品/击杀数量
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentNeedItemNum = 0;

	// 当前已完成的物品/击杀数量
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentFinishedItemNum = 0;
	
};
