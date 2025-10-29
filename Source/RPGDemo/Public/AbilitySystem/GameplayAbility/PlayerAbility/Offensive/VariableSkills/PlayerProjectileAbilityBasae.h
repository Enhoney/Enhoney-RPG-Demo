// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAbility/Offensive/DamageAbility_Variable.h"
#include "PlayerProjectileAbilityBasae.generated.h"

class ADamageActorBase;

/**
 * 
 */
UCLASS()
class RPGDEMO_API UPlayerProjectileAbilityBasae : public UDamageAbility_Variable
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "SpawnProjectile")
	void SpawnProjectileToEnemy(FName SocketName);


private:
	// 发射物的类型
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorBase> ProjectileActorClass;
	
};
