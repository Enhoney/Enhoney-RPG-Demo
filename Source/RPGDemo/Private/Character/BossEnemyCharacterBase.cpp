// Copyright Enhoney.


#include "BossEnemyCharacterBase.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

#include "PlayerInterface.h"
#include "RPGDemo.h"

#include "EnhoneyPlayingHUD.h"
#include "PlayingWidgetController.h"
#include "CommonAlgorithmLibrary.h"
#include "EnemyAttributeSet.h"

ABossEnemyCharacterBase::ABossEnemyCharacterBase()
{
	// 入侵范围
	InvaderSphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InvaderSphereCollision"));
	InvaderSphereCollision->SetupAttachment(GetRootComponent());
	InvaderSphereCollision->InitSphereRadius(800.f);
	InvaderSphereCollision->SetGenerateOverlapEvents(true);
	InvaderSphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InvaderSphereCollision->SetCollisionObjectType(ECC_MonitoringCollision);
	InvaderSphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	InvaderSphereCollision->SetCollisionResponseToChannel(ECC_CombatMesh, ECR_Overlap);

	// BOSS默认是休憩状态
	EnemyState = EEnemyState::EES_Rest;
}

void ABossEnemyCharacterBase::MulticastSetBossHealthBarVisibility_Implementation(ACharacter* InPlayerCharacter, bool bIsVisiblie)
{
	// 这是在各个客户端
	UPlayingWidgetController* PlayingWidgetController = nullptr;
	if (UCommonAlgorithmLibrary::GetPlayingWidgetControllerByCharacter(InPlayerCharacter, PlayingWidgetController))
	{
		if (IsValid(PlayingWidgetController))
		{
			PlayingWidgetController->OnBossHealthChangedDelegate.Broadcast(bIsVisiblie, BossName, EnemyAttributeSet->GetHealth(), EnemyAttributeSet->GetMaxHealth());
		}
	}
}

void ABossEnemyCharacterBase::MulticastUpdateHealthBar_Implementation(float NewCurrentHealth, float NewCurrentMaxHealth)
{
	if (!Invaders.IsEmpty())
	{
		// 这里还是服务器，执行RPC
		MulticastUpdateBossHealthBar(BossName, NewCurrentHealth, NewCurrentMaxHealth);
	}
}

void ABossEnemyCharacterBase::TestFunc_Implementation()
{

}

void ABossEnemyCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABossEnemyCharacterBase, Invaders);
}

void ABossEnemyCharacterBase::OnPlayerCharacterStartInvade(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HasAuthority())
	{
		if (OtherActor->Implements<UPlayerInterface>())
		{
			TWeakObjectPtr<ACharacter> InvaderEnter = Cast<ACharacter>(OtherActor);

			Invaders.AddUnique(InvaderEnter);

			// 显示BOS血条
			MulticastSetBossHealthBarVisibility(InvaderEnter.Get(), true);
		}
	}
}

void ABossEnemyCharacterBase::OnPlayerCharacterQuitInvade(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority())
	{
		if (OtherActor->Implements<UPlayerInterface>())
		{
			TWeakObjectPtr<ACharacter> InvaderQuit = Cast<ACharacter>(OtherActor);

			// 隐藏BOSS血条
			MulticastSetBossHealthBarVisibility(InvaderQuit.Get(), false);

			Invaders.RemoveSingle(InvaderQuit);
		}
	}
}

void ABossEnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	InvaderSphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossEnemyCharacterBase::OnPlayerCharacterStartInvade);
	InvaderSphereCollision->OnComponentEndOverlap.AddDynamic(this, &ABossEnemyCharacterBase::OnPlayerCharacterQuitInvade);
}

void ABossEnemyCharacterBase::MulticastUpdateBossHealthBar_Implementation(FName InBossName, float NewCurrentHealth, float NewCurrentMaxHealth)
{
	// 这里就是客户端了
	UPlayingWidgetController* PlayingWidgetControler = nullptr;

	if (!Invaders.IsEmpty())
	{
		for (TWeakObjectPtr<ACharacter> InvadeCharacter : Invaders)
		{
			// 能够拿到Controller，就表明这个角色就是在范围内的入侵者
			if (UCommonAlgorithmLibrary::GetPlayingWidgetControllerByCharacter(InvadeCharacter.Get(), PlayingWidgetControler))
			{
				if (IsValid(PlayingWidgetControler))
				{
					PlayingWidgetControler->OnBossHealthChangedDelegate.Broadcast(true, InBossName, NewCurrentHealth, NewCurrentMaxHealth);
				}
			}
		}
	}
	
}
