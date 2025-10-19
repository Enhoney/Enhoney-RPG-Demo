// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacterBase.h"
#include "BossInterface.h"
#include "BossEnemyCharacterBase.generated.h"

class USphereComponent;

/**
 * 
 */
UCLASS()
class RPGDEMO_API ABossEnemyCharacterBase : public AEnemyCharacterBase, public IBossInterface
{
	GENERATED_BODY()

public:

	ABossEnemyCharacterBase();

	/** Enemy Interface Start*/
	virtual void MulticastUpdateHealthBar_Implementation(float NewCurrentHealth, float NewCurrentMaxHealth) override;
	/** Enemy Interface End*/

	/** Boss Interface Start*/
	virtual void TestFunc_Implementation() override;
	/** Boss Interface End*/


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 玩家角色进入入侵范围
	UFUNCTION()
	void OnPlayerCharacterStartInvade(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 玩家角色退出入侵范围
	UFUNCTION()
	void OnPlayerCharacterQuitInvade(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	virtual void BeginPlay() override;

	// 显示或者隐藏BOSS血条
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetBossHealthBarVisibility(ACharacter* InPlayerCharacter, bool bIsVisiblie);

	// 更新BOSS血条
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateBossHealthBar(FName InBossName, float NewCurrentHealth, float NewCurrentMaxHealth);

protected:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Invaders")
	TArray<TWeakObjectPtr<ACharacter>> Invaders;

	// BOSS名字
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BossName")
	FName BossName = TEXT("BOSS");

private:
	UPROPERTY(EditDefaultsOnly, Category = "ScopeOfInvasion")
	TObjectPtr<USphereComponent> InvaderSphereCollision;
};
