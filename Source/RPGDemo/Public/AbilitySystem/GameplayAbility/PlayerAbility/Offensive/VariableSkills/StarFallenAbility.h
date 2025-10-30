// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "PlayerAbility/Offensive/DamageAbility_Variable.h"
#include "StarFallenAbility.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UStarFallenAbility : public UDamageAbility_Variable
{
	GENERATED_BODY()

protected:
	// 获取某一等级的描述信息--已解锁或者已装备状态
	virtual void GetAbilityDescOnLevel(UAbilitySystemComponent* InASC, int32 InAbilityLevel, FString& OutDescription) const override;
	
};
