// Copyright Enhoney.


#include "EnhoneyWeaponComponent.h"

#include "EnhoneyWeaponConfigInfo.h"
#include "EnhoneyGameplayTags.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"

#include "RPGDemo.h"

#include "EnemyInterface.h"
#include "CombatInterface.h"

#include "Net/UnrealNetwork.h"
#include "EnhoneyLogChannel.h"

UEnhoneyWeaponComponent::UEnhoneyWeaponComponent()
{
	FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();
	// 添加默认配置
	WeaponBox.Add(EnhoneyTags.Weapon_CurvedKnife, 1);
	WeaponBox.Add(EnhoneyTags.Weapon_FlyKnife, 1);
	WeaponBox.Add(EnhoneyTags.Weapon_Crossbow, 1);

	// 这个静态网格体组件本身没有碰撞
	SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 默认携带弯刀
	CurrentEquippedWeaponTag = EnhoneyTags.Weapon_CurvedKnife;
}

UEnhoneyWeaponConfigInfo* UEnhoneyWeaponComponent::GetWeaponConfig() const
{
	return WeaponConfig;
}

void UEnhoneyWeaponComponent::SwitchEquippedWeapon(const FGameplayTag& NewWeaponTag)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	check(ASC);
	// 先保存当前武器等级
	WeaponBox[CurrentEquippedWeaponTag] = CurrentEquippedWeaponLevel;
	// 移除原来的武器效果
	UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())->RemoveActiveGameplayEffect(CurrentWeaponEffectHandle);

	CurrentEquippedWeaponTag = NewWeaponTag;
	CurrentEquippedWeaponLevel = WeaponBox[CurrentEquippedWeaponTag];
	SetStaticMesh(GetWeaponMeshByTag(CurrentEquippedWeaponTag));
	// 施加新武器的效果
	FGameplayEffectContextHandle WeaponEffectContextHandle = ASC->MakeEffectContext();
	WeaponEffectContextHandle.AddSourceObject(GetOwner());
	FGameplayEffectSpecHandle WeaponSpecHandle = ASC->MakeOutgoingSpec(GetWeaponEffectByTag(NewWeaponTag), CurrentEquippedWeaponLevel, WeaponEffectContextHandle);
	CurrentWeaponEffectHandle = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())->ApplyGameplayEffectSpecToSelf(*WeaponSpecHandle.Data);

	// 通知UI更新
	BroadcastCurrentWeaponInfo();
}

void UEnhoneyWeaponComponent::BroadcastCurrentWeaponInfo()
{
	// 通知UI更新
	OnWeaponChangedDelegate.Broadcast(CurrentEquippedWeaponTag, CurrentEquippedWeaponLevel,
		GetWeaponIconByTag(CurrentEquippedWeaponTag), GetLevelUpInfoByTag(CurrentEquippedWeaponTag, CurrentEquippedWeaponLevel));
}

void UEnhoneyWeaponComponent::UpgradeCurrentWeaponLevel()
{
	// 提升当前武器等级
	int32 PreWeaponLevel = FMath::Clamp(CurrentEquippedWeaponLevel + 1, 1, 5);
	if (PreWeaponLevel != CurrentEquippedWeaponLevel)
	{
		CurrentEquippedWeaponLevel = PreWeaponLevel;
		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
		check(ASC);
		// 移除原来的武器效果
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())->RemoveActiveGameplayEffect(CurrentWeaponEffectHandle);

		// 施加新等级的效果
		FGameplayEffectContextHandle WeaponEffectContextHandle = ASC->MakeEffectContext();
		WeaponEffectContextHandle.AddSourceObject(GetOwner());
		FGameplayEffectSpecHandle WeaponSpecHandle = ASC->MakeOutgoingSpec(GetWeaponEffectByTag(CurrentEquippedWeaponTag), CurrentEquippedWeaponLevel, WeaponEffectContextHandle);
		CurrentWeaponEffectHandle = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())->ApplyGameplayEffectSpecToSelf(*WeaponSpecHandle.Data);

		// 通知UI更新
		BroadcastCurrentWeaponInfo();
	}
	
}


FName UEnhoneyWeaponComponent::GetWeaponAttachmentSocketName(const FGameplayTag& WeaponTag)
{
	FEnhoneyGameplayTags EnhoneyGameplayTags = FEnhoneyGameplayTags::Get();

	if (WeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_CurvedKnife))
	{
		// 弯刀
		return WeaponConfig->CurvedKnifeConfig.WeaponAttachmentSocketName;
	}
	else if (WeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_FlyKnife))
	{
		// 飞刀
		return WeaponConfig->FlyKnifeConfig.WeaponAttachmentSocketName;
	}
	else if (WeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_Crossbow))
	{
		// 弓弩
		return WeaponConfig->CrossbowConfig.WeaponAttachmentSocketName;
	}

	return FName();
}

FGameplayTag UEnhoneyWeaponComponent::GetCurrentEquippedWeapon() const
{
	return CurrentEquippedWeaponTag;
}

UAnimMontage* UEnhoneyWeaponComponent::GetCurrentCommontAttackMontage()
{
	FEnhoneyGameplayTags EnhoneyGameplayTags = FEnhoneyGameplayTags::Get();

	if (CurrentEquippedWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_CurvedKnife))
	{
		// 弯刀
		return WeaponConfig->CurvedKnifeConfig.CommonAttackMontage;
	}
	else if (CurrentEquippedWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_FlyKnife))
	{
		// 飞刀
		return WeaponConfig->FlyKnifeConfig.CommonAttackMontage;
	}
	else if (CurrentEquippedWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_Crossbow))
	{
		// 弓弩
		return WeaponConfig->CrossbowConfig.CommonAttackMontage;
	}

	return nullptr;
}

