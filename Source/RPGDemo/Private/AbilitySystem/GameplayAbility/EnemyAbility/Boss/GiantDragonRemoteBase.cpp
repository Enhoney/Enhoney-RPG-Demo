// Copyright Enhoney.


#include "EnemyAbility/Boss/GiantDragonRemoteBase.h"
#include "BossInterface.h"
#include "CombatInterface.h"
#include "DamageActorBase.h"

void UGiantDragonRemoteBase::SpawnProjectileToTargetPlayer(FName SocketName)
{
	// 在服务器生成
	const bool bServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bServer)
	{
		return;
	}

	
	// 仅供Boss使用
	if (!GetAvatarActorFromActorInfo()->Implements<UBossInterface>())
	{
		return;
	}

	check(ProjectileActorClass_Phase1);
	check(ProjectileActorClass_Phase2);

	// 判定阶段
	EBossPhase BossPhase = IBossInterface::Execute_GetBossPhase(GetAvatarActorFromActorInfo());

	if (GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		FVector FireSourceLocation = ICombatInterface::Execute_GetFireSocketLocation(GetAvatarActorFromActorInfo(), SocketName);
		FRotator FireRotation = ICombatInterface::Execute_GetFireSocketRotation(GetAvatarActorFromActorInfo(), SocketName);

		
		FTransform SpawnActorTransform(FireRotation.Quaternion(), FireSourceLocation);

		ADamageActorBase* DamageActor = nullptr;
		
		// 一阶段火球
		if (BossPhase == EBossPhase::EBP_Phase1)
		{
			DamageActor = GetWorld()->SpawnActorDeferred<ADamageActorBase>(ProjectileActorClass_Phase1, SpawnActorTransform);
		}
		else if (BossPhase == EBossPhase::EBP_Phase2)
		{
			// 二阶段冰球
			DamageActor = GetWorld()->SpawnActorDeferred<ADamageActorBase>(ProjectileActorClass_Phase2, SpawnActorTransform);
		}

		if (DamageActor)
		{
			// 设置施加伤害的GE参数
			DamageActor->TmpDamageEffectParams = MakeDamageEffectParamsByPhase(nullptr, false);
			DamageActor->SetActorScale3D(FVector(2.0f));	// 巨龙释放两倍大小的冰火球
			DamageActor->FinishSpawning(SpawnActorTransform);
		}
	}
}
