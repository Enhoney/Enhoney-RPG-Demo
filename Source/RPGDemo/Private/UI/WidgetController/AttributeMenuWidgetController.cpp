// Copyright Enhoney.


#include "AttributeMenuWidgetController.h"

#include "PlayerAttributeSet.h"
#include "EnhoneyAbilitySystemComponent.h"
#include "EnhoneyPlayerState.h"

#include "EnhoneyPlayerController.h"
#include "PlayerInterface.h"
#include "PlayerCharacterBase.h"

#include "EnhoneyWeaponComponent.h"

#include "InventoryComponent.h"
#include "EnhoneyPlayingHUD.h"
#include "EnhoneyWeaponConfigInfo.h"
#include "EnhoneyAbilitySystemLibrary.h"
#include "InventoryItemInfo.h"
#include "EnhoneyLogChannel.h"

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	if (IsValid(AttributeSet))
	{
		for (auto& AttributePair : AttributeSet->MapGameplayTagToAttribute)
		{
			BroadcastSingleAttributeInfo(AttributePair.Key, AttributePair.Value());
		}
	}

	if (IsValid(PlayerState))
	{
		OnCharacterLevelChangedDelegate.Broadcast(PlayerState->GetCharacterLevel());

		OnAttributePointChangedDelegate.Broadcast(PlayerState->GetAttributePoint());
	}

	if (GetPlayerWeaponComponent())
	{
		GetPlayerWeaponComponent()->BroadcastCurrentWeaponInfo();
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependiencies()
{
	check(EnhoneyAttribyteInfoData);

	if (IsValid(AttributeSet))
	{
		for (auto& AttributePair : AttributeSet->MapGameplayTagToAttribute)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AttributePair.Value()).AddWeakLambda(this,
				[this, AttributePair](const FOnAttributeChangeData& Data)
				{
					BroadcastSingleAttributeInfo(AttributePair.Key, AttributePair.Value());
				});
		}
	}

	if (IsValid(PlayerState))
	{
		PlayerState->OnCharacterLevelChanged.AddWeakLambda(this, [this](int32 NewCharacterLevel)
			{
				OnCharacterLevelChangedDelegate.Broadcast(NewCharacterLevel);
			});

		PlayerState->OnAttributePointChanged.AddWeakLambda(this, [this](int32 NewAttributePoint)
			{
				OnAttributePointChangedDelegate.Broadcast(NewAttributePoint);
			});
	}

	if (GetPlayerWeaponComponent())
	{
		GetPlayerWeaponComponent()->OnWeaponChangedDelegate.AddWeakLambda(this, [this](const FGameplayTag& NewWeaponTag,
			int32 NewWeaponLevel, UTexture2D* NewWeaponIcon, FEnhoneyWeaponLevelUpInfo WeaponLevelUpItemRequired) 
			{
				OnWeaponChangedDelegate.Broadcast(NewWeaponTag, NewWeaponLevel, NewWeaponIcon, WeaponLevelUpItemRequired);
			});
	}
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	if (IsValid(PlayerController) && (PlayerController->GetCharacter())->Implements<UPlayerInterface>())
	{
		if (IPlayerInterface::Execute_CanAttributeConsumed(PlayerController->GetCharacter(), 1))
		{
			if(IsValid(AbilitySystemComponent))
			{
				AbilitySystemComponent->ServerUpgradeAttributePoint(AttributeTag);
			}
		}
	}
}

void UAttributeMenuWidgetController::SwitchEquippedWeapon()
{
	if (IsValid(PlayerController))
	{
		if (APlayerCharacterBase* PlayerCharacter = Cast<APlayerCharacterBase>(PlayerController->GetCharacter()))
		{
			PlayerCharacter->ServerSwitchEquippedWeapon();
		}
	}
}

void UAttributeMenuWidgetController::BroadcastSingleAttributeInfo(const FGameplayTag& TagToFind, const FGameplayAttribute& Attribute)
{
	FEnhoneyAttributeInformation AttributeInfo = EnhoneyAttribyteInfoData->FindAttributeInfoByTag(TagToFind);
	AttributeInfo.AttributeValue = Attribute.GetNumericValue(AttributeSet);

	OnAttributeChangedDelegate.Broadcast(AttributeInfo);
}