bool UEnhoneyWeaponComponent::QueryCurrentWeaponLevelUpRequirements(FEnhoneyWeaponLevelUpInfo& OutInfo) const
{
	if (CurrentEquippedWeaponLevel >= 5)
	{
		UE_LOG(LogEnhoney, Warning, TEXT("The current weapon has reached the highest level and cannot be upgraded"));
		return false;
	}
	else if(CurrentEquippedWeaponLevel >= 1 && CurrentEquippedWeaponLevel < 5)
	{
		FEnhoneyGameplayTags EnhoneyGameplayTags = FEnhoneyGameplayTags::Get();
		if (CurrentEquippedWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_CurvedKnife))
		{
			// 弯刀
			OutInfo = WeaponConfig->CurvedKnifeConfig.WeaponLevelUpConfig[CurrentEquippedWeaponLevel];
			return true;
		}
		else if (CurrentEquippedWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_FlyKnife))
		{
			// 飞刀
			OutInfo = WeaponConfig->FlyKnifeConfig.WeaponLevelUpConfig[CurrentEquippedWeaponLevel];
			return true;
		}
		else if (CurrentEquippedWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_Crossbow))
		{
			// 弓弩
			OutInfo = WeaponConfig->CrossbowConfig.WeaponLevelUpConfig[CurrentEquippedWeaponLevel];
			return true;
		}
	}

	UE_LOG(LogEnhoney, Error, TEXT("The weapon level configuration is incorrect. Please check the code configuration"));
	return false;
}

void UEnhoneyWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyWeaponComponent, CurrentEquippedWeaponTag, COND_None, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UEnhoneyWeaponComponent, CurrentEquippedWeaponLevel, COND_None, REPNOTIFY_OnChanged);
}


TSubclassOf<UGameplayEffect> UEnhoneyWeaponComponent::GetWeaponEffectByTag(const FGameplayTag& NewWeaponTag)
{
	FEnhoneyGameplayTags EnhoneyGameplayTags = FEnhoneyGameplayTags::Get();

	if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_CurvedKnife))
	{
		// 弯刀
		return WeaponConfig->CurvedKnifeConfig.WeaponEffect;
	}
	else if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_FlyKnife))
	{
		// 飞刀
		return WeaponConfig->FlyKnifeConfig.WeaponEffect;
	}
	else if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_Crossbow))
	{
		// 弓弩
		return WeaponConfig->CrossbowConfig.WeaponEffect;
	}

	return TSubclassOf<UGameplayEffect>();
}

UTexture2D* UEnhoneyWeaponComponent::GetWeaponIconByTag(const FGameplayTag& NewWeaponTag)
{
	FEnhoneyGameplayTags EnhoneyGameplayTags = FEnhoneyGameplayTags::Get();

	if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_CurvedKnife))
	{
		// 弯刀
		return WeaponConfig->CurvedKnifeConfig.WeaponIcon;
	}
	else if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_FlyKnife))
	{
		// 飞刀
		return WeaponConfig->FlyKnifeConfig.WeaponIcon;
	}
	else if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_Crossbow))
	{
		// 弓弩
		return WeaponConfig->CrossbowConfig.WeaponIcon;
	}

	return nullptr;
}

FEnhoneyWeaponLevelUpInfo UEnhoneyWeaponComponent::GetLevelUpInfoByTag(const FGameplayTag& NewWeaponTag, int32 NewWeaponLevel)
{
	FEnhoneyGameplayTags EnhoneyGameplayTags = FEnhoneyGameplayTags::Get();

	// 这个数组我们，从1号开始设计，0号空着，方便查询使用

	if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_CurvedKnife))
	{
		// 弯刀
		return WeaponConfig->CurvedKnifeConfig.WeaponLevelUpConfig[NewWeaponLevel];
	}
	else if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_FlyKnife))
	{
		// 飞刀
		return WeaponConfig->FlyKnifeConfig.WeaponLevelUpConfig[NewWeaponLevel];
	}
	else if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_Crossbow))
	{
		// 弓弩
		return WeaponConfig->CrossbowConfig.WeaponLevelUpConfig[NewWeaponLevel];
	}

	return FEnhoneyWeaponLevelUpInfo();
}

UStaticMesh* UEnhoneyWeaponComponent::GetWeaponMeshByTag(const FGameplayTag& NewWeaponTag)
{
	FEnhoneyGameplayTags EnhoneyGameplayTags = FEnhoneyGameplayTags::Get();

	if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_CurvedKnife))
	{
		// 弯刀
		return WeaponConfig->CurvedKnifeConfig.WeaponMesh;
	}
	else if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_FlyKnife))
	{
		// 飞刀
		return WeaponConfig->FlyKnifeConfig.WeaponMesh;
	}
	else if (NewWeaponTag.MatchesTagExact(EnhoneyGameplayTags.Weapon_Crossbow))
	{
		// 弓弩
		return WeaponConfig->CrossbowConfig.WeaponMesh;
	}

	return nullptr;
}

void UEnhoneyWeaponComponent::OnRep_CurrentEquippedWeaponTag()
{
	BroadcastCurrentWeaponInfo();
}

void UEnhoneyWeaponComponent::OnRep_CurrentEquippedWeaponLevel()
{
	BroadcastCurrentWeaponInfo();
}
