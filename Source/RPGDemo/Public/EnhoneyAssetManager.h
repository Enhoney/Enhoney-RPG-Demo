// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "EnhoneyAssetManager.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class RPGDEMO_API UEnhoneyAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	static const UEnhoneyAssetManager& Get();

protected:
	// 这个函数很早就会被调用，加载游戏资源的时候就会调用了
	virtual void StartInitialLoading() override;
	
};
