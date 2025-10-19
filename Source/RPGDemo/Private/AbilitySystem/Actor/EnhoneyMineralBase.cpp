// Copyright Enhoney.


#include "AbilitySystem/Actor/EnhoneyMineralBase.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "DamageBoxComponent.h"
#include "RPGDemo.h"
#include "EnhoneyLogChannel.h"
#include "PlayerInterface.h"
#include "InventoryComponent.h"
#include "PlayerTaskComponent.h"
#include "Field/FieldSystemActor.h" // ��������
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
	GeometryCollectionComponent->SetSimulatePhysics(true);	// ����ģ������
	GeometryCollectionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GeometryCollectionComponent->SetCollisionObjectType(ECC_CombatMesh);	// ʹ�ý����˺�����ײ���󣬵��Ƕ���Pawnִ��Block������DamageObjectִ��Overlap
	GeometryCollectionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_MonitoringCollision, ECR_Ignore);	// ���ڼ����ײ����
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_EnemyLocking, ECR_Ignore);	// ���ڵ�����������
	GeometryCollectionComponent->SetCollisionResponseToChannel(ECC_DamageObject, ECR_Overlap);	// �����˺�����ִ��Overlap
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

	// ����һ��п�ʯ���¼�
	GeometryCollectionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnHitByPlayer);

	// ����Ϸһ��ʼ������ֵ�����ص�
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
		// ��Geometry Collection��λ�����ɳ�
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
		// ��ȡ��ҵ��������
		UPlayerTaskComponent* PlayerTaskComp = IPlayerInterface::Execute_GetPlayerTaskComponent(Breaker.Get());
		if (IsValid(InventoryComponent))
		{
			for (const FMineralRewardConfig& Reward : RewardItemsConfig)
			{
				FInventoryItem ItemToReward;
				if (Reward.GetRandomRewardItem(ItemToReward))
				{
					InventoryComponent->ServerAddItemByTag(ItemToReward);

					// ����������Ʒ����
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
			// �������ֵ
			CurrentBreakValue += FMath::Max((DamageBox->GetBaseBreakValue() - Toughness), 0.f);
		}


		// ��Ҫ�ڱ��ز��ź���ʾ
		// �����ܻ���Ч����ʾ����ֵ
		if (UCommonAlgorithmLibrary::IsLocalEnhoneyPlayer(OtherComp->GetOwner()))
		{
			MineralHitReact();
		}
		
		// ��������ˣ����ڷ�������¼�����ߣ������Ž������ڿͻ���ִ������
		if (CurrentBreakValue >= BreakThreshold && !Breaker.IsValid())
		{
			SetLifeSpan(3.f);	// 3�������

			// ����Ƿ��������ͼ�¼�����ߣ������Ž���
			if (GetLocalRole() == ROLE_Authority)
			{
				// ������Ƶ�ʱ�����ﴫ�ݵľ������--AvatarActor
				if (IsValid(DamageBox->DamageEffectParams.WorldContextObject) && CastChecked<AActor>(DamageBox->DamageEffectParams.WorldContextObject))
				{
					// Զ�̹���������£�WorldContextObject���ݵ���AvatarActor
					Breaker = Cast<AActor>(DamageBox->DamageEffectParams.WorldContextObject);
					
				}
				else if(DamageBox->GetOwner()->Implements<UPlayerInterface>())
				{
					// ��ս����������£�DamageEffectParamsֻ����ʩ���˺���ʱ������ã��������ﴫ��DamageBox��Owner
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
			// ����ǵ���ģʽ�������Ǳ�����ң���ֱ������
			if (GetNetMode() == NM_Standalone ||
				(GetNetMode() == NM_ListenServer) && (GetLocalRole() != ROLE_SimulatedProxy) ||
				(GetNetMode() == NM_ListenServer) && (GetLocalRole() == ROLE_AutonomousProxy))
			{
				BreakMineral();
			}
			// ����֮������ӦDamageBox����ײ
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

	// ��ʾ��������������
	ToughnessBar->SetVisibility(true);
	if (UEnemyHealthBarWidget* ToughnessBarWidget = Cast<UEnemyHealthBarWidget>(ToughnessBar->GetWidget()))
	{
		ToughnessBarWidget->UpdateHealthBarValue(BreakThreshold - CurrentBreakValue, BreakThreshold);
	}
	// ��ʱ���ػ�����
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
	// ���ж��Ƿ����
	if (FMath::FRand() <= RewardItemProbability && RewardItemTag.IsValid())
	{
		// �����������
		int32 ItemRewardNum = FMath::RandRange(MinRewardItemCount, MaxRewardItemCount);
		OutDroppedItem = FInventoryItem(RewardItemTag, ItemRewardNum);
		return true;
	}
	return false;
}
