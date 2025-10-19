// Copyright Enhoney.


#include "AbilitySystem/Actor/EnhoneyMineralBase.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "DamageBoxComponent.h"
#include "RPGDemo.h"
#include "EnhoneyLogChannel.h"
#include "PlayerInterface.h"
#include "InventoryComponent.h"
#include "PlayerTaskComponent.h"
#include "Field/FieldSystemActor.h" // 用于物理场
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include "CommonAlgorithmLibrary.h"
#include "TimerManager.h"
#include "EnemyHealthBarWidget.h"


AEnhoneyMineralBase::AEnhoneyMineralBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComponent"));
	SetRootComponent(GeometryCollectionComponent);
	GeometryCollectionComponent->SetIsReplicated(true);
	GeometryCollectionComponent->SetSimulatePhysics(true);	// 开启模拟物理
	GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GeometryCollectionComponent->SetCollisionObjectType(ECC_CombatMesh);	// 使用接受伤害的碰撞对象，但是对于Pawn执行Block，对于DamageObject执行Overlap
	GeometryCollectionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_MonitoringCollision, ECR_Ignore);	// 对于监测碰撞忽略
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_EnemyLocking, ECR_Ignore);	// 对于敌人锁定忽略
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_DamageObject, ECR_Overlap);	// 对于伤害物体执行Overlap
	GeometryCollectionComponent->SetGenerateOverlapEvents(true);

	ToughnessBar = CreateDefaultSubobject<UWidgetComponent>(TEXT("ToughnessBar"));
	ToughnessBar->SetupAttachment(GetRootComponent());
	ToughnessBar->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	HitSoundComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("HitSoundComponent"));
	HitSoundComponent->SetupAttachment(GetRootComponent());
	HitSoundComponent->bAutoActivate = false;
}


void AEnhoneyMineralBase::BeginPlay()
{
	Super::BeginPlay();

	// 绑定玩家击中矿石的事件
	GeometryCollectionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnHitByPlayer);

	// 在游戏一开始，击破值是隐藏的
	ToughnessBar->SetVisibility(false);
	
}

void AEnhoneyMineralBase::BreakMineral()
{
	if (!ForceFieldClass) return;
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		// 在Geometry Collection的位置生成场
		AFieldSystemActor* ForceField = World->SpawnActor<AFieldSystemActor>(ForceFieldClass, GetActorLocation(), GetActorRotation(), SpawnParams);
	}
}

