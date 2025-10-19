// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Character/NPCCharacterBase.h"
#include "PropMerchantCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API APropMerchantCharacter : public ANPCCharacterBase
{
	GENERATED_BODY()

public:
	void ExitInteractState_Implementation() override;

protected:
	// 客户端回调，对于商人NPC，打开商店UI
	virtual void OnInteract(AActor* InInteractivePlayer) override;
	
};
