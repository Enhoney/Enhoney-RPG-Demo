// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "EnhoneyAbilityType.h"
#include "DamageBoxComponent.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UDamageBoxComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	UDamageBoxComponent();

	// 启用和取消CombatBoxCollision的碰撞
	UFUNCTION(BlueprintCallable)
	void SetCombatBoxCollisionEnable(bool bIsEnable);

	// 属性复制--为了DamageEffectParams
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 设置击破值
	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetBaseBreakValue(float InBaseBreakValue) 
	{ 
		BaseBreakValue = InBaseBreakValue; 
	}

	// 获取击破值
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetBaseBreakValue() const 
	{ 
		return BaseBreakValue; 
	}


public:
	// DamageEffectParams
	UPROPERTY(BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn = "true"))
	FEnhoneyDamageEffectParams DamageEffectParams;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnDamageBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UPROPERTY(EditDefaultsOnly,Category = "DamageBox")
	bool bIsMeleeAttack = true;

private:
	// 基础击破值
	UPROPERTY(EditDefaultsOnly, Category = "GeometryCollection")
	float BaseBreakValue = 20.f;
	
};
