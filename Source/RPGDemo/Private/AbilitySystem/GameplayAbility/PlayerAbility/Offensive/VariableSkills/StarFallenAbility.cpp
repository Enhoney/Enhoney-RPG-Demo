// Copyright Enhoney.


#include "PlayerAbility/Offensive/VariableSkills/StarFallenAbility.h"
#include "CombatInterface.h"
#include "PlayerInterface.h"
#include "EnemyInterface.h"
#include "CommonAlgorithmLibrary.h"
#include "Actor/FallenStarBase.h"

bool UStarFallenAbility::SpawnFallenStarToEnemy(FName EnemyFallenStarSocketName)
{
	// 在服务器生成
	const bool bServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bServer)
	{
		return false;
	}
	check(FallenStarActorClass);

	if (GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{

		// 找到目标敌人
		AActor* TargetActor = nullptr;
		if (GetAvatarActorFromActorInfo()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_GetPlayerTargetEnemy(GetAvatarActorFromActorInfo(), TargetActor);
		}

		if (!IsValid(TargetActor))
		{
			// 如果没有锁定敌人，就找到最近的敌人，向他发射
			TargetActor = UCommonAlgorithmLibrary::GetNearstEnemyInRadius(GetAvatarActorFromActorInfo(), 2000.f, false);
		}

		// 找不到目标敌人，返回false
		if (!IsValid(TargetActor))
		{
			return false;
		}
		// 得到生成位置
		FVector SpawneLocation = ICombatInterface::Execute_GetFireSocketLocation(TargetActor, EnemyFallenStarSocketName);
		FTransform SpawnActorTransform(FRotator::ZeroRotator, SpawneLocation);

		AFallenStarBase* FallenStar = GetWorld()->SpawnActorDeferred<AFallenStarBase>(FallenStarActorClass, SpawnActorTransform);

		// 设置施加伤害的GE参数
		FallenStar->TmpDamageEffectParams = MakeDamageEffectParams(nullptr, false);
		// 是否满级了，切换陨星特效
		FallenStar->SetFallenStarNiagaraIfOnMaxLevel(GetAbilityLevel() >= MaxAbilityLevel);

		FallenStar->FinishSpawning(SpawnActorTransform);

		return true;

	}


	return false;
}

void UStarFallenAbility::GetAbilityDescOnLevel(UAbilitySystemComponent* InASC, int32 InAbilityLevel, FString& OutDescription) const
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
		<AbilityDesc>在敌人头顶召唤星尘坠落，持续向下方敌人造成大量：</><Orange>%s</><AbilityDesc>元素伤害</>\t\
		<AbilityDesc>技能倍率：</><Orange>%.0f%%</>\
		<AbilityDesc>冷却时间：</><Green>%.1f秒</>\
		<AbilityDesc>奥义消耗：</><Pink>%.0f</>"),
		InAbilityLevel,
		*DamageTypeDescription,
		DamageMultiplier * 100.0f,
		CooldownTime,
		ArcaneCost);
}
