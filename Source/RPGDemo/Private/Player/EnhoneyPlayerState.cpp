// Copyright Enhoney.


#include "Player/EnhoneyPlayerState.h"
#include "PlayerAttributeSet.h"
#include "EnhoneyAbilitySystemComponent.h"
#include "InventoryComponent.h"
#include "PlayerTaskComponent.h"

#include "Net/UnrealNetwork.h"

AEnhoneyPlayerState::AEnhoneyPlayerState()
{
	NetUpdateFrequency = 100.f;

	PlayerASC = CreateDefaultSubobject<UEnhoneyAbilitySystemComponent>(TEXT("PlayerAbilitySystemComponent"));
	PlayerASC->SetIsReplicated(true);
	PlayerASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	PlayerAttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("PlayerAttributeSet"));

	// 库存组件
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);	// 这个需要开启网络复制的

	// 玩家任务组件
	PlayerTaskComponent = CreateDefaultSubobject<UPlayerTaskComponent>(TEXT("PlayerTaskComponent"));
	PlayerTaskComponent->SetIsReplicated(true);
}

UAbilitySystemComponent* AEnhoneyPlayerState::GetAbilitySystemComponent() const
{
	return PlayerASC;
}

UPlayerAttributeSet* AEnhoneyPlayerState::GetPlayerAttributeSet() const
{
	return PlayerAttributeSet;
}

void AEnhoneyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AEnhoneyPlayerState, CharacterLevel,		COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnhoneyPlayerState, CurrentExp,			COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnhoneyPlayerState, AttributePoint,		COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnhoneyPlayerState, SkillPoint,			COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnhoneyPlayerState, CoinNum,			COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(AEnhoneyPlayerState, bAlive,				COND_None, REPNOTIFY_OnChanged);
}

void AEnhoneyPlayerState::AddCharacterLevel(int32 LevelToAdd)
{
	if (HasAuthority())
	{
		int32 LastCharacterLevel = CharacterLevel;
		CharacterLevel = FMath::Clamp(CharacterLevel + LevelToAdd, MinValueOfLevel, MaxValueOfLevel);

		// 增加属性点和技能点
		if (CharacterLevel > LastCharacterLevel)
		{
			int32 LevelDifference = CharacterLevel - LastCharacterLevel;
			AddAttributePoint(LevelDifference * AttributePointPerLevel);
			AddSkillPoint(LevelDifference / SkillPointPerLevel);
		}

		if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
		{
			if (LastCharacterLevel != CharacterLevel)
			{
				OnCharacterLevelChanged.Broadcast(CharacterLevel);
			}
		}
		
	}
}

void AEnhoneyPlayerState::SetCharacterLevel(int32 LevelToSet)
{
	if (HasAuthority())
	{
		int32 LastCharacterLevel = CharacterLevel;
		CharacterLevel = FMath::Clamp(LevelToSet, MinValueOfLevel, MaxValueOfLevel);

		if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
		{
			if (LastCharacterLevel != CharacterLevel)
			{
				OnCharacterLevelChanged.Broadcast(CharacterLevel);
			}
		}
	}
}

void AEnhoneyPlayerState::AddCurrentExp(int32 ExpToAdd)
{
	if (HasAuthority())
	{
		int32 LastCurrentExp = CurrentExp;
		int32 TempTargetExp = FMath::Clamp(CurrentExp + ExpToAdd, 0, INT32_MAX);

		// 进行升级判定
		if (GetLevelUpToLevel(ExpToAdd) > 0)
		{
			int32 LevelUpToLevel = GetLevelUpToLevel(ExpToAdd);
			int32 LastCharacterLevel = GetCharacterLevel();
			// 循环，目标是扣除升级需要的经验值
			for (int32 TempTargetLevel = LastCharacterLevel; TempTargetLevel <  LevelUpToLevel; ++TempTargetLevel)
			{
				int32 ExpForLevelUp = GetExpForLevelUp(TempTargetLevel);
				TempTargetExp = FMath::Clamp(TempTargetExp - ExpForLevelUp, 0, INT32_MAX);
			}
			// 升级
			AddCharacterLevel(LevelUpToLevel - GetCharacterLevel());
		}

		CurrentExp = TempTargetExp;

		if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
		{
			if (LastCurrentExp != CurrentExp)
			{
				OnCurrentExpChanged.Broadcast(CurrentExp, GetExpForLevelUp(GetCharacterLevel()));
			}
		}
	}
}

