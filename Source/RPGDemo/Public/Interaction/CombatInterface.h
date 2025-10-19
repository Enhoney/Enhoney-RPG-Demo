// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGDEMO_API ICombatInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsCharacterAlive() const;

	// 执行死亡逻辑
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CharacterDie();

	// 获取开火的位置
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetFireSocketLocation(FName FireSocketName);

	// 播放受击动画
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PlayHitReactionAnim(const FHitResult& ImpactResult);

};