void UAttributeMenuWidgetController::PreUpgradeWeapon()
{
	// �ҵ���ǰ������Ҫ�Ĳ����б�ͽ��������
	if (GetPlayerWeaponComponent())
	{
		if (GetPlayerWeaponComponent()->QueryCurrentWeaponLevelUpRequirements(CurrentWeaponPreUpgradeConfig))
		{
			// ���ݲ���������ЩWidget
			EnableWeaponUpgradeMatWidgetsFromConfig(CurrentWeaponPreUpgradeConfig.WeaponLevelUpMatList);

			// �㲥�����е���������ͼ������Ҫ��ʾ����������ЩWidget��ӵ�Box�У����������ݵ�UI��
			OnWeaponPreUpgradeDelegate.Broadcast(CanUpgradeWeapon(), MatListWidgetsInUse, CurrentWeaponPreUpgradeConfig.CoinsRequired, PlayerState->GetCoinNum());
		}
	}
}

void UAttributeMenuWidgetController::UpgradeWeapon()
{
	if (CanUpgradeWeapon())
	{
		// �ڷ�����ִ����Ʒ
		if (IsValid(PlayerController))
		{
			if (APlayerCharacterBase* PlayerCharacter = Cast<APlayerCharacterBase>(PlayerController->GetCharacter()))
			{
				PlayerCharacter->ServerUpgradeEquippedWeapon();
			}
		}

		// ��Ǯ
		if (IsValid(PlayerState))
		{
			PlayerState->ServerConsumeCoinNum(CurrentWeaponPreUpgradeConfig.CoinsRequired);
		}

		// �۳������е���Ʒ
		for (const FEnhoneyWeaponLevelUpMats& MatsConfig : CurrentWeaponPreUpgradeConfig.WeaponLevelUpMatList)
		{
			GetPlayerInventoryComponent()->ServerConsumeItemByTag(
				FInventoryItem(MatsConfig.LevelUpMaterialTag, MatsConfig.NumMaterialRequired));
		}
		
	}

	// �����ܷ���Ʒ�����֮�󶼹رյ�����ʵ���ϲ�����Ʒ�Ļ�����ť�ǲ����õģ�
	ClosePreUpgradeWindow();
}

void UAttributeMenuWidgetController::ClosePreUpgradeWindow()
{
	// ��������Widget
	RecycleAllUsingWeaponUpgradeMatList();

	// ִ�д������ص���
	OnCloseWeaponPreUpgradeWindowDelegate.Broadcast();

}

bool UAttributeMenuWidgetController::QueryMatIconAndName(const FGameplayTag& InMatType, UTexture2D*& OutMatIcon, FString& OutMatName)
{
	FInventoryItem_Valuable ValuableItemConfig;
	if (UEnhoneyAbilitySystemLibrary::GetInventoryItemConfig_Valuable(PlayerState, InMatType, ValuableItemConfig))
	{
		OutMatIcon = ValuableItemConfig.InventoryItemBaseInfo.InventoryItemIcon;
		OutMatName = ValuableItemConfig.InventoryItemBaseInfo.InventoryItemName;
		return true;
	}
	return false;
}

UEnhoneyWeaponComponent* UAttributeMenuWidgetController::GetPlayerWeaponComponent()
{
	if (!WeaponComponent.IsValid())
	{
		if (IsValid(PlayerController) && IsValid(PlayerController->GetCharacter()) &&
			PlayerController->GetCharacter()->Implements<UPlayerInterface>())
		{
			WeaponComponent =
				IPlayerInterface::Execute_GetPlayerWeaponComponent(PlayerController->GetCharacter());
		}

	}
	return WeaponComponent.Get();
}

UInventoryComponent* UAttributeMenuWidgetController::GetPlayerInventoryComponent()
{
	if (!InventoryComponent.IsValid())
	{
		if (IsValid(PlayerController) && IsValid(PlayerController->GetCharacter()) &&
			PlayerController->GetCharacter()->Implements<UPlayerInterface>())
		{
			InventoryComponent =
				IPlayerInterface::Execute_GetPlayerInventoryComponent(PlayerController->GetCharacter());
		}

	}
	return InventoryComponent.Get();
}

