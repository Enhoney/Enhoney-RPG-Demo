// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interaction/PlayerInterface.h"
#include "EnhoneyPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UEnhoneyAbilitySystemComponent;
class UAbilityInputConfig;

struct FInputActionValue;

/**
 * 
 */
UCLASS()
class RPGDEMO_API AEnhoneyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEnhoneyPlayerController();

	void CloseCharacterMenu();

	// 获取NPC交互委托
	FORCEINLINE FOnInteractWithNPCSignature* GetOnInteractWithNPCDelegate() 
	{ 
		return &OnInteractWithNPCDelegate; 
	}
	// 设置是否可以交互
	FORCEINLINE void SetCanInteractWithNPC(bool bInCanInteract)
	{
		bCanInteract = bInCanInteract;
	}

	// 设置部分成员变量的复制
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 购买物品
	UFUNCTION(Server, Reliable)
	void ServerBuyItem(const FGameplayTag& InItemTag, int32 InItemNum);

	// 设置交易者
	UFUNCTION(Server, Reliable)
	void ServerSetTransactionPlayer(AActor* InPlayerCharacter);

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;

	UEnhoneyAbilitySystemComponent* GetEnhoneyAbilitySystemComponent();

private:
	// 常规输入行为
	void Move(const FInputActionValue& InputActionValue);
	void Look(const FInputActionValue& InputActionValue);
	void Jump();
	void StopJumping();
	// 打开角色菜单
	void OpenCharacterMenu();
	// 切武器
	void SwitchWeapon();
	// 打开或者隐藏任务菜单
	void ShowOrHidePlayerTaskPanel();
	// 打开暂停菜单
	void OpenPauseMenu();

	/** Abilities*/
	// 锁敌
	void EnemyTargetLocking();
	// 普通攻击
	void TryActivateAbility_CommonAttack();
	// 与NPC交互
	void TryInteractWithNPC();

	// 技能按钮抬起
	void AbilityInputReleased(FGameplayTag InAbilityInputTag);

private:
	// NPC交互委托
	FOnInteractWithNPCSignature OnInteractWithNPCDelegate;

	// 是否可以交互。这个值是在Server上修改的
	UPROPERTY(Replicated)
	bool bCanInteract = false;

	/** 常规移动和视角*/
	UPROPERTY(EditDefaultsOnly, Category = "Input|Common")
	TObjectPtr<UInputMappingContext> CommonInputMapping;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Common")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Common")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input|Common")
	TObjectPtr<UInputAction> JumpAction;

	// 打开CharacterMenu
	UPROPERTY(EditDefaultsOnly, Category = "Input|Common")
	TObjectPtr<UInputAction> OpenCharacterMenuAction;
	// 打开或者隐藏玩家任务面板
	UPROPERTY(EditDefaultsOnly, Category = "Input|Common")
	TObjectPtr<UInputAction> OpenTaskPanelAction;
	// 切换装备的武器
	UPROPERTY(EditDefaultsOnly, Category = "Input|Common")
	TObjectPtr<UInputAction> SwitchEquippedWeaponAction;
	// 打开暂停菜单
	UPROPERTY(EditDefaultsOnly, Category = "Input|Common")
	TObjectPtr<UInputAction> OpenGamePasueMenuAction;

	// 激活锁敌技能
	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> TargetEnemyLockingAction;
	// 激活普通攻击技能
	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> AbilityAction_CommonAttack;

	// NPC交互
	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UInputAction> InteractWithNPCAction;

	// AbilityInputActinoConfig
	UPROPERTY(EditDefaultsOnly, Category = "Input|Ability")
	TObjectPtr<UAbilityInputConfig> AbilityInputActionConfig;

	UPROPERTY()
	TWeakObjectPtr<UEnhoneyAbilitySystemComponent> ASC;


	bool bIsCharacterMenuOpened = false;

};
