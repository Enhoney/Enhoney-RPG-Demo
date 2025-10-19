// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "EnhoneyPlayingHUD.generated.h"

class UEnhoneyWidgetControllerBase;

class UEnhoneyUserWidgetBase;
class UDamageFloatWidgetController;
class UStoreWidgetController;
class AEnhoneyPlayerController;
class UTaskWidgetController;

class AEnhoneyPlayerState;
class UEnhoneyAbilitySystemComponent;
class UPlayerAttributeSet;
class UAudioComponent;
class USoundBase;


struct FWidgetControllerParam;

/**
 * 
 */
UCLASS()
class RPGDEMO_API AEnhoneyPlayingHUD : public AHUD
{
	GENERATED_BODY()

public:

	AEnhoneyPlayingHUD();

	// 单例，获取PlayingWidgerController
	UFUNCTION(BlueprintCallable)
	UEnhoneyWidgetControllerBase* GetPlayingWidgetController(const FWidgetControllerParam& InWCParam);
	// 获取CharacterMenuWidgetController
	UFUNCTION(BlueprintCallable)
	UEnhoneyWidgetControllerBase* GetCharacterMenuWidgetController(const FWidgetControllerParam& InWCParam);
	// 获取AttributeMenuWidgetController
	UFUNCTION(BlueprintCallable)
	UEnhoneyWidgetControllerBase* GetAttributeMenuWidgetController(const FWidgetControllerParam& InWCParam);
	// 获取InventoryWidgetController
	UFUNCTION(BlueprintCallable)
	UEnhoneyWidgetControllerBase* GetInventoryWidgetController(const FWidgetControllerParam& InWCParam);
	// 获取StoreWidgetController
	UFUNCTION(BlueprintCallable)
	UEnhoneyWidgetControllerBase* GetStoreWidgetController(const FWidgetControllerParam& InWCParam);
	// 获取玩家任务面板WidgetController
	UFUNCTION(BlueprintCallable)
	UEnhoneyWidgetControllerBase* GetPlayerTaskWidgetController(const FWidgetControllerParam& InWCParam);

	// 获取DamageFloatWidgetController
	UFUNCTION(BlueprintCallable)
	UDamageFloatWidgetController* GetDamageFloatWidgetController();

	// 创建PlayingWidget主UI，并显示到Screen
	void InitPlayingWidgetUI(AEnhoneyPlayerController* InPC, AEnhoneyPlayerState* InPS, UEnhoneyAbilitySystemComponent* InASC, UPlayerAttributeSet* InAS);

	// 隐藏或者显示PlayingWidget主UI，在点击背包、属性菜单、技能菜单和那些UI关闭的时候调用
	UFUNCTION(BlueprintCallable)
	void SetPlayingWidgetVisibility(bool bIsVisible);

	// 打开CharacterMenu
	UFUNCTION(BlueprintCallable)
	void OpenCharacterMenu(AEnhoneyPlayerController* InPC,
		AEnhoneyPlayerState* InPS, UEnhoneyAbilitySystemComponent* InASC, UPlayerAttributeSet* InAS);

	// 关闭CharacterMenu
	UFUNCTION(BlueprintCallable)
	void CloseCharacterMenu();

	// 初始化DamageFloatWidgetController
	UFUNCTION(BlueprintCallable)
	void InitializeDamageFloatWidgetController(AEnhoneyPlayerController* InPC,
		AEnhoneyPlayerState* InPS, UEnhoneyAbilitySystemComponent* InASC, UPlayerAttributeSet* InAS);

protected:
	virtual void BeginPlay() override;

	// 切换BGM
	void SwitchBGM();

private:
	// WidgetController，可以使用TWeakObjectPtr，但是考虑到性能问题，用TObjectPtr更好
	// 用于主UI的，显示一部分属性和技能
	UPROPERTY()
	TObjectPtr<UEnhoneyWidgetControllerBase> PlayingWidgetController;
	// 主界面使用的UI
	UPROPERTY()
	TObjectPtr<UEnhoneyUserWidgetBase> PlayingWidget;

	// 主界面使用的WidgetController类
	UPROPERTY(EditDefaultsOnly, Category = "EnhoneyUserWidget|WidgetController")
	TSubclassOf<UEnhoneyWidgetControllerBase> PlayingWidgetControllerClass;
	// 主界面使用的UI类
	UPROPERTY(EditDefaultsOnly, Category = "EnhoneyUserWidget|Widget")
	TSubclassOf<UEnhoneyUserWidgetBase> PlayingWidgetClass;

	/** CharacterMenu */
	UPROPERTY()
	TObjectPtr<UEnhoneyWidgetControllerBase> CharacterMenuWidgetController;
	UPROPERTY()
	TObjectPtr<UEnhoneyUserWidgetBase> CharacterMenuWidget;
	UPROPERTY(EditDefaultsOnly, Category = "EnhoneyUserWidget|WidgetController")
	TSubclassOf<UEnhoneyWidgetControllerBase> CharacterMenuWidgetControllerClass;
	UPROPERTY(EditDefaultsOnly, Category = "EnhoneyUserWidget|Widget")
	TSubclassOf<UEnhoneyUserWidgetBase> CharacterMenuWidgetClass;

	// AttributeMenuWidgetController
	UPROPERTY(EditDefaultsOnly, Category = "EnhoneyUserWidget|WidgetController")
	TSubclassOf<UEnhoneyWidgetControllerBase> AttributeMenuWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UEnhoneyWidgetControllerBase> AttributeMenuWidgetController;

	// InventoryWidgetController
	UPROPERTY(EditDefaultsOnly, Category = "EnhoneyUserWidget|WidgetController")
	TSubclassOf<UEnhoneyWidgetControllerBase> InventoryWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UEnhoneyWidgetControllerBase> InventoryWidgetController;


	// DamageFloatWidgetController
	UPROPERTY(EditDefaultsOnly, Category = "EnhoneyUserWidget|WidgetController")
	TSubclassOf<UDamageFloatWidgetController> DamageFloatWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UDamageFloatWidgetController> DamageFloatWidgetController;

	// StoreWidgetController
	UPROPERTY(EditDefaultsOnly, Category = "EnhoneyUserWidget|WidgetController")
	TSubclassOf<UStoreWidgetController> StoreWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UStoreWidgetController> StoreWidgetController;

	// TaskWidgetController
	UPROPERTY(EditDefaultsOnly, Category = "EnhoneyUserWidget|WidgetController")
	TSubclassOf<UTaskWidgetController> PlayerTaskWidgetControllerClass;
	UPROPERTY()
	TObjectPtr<UTaskWidgetController> PlayerTaskWidgetController;


	// 定时器句柄，切换BGM
	UPROPERTY()
	FTimerHandle TimerHandle_SwitchBGM;
	// 切换BGM的时间间隔，单位秒
	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	float BGMSwitchInterval = 300.f;
	
	// 音频组件
	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	TObjectPtr<UAudioComponent> BGMComponent;
	// 游戏的BGM
	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	TArray<TObjectPtr<USoundBase>> GameBGMArray;
	// 当前播放的BGM
	UPROPERTY()
	TObjectPtr<USoundBase> CurrentBGM;
	// 当前播放的BGM索引
	UPROPERTY()
	int32 CurrentBGMIndex = 0;
};
