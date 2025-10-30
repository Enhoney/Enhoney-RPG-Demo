// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "DamageActorBase.h"
#include "DamageProjectileActor.generated.h"

class UProjectileMovementComponent;

/**
 * 
 */
UCLASS()
class RPGDEMO_API ADamageProjectileActor : public ADamageActorBase
{
	GENERATED_BODY()

public:
	ADamageProjectileActor();

	// 设置发射物移动组件的归航
	void SetHomingTargetComponent(USceneComponent* InHomingTargetComponent);

protected:
	// 绑定归航敌人死亡回调--销毁
	void BeginPlay() override;

	UFUNCTION()
	void OnHomingTargetDead(AActor* DeadActor);

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	
};
