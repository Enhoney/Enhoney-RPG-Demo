// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnhoneyTaskSystem/TaskInfoConfigDataAsset.h"
#include "PlayerTaskComponent.generated.h"

// ������ύ���߲����ύ״̬�仯��������ʾ��ҵ�ǰ������Ⱥ��ύ��ť�Ŀ��ÿ���
DECLARE_MULTICAST_DELEGATE_OneParam(FCanPlayerTaskCommitSignature, bool /** bCanCommit*/);

// �ӵ����µ�����
DECLARE_MULTICAST_DELEGATE_ThreeParams(FNewTaskSignature, const FPlayerTaskConfigItem& /** TaskConfigInfo*/, int32 /** CurrentCount*/, int32 /** ItemNumToCommit*/);

// �ύ����ȡ���˵�ǰ���񣨻�����һ��ʼɶ����û�е�ʱ��
DECLARE_MULTICAST_DELEGATE(FFinishOrCancelTaskSignature);

// ������Ʒ�����ı�
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerTaskItemCountChangedSignature, int32 /**NewCount*/);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGDEMO_API UPlayerTaskComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerTaskComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ��ȡ��ǰ����
	void GetCurrentTask(FGameplayTag& OutCurrentTaskType) const;

	// ��ǰ�Ƿ���������ִ��
	bool HasTaskInProgress() const;
	
	// �Ƿ�Ϊ��ɱ����
	bool IsKillingTask() const;

	// �Ƿ�Ϊ�ύ����
	bool IsCommittingTask() const;

	// ��ɱ�ĵ����Ƿ�Ϊ����Ҫ��ĵ���
	bool IsKillingTheTaskEnemy(const FGameplayTag& InEnemyType) const;

	// ��ȡ����Ʒ�Ƿ�Ϊ����Ҫ�����Ʒ
	bool IsGettingTheTaskItem(const FGameplayTag& InItemType) const;

	// ��鵱ǰ�����Ƿ�����ύ
	bool CanCurrentTaskCommit() const;

	// ��鵱ǰ�����Ƿ����ȡ��--����Ǯ������
	bool CanCurrentTaskCancel() const;

	// ��������������
	void TryRequestNewTask(const FGameplayTag& InTaskType);

	// �����ύ����--����ͻ��˵��õ�
	void TryCommitTask();

	// ����ȡ������--����ͻ��˵��õ�
	void TryCancelTask();

	// ���������ύ��Ʒ���߻�ɱ����--���ֻ���ڷ���˵���
	void TryAddCommitItemCount(const FGameplayTag& InItemType, int32 InKillingNum = 1);

	// �㲥һ�γ�ʼ״̬
	void BroadcastInitialTaskState();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnRep_CurrentTask(FPlayerTask OldPlayerTask);

	UFUNCTION()
	void OnRep_CommitItemCount(int32 OldCommitItemCount);

	// ��������������
	UFUNCTION(Server, Reliable)
	void ServerTryRequestNewTask(const FGameplayTag& InTaskType);

	// �����ύ����
	UFUNCTION(Server, Reliable)
	void ServerTryCommitTask();

	// ����ȡ������
	UFUNCTION(Server, Reliable)
	void ServerTryCancelTask();

	// ��ʼ����Ʒ����--���뵽������ʱ����
	void InitCommitItemCount();

public:
	// ��ǰ�����Ƿ�����ύ
	FCanPlayerTaskCommitSignature OnPlayerTaskCommitDelegate;

	// �������µ�����
	FNewTaskSignature OnNewTaskDelegate;

	// �����ύ����ȡ��֮��Ļص�
	FFinishOrCancelTaskSignature OnFinishOrCancelTaskDelegate;

	// ��ɱ���仯���߿��ύ��Ʒ���仯
	FPlayerTaskItemCountChangedSignature OnPlayerTaskItemCountChangedDelegate;


private:
	// ��ǰִ�е�������ֻ�������롢�ύ��ȡ����ʱ��
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentTask, Category = "PlayerTask")
	FPlayerTask CurrentTask;

	// ��ɱ���������ύ����Ʒ����
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CommitItemCount, Category = "PlayerTask")
	int32 CommitItemCount = 0;

		
};