void AEnhoneyPlayerState::SetCurrentExp(int32 ExpToSet)
{
	if (HasAuthority())
	{
		int32 LastCurrentExp = CurrentExp;
		CurrentExp = FMath::Clamp(ExpToSet, 0, INT32_MAX);

		if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
		{
			if (LastCurrentExp != CurrentExp)
			{
				OnCurrentExpChanged.Broadcast(CurrentExp, GetExpForLevelUp(GetCharacterLevel()));
			}
		}
	}
}

void AEnhoneyPlayerState::AddAttributePoint(int32 AttributePointToAdd)
{
	if (HasAuthority())
	{
		int32 LastAttributePoint = AttributePoint;
		AttributePoint += AttributePointToAdd;

		if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
		{
			if (LastAttributePoint != AttributePoint)
			{
				OnAttributePointChanged.Broadcast(AttributePoint);
			}
		}
	}
}

void AEnhoneyPlayerState::SetAttributePoint(int32 AttributePointToSet)
{
	if (HasAuthority())
	{
		int32 LastAttributePoint = AttributePoint;
		AttributePoint = FMath::Clamp(AttributePointToSet, 0, INT32_MAX);

		if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
		{
			if (LastAttributePoint != AttributePoint)
			{
				OnAttributePointChanged.Broadcast(AttributePoint);
			}
		}
	}
}

void AEnhoneyPlayerState::ConsumeAttributePoint(int32 AttributePointToComsume)
{
	if (HasAuthority())
	{
		if (CanAttributeConsumed(AttributePointToComsume))
		{
			int32 LastAttributePoint = AttributePoint;
			AttributePoint -= AttributePointToComsume;

			if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
			{
				if (LastAttributePoint != AttributePoint)
				{
					OnAttributePointChanged.Broadcast(AttributePoint);
				}
			}
		}
	}
}

void AEnhoneyPlayerState::AddSkillPoint(int32 SkillPointToAdd)
{
	if (HasAuthority())
	{
		int32 LastSkillPoint = SkillPoint;
		SkillPoint += SkillPointToAdd;

		if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
		{
			if (LastSkillPoint != SkillPoint)
			{
				OnSkillPointChanged.Broadcast(SkillPoint);
			}
		}
	}
}

void AEnhoneyPlayerState::SetSkillPoint(int32 SkillPointToSet)
{
	if (HasAuthority())
	{
		int32 LastSkillPoint = SkillPoint;
		SkillPoint = FMath::Clamp(SkillPointToSet, 0, INT32_MAX);

		if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
		{
			if (LastSkillPoint != SkillPoint)
			{
				OnSkillPointChanged.Broadcast(SkillPoint);
			}
		}
	}
}

void AEnhoneyPlayerState::ConsumeSkillPoint(int32 SkillPointToComsume)
{
	if (HasAuthority())
	{
		if (CanSkillPointConsumed(SkillPointToComsume))
		{
			int32 LastSkillPoint = SkillPoint;
			SkillPoint -= SkillPointToComsume;

			if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
			{
				if (LastSkillPoint != SkillPoint)
				{
					OnSkillPointChanged.Broadcast(SkillPoint);
				}
			}
		}
	}
}

void AEnhoneyPlayerState::AddCoinNum(int64 CoinNumToAdd)
{
	if (HasAuthority())
	{
		int64 LastCoinNum = CoinNum;
		CoinNum += CoinNumToAdd;

		if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
		{
			if (LastCoinNum != CoinNum)
			{
				OnCoinNumChanged.Broadcast(CoinNum);
			}
		}
	}
}

void AEnhoneyPlayerState::SetCoinNum(int64 CoinNumToSet)
{
	if (HasAuthority())
	{
		int64 LastCoinNum = CoinNum;
		CoinNum = FMath::Clamp<int64>(CoinNumToSet, 0, INT64_MAX);

		if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
		{
			if (LastCoinNum != CoinNum)
			{
				OnCoinNumChanged.Broadcast(CoinNum);
			}
		}
	}
}

