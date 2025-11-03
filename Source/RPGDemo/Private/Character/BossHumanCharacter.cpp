// Copyright Enhoney.


#include "BossHumanCharacter.h"
#include "DamageBoxComponent.h"
#include "EnhoneyGameplayTags.h"

ABossHumanCharacter::ABossHumanCharacter()
{
	// 武器的网格体组件
	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMeshComponent"));
	WeaponMeshComponent->SetupAttachment(GetMesh(), TEXT("WeaponSocket_Unequip"));
	WeaponMeshComponent->SetIsReplicated(true);

	// 左手的伤害盒子
	LeftHandDamageBox = CreateDefaultSubobject<UDamageBoxComponent>(TEXT("LeftHandDamageBox"));
	LeftHandDamageBox->SetupAttachment(GetMesh(), TEXT("LeftHandDamageSocket"));
	LeftHandDamageBox->SetIsReplicated(true);

	// 右手的伤害盒子
	RightHandDamageBox = CreateDefaultSubobject<UDamageBoxComponent>(TEXT("RightHandDamageBox"));
	RightHandDamageBox->SetupAttachment(GetMesh(), TEXT("RightHandDamageSocket"));
	RightHandDamageBox->SetIsReplicated(true);

	// 武器的伤害盒子--附着到武器上
	WeaponDamageBox = CreateDefaultSubobject<UDamageBoxComponent>(TEXT("WeaponHandDamageBox"));
	WeaponDamageBox->SetupAttachment(WeaponMeshComponent);
	WeaponDamageBox->SetIsReplicated(true);

}


void ABossHumanCharacter::SwitchToPhaseTwoVisual_Implementation()
{
	// 将武器附着到角色手上
	WeaponMeshComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket_Equip"));
}

void ABossHumanCharacter::SetMeleeCollisionEnabled_Implementation(const FGameplayTag DamageLocationTag, bool bIsEnabled)
{
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();

	if (DamageLocationTag.MatchesTagExact(EnhoneyTags.AbilityEventTag_MeleeCollisionEnabled_LeftHand))
	{
		LeftHandDamageBox->SetCombatBoxCollisionEnable(bIsEnabled);
	}
	if (DamageLocationTag.MatchesTagExact(EnhoneyTags.AbilityEventTag_MeleeCollisionEnabled_RightHand))
	{
		RightHandDamageBox->SetCombatBoxCollisionEnable(bIsEnabled);
	}
	if (DamageLocationTag.MatchesTagExact(EnhoneyTags.AbilityEventTag_MeleeCollisionEnabled_Weapon))
	{
		WeaponDamageBox->SetCombatBoxCollisionEnable(bIsEnabled);
	}
}

