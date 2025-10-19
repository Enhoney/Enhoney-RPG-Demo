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
	// 客户端回调，对于村长NPC，打开任务交互面板
	virtual void OnInteract(AActor* InInteractivePlayer) override;
	
};
