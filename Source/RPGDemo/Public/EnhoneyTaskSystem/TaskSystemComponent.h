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

	// ����У�飬�鿴�����Ƿ��Ѵ��ڣ��ύ�����ʱ��ֻ��Ҫ������У��һ��
	bool IsTaskValid(const FGuid& InTaskId) const;

	// ���������񣬲���¼����Id
	bool RequestNewTask(const FGameplayTag& InTaskType, FPlayerTask& OutPlayerTask);

	// ȡ�����񣬲��Ƴ�����Id
	bool CancelOrFinishTask(const FGuid& InTaskId);

	// ��ȡ������Ϣ����
	FORCEINLINE UTaskInfoConfigDataAsset* GetTaskInfoConfigDataAsset() const { return TaskInfoConfigDataAsset; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// ������Ϣ���������ʲ�
	UPROPERTY(EditDefaultsOnly, Category = "TaskConfig")
	TObjectPtr<UTaskInfoConfigDataAsset> TaskInfoConfigDataAsset = nullptr;

	// �洢��ҵ�ǰ������Id������У�������Ƿ��Ѵ���
	UPROPERTY(Replicated)
	TArray<FGuid> CurrentTaskIds;

		
};
