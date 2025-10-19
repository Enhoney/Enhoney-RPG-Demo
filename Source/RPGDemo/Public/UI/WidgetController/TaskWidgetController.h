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

// �鿴ѡ�е�����--�����Ƿ�ѡ��
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTaskSelectedChangedSignature, const FGameplayTag&, SelectedTaskType);

// �鿴ѡ�е�����--������ϸ��Ϣ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTaskDetailInfoChangedSignature, const FPlayerTaskConfigItem&, SelectedTaskInfo, UTexture2D*, RewardItemIcon);

// ��ǰ�Ƿ�������--��ϵ��������ť����ʾ�Ϳ���״̬
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerTaskStatusChangedSignature, bool, bAvailableOrSubmitted);

// ���µ�ǰ����ͼ��
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCurrentTaskChangedSignature, UTexture2D*, NewTaskIcon);



/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class RPGDEMO_API UTaskWidgetController : public UEnhoneyWidgetControllerBase
{
	GENERATED_BODY()

public:
	// �������������壬��һ���ǳ���Ҫ������UI������WidgetController��һ��Ҫ����PlayerTaskWidget
	void SetPlayerTaskWidget(UPlayerTaskPanelWidget* InPlayerTaskWidget);

	// ��ʼ������NPC
	void InitializeForTaskNPC();

	// �����񽻻����
	void OpenTaskInteractPanel();

	// �ر����񽻻����
	UFUNCTION(BlueprintCallable, Category = "TaskWidgetController")
	void CloseTaskInteractPanel();

	// ��ʾ���������������--Ҫ�󶨵�PlayerController�������¼���
	void ShowOrHidePlayerTaskPanel();

	// �鿴ѡ�е�����
	UFUNCTION(BlueprintCallable, Category = "TaskWidgetController")
	void ViewSelectedTask(const FGameplayTag& InSelectedTaskType);

	// ��������
	UFUNCTION(BlueprintCallable, Category = "TaskWidgetController")
	void AcceptCurrentTask();

	// �ύ����
	UFUNCTION(BlueprintCallable, Category = "TaskWidgetController")
	void SubmitCurrentTask();

	// ȡ������
	UFUNCTION(BlueprintCallable, Category = "TaskWidgetController")
	void CancelCurrentTask();

	virtual void BroadcastInitialValue() override;

	virtual void BindCallbacksToDependiencies() override;

private:
	void CheckIfCanAcceptTask();

public:
	// ѡ��Ҫ�鿴������--����ѡ�п�
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnTaskSelectedChangedSignature OnTaskSelectedChangedDelegate;

	// ѡ��Ҫ�鿴������--����������ϸ��Ϣ
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnTaskDetailInfoChangedSignature OnTaskDetailInfoChangedDelegate;

	// ��ǰ�Ƿ�������--��ǰ�����Ƿ�����ύ
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnPlayerTaskStatusChangedSignature OnTaskCanSubmitDelegate;

	// ��ǰ�Ƿ�������--��ǰ�����Ƿ����ȡ��
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnPlayerTaskStatusChangedSignature OnTaskCanCancelDelegate;

	// ��ǰ�Ƿ�������--�Ƿ���Խ�ȡ������
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnPlayerTaskStatusChangedSignature OnTaskCanAcceptDelegate;

	// ��ǰ����ı�--��������ͼ�꣨�����ڴ����񽻻��������������Լ�ȡ��֮��ʵʱ���µ�ǰ����ͼ�꣩
	UPROPERTY(BlueprintAssignable, Category = "TaskWidgetController")
	FOnCurrentTaskChangedSignature OnCurrentTaskChangedDelegate;

private:
	// ���������� ʵ��
	UPROPERTY()
	TObjectPtr<UPlayerTaskPanelWidget> PlayerTaskWidget;

	// ��ǰ�鿴������
	UPROPERTY()
	FGameplayTag CurrentSelectedTaskType = FGameplayTag();

	// ���񽻻������
	UPROPERTY(EditDefaultsOnly, Category = "TaskWidgetController")
	TSubclassOf<UTaskIntreactPanelWidget> TaskInteractPanelWidgetClass;

	// ���񽻻�������ʵ��
	UPROPERTY()
	TObjectPtr<UTaskIntreactPanelWidget> TaskInteractPanelWidget;

	// ����NPCʵ��
	UPROPERTY()
	TWeakObjectPtr<AVillageHeadCharacter> VillageHead;
};
