// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "OffensiveAbility_Damage.h"
#include "ProjectileAbilityBase.generated.h"

class ADamageActorBase;

/**
 * 
 */
UCLASS()
class RPGDEMO_API UProjectileAbilityBase : public UOffensiveAbility_Damage
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "SpawnProjectile")
	void SpawnProjectileToActor(FName SocketName);


private:
	// 发射物的类型
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageActorBase> ProjectileActorClass;
	
};
