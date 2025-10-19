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
			// 只在本地玩家显示交互提示
			if (IPlayerInterface::Execute_IsPlayerLocallyControlled(OtherActor))
			{
				NPCInteractWidgetComponent->SetVisibility(true);
			}
			
			// 启用交互功能并绑定代理
			if (UInventoryComponent* InventoryComp = IPlayerInterface::Execute_GetPlayerInventoryComponent(OtherActor))
			{
				// 允许交互
				IPlayerInterface::Execute_SetCanInteractWithNPC(OtherActor, true);
				if (FOnInteractWithNPCSignature* InteractDelegate = PlayerIntreface->GetOnInteractWithNPCDelegate())
				{
					// 绑定交互代理
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
			// 只在本地玩家显示交互提示
			if (IPlayerInterface::Execute_IsPlayerLocallyControlled(OtherActor))
			{
				NPCInteractWidgetComponent->SetVisibility(false);
			}

			// 禁用交互功能并解绑代理
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
	// 设置NPC为空闲状态
	SetNPCIdleState(true);

	// 显示名字
	NPCNameWidgetComponent->SetVisibility(true);
	// 显示交互提示
	NPCInteractWidgetComponent->SetVisibility(true);
	// 结束玩家的交互能力
	if (InteractivePlayer.IsValid())
	{
		UEnhoneyAbilitySystemLibrary::CancelAbilityWithAbilityTag(InteractivePlayer.Get(), FEnhoneyGameplayTags::Get().Ability_Offensive_Inherent_InteractWithNPC);
	}
	// 清除当前交互的玩家
	SetInteractivePlayer(nullptr);


}

void ANPCCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// 绑定重叠代理
	NPCNameSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ANPCCharacterBase::OnNPCNameSphereBeginOverlap);
	NPCNameSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ANPCCharacterBase::OnNPCNameSphereEndOverlap);
	NPCInteractSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ANPCCharacterBase::OnNPCInteractSphereBeginOverlap);
	NPCInteractSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ANPCCharacterBase::OnNPCInteractSphereEndOverlap);

	// 设置配置的名字和交互文本
	if(UInteractivePromptWidgetBase* NameWidget = Cast<UInteractivePromptWidgetBase>(NPCNameWidgetComponent->GetUserWidgetObject()))
	{
		// 设置名字
		NameWidget->SetPromptText(FText::FromName(NPCName));
		// 默认隐藏
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
			// 隐藏名字和交互提示
			NPCNameWidgetComponent->SetVisibility(false);
			NPCInteractWidgetComponent->SetVisibility(false);
		}

		// 记录当前交互的玩家的背包组件--这里要在客户端和服务器上都记录，所以看起来有点怪
		SetInteractivePlayer(InInteractivePlayer);

		// 设置NPC为非空闲状态
		SetNPCIdleState(false);
		// 交互逻辑，子类实现
	}
}

void ANPCCharacterBase::SetInteractivePlayer(AActor* InInteractivePlayer)
{
	// 用于在服务器记录当前交互的玩家
	InteractivePlayer = InInteractivePlayer;
}

void ANPCCharacterBase::SetNPCIdleState(bool bNewIdleState)
{
	bIsNPCIdle = bNewIdleState;
}
