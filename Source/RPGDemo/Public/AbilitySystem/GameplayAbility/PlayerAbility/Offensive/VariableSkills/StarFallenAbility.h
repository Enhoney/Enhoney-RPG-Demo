// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAbility/Offensive/DamageAbility_Variable.h"
#include "StarFallenAbility.generated.h"

class AFallenStarBase;

/**
 * 
 */
UCLASS()
class RPGDEMO_API UStarFallenAbility : public UDamageAbility_Variable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SpawnFallenStar")
	bool SpawnFallenStarToEnemy(FName EnemyFallenStarSocketName);

protected:
	// 获取某一等级的描述信息--已解锁或者已装备状态
	virtual void GetAbilityDescOnLevel(UAbilitySystemComponent* InASC, int32 InAbilityLevel, FString& OutDescription) const override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFallenStarBase> FallenStarActorClass;
	
};
