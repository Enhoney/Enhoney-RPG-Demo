// Copyright Enhoney.


#include "OffensiveAbility_Damage.h"

#include "ScalableFloat.h"
#include "EnhoneyAbilityType.h"
#include "EnhoneyAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "AbilitySystemInterface.h"

FEnhoneyDamageEffectParams UOffensiveAbility_Damage::MakeDamageEffectParams(const AActor* InTargetActor, bool bIsMeleeAbility)
{
    FEnhoneyDamageEffectParams OutParams;

    OutParams.WorldContextObject = GetAvatarActorFromActorInfo();
    OutParams.SourceASC = Cast<UEnhoneyAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
    if (const IAbilitySystemInterface* TragetAbilitySystemInterface = Cast<IAbilitySystemInterface>(InTargetActor))
    {
        OutParams.TargetASC = Cast<UEnhoneyAbilitySystemComponent>(TragetAbilitySystemInterface->GetAbilitySystemComponent());
    }
    OutParams.DamageGameplayEffectClass = DamageEffectClass;
    OutParams.AbilityDamageType = AbilityDamageType;
    OutParams.AbilityLevel = GetAbilityLevel();
    OutParams.SkillMultiplier = GetSkillMultiplierByLevel();
    OutParams.KnockbackChance = KnockbackChance;
    OutParams.KnockbackForceMagnitude = KnockbackForceMagnitude;

    // 只有在近战攻击的时候，才会立即进行击退判定
    if (bIsMeleeAbility)
    {
        if (FMath::RandRange(1, 1000) < OutParams.KnockbackChance * 1000)
        {
            // 近战攻击的时候，这个传递的必然就是敌人
            check(InTargetActor);

            FVector SourceActorLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
            FVector TargetActorLocation = InTargetActor->GetActorLocation();
            FRotator KnockbackRotation = (TargetActorLocation - SourceActorLocation).Rotation();
            KnockbackRotation.Pitch = KnockbackPitch;

            OutParams.KnockbackVector = KnockbackRotation.Vector().GetSafeNormal() * OutParams.KnockbackForceMagnitude;
        }
    }

    return OutParams;
}

float UOffensiveAbility_Damage::GetSkillMultiplierByLevel() const
{
    return SkillMultipliers.GetValueAtLevel(GetAbilityLevel());
}
