// Copyright Enhoney.


#include "EnhoneyPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "EnhoneyEnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "EnhoneyWeaponComponent.h"

#include "EnhoneyPlayingHUD.h"

#include "EnhoneyPlayerState.h"
#include "PlayerCharacterBase.h"

#include "EnhoneyAbilitySystemComponent.h"
#include "PlayerAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhoneyAbilitySystemLibrary.h"

#include "EnhoneyGameplayTags.h"

#include "PlayerInterface.h"
#include "EnhoneyGameStateBase.h"
#include "Net/UnrealNetwork.h"

#include "TaskWidgetController.h"
#include "CommonAlgorithmLibrary.h"


AEnhoneyPlayerController::AEnhoneyPlayerController()
{
	// 这个一定要开，否则不能接收输入
	PrimaryActorTick.bCanEverTick = true;
}

void AEnhoneyPlayerController::CloseCharacterMenu()
{
	if (bIsCharacterMenuOpened)
	{
		FInputModeGameOnly InputModeData;

		SetInputMode(InputModeData);

		// 单机模式，有暂停，这里就取消暂停
		// 这个函数内部会自己判断NetMode
		SetPause(false);

		// 隐藏鼠标光标
		SetShowMouseCursor(false);

		bIsCharacterMenuOpened = false;
	}
	
}

void AEnhoneyPlayerController::ServerSetTransactionPlayer_Implementation(AActor* InPlayerCharacter)
{
	if (AEnhoneyGameStateBase* GameState = UEnhoneyAbilitySystemLibrary::GetEnhoneyGameState(this))
	{
		GameState->SetTransactionPlayer(InPlayerCharacter);
	}
}

void AEnhoneyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(CommonInputMapping);
	// 获取增强输入子系统
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(CommonInputMapping, 0);
	}

	// 设置游戏输入模式为GameAndUI
	FInputModeGameOnly InputModeData;

	SetInputMode(InputModeData);
}

void AEnhoneyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhoneyEnhancedInputComponent* EnhoneyInputComponent = CastChecked<UEnhoneyEnhancedInputComponent>(InputComponent))
	{
		EnhoneyInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AEnhoneyPlayerController::Move);
		EnhoneyInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AEnhoneyPlayerController::Look);
		EnhoneyInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AEnhoneyPlayerController::Jump);
		EnhoneyInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AEnhoneyPlayerController::StopJumping);

		EnhoneyInputComponent->BindAction(OpenCharacterMenuAction, ETriggerEvent::Triggered, this, &AEnhoneyPlayerController::OpenCharacterMenu);
		EnhoneyInputComponent->BindAction(OpenTaskPanelAction, ETriggerEvent::Completed, this, &AEnhoneyPlayerController::ShowOrHidePlayerTaskPanel);
		EnhoneyInputComponent->BindAction(SwitchEquippedWeaponAction, ETriggerEvent::Completed, this, &AEnhoneyPlayerController::SwitchWeapon);
		EnhoneyInputComponent->BindAction(OpenGamePasueMenuAction, ETriggerEvent::Completed, this, &AEnhoneyPlayerController::OpenPauseMenu);

		EnhoneyInputComponent->BindAction(TargetEnemyLockingAction, ETriggerEvent::Triggered, this, &AEnhoneyPlayerController::EnemyTargetLocking);

		EnhoneyInputComponent->BindAction(AbilityAction_CommonAttack, ETriggerEvent::Started, this, &AEnhoneyPlayerController::TryActivateAbility_CommonAttack);

		EnhoneyInputComponent->BindAction(InteractWithNPCAction, ETriggerEvent::Started, this, &AEnhoneyPlayerController::TryInteractWithNPC);

		
	}
}

void AEnhoneyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEnhoneyPlayerController, bCanInteract);
}

void AEnhoneyPlayerController::ServerBuyItem_Implementation(const FGameplayTag& InItemTag, int32 InItemNum)
{
	if (AEnhoneyGameStateBase* GameState = UEnhoneyAbilitySystemLibrary::GetEnhoneyGameState(this))
	{
		GameState->BuyItem(FInventoryItem(InItemTag, InItemNum));
	}
}

void AEnhoneyPlayerController::Move(const FInputActionValue& InputActionValue)
{
	FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	FRotator ControllerRotation = GetControlRotation();
	const FRotator YawRotation(0, ControllerRotation.Yaw, 0);

	// get forward vector
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	// get right vector 
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 直角坐标系，Y轴代表前后
	GetPawn()->AddMovementInput(ForwardDirection, MovementVector.Y);
	GetPawn()->AddMovementInput(RightDirection, MovementVector.X);

}

void AEnhoneyPlayerController::Look(const FInputActionValue& InputActionValue)
{
	if (bIsTargetLocked) return;

	FVector2D LookAxisVector = InputActionValue.Get<FVector2D>();

	AddYawInput(LookAxisVector.X);
	AddPitchInput(LookAxisVector.Y);
}

void AEnhoneyPlayerController::Jump()
{
	if (ACharacter* CharacterControlled = GetCharacter())
	{
		CharacterControlled->Jump();
	}
}

void AEnhoneyPlayerController::StopJumping()
{
	if (ACharacter* CharacterControlled = GetCharacter())
	{
		CharacterControlled->StopJumping();
	}
}

