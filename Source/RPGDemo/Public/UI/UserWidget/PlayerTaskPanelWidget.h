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
	// �л����������ʾ/����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ShowOrHideTaskPanel();

	// �ӵ���������
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnNewTaskReceived(const FPlayerTaskConfigItem& TaskConfigInfo, int32 CurrentCount, int32 ItemNumToCommit);

	// �����ύ����ȡ��
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTaskCommittedOrCanceled();

	// ������Ʒ�����ı�
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTaskItemCountChanged(int32 NewCount);

	// ������ύ--�������ʾ�ģ���㣬�Լ���ʾ��Ϣ��
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnTaskCanCommit(bool bCanCommit);


protected:
	// ���ػ�����ʾ�������
	UPROPERTY(BlueprintReadOnly)
	bool bIsTaskPanelVisible = false;

	// ��ǰ�Ƿ�������
	UPROPERTY(BlueprintReadOnly)
	bool bHasTask = false;

	// ��������
	UPROPERTY(BlueprintReadOnly)
	FName CurrentTaskName = TEXT("");

	// ����ͼ��
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTexture2D> CurrentTaskIcon = nullptr;

	// ��������
	UPROPERTY(BlueprintReadOnly)
	FString CurrentTaskDesc = TEXT("");

	// ��Ҫ����Ʒ/��ɱ����
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentNeedItemNum = 0;

	// ��ǰ����ɵ���Ʒ/��ɱ����
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentFinishedItemNum = 0;
	
};
