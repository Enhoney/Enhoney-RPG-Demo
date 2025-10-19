// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Character/EnhoneyCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "CombatInterface.h"
#include "EnemyInterface.h"
#include "EnemyCharacterBase.generated.h"

class UEnhoneyAbilitySystemComponent;
class UEnemyAttributeSet;
class UGameplayEffect;
class UEnhoneyAbilityBase;
class UWidgetComponent;

class AEnhoneyAIController;
class UBehaviorTree;

class UPawnSensingComponent;

class UMotionWarpingComponent;

struct FInventoryItem;

USTRUCT(BlueprintType)
struct RPGDEMO_API FDroppedItemConfig
{
	GENERATED_BODY()
public:
	bool GetRandomDropItem(FInventoryItem& OutDroppedItem) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroppedItem")
	FGameplayTag ItemType;			// 物品类型标签
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroppedItem")
	float DropProbability = 1.f;	// 掉落概率，0-1之间

	/** 掉落数量*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroppedItem")
	int32 MinDropNum = 1;	// 最小掉落数量
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroppedItem")
	int32 MaxDropNum = 1;	// 最大掉落数量
};

USTRUCT(BlueprintType)
struct RPGDEMO_API FEnemyDiedDropsConfig
{
	GENERATED_BODY()
public:
	// 获取掉落物品
	void GetAllDropItems(TArray<FInventoryItem>& OutDroppedItems) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroppedItem")
	TArray<FDroppedItemConfig> DroppedItemsConfig;	// 掉落物品配置
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroppedItem")
	int32 GoldNum = 0;	// 掉落金币数量
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DroppedItem")
	int32 ExpNum = 0;	// 掉落经验数量
};

/**
 * 
 */
UCLASS()
class RPGDEMO_API AEnemyCharacterBase : public AEnhoneyCharacterBase, public IAbilitySystemInterface, public ICombatInterface, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AEnemyCharacterBase();

	/** AbilitySystem Interface Start*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/** AbilitySystem Interface End*/

	/** Combat Interface Start*/
	virtual int32 GetCharacterLevel_Implementation() const override;
	virtual bool IsCharacterAlive_Implementation() const override;
	virtual void CharacterDie_Implementation() override;
	virtual FVector GetFireSocketLocation_Implementation(FName FireSocketName) override;

	virtual void PlayHitReactionAnim_Implementation(const FHitResult& ImpactResult) override;
	/** Combat Interface End*/

	/** Enemy Interface Start*/
	virtual void SetAsTargetLocking_Implementation() override;
	virtual void QuitTargetLocking_Implementation() override;
	virtual void MulticastShowDamageFloat_Implementation(const FGameplayTag& DamageType, bool bIsCriticalHit, float InDamageValue) override;
	virtual void MulticastUpdateHealthBar_Implementation(float NewCurrentHealth, float NewCurrentMaxHealth) override;

	virtual AActor* GetCurrentCombatTarget_Implementation() const override;
	virtual void SetCombatTarget_Implementation(AActor* NewCombatTarget) override;
	virtual AActor* GetCurrentPatrolPoint_Implementation() const override;
	virtual void SwitchPatrolPoint_Implementation() override;

	virtual EEnemyState GetEnemyState_Implementation() const;
	virtual void EnemyEnterCombat_Implementation(APawn* NewCombatTarget) override;
	virtual void SetWarpingFacing_Implementation() override;
	virtual void SetKillerPawn_Implementation(APawn* InKillerPawn) override;
	/** Enemy Interface End*/

	UEnemyAttributeSet* GetEnemyAttributeSet() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 在这里初始化行为树
	virtual void PossessedBy(AController* NewController) override;

protected:
	void BeginPlay() override;

	// 初始化ASC
	void InitAbilityActorInfo();

	// 初始化属性
	void InitEnemyAttributes();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowDamage(const FGameplayTag& DamageType, bool bIsCriticalHit, float InDamageValue);

	// 赋予敌人固有技能
	void EndowEnemyInherentAbility();

	// 进入战斗状态
	UFUNCTION()
	void EnterCombat(APawn* NewCombatTarget);

public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "EnemyState")
	EEnemyState EnemyState = EEnemyState::EES_Inspection;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEnhoneyAbilitySystemComponent> EnemyAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UEnemyAttributeSet> EnemyAttributeSet;

	// 用于初始化属性的GameplayEffect
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy AbilitySystem|AttributeSet")
	TSubclassOf<UGameplayEffect> GameplayEffectForAttribute;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy AbilitySystem|AttributeSet")
	TSubclassOf<UGameplayEffect> GameplayEffectForVitalAttribute;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enemy|Level")
	int32 CharacterLevel = 1;


	UPROPERTY(Replicated, BlueprintReadOnly, Category = "CharacterState")
	bool bTargetLocked = false;

protected:
	// 反射AIController
	UPROPERTY()
	TObjectPtr<AEnhoneyAIController> EnemyAIController;

	// 使用的行为树
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> EnemyBehaviorTree;

	// 当前攻击目标
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TWeakObjectPtr<AActor> CombatTarget;

	// 击杀者--用于发放奖励，不需要复制
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TWeakObjectPtr<APawn> KillerPawn;

	// 所有的巡逻点
	UPROPERTY(EditInstanceOnly, Category = "Combat")
	TArray<TWeakObjectPtr<AActor>> PatrolPoints;

	// 当前巡逻点
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TWeakObjectPtr<AActor> CurrentPatorlTarget;

private:
	UPROPERTY(VisibleAnywhere, Category = "Enemy|TargetLock")
	TObjectPtr<UWidgetComponent> EnemyTargetWidgetComponent;

	// 敌人主动技能类
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Ability|Ability Classes")
	TArray<TSubclassOf<UEnhoneyAbilityBase>> EnemyOffensiveAbilitiyClasses;
	// 敌人被动技能类
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Ability|Ability Classes")
	TArray<TSubclassOf<UEnhoneyAbilityBase>> EnemyPassiveAbilitiyClasses;

	// 受击动画蒙太奇--普通敌人
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Ability|Montage|Common HitReaction")
	TObjectPtr<UAnimMontage> HitReactionMontage;

	// 感知组件--看到玩家的时候，进入战斗状态
	UPROPERTY(VisibleAnywhere, Category = "Combat")
	TObjectPtr<UPawnSensingComponent> SensingComponent;

	// MotionWarping
	UPROPERTY(VisibleAnywhere, Category = "MotionWarping")
	TObjectPtr<UMotionWarpingComponent> MotinWarpingComponent;

	// 敌人死亡掉落配置
	UPROPERTY(EditAnywhere, Category = "Enemy|DroppedItem")
	FEnemyDiedDropsConfig EnemyDiedDropsConfig;

	UPROPERTY(EditAnywhere, Category = "Enemy|EnemyType")
	FGameplayTag EnemyTypeTag;
};