void AEnhoneyPlayerController::OpenCharacterMenu()
{
	if (!bIsCharacterMenuOpened)
	{
		FInputModeUIOnly InputModeData;

		SetInputMode(InputModeData);

		// 单机模式，有暂停
		// 这个函数内部会自己判断NetMode
		SetPause(true);

		// 显示鼠标光标
		SetShowMouseCursor(true);

		bIsCharacterMenuOpened = true;

		// 打开CharacterMenu的UI
		if (AEnhoneyPlayingHUD* PlayingHUD = Cast<AEnhoneyPlayingHUD>(GetHUD()))
		{
			if (APlayerCharacterBase* ControlledCharacter = Cast<APlayerCharacterBase>(GetCharacter()))
			{
				if (AEnhoneyPlayerState* EnhoneyPlayerState = GetPlayerState<AEnhoneyPlayerState>())
				{
					PlayingHUD->OpenCharacterMenu(this, EnhoneyPlayerState,
						CastChecked<UEnhoneyAbilitySystemComponent>(ControlledCharacter->GetAbilitySystemComponent()),
						ControlledCharacter->GetPlayerAttributeSet());
				}
				
			}
			
		}
	}
	
}

void AEnhoneyPlayerController::SwitchWeapon()
{
	if (APlayerCharacterBase* PlayerCharacter = Cast<APlayerCharacterBase>(GetCharacter()))
	{
		PlayerCharacter->ServerSwitchEquippedWeapon();
	}
}

void AEnhoneyPlayerController::ShowOrHidePlayerTaskPanel()
{
	// 获取TaskWidgetController
	if (GetPawn()->Implements<UPlayerInterface>())
	{
		if (UTaskWidgetController* TaskWidgetController = IPlayerInterface::Execute_GetPlayerTaskWidgetController(GetPawn()))
		{
			TaskWidgetController->ShowOrHidePlayerTaskPanel();
		}
	}
}

void AEnhoneyPlayerController::OpenPauseMenu()
{
	// 找到PlayingWidgetController
	UPlayingWidgetController* PlayingWidgetController;
	if (UCommonAlgorithmLibrary::GetPlayingWidgetControllerByCharacter(GetCharacter(), PlayingWidgetController))
	{
		PlayingWidgetController->OpenPauseMenu();
	}
}

void AEnhoneyPlayerController::EnemyTargetLocking()
{
	if (GetCharacter()->Implements<UPlayerInterface>())
	{
		AActor* EnemyTargetLocked;
		if (!IPlayerInterface::Execute_GetPlayerTargetEnemy(GetCharacter(), EnemyTargetLocked))
		{
			// 还未锁敌，就激活锁敌技能
			UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetCharacter());
			if (IsValid(ASC))
			{
				FGameplayTagContainer AbilityTagContainer(FEnhoneyGameplayTags::Get().Ability_Offensive_Inherent_EnemyLocking);
				ASC->TryActivateAbilitiesByTag(AbilityTagContainer);
			}

		}
		else
		{
			// 如果已经锁敌，就取消这个技能
			FGameplayEventData Payload;
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetCharacter(), FEnhoneyGameplayTags::Get().AbilityEventTag_EnemyLocking_EndAbility, Payload);
		}
	}
	
}

void AEnhoneyPlayerController::TryActivateAbility_CommonAttack()
{
	if (GetCharacter()->Implements<UPlayerInterface>())
	{
		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetCharacter());

		if (IsValid(ASC))
		{
			// 找到这个能力的Spec
			FGameplayAbilitySpec CommonAttackAbilitySpec = UEnhoneyAbilitySystemLibrary::GetAbilitySpecByTag(
				ASC, IPlayerInterface::Execute_GetCommonAttackAbilityTag(GetCharacter()));

			ASC->AbilitySpecInputPressed(CommonAttackAbilitySpec);

			if (!CommonAttackAbilitySpec.IsActive())
			{
				ASC->TryActivateAbility(CommonAttackAbilitySpec.Handle);
			}
		}
	}

}

void AEnhoneyPlayerController::TryInteractWithNPC()
{
	if (!bCanInteract || bIsCharacterMenuOpened) return;
	if (GetCharacter()->Implements<UPlayerInterface>())
	{
		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetCharacter());

		if (IsValid(ASC))
		{
			// 找到这个能力的Spec
			FGameplayAbilitySpec InteractionAbilitySpec = UEnhoneyAbilitySystemLibrary::GetAbilitySpecByTag(
				ASC, FEnhoneyGameplayTags::Get().Ability_Offensive_Inherent_InteractWithNPC);

			if (!InteractionAbilitySpec.IsActive())
			{
				ASC->TryActivateAbility(InteractionAbilitySpec.Handle);

				/** 触发NPC交互委托
				 * --记住TryInteractWithNPC，这些绑定到InputAction的函数都只会在客户端执行，
				 * 除非是单机模式或者监听服务器上，这里的广播也就只会在客户端执行，
				 * 在设计代理的回调函数的时候要注意这一点
				 */ 
				OnInteractWithNPCDelegate.Broadcast(GetCharacter());
			}
		}

	}
}
