// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "ScalableFloat.h"
#include "EnhoneyPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerStateValueChangedSignature, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FPlayerStateTwoValueChangedSignature, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerStateBoolValueChangedSignature, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FPlayerStateInt64ValueChangedSignature, int64);

class UEnhoneyAbilitySystemComponent;
class UPlayerAttributeSet;

class UEnhoneyPlayerAbilityInfo;

class UInventoryComponent;
class UPlayerTaskComponent;

struct FScalableFloat;

/**
 * 
 */
UCLASS()
class RPGDEMO_API AEnhoneyPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AEnhoneyPlayerState();

	/** AbilitySystem Interface start*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/** AbilitySystem Interface end*/

	UPlayerAttributeSet* GetPlayerAttributeSet() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	/** 获取和设置额外属性*/

	// 等级
	FORCEINLINE int32 GetCharacterLevel() const { return CharacterLevel; }
	void AddCharacterLevel(int32 LevelToAdd);
	void SetCharacterLevel(int32 LevelToSet);

	// 经验值
	FORCEINLINE int32 GetCurrentExp() const { return CurrentExp; }
	void AddCurrentExp(int32 ExpToAdd);
	void SetCurrentExp(int32 ExpToSet);

	// 属性点
	FORCEINLINE int32 GetAttributePoint() const { return AttributePoint; }
	FORCEINLINE bool CanAttributeConsumed(int32 AttributePointToComsume) const { return AttributePointToComsume <= AttributePoint; }
	void AddAttributePoint(int32 AttributePointToAdd);
	void SetAttributePoint(int32 AttributePointToSet);
	void ConsumeAttributePoint(int32 AttributePointToComsume);	// 消耗属性点

	// 技能点
	FORCEINLINE int32 GetSkillPoint() const { return SkillPoint; }
	FORCEINLINE bool CanSkillPointConsumed(int32 SkillPointToComsume) const { return SkillPointToComsume <= SkillPoint; }
	void AddSkillPoint(int32 SkillPointToAdd);
	void SetSkillPoint(int32 SkillPointToSet);
	void ConsumeSkillPoint(int32 SkillPointToComsume);	// 消耗技能点

	// 金币数量
	FORCEINLINE int64 GetCoinNum() const { return CoinNum; }
	FORCEINLINE bool CanCoinNumConsumed(int64 CoinNumToComsume) const { return CoinNumToComsume <= CoinNum; }
	void AddCoinNum(int64 CoinNumToAdd);
	void SetCoinNum(int64 CoinNumToSet);
	void ConsumeCoinNum(int64 CoinNumToComsume);	// 消耗金币
	// 增加金币
	UFUNCTION(Server, Reliable)
	void ServerAddCoinNum(int64 CoinNumToAdd);
	// 消费金币
	UFUNCTION(Server, Reliable)
	void ServerConsumeCoinNum(int64 CoinNumToComsume);

	// 是否存活
	FORCEINLINE bool IsPlayerAlive() const { return bAlive; }
	void SwitchAliveState(bool NewAliveState);

	// 获取InventoryComponent
	UFUNCTION(BlueprintCallable)
	UInventoryComponent* GetInventoryComponent();

	// 获取玩家任务组件
	UFUNCTION(BlueprintCallable)
	UPlayerTaskComponent* GetPlayerTaskComponent();

	// 查询当前等级升级所需经验
	int32 GetExpForLevelUp(int32 InLevel) const;

protected:
	UFUNCTION()
	void OnRep_CharacterLevel(int32 OldCharacterLevel);

	UFUNCTION()
	void OnRep_CurrentExp(int32 OldExp);

	UFUNCTION()
	void OnRep_AttributePoint(int32 OldAttributePoint);

	UFUNCTION()
	void OnRep_SkillPoint(int32 OldSkillPoint);

	UFUNCTION()
	void OnRep_IsAlive(bool OldbAlive);

	UFUNCTION()
	void OnRep_CoinNum(int64 OldCoinNum);

	// 查询增加经验后可以升到多少级
	int32 GetLevelUpToLevel(int32 ExpToAdd) const;

public:
	// 等级变化
	FPlayerStateValueChangedSignature OnCharacterLevelChanged;
	// 经验值变化
	FPlayerStateTwoValueChangedSignature OnCurrentExpChanged;
	// 属性点变化
	FPlayerStateValueChangedSignature OnAttributePointChanged;
	// 技能点变化
	FPlayerStateValueChangedSignature OnSkillPointChanged;
	// 金币变化
	FPlayerStateInt64ValueChangedSignature OnCoinNumChanged;
	// 是否死亡
	FPlayerStateBoolValueChangedSignature OnIsAliveChanged;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AbilityInfo")
	TObjectPtr<UEnhoneyPlayerAbilityInfo> PlayerAbilityInfo;

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEnhoneyAbilitySystemComponent> PlayerASC;

	UPROPERTY()
	TObjectPtr<UPlayerAttributeSet> PlayerAttributeSet;

	// 玩家等级
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CharacterLevel)
	int32 CharacterLevel = 1;

	// 当前经验
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CurrentExp)
	int32 CurrentExp = 0;

	// 属性点
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_AttributePoint)
	int32 AttributePoint = 5;

	// 技能点
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_SkillPoint)
	int32 SkillPoint = 1;

	// 金币数量
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_CoinNum)
	int64 CoinNum = 1000;

	// 是否还活着
	UPROPERTY(EditAnywhere, BlueprintReadOnly, ReplicatedUsing = OnRep_IsAlive)
	bool bAlive = true;

private:
	// 等级最小值
	UPROPERTY(EditDefaultsOnly, Category = "LevelClamp|Min")
	int32 MinValueOfLevel = 1;

	// 等级最大值
	UPROPERTY(EditDefaultsOnly, Category = "LevelClamp|Max")
	int32 MaxValueOfLevel = 100;

	// 库存组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	// 玩家任务组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerTaskComponent> PlayerTaskComponent;

	/** 升级配置*/
	// 等级提升所需经验配置
	UPROPERTY(EditDefaultsOnly, Category = "LevelUpConfig")
	FScalableFloat ExpConfigForLevelUp;
	// 每提升一级获得的属性点
	UPROPERTY(EditDefaultsOnly, Category = "LevelUpConfig")
	int32 AttributePointPerLevel = 1;
	// 每提升多少级获得1点技能点
	UPROPERTY(EditDefaultsOnly, Category = "LevelUpConfig")
	int32 SkillPointPerLevel = 5;
};
