// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnhoneyWidgetControllerBase.generated.h"

class AEnhoneyPlayerState;
class AEnhoneyPlayerController;
class UEnhoneyAbilitySystemComponent;
class UPlayerAttributeSet;

// 属性和当前经验值改变
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeValueChangedSignature, float, NewValue);

// 升级后当前等级
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterLevelChangedSignature, int32, NewLevel);

USTRUCT(BlueprintType)
struct RPGDEMO_API FWidgetControllerParam
{
	GENERATED_BODY()

public:
	FWidgetControllerParam() {}
	FWidgetControllerParam(AEnhoneyPlayerController* InPC, AEnhoneyPlayerState* InPS, UEnhoneyAbilitySystemComponent* InASC, UPlayerAttributeSet* InAS)
		: PlayerState(InPS), PlayerController(InPC), AbilitySystemComponent(InASC), AttributeSet(InAS)
	{

	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetController")
	TObjectPtr<AEnhoneyPlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetController")
	TObjectPtr<AEnhoneyPlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetController")
	TObjectPtr<UEnhoneyAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetController")
	TObjectPtr<UPlayerAttributeSet> AttributeSet = nullptr;
};

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnhoneyWidgetControllerBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllrtParams(const FWidgetControllerParam& InWCParams);

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValue();

	// 监听属性变化
	virtual void BindCallbacksToDependiencies();


protected:
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AEnhoneyPlayerState> PlayerState;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<AEnhoneyPlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UEnhoneyAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	TObjectPtr<UPlayerAttributeSet> AttributeSet;
};
