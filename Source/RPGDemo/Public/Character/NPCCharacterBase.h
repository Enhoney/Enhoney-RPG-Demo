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

	// 显示名字的碰撞体积重叠开始回调
	UFUNCTION(BlueprintNativeEvent)
	void OnNPCNameSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 显示名字的碰撞体积重叠结束回调
	UFUNCTION(BlueprintNativeEvent)
	void OnNPCNameSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 显示交互提示的碰撞体积重叠开始回调
	UFUNCTION(BlueprintNativeEvent)
	void OnNPCInteractSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 显示交互提示的碰撞体积重叠结束回调
	UFUNCTION(BlueprintNativeEvent)
	void OnNPCInteractSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	// 退出交互状态，关闭交互UI的时候手动调用
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void ExitInteractState();

	// 开启分成员的属性复制
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;

	// 处理交互的回调，子类必须实现--这个是在客户端进行回调的
	virtual void OnInteract(AActor* InInteractivePlayer);

	//设置NPC状态的函数，在服务器上调用
	UFUNCTION(BlueprintCallable)
	void SetNPCIdleState(bool bNewIdleState);

	// 设置当前交互的玩家，在服务器上调用
	UFUNCTION(BlueprintCallable)
	void SetInteractivePlayer(AActor* InInteractivePlayer);

protected:
	// 记录当前交互的玩家
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> InteractivePlayer;

	// 记录NPC状态--是否空闲，在多人游戏中，同一时间只能有一个玩家与NPC交互
	UPROPERTY(Replicated)
	bool bIsNPCIdle = true;

	// 显示NPC名称的组件
	UPROPERTY(VisibleAnywhere, Category = "NPCWidget|Name")
	TObjectPtr<UWidgetComponent> NPCNameWidgetComponent;

	// 显示NPC交互的组件
	UPROPERTY(VisibleAnywhere, Category = "NPCWidget|Interact")
	TObjectPtr<UWidgetComponent> NPCInteractWidgetComponent;

	// NPC名称
	UPROPERTY(EditAnywhere, Category = "NPCWidget|Name")
	FName NPCName;

	// NPC交互提示
	UPROPERTY(EditAnywhere, Category = "NPCWidget|Interact")
	FString NPCInteractText;

	// 显示名字的碰撞体积
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> NPCNameSphereComponent;

	// 显示交互提示的碰撞体积
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> NPCInteractSphereComponent;

	
};
