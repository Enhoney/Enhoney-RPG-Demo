// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacterBase.h"
#include "BossInterface.h"
#include "ActiveGameplayEffectHandle.h"
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

	/** Combat Interfce Start*/
	virtual void CharacterDie_Implementation() override;
	/** Combat Interfce End*/

	/** Boss Interface Start*/
	virtual EBossPhase GetBossPhase_Implementation() const override;
	virtual void SwitchToPhaseTwoForAbility_Implementation() override;
	virtual void SwitchToPhaseTwoForBuff_Implementation() override;
	/** Boss Interface End*/


	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 玩家角色进入入侵范围
	UFUNCTION()
	void OnPlayerCharacterStartInvade(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 玩家角色退出入侵范围
	UFUNCTION()
	void OnPlayerCharacterQuitInvade(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
	virtual void BeginPlay() override;

	// 显示或者隐藏BOSS血条
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetBossHealthBarVisibility(ACharacter* InPlayerCharacter, bool bIsVisiblie);

	// 更新BOSS血条
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateBossHealthBar(FName InBossName, float NewCurrentHealth, float NewCurrentMaxHealth);

	// 找到下一个入侵者作为攻击目标
	void FindNextInvaderAsTarget();

protected:
	UPROPERTY(Replicated, VisibleAnywhere, Category = "Invaders")
	TArray<ACharacter*> Invaders;
	// 出生点
	UPROPERTY(EditInstanceOnly, Category = "BirthPoint")
	TWeakObjectPtr<AActor> BirthPoint;

	// BOSS当前阶段
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "BossPhase")
	EBossPhase BossPhase = EBossPhase::EBP_Phase1;

	// 一阶段的Buff
	UPROPERTY(EditDefaultsOnly, Category = "BossPhase")
	TSubclassOf<UGameplayEffect> BuffOnPhase1;
	// 二阶段的Buff
	UPROPERTY(EditDefaultsOnly, Category = "BossPhase")
	TSubclassOf<UGameplayEffect> BuffOnPhase2;
	// Buff的Spec，用于
	UPROPERTY()
	FActiveGameplayEffectHandle PhaseBuffHandle;

	// BOSS名字
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BossName")
	FName BossName = TEXT("BOSS");


private:
	UPROPERTY(EditDefaultsOnly, Category = "ScopeOfInvasion")
	TObjectPtr<USphereComponent> InvaderSphereCollision;
};
