// Copyright Enhoney.


#include "NPCCharacterBase.h"

#include "Components/WidgetComponent.h"
#include "InventoryComponent.h"
#include "Components/SphereComponent.h"
#include "InteractivePromptWidgetBase.h"
#include "PlayerInterface.h"
#include "RPGDemo.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "EnhoneyGameplayTags.h"

ANPCCharacterBase::ANPCCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SetReplicates(true);

	NPCNameWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NPCNameWidget"));
	NPCNameWidgetComponent->SetupAttachment(GetMesh());
	NPCInteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("NPCInteractWidget"));
	NPCInteractWidgetComponent->SetupAttachment(GetMesh());

	NPCNameSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("NPCNameSphere"));
	NPCNameSphereComponent->SetupAttachment(GetRootComponent());
	NPCNameSphereComponent->SetSphereRadius(250.f);
	NPCNameSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NPCNameSphereComponent->SetGenerateOverlapEvents(true);
	NPCNameSphereComponent->SetCollisionObjectType(ECC_MonitoringCollision);
	NPCNameSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	NPCNameSphereComponent->SetCollisionResponseToChannel(ECC_CombatMesh, ECR_Overlap);

	NPCInteractSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("NPCInteractSphere"));
	NPCInteractSphereComponent->SetupAttachment(GetRootComponent());
	NPCInteractSphereComponent->SetSphereRadius(150.f);
	NPCInteractSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NPCInteractSphereComponent->SetGenerateOverlapEvents(true);
	NPCInteractSphereComponent->SetCollisionObjectType(ECC_MonitoringCollision);
	NPCInteractSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	NPCInteractSphereComponent->SetCollisionResponseToChannel(ECC_CombatMesh, ECR_Overlap);

	GetMesh()->SetCollisionObjectType(ECC_Pawn);

}

void ANPCCharacterBase::OnNPCNameSphereBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsNPCIdle && OtherActor->Implements<UPlayerInterface>()
		&& IPlayerInterface::Execute_IsPlayerLocallyControlled(OtherActor))
	{
		NPCNameWidgetComponent->SetVisibility(true);
	}
}

void ANPCCharacterBase::OnNPCNameSphereEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bIsNPCIdle && OtherActor->Implements<UPlayerInterface>() 
		&& IPlayerInterface::Execute_IsPlayerLocallyControlled(OtherActor))
	{
		NPCNameWidgetComponent->SetVisibility(false);
	}
}

void ANPCCharacterBase::OnNPCInteractSphereBeginOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsNPCIdle)
	{
		if (IPlayerInterface* PlayerIntreface = Cast<IPlayerInterface>(OtherActor))
		{
			// ֻ�ڱ��������ʾ������ʾ
			if (IPlayerInterface::Execute_IsPlayerLocallyControlled(OtherActor))
			{
				NPCInteractWidgetComponent->SetVisibility(true);
			}
			
			// ���ý������ܲ��󶨴���
			if (UInventoryComponent* InventoryComp = IPlayerInterface::Execute_GetPlayerInventoryComponent(OtherActor))
			{
				// ������
				IPlayerInterface::Execute_SetCanInteractWithNPC(OtherActor, true);
				if (FOnInteractWithNPCSignature* InteractDelegate = PlayerIntreface->GetOnInteractWithNPCDelegate())
				{
					// �󶨽�������
					InteractDelegate->AddUObject(this, &ANPCCharacterBase::OnInteract);
				}
				
			}
		}
	}
	
}

void ANPCCharacterBase::OnNPCInteractSphereEndOverlap_Implementation(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bIsNPCIdle)
	{
		if (IPlayerInterface* PlayerIntreface = Cast<IPlayerInterface>(OtherActor))
		{
			// ֻ�ڱ��������ʾ������ʾ
			if (IPlayerInterface::Execute_IsPlayerLocallyControlled(OtherActor))
			{
				NPCInteractWidgetComponent->SetVisibility(false);
			}

			// ���ý������ܲ�������
			if (UInventoryComponent* InventoryComp = IPlayerInterface::Execute_GetPlayerInventoryComponent(OtherActor))
			{
				IPlayerInterface::Execute_SetCanInteractWithNPC(OtherActor, false);
				if (FOnInteractWithNPCSignature* InteractDelegate = PlayerIntreface->GetOnInteractWithNPCDelegate())
				{
					InteractDelegate->RemoveAll(this);
				}
			}
		}
	}
	
}

void ANPCCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANPCCharacterBase, bIsNPCIdle);
}

void ANPCCharacterBase::ExitInteractState_Implementation()
{
	// ����NPCΪ����״̬
	SetNPCIdleState(true);

	// ��ʾ����
	NPCNameWidgetComponent->SetVisibility(true);
	// ��ʾ������ʾ
	NPCInteractWidgetComponent->SetVisibility(true);
	// ������ҵĽ�������
	if (InteractivePlayer.IsValid())
	{
		UEnhoneyAbilitySystemLibrary::CancelAbilityWithAbilityTag(InteractivePlayer.Get(), FEnhoneyGameplayTags::Get().Ability_Offensive_Inherent_InteractWithNPC);
	}
	// �����ǰ���������
	SetInteractivePlayer(nullptr);


}

void ANPCCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// ���ص�����
	NPCNameSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ANPCCharacterBase::OnNPCNameSphereBeginOverlap);
	NPCNameSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ANPCCharacterBase::OnNPCNameSphereEndOverlap);
	NPCInteractSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ANPCCharacterBase::OnNPCInteractSphereBeginOverlap);
	NPCInteractSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ANPCCharacterBase::OnNPCInteractSphereEndOverlap);

	// �������õ����ֺͽ����ı�
	if(UInteractivePromptWidgetBase* NameWidget = Cast<UInteractivePromptWidgetBase>(NPCNameWidgetComponent->GetUserWidgetObject()))
	{
		// ��������
		NameWidget->SetPromptText(FText::FromName(NPCName));
		// Ĭ������
		NPCNameWidgetComponent->SetVisibility(false);

	}
	if (UInteractivePromptWidgetBase* InteractTipWidget = Cast<UInteractivePromptWidgetBase>(NPCInteractWidgetComponent->GetUserWidgetObject()))
	{
		InteractTipWidget->SetPromptText(FText::FromString(NPCInteractText));
		NPCInteractWidgetComponent->SetVisibility(false);
	}

}

void ANPCCharacterBase::OnInteract(AActor* InInteractivePlayer)
{
	if (bIsNPCIdle)
	{
		if(InInteractivePlayer->Implements<UPlayerInterface>() &&
			IPlayerInterface::Execute_IsPlayerLocallyControlled(InInteractivePlayer))
		{
			// �������ֺͽ�����ʾ
			NPCNameWidgetComponent->SetVisibility(false);
			NPCInteractWidgetComponent->SetVisibility(false);
		}

		// ��¼��ǰ��������ҵı������--����Ҫ�ڿͻ��˺ͷ������϶���¼�����Կ������е��
		SetInteractivePlayer(InInteractivePlayer);

		// ����NPCΪ�ǿ���״̬
		SetNPCIdleState(false);
		// �����߼�������ʵ��
	}
}

void ANPCCharacterBase::SetInteractivePlayer(AActor* InInteractivePlayer)
{
	// �����ڷ�������¼��ǰ���������
	InteractivePlayer = InInteractivePlayer;
}

void ANPCCharacterBase::SetNPCIdleState(bool bNewIdleState)
{
	bIsNPCIdle = bNewIdleState;
}
