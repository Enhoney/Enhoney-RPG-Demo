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

	// ������ƷTag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MineralRewardItem")
	FGameplayTag RewardItemTag;
	// ��Ʒ��������
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MineralRewardItem")
	float RewardItemProbability = 1.f;
	// ��С������Ʒ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MineralRewardItem")
	int32 MinRewardItemCount = 1;
	// �������Ʒ����
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

	// ִ������
	void BreakMineral();

	// ���ͽ���
	void GiveReward();

	// ��һ��п�ʯ
	UFUNCTION()
	void OnHitByPlayer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void MineralHitReact();
	
private:
	// ��������ļ��������
	UPROPERTY(VisibleAnywhere, Category = "GeometryCollection")
	TObjectPtr<UGeometryCollectionComponent> GeometryCollectionComponent;

	// ������ֵ
	UPROPERTY(EditAnywhere, Category = "GeometryCollection")
	float BreakThreshold = 100.f;

	// ��ǰ����ֵ
	UPROPERTY(EditAnywhere, Category = "GeometryCollection")
	float CurrentBreakValue = 0.f;
	
	// ����ֵ--��ҹ������ӻ���ֵ��ʱ������ֵԽ�󣬵��ӵĻ���ֵԽ��
	UPROPERTY(EditAnywhere, Category = "GeometryCollection")
	float Toughness = 3.f;

	// ������
	UPROPERTY()
	TWeakObjectPtr<AActor> Breaker = nullptr;

	// ������Ʒ�͸���
	UPROPERTY(EditAnywhere, Category = "GeometryCollection|Reward")
	TArray<FMineralRewardConfig> RewardItemsConfig;
	
	// ������
	UPROPERTY(EditAnywhere, Category = "GeometryCollection|Reward")
	TSubclassOf<AFieldSystemActor> ForceFieldClass;

	// WidgetComponent--��ʾ�;ðٷֱ�
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> ToughnessBar;

	// �������--���ڲ���������Ч
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> HitSoundComponent;

	// ��ʾ���ưٷֱȵ�
	UPROPERTY()
	FTimerHandle HideToughnessBarHandle;

};
