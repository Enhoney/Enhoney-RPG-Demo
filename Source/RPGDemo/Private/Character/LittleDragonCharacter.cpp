// Copyright Enhoney.


#include "LittleDragonCharacter.h"

#include "DamageBoxComponent.h"
#include "EnhoneyGameplayTags.h"

ALittleDragonCharacter::ALittleDragonCharacter()
{
	// 嘴部的伤害盒子
	MouthDamageBox = CreateDefaultSubobject<UDamageBoxComponent>(TEXT("MouthDamageBox"));
	MouthDamageBox->SetupAttachment(GetMesh(), TEXT("MouthDamageSocket"));
	MouthDamageBox->SetIsReplicated(true);

	// 尾部的伤害盒子
	TailDamageBox = CreateDefaultSubobject<UDamageBoxComponent>(TEXT("TailDamageBox"));
	TailDamageBox->SetupAttachment(GetMesh(), TEXT("TailDamageSocket"));
	TailDamageBox->SetIsReplicated(true);

}

void ALittleDragonCharacter::SetMeleeCollisionEnabled_Implementation(const FGameplayTag DamageLocationTag, bool bIsEnabled)
{
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();
	if (DamageLocationTag.MatchesTagExact(EnhoneyTags.AbilityEventTag_MeleeCollisionEnabled_Mouth))
	{
		MouthDamageBox->SetCombatBoxCollisionEnable(bIsEnabled);
	}
	if (DamageLocationTag.MatchesTagExact(EnhoneyTags.AbilityEventTag_MeleeCollisionEnabled_Tail))
	{
		TailDamageBox->SetCombatBoxCollisionEnable(bIsEnabled);
	}
}
