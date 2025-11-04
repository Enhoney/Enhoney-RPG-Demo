// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// 死亡回调
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActorDeathSignature, AActor*, DeadActor);

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
	// 获取SocketRotation
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FRotator GetFireSocketRotation(FName FireSocketName);

	// 播放受击动画
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void PlayHitReactionAnim(const FHitResult& ImpactResult);

	// 获取角色死亡委托
	virtual FOnActorDeathSignature& GetOnActorDeathDelegate() = 0;

};
