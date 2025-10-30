// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAbility/Offensive/DamageAbility_Variable.h"
#include "LimboIceAbility.generated.h"

class ADamageProjectileActor;

/**
 * 
 */
UCLASS()
class RPGDEMO_API ULimboIceAbility : public UDamageAbility_Variable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SpawnLimboIce")
	bool SpawnLimboIceToEnemy();

protected:
	// 获取某一等级的描述信息--已解锁或者已装备状态
	virtual void GetAbilityDescOnLevel(UAbilitySystemComponent* InASC, int32 InAbilityLevel, FString& OutDescription) const override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ADamageProjectileActor> LimboIceActorClass;
	
};
