// Copyright Enhoney.


#include "EnemyAttributeSet.h"

#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"

#include "EnemyInterface.h"
#include "BossInterface.h"
#include "CombatInterface.h"
#include "EnhoneyAbilityType.h"

#include "EnhoneyGameplayTags.h"

UEnemyAttributeSet::UEnemyAttributeSet()
{

}

void UEnemyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties EffectProps;
	GetEffectProps(EffectProps, Data);

	// 属性约束
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0, GetMaxHealth()));
	}

	if (Data.EvaluatedData.Attribute == GetMaxHealthAttribute())
	{
		SetMaxHealth(FMath::Clamp(GetMaxHealth(), 1.f, INT32_MAX));
	}

	// 接收到的伤害--最终伤害
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		HandleIncomingDamage(EffectProps);
	}
}

void UEnemyAttributeSet::HandleIncomingDamage(const FEffectProperties& EffectProps)
{
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();

	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);

	if (LocalIncomingDamage >= 0.f)
	{
		// 切换到战斗状态
		if (EffectProps.TargetAvatarActor->Implements<UEnemyInterface>())
		{
			IEnemyInterface::Execute_EnemyEnterCombat(EffectProps.TargetAvatarActor, EffectProps.SourceCharacter);
		}

		const float NewHealth = FMath::Clamp(GetHealth() - LocalIncomingDamage, 0.f, GetMaxHealth());
		SetHealth(NewHealth);
		// 是否为致命伤害
		const bool bDamageFatal = FMath::IsNearlyZero(GetHealth());
		if (bDamageFatal)
		{
			// 死亡
			if (EffectProps.TargetAvatarActor->Implements<UCombatInterface>())
			{
				// 设置击杀者
				if (EffectProps.TargetAvatarActor->Implements<UEnemyInterface>())
				{
					IEnemyInterface::Execute_SetKillerPawn(EffectProps.TargetAvatarActor, EffectProps.SourceCharacter);
				}
				// 执行死亡逻辑
				ICombatInterface::Execute_CharacterDie(EffectProps.TargetAvatarActor);
			}
		}
		else
		{
			
			if (EffectProps.TargetAvatarActor->Implements<UEnemyInterface>())
			{
				/** 伤害飘字*/
				const FEnhoneyGameplayEffectContext* Context = static_cast<const FEnhoneyGameplayEffectContext*>(EffectProps.GEContectHandle.Get());
				if (Context != nullptr)
				{
					IEnemyInterface::Execute_MulticastShowDamageFloat(EffectProps.TargetAvatarActor, 
						Context->GetAbilityDamageType(), Context->GetIsCriticalHit(), LocalIncomingDamage);
				}

				/** 更新血条*/
				IEnemyInterface::Execute_MulticastUpdateHealthBar(EffectProps.TargetAvatarActor, GetHealth(), GetMaxHealth());

			}
			// 转换为自定义的GameplayEffectContext
			const FEnhoneyGameplayEffectContext* EnhoneyEffectContext =
				static_cast<const FEnhoneyGameplayEffectContext*>(EffectProps.GEContectHandle.Get());
			// 受击，作为一个能力去设计
			if (EffectProps.TargetAvatarActor->Implements<UBossInterface>())
			{
				// 如果是BOSS，就需要额外进行判定，只有击退并且暴击的时候，BOSS才会触发受击能力
				if (EnhoneyEffectContext->GetKnockbackVector() != FVector::ZeroVector && EnhoneyEffectContext->GetIsCriticalHit())
				{
					// 激活受击能力
					if (EffectProps.TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(EnhoneyTags.Ability_Offensive_Inherent_HitReaction)))
					{
						// 播放受击动画
						ICombatInterface::Execute_PlayHitReactionAnim(EffectProps.TargetAvatarActor, *(EnhoneyEffectContext->GetHitResult()));
					}
				}
			}
			else
			{
				// 如果是普通敌人，就直接激活受击能力，如果触发击退，这个时候就击退
				if (EnhoneyEffectContext->GetKnockbackVector() != FVector::ZeroVector)
				{
					EffectProps.TargetCharacter->LaunchCharacter(EnhoneyEffectContext->GetKnockbackVector(), true, true);
				}
				else
				{
					// 激活受击能力
					if (EffectProps.TargetASC->TryActivateAbilitiesByTag(FGameplayTagContainer(EnhoneyTags.Ability_Offensive_Inherent_HitReaction)))
					{
						// 播放受击动画
						ICombatInterface::Execute_PlayHitReactionAnim(EffectProps.TargetAvatarActor, *(EnhoneyEffectContext->GetHitResult()));
					}
				}
			}
		}
	}
}
