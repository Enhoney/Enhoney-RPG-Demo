// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "GameplayTagContainer.h"
#include "EnhoneyWeaponConfigInfo.h"
#include "ActiveGameplayEffectHandle.h"
#include "EnhoneyWeaponComponent.generated.h"

class UBoxComponent;


// 广播当前装备的武器、武器等级、武器图标以及升级到下一级需要的材料清单
DECLARE_MULTICAST_DELEGATE_FourParams(FEnhoneyWeaponChangedSignature, FGameplayTag /** NewWeaponTag*/,
	int32 /** NewWeaponLevel*/, UTexture2D* /** NewWeaponIcon*/, FEnhoneyWeaponLevelUpInfo /** WeaponLevelUpItemRequired*/);

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnhoneyWeaponComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:
	UEnhoneyWeaponComponent();

	// 获取武器配置
	UFUNCTION(BlueprintCallable)
	UEnhoneyWeaponConfigInfo* GetWeaponConfig() const;

	UFUNCTION(BlueprintCallable)
	void SwitchEquippedWeapon(const FGameplayTag& NewWeaponTag);

	// 广播当前武器信息
	void BroadcastCurrentWeaponInfo();

	// 提升当前武器等级
	void UpgradeCurrentWeaponLevel();


	// 获取附着到的Socket
	FName GetWeaponAttachmentSocketName(const FGameplayTag& WeaponTag);

	UFUNCTION(BlueprintCallable, Category = "WeaponSystem")
	FGameplayTag GetCurrentEquippedWeapon() const;

	// 获取当前装备的武器的普通攻击动画蒙太奇
	UFUNCTION(BlueprintCallable, Category = "WeaponSystem")
	UAnimMontage* GetCurrentCommontAttackMontage();

	// 查询当前武器升级需要的材料清单和金币数量
	bool QueryCurrentWeaponLevelUpRequirements(FEnhoneyWeaponLevelUpInfo& OutInfo) const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	TSubclassOf<UGameplayEffect> GetWeaponEffectByTag(const FGameplayTag& NewWeaponTag);

	UTexture2D* GetWeaponIconByTag(const FGameplayTag& NewWeaponTag);

	FEnhoneyWeaponLevelUpInfo GetLevelUpInfoByTag(const FGameplayTag& NewWeaponTag, int32 NewWeaponLevel);

	UStaticMesh* GetWeaponMeshByTag(const FGameplayTag& NewWeaponTag);

	UFUNCTION()
	void OnRep_CurrentEquippedWeaponTag();

	UFUNCTION()
	void OnRep_CurrentEquippedWeaponLevel();

public:
	FEnhoneyWeaponChangedSignature OnWeaponChangedDelegate;

protected:
	// 当前装备的武器
	UPROPERTY(ReplicatedUsing = OnRep_CurrentEquippedWeaponTag, BlueprintReadOnly, Category = "CurrentWeapon")
	FGameplayTag CurrentEquippedWeaponTag;

	// 当前装备的武器等级
	UPROPERTY(ReplicatedUsing = OnRep_CurrentEquippedWeaponLevel, BlueprintReadOnly, Category = "CurrentWeapon")
	int32 CurrentEquippedWeaponLevel = 1;

	// 当前激活的武器的效果
	UPROPERTY()
	FActiveGameplayEffectHandle CurrentWeaponEffectHandle;

	// 存储武器的匣子，存储了武器及其等级
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "WeaponBox")
	TMap<FGameplayTag /** WeaponTag*/, int32 /** WeaponLevel*/> WeaponBox;

private:
	UPROPERTY(EditDefaultsOnly, Category = "WeaponConfig")
	TObjectPtr<UEnhoneyWeaponConfigInfo> WeaponConfig;
	
};
