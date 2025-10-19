// Copyright Enhoney.


#include "UI/UserWidget/PlayerTaskPanelWidget.h"
#include "TaskInfoConfigDataAsset.h"

void UPlayerTaskPanelWidget::ShowOrHideTaskPanel_Implementation()
{
	bIsTaskPanelVisible = !bIsTaskPanelVisible;

	// 剩下的就是在蓝图播放UI动画了
}

void UPlayerTaskPanelWidget::OnNewTaskReceived_Implementation(const FPlayerTaskConfigItem& TaskConfigInfo, int32 CurrentCount, int32 ItemNumToCommit)
{
	bHasTask = true;

	// 更新任务信息
	CurrentTaskName = TaskConfigInfo.TaskName;
	CurrentTaskIcon = TaskConfigInfo.TaskIcon;
	CurrentTaskDesc = TaskConfigInfo.TaskDesc;
	CurrentNeedItemNum = ItemNumToCommit;
	CurrentFinishedItemNum = CurrentCount;

	// 接下来就是在蓝图里面更新UI显示了
}

void UPlayerTaskPanelWidget::OnTaskCommittedOrCanceled_Implementation()
{
	bHasTask = false;

	// 清空任务信息
	CurrentTaskName = TEXT("");
	CurrentTaskIcon = nullptr;
	CurrentTaskDesc = TEXT("");
	CurrentNeedItemNum = 0;
	CurrentFinishedItemNum = 0;

	// 接下来就是在蓝图里面更新UI显示了
}

void UPlayerTaskPanelWidget::OnTaskItemCountChanged_Implementation(int32 NewCount)
{
	CurrentFinishedItemNum = NewCount;

	// 接下来就是在蓝图里面更新UI显示了--比如进度
}
