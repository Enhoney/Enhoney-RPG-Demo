// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnemyAbility/Boss/GiantDragonDamageAbilityBase.h"
#include "GiantDragonRemoteBase.generated.h"

class ADamageActorBase;

/**
 * 
 */
UCLASS()
class RPGDEMO_API UGiantDragonRemoteBase : public UGiantDragonDamageAbilityBase
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "SpawnProjectile")
	void SpawnProjectileToTargetPlayer(FName SocketName);


private:
	// 一阶段发射物的类型
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorBase> ProjectileActorClass_Phase1;
	// 二阶段发射物的类型
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorBase> ProjectileActorClass_Phase2;
	
};
