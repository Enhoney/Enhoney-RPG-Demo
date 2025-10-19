// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "EnhoneyMineralBase.generated.h"

class UGeometryCollectionComponent;
class AFieldSystemActor;
class UWidgetComponent;
class UAudioComponent;

struct FInventoryItem;

USTRUCT(BlueprintType)
struct RPGDEMO_API FMineralRewardConfig
{
	GENERATED_BODY()
public:
	bool GetRandomRewardItem(FInventoryItem& OutDroppedItem) const;

	// 奖励物品Tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MineralRewardItem")
	FGameplayTag RewardItemTag;
	// 物品奖励概率
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MineralRewardItem")
	float RewardItemProbability = 1.f;
	// 最小奖励物品个数
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MineralRewardItem")
	int32 MinRewardItemCount = 1;
	// 最大奖励物品个数
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MineralRewardItem")
	int32 MaxRewardItemCount = 1;

};

UCLASS()
class RPGDEMO_API AEnhoneyMineralBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnhoneyMineralBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 执行破碎
	void BreakMineral();

	// 发送奖励
	void GiveReward();

	// 玩家击中矿石
	UFUNCTION()
	void OnHitByPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void MineralHitReact();
	
private:
	// 用于破碎的几何体组件
	UPROPERTY(VisibleAnywhere, Category = "GeometryCollection")
	TObjectPtr<UGeometryCollectionComponent> GeometryCollectionComponent;

	// 击破阈值
	UPROPERTY(EditAnywhere, Category = "GeometryCollection")
	float BreakThreshold = 100.f;

	// 当前击破值
	UPROPERTY(EditAnywhere, Category = "GeometryCollection")
	float CurrentBreakValue = 0.f;
	
	// 韧性值--玩家攻击叠加击破值的时候，韧性值越大，叠加的击破值越少
	UPROPERTY(EditAnywhere, Category = "GeometryCollection")
	float Toughness = 3.f;

	// 击碎者
	UPROPERTY()
	TWeakObjectPtr<AActor> Breaker = nullptr;

	// 奖励物品和个数
	UPROPERTY(EditAnywhere, Category = "GeometryCollection|Reward")
	TArray<FMineralRewardConfig> RewardItemsConfig;
	
	// 力场类
	UPROPERTY(EditAnywhere, Category = "GeometryCollection|Reward")
	TSubclassOf<AFieldSystemActor> ForceFieldClass;

	// WidgetComponent--显示耐久百分比
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> ToughnessBar;

	// 声音组件--用于播放命中音效
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> HitSoundComponent;

	// 显示击破百分比的
	UPROPERTY()
	FTimerHandle HideToughnessBarHandle;

};
