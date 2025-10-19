// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnhoneyAbilityType.h"
#include "DamageActorBase.generated.h"

class UDamageBoxComponent;

UCLASS()
class RPGDEMO_API ADamageActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	ADamageActorBase();

	virtual void Tick(float DeltaTime) override;

	// 属性复制--为了DamageEffectParams
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	// DamageEffectParams
	UPROPERTY(BlueprintReadWrite, Replicated, meta = (ExposeOnSpawn = "true"))
	FEnhoneyDamageEffectParams TmpDamageEffectParams;

private:
	UPROPERTY(VisibleAnywhere, Category = "ProjectileBox")
	TObjectPtr<UDamageBoxComponent> ProjectileBox;

};
