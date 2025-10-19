// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Character/EnhoneyCharacterBase.h"
#include "NPCCharacterBase.generated.h"

class UWidgetComponent;
class USphereComponent;
class UInventoryComponent;

/**
 * 
 */
UCLASS(Abstract)
class RPGDEMO_API ANPCCharacterBase : public AEnhoneyCharacterBase
{
	GENERATED_BODY()

public:
	ANPCCharacterBase();

	// ��ʾ���ֵ���ײ����ص���ʼ�ص�
	UFUNCTION(BlueprintNativeEvent)
	void OnNPCNameSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ��ʾ���ֵ���ײ����ص������ص�
	UFUNCTION(BlueprintNativeEvent)
	void OnNPCNameSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// ��ʾ������ʾ����ײ����ص���ʼ�ص�
	UFUNCTION(BlueprintNativeEvent)
	void OnNPCInteractSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ��ʾ������ʾ����ײ����ص������ص�
	UFUNCTION(BlueprintNativeEvent)
	void OnNPCInteractSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	// �˳�����״̬���رս���UI��ʱ���ֶ�����
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ExitInteractState();

	// �����ֳ�Ա�����Ը���
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;

	// �������Ļص����������ʵ��--������ڿͻ��˽��лص���
	virtual void OnInteract(AActor* InInteractivePlayer);

	//����NPC״̬�ĺ������ڷ������ϵ���
	UFUNCTION(BlueprintCallable)
	void SetNPCIdleState(bool bNewIdleState);

	// ���õ�ǰ��������ң��ڷ������ϵ���
	UFUNCTION(BlueprintCallable)
	void SetInteractivePlayer(AActor* InInteractivePlayer);

protected:
	// ��¼��ǰ���������
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> InteractivePlayer;

	// ��¼NPC״̬--�Ƿ���У��ڶ�����Ϸ�У�ͬһʱ��ֻ����һ�������NPC����
	UPROPERTY(Replicated)
	bool bIsNPCIdle = true;

	// ��ʾNPC���Ƶ����
	UPROPERTY(VisibleAnywhere, Category = "NPCWidget|Name")
	TObjectPtr<UWidgetComponent> NPCNameWidgetComponent;

	// ��ʾNPC���������
	UPROPERTY(VisibleAnywhere, Category = "NPCWidget|Interact")
	TObjectPtr<UWidgetComponent> NPCInteractWidgetComponent;

	// NPC����
	UPROPERTY(EditAnywhere, Category = "NPCWidget|Name")
	FName NPCName;

	// NPC������ʾ
	UPROPERTY(EditAnywhere, Category = "NPCWidget|Interact")
	FString NPCInteractText;

	// ��ʾ���ֵ���ײ���
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> NPCNameSphereComponent;

	// ��ʾ������ʾ����ײ���
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> NPCInteractSphereComponent;

	
};
