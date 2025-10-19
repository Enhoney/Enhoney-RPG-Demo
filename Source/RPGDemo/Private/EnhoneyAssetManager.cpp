// Copyright Enhoney.

#include "EnhoneyAssetManager.h"
#include "EnhoneyGameplayTags.h"
#include "AbilitySystemGlobals.h"

const UEnhoneyAssetManager& UEnhoneyAssetManager::Get()
{
	check(GEngine);
	// 需要在虚幻编辑的项目设置中进行配置
	UEnhoneyAssetManager* EnhoneyAssetManager = Cast<UEnhoneyAssetManager>(GEngine->AssetManager);

	return *EnhoneyAssetManager;
}

void UEnhoneyAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// 注册 Native GameplayTag
	FEnhoneyGameplayTags::InitializeGameplayTags();

	// 不加上这个可能在使用GAS多人游戏的时候，遇到一些难以排查的BUG
	UAbilitySystemGlobals::Get().InitGlobalData();
}
