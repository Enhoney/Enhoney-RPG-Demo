// Copyright Enhoney.


#include "UI/UserWidget/PlayerTaskPanelWidget.h"
#include "TaskInfoConfigDataAsset.h"

void UPlayerTaskPanelWidget::ShowOrHideTaskPanel_Implementation()
{
	bIsTaskPanelVisible = !bIsTaskPanelVisible;

	// ʣ�µľ�������ͼ����UI������
}

void UPlayerTaskPanelWidget::OnNewTaskReceived_Implementation(const FPlayerTaskConfigItem& TaskConfigInfo, int32 CurrentCount, int32 ItemNumToCommit)
{
	bHasTask = true;

	// ����������Ϣ
	CurrentTaskName = TaskConfigInfo.TaskName;
	CurrentTaskIcon = TaskConfigInfo.TaskIcon;
	CurrentTaskDesc = TaskConfigInfo.TaskDesc;
	CurrentNeedItemNum = ItemNumToCommit;
	CurrentFinishedItemNum = CurrentCount;

	// ��������������ͼ�������UI��ʾ��
}

void UPlayerTaskPanelWidget::OnTaskCommittedOrCanceled_Implementation()
{
	bHasTask = false;

	// ���������Ϣ
	CurrentTaskName = TEXT("");
	CurrentTaskIcon = nullptr;
	CurrentTaskDesc = TEXT("");
	CurrentNeedItemNum = 0;
	CurrentFinishedItemNum = 0;

	// ��������������ͼ�������UI��ʾ��
}

void UPlayerTaskPanelWidget::OnTaskItemCountChanged_Implementation(int32 NewCount)
{
	CurrentFinishedItemNum = NewCount;

	// ��������������ͼ�������UI��ʾ��--�������
}
