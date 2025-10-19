// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "Character/NPCCharacterBase.h"
#include "VillageHeadCharacter.generated.h"

/**
 * 
 */
UCLASS()
class RPGDEMO_API AVillageHeadCharacter : public ANPCCharacterBase
{
	GENERATED_BODY()

protected:
	// �ͻ��˻ص������ڴ峤NPC�������񽻻����
	virtual void OnInteract(AActor* InInteractivePlayer) override;
	
};
