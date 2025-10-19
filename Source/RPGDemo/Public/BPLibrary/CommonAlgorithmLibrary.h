// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "PlayingWidgetController.h"
#include "CommonAlgorithmLibrary.generated.h"

UENUM(BlueprintType)
enum class EEnhoneyHitDirection : uint8
{
	Hit_Forward			UMETA(DisplayName = "Hit Direction -- Forward"),
	Hit_Backward		UMETA(DisplayName = "Hit Direction -- Backward"),
	Hit_Left			UMETA(DisplayName = "Hit Direction -- Left"),
	Hit_Right			UMETA(DisplayName = "Hit Direction -- Right")
};

/**
 * 
 */
UCLASS()
class RPGDEMO_API UCommonAlgorithmLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "CommonAlgorithmLibrary|GetNearstEnemyInRadius")
	static AActor* GetNearstEnemyInRadius(const AActor* InSourceActor, float InRadiusThreshold, bool bWarningNotFound = false);

	UFUNCTION(BlueprintCallable, Category = "CommonAlgorithmLibrary|GetPlayingWidgetController")
	static bool GetPlayingWidgetControllerByCharacter(ACharacter* PlayerCharacter, UPlayingWidgetController*& OutPlayingWidgetController);

	UFUNCTION(BlueprintCallable, Category = "CommonAlgorithmLibrary|GetHitDirection")
	static EEnhoneyHitDirection GetHitDirection(const FHitResult& InHitResult, const FVector& CharacterForwardVector, const FVector& RootLocation);

	// 检查是否是在本地--单机模式、DS的客户端、LS的服务器或者客户端
	UFUNCTION(BlueprintCallable, Category = "CommonAlgorithmLibrary|IsLocalEnhoneyPlayer")
	static bool IsLocalEnhoneyPlayer(const AActor* InWorldObjectContext);
};
