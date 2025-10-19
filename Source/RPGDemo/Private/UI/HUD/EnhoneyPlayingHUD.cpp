// Copyright Enhoney.


#include "EnhoneyPlayingHUD.h"

#include "EnhoneyUserWidgetBase.h"
#include "EnhoneyWidgetControllerBase.h"

#include "CharacterMenuWidgetController.h"
#include "DamageFloatWidgetController.h"
#include "InventoryWidgetController.h"
#include "StoreWidgetController.h"
#include "TaskWidgetController.h"
#include "AttributeMenuWidgetController.h"
#include "Components/AudioComponent.h"

AEnhoneyPlayingHUD::AEnhoneyPlayingHUD()
{
	// 初始化音频组件
	BGMComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("BGMComponent"));
	BGMComponent->SetupAttachment(GetRootComponent());
	BGMComponent->bAutoActivate = false; // 不要自动播放
}

UEnhoneyWidgetControllerBase* AEnhoneyPlayingHUD::GetPlayingWidgetController(const FWidgetControllerParam& InWCParam)
{
	if (!IsValid(PlayingWidgetController))
	{
		PlayingWidgetController = NewObject<UEnhoneyWidgetControllerBase>(this, PlayingWidgetControllerClass);
		PlayingWidgetController->SetWidgetControllrtParams(InWCParam);

		// 绑定代理，WidgetController开始监听属性的变化
		PlayingWidgetController->BindCallbacksToDependiencies();

	}

	return PlayingWidgetController;
}

UEnhoneyWidgetControllerBase* AEnhoneyPlayingHUD::GetCharacterMenuWidgetController(const FWidgetControllerParam& InWCParam)
{
	if (!IsValid(CharacterMenuWidgetController))
	{
		CharacterMenuWidgetController = NewObject<UEnhoneyWidgetControllerBase>(this, CharacterMenuWidgetControllerClass);
		CharacterMenuWidgetController->SetWidgetControllrtParams(InWCParam);
	}

	return CharacterMenuWidgetController;
}

UEnhoneyWidgetControllerBase* AEnhoneyPlayingHUD::GetAttributeMenuWidgetController(const FWidgetControllerParam& InWCParam)
{
	if (!IsValid(AttributeMenuWidgetController))
	{
		AttributeMenuWidgetController = NewObject<UEnhoneyWidgetControllerBase>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllrtParams(InWCParam);
		CastChecked<UAttributeMenuWidgetController>(AttributeMenuWidgetController)->InitWeaponUpgradeMatListPool(10);	// 10个预设

		// 绑定代理，WidgetController开始监听属性的变化
		AttributeMenuWidgetController->BindCallbacksToDependiencies();

	}

	return AttributeMenuWidgetController;
}

UEnhoneyWidgetControllerBase* AEnhoneyPlayingHUD::GetInventoryWidgetController(const FWidgetControllerParam& InWCParam)
{
	if (!IsValid(InventoryWidgetController))
	{
		InventoryWidgetController = NewObject<UEnhoneyWidgetControllerBase>(this, InventoryWidgetControllerClass);
		InventoryWidgetController->SetWidgetControllrtParams(InWCParam);

		// 初始化对象池
		CastChecked<UInventoryWidgetController>(InventoryWidgetController)->Initialize();

		// 绑定代理，WidgetController开始监听背包中物品的变化
		InventoryWidgetController->BindCallbacksToDependiencies();

	}

	return InventoryWidgetController;
}

UEnhoneyWidgetControllerBase* AEnhoneyPlayingHUD::GetStoreWidgetController(const FWidgetControllerParam& InWCParam)
{
	if (!IsValid(StoreWidgetController))
	{
		StoreWidgetController = NewObject<UStoreWidgetController>(this, StoreWidgetControllerClass);
		StoreWidgetController->SetWidgetControllrtParams(InWCParam);

		// 初始化StoreWidgetController
		CastChecked<UStoreWidgetController>(StoreWidgetController)->Initialize();

		// 绑定代理，WidgetController开始监听背包中物品的变化
		StoreWidgetController->BindCallbacksToDependiencies();

	}

	return StoreWidgetController;
}

UEnhoneyWidgetControllerBase* AEnhoneyPlayingHUD::GetPlayerTaskWidgetController(const FWidgetControllerParam& InWCParam)
{
	if (!IsValid(PlayerTaskWidgetController))
	{
		PlayerTaskWidgetController = NewObject<UTaskWidgetController>(this, PlayerTaskWidgetControllerClass);
		PlayerTaskWidgetController->SetWidgetControllrtParams(InWCParam);
		PlayerTaskWidgetController->InitializeForTaskNPC();

		// 绑定代理，WidgetController开始监听任务组件中内容的变化
		PlayerTaskWidgetController->BindCallbacksToDependiencies();
	}

	return PlayerTaskWidgetController;
}

UDamageFloatWidgetController* AEnhoneyPlayingHUD::GetDamageFloatWidgetController()
{
	return DamageFloatWidgetController;
}

