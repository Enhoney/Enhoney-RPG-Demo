// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Character/BossEnemyCharacterBase.h"
#include "BossHumanCharacter.generated.h"

class UDamageBoxComponent;

/**
 * 
 */
UCLASS()
class RPGDEMO_API ABossHumanCharacter : public ABossEnemyCharacterBase
{
	GENERATED_BODY()

public:

	ABossHumanCharacter();

	/** Boss Interface Start*/
	virtual void SwitchToPhaseTwoVisual_Implementation() override;	// 武器附着
	/** Boss Interface End*/

	/** Enemy Interface Start*/
	virtual void SetMeleeCollisionEnabled_Implementation(const FGameplayTag DamageLocationTag, bool bIsEnabled) override;
	/** Enemy Interface End*/


private:
	// 武器网格体
	UPROPERTY(VisibleAnywhere, Category = "WeaponMesh")
	TObjectPtr<USkeletalMeshComponent> WeaponMeshComponent;
	// 三种位置的DamageBox
	UPROPERTY(VisibleAnywhere, Category = "DamageBox|LeftHand")
	TObjectPtr<UDamageBoxComponent> LeftHandDamageBox;
	UPROPERTY(VisibleAnywhere, Category = "DamageBox|RightHand")
	TObjectPtr<UDamageBoxComponent> RightHandDamageBox;
	UPROPERTY(VisibleAnywhere, Category = "DamageBox|Weapon")
	TObjectPtr<UDamageBoxComponent> WeaponDamageBox;
};
