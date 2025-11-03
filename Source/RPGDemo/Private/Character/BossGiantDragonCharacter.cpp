// Copyright Enhoney.


#include "BossGiantDragonCharacter.h"
#include "DamageBoxComponent.h"
#include "EnhoneyGameplayTags.h"

ABossGiantDragonCharacter::ABossGiantDragonCharacter()
{
	// 嘴部的伤害盒子
	MouthDamageBox = CreateDefaultSubobject<UDamageBoxComponent>(TEXT("MouthDamageBox"));
	MouthDamageBox->SetupAttachment(GetMesh(), TEXT("MouthDamageSocket"));
	MouthDamageBox->SetIsReplicated(true);
}

void ABossGiantDragonCharacter::SetMeleeCollisionEnabled_Implementation(const FGameplayTag DamageLocationTag, bool bIsEnabled)
{
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();

	if (DamageLocationTag.MatchesTagExact(EnhoneyTags.AbilityEventTag_MeleeCollisionEnabled_Mouth))
	{
		MouthDamageBox->SetCombatBoxCollisionEnable(bIsEnabled);
	}
}

void ABossGiantDragonCharacter::SwitchToPhaseTwoVisual_Implementation()
{
	GetMesh()->SetMaterial(0, MatOnPhaseTwo);
}