void AEnhoneyPlayingHUD::InitPlayingWidgetUI(AEnhoneyPlayerController* InPC, 
	AEnhoneyPlayerState* InPS, UEnhoneyAbilitySystemComponent* InASC, UPlayerAttributeSet* InAS)
{
	checkf(PlayingWidgetClass, TEXT("Playing Widget Class uninitialized, please fill out BP_EnhoneyPlaying."));
	checkf(PlayingWidgetControllerClass, TEXT("Playing Widget Controller Class uninitialized, please fill out BP_EnhoneyPlaying."));

	PlayingWidget = CreateWidget<UEnhoneyUserWidgetBase>(GetWorld(), PlayingWidgetClass);

	FWidgetControllerParam WidgetControllerParam(InPC, InPS, InASC, InAS);
	PlayingWidgetController = GetPlayingWidgetController(WidgetControllerParam);

	// 添加到视口
	PlayingWidget->AddToViewport();

	// 需要在添加到视口之后再设置WidgetController，因为绑定代理有可能会用到Widget里面的Button
	// 只有在添加到视口之后，这些Button才有效
	PlayingWidget->SetWidgetController(PlayingWidgetController);

	// 广播初始值
	PlayingWidgetController->BroadcastInitialValue();
}

void AEnhoneyPlayingHUD::SetPlayingWidgetVisibility(bool bIsVisible)
{
	if (IsValid(PlayingWidget))
	{
		PlayingWidget->HandleEnhoneyWidgetVisibility(bIsVisible);
	}
}

void AEnhoneyPlayingHUD::OpenCharacterMenu(AEnhoneyPlayerController* InPC,
	AEnhoneyPlayerState* InPS, UEnhoneyAbilitySystemComponent* InASC, UPlayerAttributeSet* InAS)
{

	checkf(CharacterMenuWidgetClass, TEXT("CharacterMenu Widget Class uninitialized, please fill out BP_CharacterMenu."));
	checkf(CharacterMenuWidgetControllerClass, TEXT("CharacterMenu Widget Controller Class uninitialized, please fill out BP_CharacterMenuWidgetController."));

	CharacterMenuWidget = CreateWidget<UEnhoneyUserWidgetBase>(GetWorld(), CharacterMenuWidgetClass);

	FWidgetControllerParam WidgetControllerParam(InPC, InPS, InASC, InAS);
	CharacterMenuWidgetController = GetCharacterMenuWidgetController(WidgetControllerParam);
	// 绑定关闭的回调函数
	if (UCharacterMenuWidgetController* CMWidgetController = Cast<UCharacterMenuWidgetController>(CharacterMenuWidgetController))
	{
		CMWidgetController->OnCharacterMenuClosedDelegate.BindUObject(this, &ThisClass::CloseCharacterMenu);
	}

	// 添加到视口
	CharacterMenuWidget->AddToViewport();

	CharacterMenuWidget->SetWidgetController(CharacterMenuWidgetController);


	// 隐藏PlayingWidget
	SetPlayingWidgetVisibility(false);
}

void AEnhoneyPlayingHUD::CloseCharacterMenu()
{
	if (IsValid(CharacterMenuWidget))
	{
		CharacterMenuWidget->RemoveFromParent();
		// 回收CharacterMenu
		CharacterMenuWidget = nullptr;
	}
	// 解绑代理
	if (UCharacterMenuWidgetController* CMWidgetController = Cast<UCharacterMenuWidgetController>(CharacterMenuWidgetController))
	{
		CMWidgetController->OnCharacterMenuClosedDelegate.Unbind();
	}

	// 显示PlayingWidget
	SetPlayingWidgetVisibility(true);
}

void AEnhoneyPlayingHUD::InitializeDamageFloatWidgetController(AEnhoneyPlayerController* InPC,
	AEnhoneyPlayerState* InPS, UEnhoneyAbilitySystemComponent* InASC, UPlayerAttributeSet* InAS)
{
	if (!IsValid(DamageFloatWidgetController))
	{
		DamageFloatWidgetController = NewObject<UDamageFloatWidgetController>(this, DamageFloatWidgetControllerClass);
		DamageFloatWidgetController->SetWidgetControllrtParams(FWidgetControllerParam(InPC, InPS, InASC, InAS));

		// 初始化对象池
		DamageFloatWidgetController->Initialize();

	}
}

void AEnhoneyPlayingHUD::BeginPlay()
{
	Super::BeginPlay();
	// 设置定时器，定时切换BGM
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(TimerHandle_SwitchBGM, this, &AEnhoneyPlayingHUD::SwitchBGM, BGMSwitchInterval, true, 0);
}

void AEnhoneyPlayingHUD::SwitchBGM()
{
	if (!GameBGMArray.IsEmpty())
	{
		if (!IsValid(CurrentBGM))
		{
			// 如果是第一次播放BGM，随机一个
			CurrentBGMIndex = FMath::RandRange(0, GameBGMArray.Num() - 1);
			
		}
		else
		{
			// 否则就列表循环
			CurrentBGMIndex = (CurrentBGMIndex + 1) % GameBGMArray.Num();
			BGMComponent->Stop();	// 停止当前的BGM
		}
		CurrentBGM = GameBGMArray[CurrentBGMIndex];
		if (IsValid(CurrentBGM))
		{
			BGMComponent->SetSound(CurrentBGM);
			BGMComponent->Play();
		}
	}
}