void AEnhoneyPlayerState::ConsumeCoinNum(int64 CoinNumToComsume)
{
	if (HasAuthority())
	{
		if (CanCoinNumConsumed(CoinNumToComsume))
		{
			int64 LastCoinNum = CoinNum;
			CoinNum -= CoinNumToComsume;

			if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
			{
				if (LastCoinNum != CoinNum)
				{
					OnCoinNumChanged.Broadcast(CoinNum);
				}
			}
		}
	}
}

void AEnhoneyPlayerState::ServerAddCoinNum_Implementation(int64 CoinNumToAdd)
{
	AddCoinNum(CoinNumToAdd);
}

void AEnhoneyPlayerState::ServerConsumeCoinNum_Implementation(int64 CoinNumToComsume)
{
	ConsumeCoinNum(CoinNumToComsume);
}

void AEnhoneyPlayerState::SwitchAliveState(bool NewAliveState)
{
	if (HasAuthority())
	{
		bool LastAliveState = bAlive;
		bAlive = NewAliveState;

		if (GetNetMode() == ENetMode::NM_Standalone || GetNetMode() == ENetMode::NM_ListenServer)
		{
			if (LastAliveState != bAlive)
			{
				OnIsAliveChanged.Broadcast(bAlive);
			}
		}

	}
}

UInventoryComponent* AEnhoneyPlayerState::GetInventoryComponent()
{
	return InventoryComponent;
}

UPlayerTaskComponent* AEnhoneyPlayerState::GetPlayerTaskComponent()
{
	return PlayerTaskComponent;
}

void AEnhoneyPlayerState::OnRep_CharacterLevel(int32 OldCharacterLevel)
{
	if (OldCharacterLevel != CharacterLevel)
	{
		OnCharacterLevelChanged.Broadcast(CharacterLevel);
	}
}

void AEnhoneyPlayerState::OnRep_CurrentExp(int32 OldExp)
{
	if (OldExp != CurrentExp)
	{
		OnCurrentExpChanged.Broadcast(CurrentExp, GetExpForLevelUp(GetCharacterLevel()));
	}
}

void AEnhoneyPlayerState::OnRep_AttributePoint(int32 OldAttributePoint)
{
	if (OldAttributePoint != AttributePoint)
	{
		OnAttributePointChanged.Broadcast(AttributePoint);
	}
}

void AEnhoneyPlayerState::OnRep_SkillPoint(int32 OldSkillPoint)
{
	if (OldSkillPoint != SkillPoint)
	{
		OnSkillPointChanged.Broadcast(SkillPoint);
	}
}

void AEnhoneyPlayerState::OnRep_IsAlive(bool OldbAlive)
{
	if (OldbAlive != bAlive)
	{
		OnIsAliveChanged.Broadcast(bAlive);
	}
}

void AEnhoneyPlayerState::OnRep_CoinNum(int64 OldCoinNum)
{
	if (OldCoinNum != CoinNum)
	{
		OnCoinNumChanged.Broadcast(CoinNum);
	}
}

int32 AEnhoneyPlayerState::GetLevelUpToLevel(int32 ExpToAdd) const
{
	if(ExpConfigForLevelUp.IsValid())
	{
		int32 TotalExp = CurrentExp + ExpToAdd;
		int32 TempLevel = CharacterLevel;
		while (TempLevel < MaxValueOfLevel)
		{
			int32 ExpForNextLevel = GetExpForLevelUp(TempLevel);
			if (TotalExp >= ExpForNextLevel)
			{
				TotalExp -= ExpForNextLevel;
				++TempLevel;
			}
			else
			{
				break;
			}
		}
		return TempLevel;
	}
	return CharacterLevel;
}

int32 AEnhoneyPlayerState::GetExpForLevelUp(int32 InLevel) const
{
	check(ExpConfigForLevelUp.IsValid());
	if (InLevel >= MinValueOfLevel && InLevel < MaxValueOfLevel)
	{
		return ExpConfigForLevelUp.GetValueAtLevel(InLevel);
	}
	return 0;
}
