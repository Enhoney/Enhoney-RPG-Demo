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
	// �ͻ��˻ص�����������NPC�����̵�UI
	virtual void OnInteract(AActor* InInteractivePlayer) override;
	
};
