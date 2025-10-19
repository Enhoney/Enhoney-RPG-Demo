// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "EnhoneyWeaponConfigInfo.generated.h"

class UStaticMesh;
class UTexture2D;

class UGameplayEffect;
class UAnimMontage;

USTRUCT(BlueprintType)
struct RPGDEMO_API FEnhoneyWeaponLevelUpMats
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|LevelUpConfig|Material")
	FGameplayTag LevelUpMaterialTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|LevelUpConfig|Material")
	int32 NumMaterialRequired = 0;
};

USTRUCT(BlueprintType)
struct RPGDEMO_API FEnhoneyWeaponLevelUpInfo
{
	GENERATED_BODY()
public:
	// 升级需要的金币
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|LevelUpConfig")
	int32 CoinsRequired = 0;

	// 升级需要的材料清单
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo|LevelUpConfig")
	TArray<FEnhoneyWeaponLevelUpMats> WeaponLevelUpMatList;
};

USTRUCT(BlueprintType)
struct RPGDEMO_API FEnhoneyWeaponInfo
{
	GENERATED_BODY()

public:
	// 武器的Tag
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo")
	FGameplayTag WeaponTag = FGameplayTag();
	// 武器使用的Mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo")
	TObjectPtr<UStaticMesh> WeaponMesh;
	// 武器的图标
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo")
	TObjectPtr<UTexture2D> WeaponIcon;
	// 武器的效果
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo")
	TSubclassOf<UGameplayEffect> WeaponEffect;
	// 是否支持连击
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo")
	bool bComboAttack = false;
	// 使用的攻击动画蒙太奇如果是连击，会将这几段动画蒙太奇放在一个里面
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo")
	TObjectPtr<UAnimMontage> CommonAttackMontage;
	// 武器升级配置信息
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo")
	TArray<FEnhoneyWeaponLevelUpInfo> WeaponLevelUpConfig;
	// 武器附着的SocketName
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponInfo")
	FName WeaponAttachmentSocketName = FName();
};

/**
 * 
 */
UCLASS()
class RPGDEMO_API UEnhoneyWeaponConfigInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	// 弯刀的数据
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponConfig")
	FEnhoneyWeaponInfo CurvedKnifeConfig;
	// 飞刀的数据
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponConfig")
	FEnhoneyWeaponInfo FlyKnifeConfig;
	// 弓弩的数据
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponConfig")
	FEnhoneyWeaponInfo CrossbowConfig;
	
};
