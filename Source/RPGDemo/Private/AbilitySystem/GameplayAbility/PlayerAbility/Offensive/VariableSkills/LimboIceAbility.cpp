// Copyright Enhoney.


#include "PlayerAbility/Offensive/VariableSkills/LimboIceAbility.h"
#include "CombatInterface.h"
#include "PlayerInterface.h"
#include "EnemyInterface.h"
#include "CommonAlgorithmLibrary.h"
#include "Actor/DamageProjectileActor.h"

bool ULimboIceAbility::SpawnLimboIceToEnemy()
{
	// 在服务器生成
	const bool bServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bServer)
	{
		return false;
	}
	check(LimboIceActorClass);

	if (GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		// 找到发射点位置--玩家位置向上3m处
		FVector FireSourceLocation = IsValid(GetAvatarActorFromActorInfo())
			? GetAvatarActorFromActorInfo()->GetActorLocation() + FVector(0.f, 0.f, 300.f)
			: FVector::ZeroVector;
		// 计算发射朝向
		FRotator FireRotation = FRotator();
		AActor* TargetActor = nullptr;
		// 如果锁敌了，就向敌人目标组件处发射
		if (GetAvatarActorFromActorInfo()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_GetPlayerTargetEnemy(GetAvatarActorFromActorInfo(), TargetActor);
		}

		if(!IsValid(TargetActor))
		{
			// 如果没有锁定敌人，就找到最近的敌人，向他发射
			TargetActor = UCommonAlgorithmLibrary::GetNearstEnemyInRadius(GetAvatarActorFromActorInfo(), 2000.f, false);
		}

		// 如果找到了目标敌人，就朝他发射
		if (IsValid(TargetActor))
		{
			// 如果目标是存在的，就向敌人目标组件处发射
			FVector TargetLocation = FVector::ZeroVector;

			if (TargetActor->Implements<UEnemyInterface>())
			{
				IEnemyInterface::Execute_GetTargetEnemyLocation(TargetActor, TargetLocation);
			}
			else
			{
				TargetLocation = TargetActor->GetActorLocation();
			}
			FireRotation = (TargetLocation - FireSourceLocation).Rotation();

			FTransform SpawnActorTransform(FireRotation.Quaternion(), FireSourceLocation);

			ADamageProjectileActor* LimboIceActor = GetWorld()->SpawnActorDeferred<ADamageProjectileActor>(LimboIceActorClass, SpawnActorTransform);

			// 设置施加伤害的GE参数
			LimboIceActor->TmpDamageEffectParams = MakeDamageEffectParams(TargetActor, false);

			// 开启归航
			USceneComponent* HomingTargetComponent = IEnemyInterface::Execute_GetTargetingComponent(TargetActor);
			LimboIceActor->SetHomingTargetComponent(HomingTargetComponent);
			// 完成生成
			LimboIceActor->FinishSpawning(SpawnActorTransform);

			return true;
		}
		else
		{
			// 没有找到目标敌人，返回false，不发射
			return false;
		}

	}
		return false;
}

void ULimboIceAbility::GetAbilityDescOnLevel(UAbilitySystemComponent* InASC, int32 InAbilityLevel, FString& OutDescription) const
{
	if (InAbilityLevel > MaxAbilityLevel)
	{
		return;
	}

	// 技能伤害类型
	FString DamageTypeDescription = GetDamageAbilityTypeDescription();
	// 技能伤害倍率
	float DamageMultiplier = GetDamageMultiplier(InAbilityLevel);
	// 技能冷却时间
	float CooldownTime = GetAbilityCooldownTime(InAbilityLevel);
	// 技能消耗魔法值
	float ArcaneCost = GetAbilityManaOrManeCost(InAbilityLevel);

	OutDescription = FString::Printf(TEXT(
		"<AbilityDesc>等级：Lv </><Golden>%d</>\t\
		<AbilityDesc>在玩家上方召唤冰雾，向目标敌人追踪攻击，对沿途敌人造成大量：</><Blue>%s</><AbilityDesc>元素伤害</>\t\
		<AbilityDesc>技能倍率：</><Orange>%.0f%%</>\
		<AbilityDesc>冷却时间：</><Green>%.1f秒</>\
		<AbilityDesc>奥义消耗：</><Pink>%.0f</>"),
		InAbilityLevel,
		*DamageTypeDescription,
		DamageMultiplier * 100.0f,
		CooldownTime,
		ArcaneCost);
}
