// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Character/BossEnemyCharacterBase.h"
#include "BossGiantDragonCharacter.generated.h"

class UDamageBoxComponent;

/**
 * 
 */
UCLASS()
class RPGDEMO_API ABossGiantDragonCharacter : public ABossEnemyCharacterBase
{
	GENERATED_BODY()
public:
	ABossGiantDragonCharacter();

	/** Enemy Interface Start*/
	virtual void SetMeleeCollisionEnabled_Implementation(const FGameplayTag DamageLocationTag, bool bIsEnabled) override;
	/** Enemy Interface End*/

	/** Boss Interface Start*/
	virtual void SwitchToPhaseTwoVisual_Implementation() override;	// 改变材质
	/** Boss Interface End*/


private:
	UPROPERTY(VisibleAnywhere, Category = "DamageBox|Mouth")
	TObjectPtr<UDamageBoxComponent> MouthDamageBox;

	// 二阶段的材质
	UPROPERTY(EditDefaultsOnly, Category = "Material")
	TObjectPtr<UMaterialInterface> MatOnPhaseTwo;
	
};
