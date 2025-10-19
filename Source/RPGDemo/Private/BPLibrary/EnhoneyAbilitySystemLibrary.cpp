// Copyright Enhoney.


#include "EnhoneyAbilitySystemLibrary.h"

#include "EnhoneyAbilitySystemComponent.h"
#include "GameplayEffectTypes.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhoneyGameplayTags.h"
#include "EnhoneyGameStateBase.h"
#include "InventoryItemInfo.h"
#include "EnhoneyAbilityType.h"
#include "TaskSystemComponent.h"

void UEnhoneyAbilitySystemLibrary::ApplyAttributeEffectToSelf(const UObject* InWorldContextObject, 
	const TSubclassOf<UGameplayEffect> EffectToApply,
	UAbilitySystemComponent* AbilitySystemComponent, 
	float CharacterLevel)
{
	check(AbilitySystemComponent);
	check(EffectToApply);

	FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(InWorldContextObject);
	const FGameplayEffectSpecHandle EffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(EffectToApply, CharacterLevel, EffectContextHandle);
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}

FGameplayAbilitySpec UEnhoneyAbilitySystemLibrary::GetAbilitySpecByTag(UAbilitySystemComponent* InASC, const FGameplayTag& InAbilityTag)
{
	if (UEnhoneyAbilitySystemComponent* EnhoneyASC = Cast<UEnhoneyAbilitySystemComponent>(InASC))
	{
		FGameplayAbilitySpec* OutAbilitySpec = EnhoneyASC->GetAbilitySpecByTag(InAbilityTag);
		return (OutAbilitySpec == nullptr)? FGameplayAbilitySpec() : *OutAbilitySpec;
	}
	return FGameplayAbilitySpec();
}

FGameplayEffectContextHandle UEnhoneyAbilitySystemLibrary::CauseDamageByParams(const FEnhoneyDamageEffectParams& InDamageParams)
{
	check(IsValid(InDamageParams.WorldContextObject));

	const FEnhoneyGameplayTags EnhoneyTags = FEnhoneyGameplayTags::Get();
	const AActor* AvatarActor = InDamageParams.SourceASC->GetAvatarActor();

	FGameplayEffectContextHandle DamageEffectContextHandle = InDamageParams.SourceASC->MakeEffectContext();

	FEnhoneyGameplayEffectContext* DamageEffectContext = static_cast<FEnhoneyGameplayEffectContext*>(DamageEffectContextHandle.Get());
	DamageEffectContext->AddSourceObject(AvatarActor);
	// 伤害类型
	DamageEffectContext->SetAbilityDamageType(InDamageParams.AbilityDamageType);
	// 击退向量
	DamageEffectContext->SetKnockbackVector(InDamageParams.KnockbackVector);
	// 命中结果
	DamageEffectContext->AddHitResult(InDamageParams.ImpactResult, true);

	FGameplayEffectSpecHandle DamageEffectSpecHandle = InDamageParams.SourceASC->MakeOutgoingSpec(
		InDamageParams.DamageGameplayEffectClass, InDamageParams.AbilityLevel, DamageEffectContextHandle);
	// 技能倍率，它和伤害类型关联
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageEffectSpecHandle, 
		InDamageParams.AbilityDamageType, InDamageParams.SkillMultiplier);

	InDamageParams.TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data);

	return DamageEffectContextHandle;
}

void UEnhoneyAbilitySystemLibrary::ApplyBuffEffectToTarget(const UObject* InWorldContextObject, const TSubclassOf<UGameplayEffect> EffectToApply, AActor* TargetActor, float CharacterLevel)
{
	check(InWorldContextObject);
	check(EffectToApply);
	check(TargetActor);
	// 目标ASC
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (IsValid(TargetASC))
	{
		// 施加到自己身上
		UEnhoneyAbilitySystemLibrary::ApplyAttributeEffectToSelf(InWorldContextObject, EffectToApply, TargetASC, CharacterLevel);
	}

}

void UEnhoneyAbilitySystemLibrary::CancelAbilityWithAbilityTag(AActor* InTargetActor, const FGameplayTag& InAbilityTag)
{
	if (IsValid(InTargetActor))
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InTargetActor);
		check(TargetASC);

		FGameplayAbilitySpec AbilitySpec = UEnhoneyAbilitySystemLibrary::GetAbilitySpecByTag(TargetASC, InAbilityTag);
		if (AbilitySpec.IsActive())
		{
			TargetASC->CancelAbility(AbilitySpec.Ability);
		}
	}
}

