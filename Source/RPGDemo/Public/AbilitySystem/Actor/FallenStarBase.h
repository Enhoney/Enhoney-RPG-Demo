// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnhoneyAbilityType.h"
#include "FallenStarBase.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class RPGDEMO_API AFallenStarBase : public AActor
{
	GENERATED_BODY()
	
public:	

	AFallenStarBase();

	// 属性复制--为了DamageEffectParams
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	// 定期向下发射多球体追踪，对命中敌人造成伤害
	void EmitFallenStarProjectiles();

public:
	// DamageEffectParams
	UPROPERTY(BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn = "true"))
	FEnhoneyDamageEffectParams TmpDamageEffectParams;

	// 如果满级了，切换陨星特效
	void SetFallenStarNiagaraIfOnMaxLevel(bool bIsOnMaxLevel);
private:
	UPROPERTY(VisibleAnywhere, Category = "FallenStarNiagaraComponent")
	TObjectPtr<UNiagaraComponent> FallenStarNiagaraComponent;

	UPROPERTY()
	FTimerHandle EmitFallenStarTimerHandle;

	// 满级的粒子效果
	UPROPERTY(EditDefaultsOnly, Category = "FallenStar")
	TObjectPtr<UNiagaraSystem> MaxLevelFallenStarNiagaraSystem;

	// 检测高度
	UPROPERTY(EditDefaultsOnly, Category = "FallenStar")
	float MinTraceHeight = -500.f;
	// 检测半径
	UPROPERTY(EditDefaultsOnly, Category = "FallenStar")
	float TraceRadius = 150.f;
	// 是否绘制调试线框
	UPROPERTY(EditDefaultsOnly, Category = "FallenStar")
	bool bIsDrawDebugTrace = true;
	// 频率
	UPROPERTY(EditDefaultsOnly, Category = "FallenStar")
	float TraceFrequency = 0.8f;
	// 初始延迟
	UPROPERTY(EditDefaultsOnly, Category = "FallenStar")
	float TraceInitialDelay = 2.5f;


};
