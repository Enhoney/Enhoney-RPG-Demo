// Copyright Enhoney.


#include "ProjectileAbilityBase.h"

#include "CombatInterface.h"
#include "PlayerInterface.h"
#include "EnemyInterface.h"
#include "Actor/DamageActorBase.h"


void UProjectileAbilityBase::SpawnProjectileToActor(FName SocketName)
{
	// 在服务器生成
	const bool bServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bServer)
	{
		return;
	}
	check(ProjectileActorClass);

	if (GetAvatarActorFromActorInfo()->Implements<UCombatInterface>())
	{
		FVector FireSourceLocation = ICombatInterface::Execute_GetFireSocketLocation(GetAvatarActorFromActorInfo(), SocketName);
		FRotator FireRotation = FRotator();

		AActor* TargetActor = nullptr;
		bool bTargetLocking = false;
		// 确定是否锁敌
		if (GetAvatarActorFromActorInfo()->Implements<UPlayerInterface>())
		{
			bTargetLocking = IPlayerInterface::Execute_GetPlayerTargetEnemy(GetAvatarActorFromActorInfo(), TargetActor);
		}

		if (bTargetLocking && IsValid(TargetActor))
		{
			// 如果锁敌了，就向敌人目标组件处发射
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
		}
		else
		{
			FireRotation = GetAvatarActorFromActorInfo()->GetActorRotation();
		}
		FTransform SpawnActorTransform(FireRotation.Quaternion(), FireSourceLocation);

		ADamageActorBase* DamageActor = GetWorld()->SpawnActorDeferred<ADamageActorBase>(ProjectileActorClass, SpawnActorTransform);

		// 设置施加伤害的GE参数
		DamageActor->TmpDamageEffectParams = MakeDamageEffectParams(nullptr, false);

		DamageActor->FinishSpawning(SpawnActorTransform);
	}
	
}
