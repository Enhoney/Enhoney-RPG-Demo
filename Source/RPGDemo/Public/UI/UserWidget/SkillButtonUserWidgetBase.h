// Copyright Enhoney.

#pragma once

#include "CoreMinimal.h"
#include "UI/UserWidget/EnhoneyUserWidgetBase.h"
#include "EnhoneyGameplayTags.h"
#include "SkillButtonUserWidgetBase.generated.h"


/**
 * 
 */
UCLASS()
class RPGDEMO_API USkillButtonUserWidgetBase : public UEnhoneyUserWidgetBase
{
	GENERATED_BODY()


protected:
	// ¼¼ÄÜTag
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Skill")
	FGameplayTag SkillTag;
	
};
