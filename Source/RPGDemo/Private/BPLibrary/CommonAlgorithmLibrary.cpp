// Copyright Enhoney.


#include "CommonAlgorithmLibrary.h"

#include "Kismet/GameplayStatics.h"
#include "EnemyCharacterBase.h"
#include "PlayerCharacterBase.h"
#include "CombatInterface.h"

#include "EnhoneyLogChannel.h"

#include "EnhoneyPlayingHUD.h"

AActor* UCommonAlgorithmLibrary::GetNearstEnemyInRadius(const AActor* InSourceActor, float InRadiusThreshold, bool bWarningNotFound)
{
	AActor* NearstActor = nullptr;

	TArray<AActor*> AllAliveEnemies;
	// 查找所有敌人
	UGameplayStatics::GetAllActorsOfClass(InSourceActor, AEnemyCharacterBase::StaticClass(), AllAliveEnemies);
	if (!AllAliveEnemies.IsEmpty())
	{
		// 找到其中已经死亡，但是还没有回收的敌人
		TArray<AActor*> AllDeadEnemies;
		for (AActor* Enemy : AllAliveEnemies)
		{
			if (!ICombatInterface::Execute_IsCharacterAlive(Enemy))
			{
				AllDeadEnemies.Add(Enemy);
			}
		}
		// 剔除已经死亡的敌人
		AllAliveEnemies.RemoveAll([AllDeadEnemies](AActor* Enemy)
			{
				return AllDeadEnemies.Contains(Enemy);
			});

		// 找到最近的存活着的敌人
		
		double NearstDistance = TNumericLimits<double>::Max();
		for (AActor* Enemy : AllAliveEnemies)
		{
			double Distance2D = FVector::DistXY(InSourceActor->GetActorLocation(), Enemy->GetActorLocation());
			if (Distance2D < NearstDistance)
			{
				NearstDistance = Distance2D;
				NearstActor = Enemy;
			}
		}

		// 如果这个距离小于阈值半径，就返回这个Enemy
		if (NearstDistance < InRadiusThreshold)
		{
			return NearstActor;
		}
	}

	if (bWarningNotFound)
	{
		UE_LOG(LogEnhoney, Warning, TEXT("No Enemy In Radius Range!"));
	}

	return nullptr;
}

bool UCommonAlgorithmLibrary::GetPlayingWidgetControllerByCharacter(ACharacter* PlayerCharacter, UPlayingWidgetController*& OutPlayingWidgetController)
{
	if (IsValid(PlayerCharacter))
	{
		if (PlayerCharacter->GetController())
		{
			if (APlayerController* PlayerControlelr = Cast<APlayerController>(PlayerCharacter->GetController()))
			{
				if(AEnhoneyPlayingHUD* EnhoneyHUD = Cast<AEnhoneyPlayingHUD>(PlayerControlelr->GetHUD()))
				{
					// 这个时候，PlayingWidgetController一定是有效的，所以传递的参数不用在意
					OutPlayingWidgetController = Cast<UPlayingWidgetController>(EnhoneyHUD->GetPlayingWidgetController(FWidgetControllerParam()));
					if (IsValid(OutPlayingWidgetController))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

EEnhoneyHitDirection UCommonAlgorithmLibrary::GetHitDirection(const FHitResult& InHitResult, const FVector& CharacterForwardVector, const FVector& RootLocation)
{
	EEnhoneyHitDirection OutHitDirection = EEnhoneyHitDirection::Hit_Forward;

	if (IsValid(InHitResult.GetComponent()))
	{
		// 受击Vector
		const FVector HitVector = InHitResult.ImpactPoint - RootLocation;
		const FVector HitVectorNormal = FVector(HitVector.X, HitVector.Y, 0.f).GetSafeNormal();

		// 点乘
		const double CosAngle = FVector::DotProduct(CharacterForwardVector, HitVectorNormal);
		// 计算夹角--计算出来的角度只会在0~180范围，也就是说，只能区分前后，不能区分左右
		double Angle = FMath::Acos(CosAngle);

		// 叉乘--夹角范围为0~180的时候，Z向才是正数，那么如果小于0，就是左向
		const FVector CrossVector = FVector::CrossProduct(CharacterForwardVector, HitVectorNormal);
		if (CrossVector.Z < 0)
		{
			Angle *= -1;
		}

		// 那么这个时候，角度就是-180~180的范围了

		if (Angle <= 45.f && Angle >= -45.f)
		{
			// 前向
			OutHitDirection = EEnhoneyHitDirection::Hit_Forward;
		}
		else if (Angle >= 135.f || Angle <= -135.f)
		{
			// 后向
			OutHitDirection = EEnhoneyHitDirection::Hit_Backward;
		}
		else if (Angle > 45.f && Angle < 135.f)
		{
			// 右向
			OutHitDirection = EEnhoneyHitDirection::Hit_Right;
		}
		else if (Angle > -135.f && Angle < -45.f)
		{
			// 左向
			OutHitDirection = EEnhoneyHitDirection::Hit_Left;

		}
	}

	return OutHitDirection;
}

bool UCommonAlgorithmLibrary::IsLocalEnhoneyPlayer(const AActor* InWorldObjectContext)
{
	check(IsValid(InWorldObjectContext));

	// 单机模式
	if (InWorldObjectContext->GetNetMode() == NM_Standalone)
	{
		return true;
	}

	// LS或者DS的客户端本地
	if (InWorldObjectContext->GetNetMode() == NM_Client && InWorldObjectContext->GetLocalRole() == ROLE_AutonomousProxy)
	{
		return true;
	}

	// LS的服务器
	if (InWorldObjectContext->GetRemoteRole() != ROLE_SimulatedProxy && InWorldObjectContext->GetRemoteRole() == ROLE_Authority)
	{
		return true;
	}

	return false;
}