void AEnhoneyMineralBase::GiveReward()
{
	if(!Breaker.IsValid())
	{
		UE_LOG(LogEnhoney, Warning, TEXT("AEnhoneyMineralBase::GiveReward: Breaker is not valid!"));
		return;
	}

	if (Breaker->Implements<UPlayerInterface>())
	{
		UInventoryComponent* InventoryComponent =  IPlayerInterface::Execute_GetPlayerInventoryComponent(Breaker.Get());
		// 获取玩家的任务组件
		UPlayerTaskComponent* PlayerTaskComp = IPlayerInterface::Execute_GetPlayerTaskComponent(Breaker.Get());
		if (IsValid(InventoryComponent))
		{
			for (const FMineralRewardConfig& Reward : RewardItemsConfig)
			{
				FInventoryItem ItemToReward;
				if (Reward.GetRandomRewardItem(ItemToReward))
				{
					InventoryComponent->ServerAddItemByTag(ItemToReward);

					// 增加任务物品计数
					if (IsValid(PlayerTaskComp) && ItemToReward.ItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Task"))))
					{
						PlayerTaskComp->TryAddCommitItemCount(ItemToReward.ItemTag, ItemToReward.ItemNum);
					}
				}
			}
		}
	}
}

void AEnhoneyMineralBase::OnHitByPlayer(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->IsA(UDamageBoxComponent::StaticClass()))
	{
		UDamageBoxComponent* DamageBox = Cast<UDamageBoxComponent>(OtherComp);
		if (IsValid(DamageBox))
		{
			// 计算击破值
			CurrentBreakValue += FMath::Max((DamageBox->GetBaseBreakValue() - Toughness), 0.f);
		}


		// 需要在本地播放和显示
		// 播放受击音效，显示击破值
		if (UCommonAlgorithmLibrary::IsLocalEnhoneyPlayer(OtherComp->GetOwner()))
		{
			MineralHitReact();
		}
		
		// 如果击破了，就在服务器记录击碎者，并发放奖励，在客户端执行破碎
		if (CurrentBreakValue >= BreakThreshold && !Breaker.IsValid())
		{
			SetLifeSpan(3.f);	// 3秒后销毁

			// 如果是服务器，就记录击碎者，并发放奖励
			if (GetLocalRole() == ROLE_Authority)
			{
				// 我们设计的时候，这里传递的就是玩家--AvatarActor
				if (IsValid(DamageBox->DamageEffectParams.WorldContextObject) && CastChecked<AActor>(DamageBox->DamageEffectParams.WorldContextObject))
				{
					// 远程攻击的情况下，WorldContextObject传递的是AvatarActor
					Breaker = Cast<AActor>(DamageBox->DamageEffectParams.WorldContextObject);
					
				}
				else if(DamageBox->GetOwner()->Implements<UPlayerInterface>())
				{
					// 近战攻击的情况下，DamageEffectParams只有在施加伤害的时候才设置，所以这里传递DamageBox的Owner
					Breaker = DamageBox->GetOwner();
				}
				
				if(Breaker.IsValid())
				{
					GiveReward();
				}
				else
				{
					UE_LOG(LogEnhoney, Warning, TEXT("AEnhoneyMineralBase::OnHitByPlayer: Breaker is not valid or not an Actor!"));
				}

			}
			// 如果是单机模式，或者是本地玩家，就直接破碎
			if (GetNetMode() == NM_Standalone ||
				(GetNetMode() == NM_ListenServer) && (GetLocalRole() != ROLE_SimulatedProxy) ||
				(GetNetMode() == NM_ListenServer) && (GetLocalRole() == ROLE_AutonomousProxy))
			{
				BreakMineral();
			}
			// 破碎之后不在响应DamageBox的碰撞
			GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_DamageObject, ECR_Ignore);
			
		}
	}
}

void AEnhoneyMineralBase::MineralHitReact()
{
	if (HitSoundComponent->IsPlaying())
	{
		HitSoundComponent->Stop();
	}
	HitSoundComponent->Play();

	// 显示击破条，并更新
	ToughnessBar->SetVisibility(true);
	if (UEnemyHealthBarWidget* ToughnessBarWidget = Cast<UEnemyHealthBarWidget>(ToughnessBar->GetWidget()))
	{
		ToughnessBarWidget->UpdateHealthBarValue(BreakThreshold - CurrentBreakValue, BreakThreshold);
	}
	// 定时隐藏击破条
	FTimerDelegate HideToughnessBarDelegate;
	HideToughnessBarDelegate.BindWeakLambda(this, [this]()
		{
			if (ToughnessBar->IsVisible())
			{
				GetWorld()->GetTimerManager().ClearTimer(this->HideToughnessBarHandle);

				ToughnessBar->SetVisibility(false);
			}

		});
	GetWorld()->GetTimerManager().SetTimer(HideToughnessBarHandle, HideToughnessBarDelegate, 5.f, false);
}

bool FMineralRewardConfig::GetRandomRewardItem(FInventoryItem& OutDroppedItem) const
{
	// 先判断是否掉落
	if (FMath::FRand() <= RewardItemProbability && RewardItemTag.IsValid())
	{
		// 计算掉落数量
		int32 ItemRewardNum = FMath::RandRange(MinRewardItemCount, MaxRewardItemCount);
		OutDroppedItem = FInventoryItem(RewardItemTag, ItemRewardNum);
		return true;
	}
	return false;
}
