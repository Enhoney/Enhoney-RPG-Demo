// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "BuffNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API UBuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	// 默认不自动激活
	UBuffNiagaraComponent() { bAutoActivate = false; }

	// 因为不确定这个BeginPlay是否会在ASC Init之前还是之后执行，所以，我们最好提供一个手动绑代理的方法，在我们Init完ASC之后手动调用
	void InitBuffNiagaraComponent(const FGameplayTag& InBuffTypeTag);

private:
	// 玩家死亡时，销毁这个特效组件
	UFUNCTION()
	void OnOwnerDeath(AActor* OwnerActor);

	void HandleBuffNiagara(const FGameplayTag InBuffTypeTag, int32 InTagCount);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "BuffNiagara")
	FGameplayTag BuffTypeTag;
	
};
