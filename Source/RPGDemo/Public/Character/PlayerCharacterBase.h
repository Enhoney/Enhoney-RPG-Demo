// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Character/EnhoneyCharacterBase.h"
#include "AbilitySystemInterface.h"
#include "CombatInterface.h"
#include "PlayerInterface.h"
#include "CommonAlgorithmLibrary.h"
#include "PlayerCharacterBase.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UEnhoneyAbilitySystemComponent;
class UPlayerAttributeSet;
class UGameplayEffect;
class USphereComponent;
class UEnhoneyWeaponComponent;
class UDamageBoxComponent;
class UInventoryComponent;
class UEnhoneyWidgetControllerBase;

/**
 * 
 */
UCLASS()
class RPGDEMO_API APlayerCharacterBase : public AEnhoneyCharacterBase, public IAbilitySystemInterface, public ICombatInterface, public IPlayerInterface
{
	GENERATED_BODY()

public:
	APlayerCharacterBase();

	// 被Controlelr掌控的时候--服务端在这里初始化ASC
	virtual void PossessedBy(AController* NewController) override;

	// PlayerState网络复制过来的时候
	virtual void OnRep_PlayerState() override;

	virtual void Tick(float DeltaTime) override;

	/** AbilitySystem Interface Start*/
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	/** AbilitySystem Interface End*/

	/** Combat Interface Start*/
	virtual int32 GetCharacterLevel_Implementation() const override;
	virtual bool IsCharacterAlive_Implementation() const override;
	virtual void CharacterDie_Implementation() override;
	virtual FVector GetFireSocketLocation_Implementation(FName FireSocketName) override;

	virtual void PlayHitReactionAnim_Implementation(const FHitResult& ImpactResult) override;
	/** Combat Interface End*/

	/** PlayerInterface Start*/
	virtual AEnhoneyPlayerController* GetEnhneyPlayerController_Implementation() override;
	virtual bool IsPlayerLocallyControlled_Implementation() const override;

	virtual bool CanAttributeConsumed_Implementation(int32 AttributePointToComsume) const override;
	virtual void ConsumeAttributePoint_Implementation(int32 AttributePointToComsume) override;
	virtual void AddAttributePoint_Implementation(int32 AttributePointToAdd) override;

	virtual bool CanSkillPointConsumed_Implementation(int32 SkillPointToComsume) const override;
	virtual void AddSkillPoint_Implementation(int32 SkillPointToAdd) override;
	virtual void ConsumeSkillPoint_Implementation(int32 SkillPointToComsume) override;

	virtual void AddCoinNum_Implementation(int64 CoinNumToAdd) override;
	virtual bool CanCoinNumConsumed_Implementation(int64 CoinNumToComsume) const override;
	virtual void ConsumeCoinNum_Implementation(int64 CoinNumToComsume) override;

	virtual void AddCurrentExp_Implementation(int32 ExpToAdd) override;
	virtual int32 GetCurrentExp_Implementation() const override;
	virtual int32 GetExpToLevelUp_Implementation() const override;

	virtual void PlayerLevelUp_Implementation(int32 TargetLevel) override;
	

	virtual UPlayerAttributeSet* GetPlayerAttributeSet_Implementation() const override;

	virtual bool GetPlayerTargetEnemy_Implementation(AActor*& OutPlayerTargetEnemy) override;
	virtual void EnterTargetEnemyLocking_Implementation() override;
	virtual void UpdateTargetEnemy_Implementation(AActor* NewTargetEnemy) override;
	virtual void QuitTargetEnemyLocking_Implementation() override;

	virtual void SwitchEquippedWeapon_Implementation(const FGameplayTag& NewWeaponTag) override;
	virtual UEnhoneyWeaponComponent* GetPlayerWeaponComponent_Implementation() override;
	virtual void SetMeleeCollisionEnabled_Implementation(bool bIsEnabled) override;
	virtual UAnimMontage* GetCurrentCommontAttackMontage_Implementation() override;
	virtual FGameplayTag GetCommonAttackAbilityTag_Implementation() const override;
	virtual UInventoryComponent* GetPlayerInventoryComponent_Implementation() override;

	virtual FOnInteractWithNPCSignature* GetOnInteractWithNPCDelegate() override;

	virtual void SetCanInteractWithNPC_Implementation(bool bInCanInteract) override;

	virtual UEnhoneyWidgetControllerBase* GetStoreWidgetController_Implementation() override;

	virtual void SwitchPlayerInputMode_Implementation(bool bShowMouseCursor, bool bIsUIOnly) override;
	virtual UPlayerTaskComponent* GetPlayerTaskComponent_Implementation() override;
	virtual UTaskWidgetController* GetPlayerTaskWidgetController_Implementation() override;
	/** PlayerInterface End*/

	UPlayerAttributeSet* GetPlayerAttributeSet() const;

	// 切换武器
	UFUNCTION(Server, Reliable, Category = "WeaponSystem")
	void ServerSwitchEquippedWeapon();

	// 武器升品
	UFUNCTION(Server, Reliable, Category = "WeaponSystem")
	void ServerUpgradeEquippedWeapon();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	// 初始化ASC
	void InitAbilityActorInfo();

	// 初始化属性
	void InitPlayerAttributes();

	// 加载属性和能力
	void LoadProgress();

	virtual void BeginPlay() override;

	/** EnemyLocking*/
	UFUNCTION()
	void OnEnemyLockingSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void InitPlayingUI();

	void HandlePlayerRotationOnEnemyLocking(float InDeltaTime);

	void EndowPlayerInherentAbility();


protected:
	UPROPERTY(VisibleAnywhere, Category = "AbilitySystem|AbilisySystemComponent")
	TObjectPtr<UEnhoneyAbilitySystemComponent> PlayerAbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UPlayerAttributeSet> PlayerAttributeSet;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	// 锁敌查询
	UPROPERTY(EditDefaultsOnly, Category = "EnemyLocking")
	TObjectPtr<USphereComponent> EnemyLockingSphere;

	// 初始化属性
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem|AttributeSet")
	TSubclassOf<UGameplayEffect> EffectForPrimaryAttribute;
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem|AttributeSet")
	TSubclassOf<UGameplayEffect> EffectForSecondaryAttribute;
	UPROPERTY(EditDefaultsOnly, Category = "AbilitySystem|AttributeSet")
	TSubclassOf<UGameplayEffect> EffectForVitalAttribute;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<UEnhoneyWeaponComponent> PlayerWeaponComponent;
	// 用于近战武器伤害处理的盒体组件
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	TObjectPtr<UDamageBoxComponent> MeleeDamageBox;

protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "EnemyLocking")
	bool bTargetingEnemy = false;

	UPROPERTY(BlueprintReadOnly, Category = "EnemyLocking")
	TObjectPtr<AActor> PlayerTargetEnemy = nullptr;

	// 空锁敌累计时间--仅在服务器上有效
	UPROPERTY()
	float EmptyEnemyTime = 0.f;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Player Ability|Montage|Common HitReaction")
	TMap<EEnhoneyHitDirection, TObjectPtr<UAnimMontage>> PlayerHitReactionMontages;
	
};
