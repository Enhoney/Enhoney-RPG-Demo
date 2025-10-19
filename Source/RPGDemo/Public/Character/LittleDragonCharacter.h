// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Character/EnemyCharacterBase.h"
#include "LittleDragonCharacter.generated.h"

class UDamageBoxComponent;

/**
 * 
 */
UCLASS()
class RPGDEMO_API ALittleDragonCharacter : public AEnemyCharacterBase
{
	GENERATED_BODY()

public:

	ALittleDragonCharacter();

	/** Enemy Interface Start*/
	virtual void SetMeleeCollisionEnabled_Implementation(const FGameplayTag DamageLocationTag, bool bIsEnabled) override;
	/** Enemy Interface End*/


private:

	UPROPERTY(VisibleAnywhere, Category = "DamageBox|Mouth")
	TObjectPtr<UDamageBoxComponent> MouthDamageBox;

	UPROPERTY(VisibleAnywhere, Category = "DamageBox|Tail")
	TObjectPtr<UDamageBoxComponent> TailDamageBox;
};
