// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BossInterface.generated.h"

UINTERFACE(MinimalAPI)
class UBossInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGDEMO_API IBossInterface
{
	GENERATED_BODY()

	
public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void TestFunc();
};