bool UAttributeMenuWidgetController::CanUpgradeWeapon()
{
	if (!IsValid(PlayerState) || CurrentWeaponPreUpgradeConfig.CoinsRequired > PlayerState->GetCoinNum())
	{
		// Ǯ����
		return false;
	}


	if (GetPlayerInventoryComponent())
	{
		// ��ѯ������Ʒ������
		for (const FEnhoneyWeaponLevelUpMats& MatsConfig : CurrentWeaponPreUpgradeConfig.WeaponLevelUpMatList)
		{
			if (MatsConfig.NumMaterialRequired > GetPlayerInventoryComponent()->GetItemNum(MatsConfig.LevelUpMaterialTag))
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

void UAttributeMenuWidgetController::InitWeaponUpgradeMatListPool(int32 InPoolSize)
{
	if (InPoolSize <= 0)
	{
		return;
	}
	WeaponUpgradeMatListPool.Reserve(InPoolSize);

	if (AEnhoneyPlayingHUD* HUD = Cast<AEnhoneyPlayingHUD>(PlayerController->GetHUD()))
	{
		for (int32 i = 0; i < InPoolSize; ++i)
		{
			if (UWeaponUpgradeMatListWidget* NewWidget = CreateWidget<UWeaponUpgradeMatListWidget>(GetWorld(), WeaponUpgradeMatListClass))
			{
				NewWidget->SetWidgetController(this);
				WeaponUpgradeMatListPool.Add(NewWidget);
			}
		}
	}
}

bool UAttributeMenuWidgetController::EnableWeaponUpgradeMatWidgetsFromConfig(const TArray<FEnhoneyWeaponLevelUpMats>& InWeaponLevelUpMatConfig)
{
	if (!InWeaponLevelUpMatConfig.IsEmpty())
	{
		for (const FEnhoneyWeaponLevelUpMats& MatItem : InWeaponLevelUpMatConfig)
		{
			UTexture2D* MatIcon = nullptr;
			FString MatName;
			if (QueryMatIconAndName(MatItem.LevelUpMaterialTag, MatIcon, MatName))
			{
				int32 CurrentMatCount = 0;
				// ��ѯ�����иò��ϵ�����
				if (IsValid(PlayerController) && IsValid(PlayerController->GetCharacter()) &&
					PlayerController->GetCharacter()->Implements<UPlayerInterface>())
				{
					if (UInventoryComponent* InventoryComp =
						IPlayerInterface::Execute_GetPlayerInventoryComponent(PlayerController->GetCharacter()))
					{
						CurrentMatCount = InventoryComp->GetItemNum(MatItem.LevelUpMaterialTag);
					}
				}
				if (UWeaponUpgradeMatListWidget* Widget = EnableOneWeaponUpgradeMatList(
					MatIcon, MatName, CurrentMatCount, MatItem.NumMaterialRequired))
				{
					// �ɹ�����һ��
					MatListWidgetsInUse.Add(Widget);
					continue;
				}
			}
			// ����ʧ��
			UE_LOG(LogEnhoney, Warning, TEXT("[%s]: Failed to enable weapon upgrade mat widget for mat %s,\
				Perhaps your object pool is too small"), *FString(__FUNCTION__), *MatItem.LevelUpMaterialTag.ToString());
			return false;
		}
	}
	return false;
}

UWeaponUpgradeMatListWidget* UAttributeMenuWidgetController::EnableOneWeaponUpgradeMatList(UTexture2D* InMatIcon, const FString& InMatName, int32 InCurrentMatCount, int32 InNeedMatCount)
{
	if (IsValid(InMatIcon))
	{
		for (UWeaponUpgradeMatListWidget* Widget : WeaponUpgradeMatListPool)
		{
			if (!Widget->IsInUse())
			{
				Widget->Enable(InMatIcon, InMatName, InCurrentMatCount, InNeedMatCount);
				return Widget;
			}
		}
	}
	return nullptr;
}

void UAttributeMenuWidgetController::RecycleAllUsingWeaponUpgradeMatList()
{
	MatListWidgetsInUse.Clear();
}