AEnhoneyGameStateBase* UEnhoneyAbilitySystemLibrary::GetEnhoneyGameState(const UObject* InWorldContextObject)
{
	if (IsValid(InWorldContextObject))
	{
		if (UWorld* World = InWorldContextObject->GetWorld())
		{
			if (AEnhoneyGameStateBase* EnhoneyGameState = Cast<AEnhoneyGameStateBase>(World->GetGameState()))
			{

				return EnhoneyGameState;
			}
		}
	}

	return nullptr;
}

UInventoryItemInfo* UEnhoneyAbilitySystemLibrary::GetInventoryItemInfoDataAsset(const UObject* InWorldContextObject)
{
	if (AEnhoneyGameStateBase* EnhoneyGameState = UEnhoneyAbilitySystemLibrary::GetEnhoneyGameState(InWorldContextObject))
	{
		return EnhoneyGameState->GetInventoryItemConfigInfo();
	}
	return nullptr;
}

bool UEnhoneyAbilitySystemLibrary::GetInventoryItemConfig_Food(const UObject* InWorldContextObject, const FGameplayTag& InItemTag, FInventoryItem_Food& OutFoodItemConfig)
{
	// Tag判定
	if (InItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Food"))))
	{
		if (UInventoryItemInfo* InventoryItemConfig = UEnhoneyAbilitySystemLibrary::GetInventoryItemInfoDataAsset(InWorldContextObject))
		{
			OutFoodItemConfig = InventoryItemConfig->FindItemInfoByTag_Food(InItemTag);
			return true;
		}
	}
	
	return false;
}

bool UEnhoneyAbilitySystemLibrary::GetInventoryItemConfig_Task(const UObject* InWorldContextObject, const FGameplayTag& InItemTag, FInventoryItem_Task& OutTaskItemConfig)
{
	if (InItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Task"))))
	{
		if (UInventoryItemInfo* InventoryItemConfig = UEnhoneyAbilitySystemLibrary::GetInventoryItemInfoDataAsset(InWorldContextObject))
		{
			OutTaskItemConfig = InventoryItemConfig->FindItemInfoByTag_Task(InItemTag);
			return true;
		}
	}
	return false;
}

bool UEnhoneyAbilitySystemLibrary::GetInventoryItemConfig_Valuable(const UObject* InWorldContextObject, const FGameplayTag& InItemTag, FInventoryItem_Valuable& OutValuableItemConfig)
{
	if (InItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Valuable"))))
	{
		if (UInventoryItemInfo* InventoryItemConfig = UEnhoneyAbilitySystemLibrary::GetInventoryItemInfoDataAsset(InWorldContextObject))
		{
			OutValuableItemConfig = InventoryItemConfig->FindItemInfoByTag_Valuable(InItemTag);
			return true;
		}
	}
	return false;
}

bool UEnhoneyAbilitySystemLibrary::GetInventoryItemConfig_Economic(const UObject* InWorldContextObject, const FGameplayTag& InItemTag, FInventoryItem_Economic& OutEconomicItemConfig)
{
	if (InItemTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Material.Economic"))))
	{
		if (UInventoryItemInfo* InventoryItemConfig = UEnhoneyAbilitySystemLibrary::GetInventoryItemInfoDataAsset(InWorldContextObject))
		{
			OutEconomicItemConfig = InventoryItemConfig->FindItemInfoByTag_Economic(InItemTag);
			return true;
		}
	}
	return false;
}

UTaskSystemComponent* UEnhoneyAbilitySystemLibrary::GetTaskSystemComponent(const UObject* InWorldContextObject)
{
	if (AEnhoneyGameStateBase* EnhoneyGameState = UEnhoneyAbilitySystemLibrary::GetEnhoneyGameState(InWorldContextObject))
	{
		return EnhoneyGameState->GetTaskSystemComponent();
	}
	return nullptr;
}

bool UEnhoneyAbilitySystemLibrary::GetSingleTaskInfoConfig(const UObject* InWorldContextObject, const FGameplayTag& InTaskTag, FPlayerTaskConfigItem& OutTaskInfo)
{
	if (UTaskSystemComponent* TaskSystemCompoent = UEnhoneyAbilitySystemLibrary::GetTaskSystemComponent(InWorldContextObject))
	{
		if(UTaskInfoConfigDataAsset* TaskInfoConfigData = TaskSystemCompoent->GetTaskInfoConfigDataAsset())
		{
			TaskInfoConfigData->GetPlayerTaskConfigInfo(InTaskTag, OutTaskInfo);
			return true;
		}
	}
	return false;
}
