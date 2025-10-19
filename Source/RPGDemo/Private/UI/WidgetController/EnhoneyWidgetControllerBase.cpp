// Copyright Enhoney.


#include "EnhoneyWidgetControllerBase.h"

void UEnhoneyWidgetControllerBase::SetWidgetControllrtParams(const FWidgetControllerParam& InWCParams)
{
	PlayerState = InWCParams.PlayerState;
	PlayerController = InWCParams.PlayerController;
	AbilitySystemComponent = InWCParams.AbilitySystemComponent;
	AttributeSet = InWCParams.AttributeSet;
}

void UEnhoneyWidgetControllerBase::BroadcastInitialValue()
{
}

void UEnhoneyWidgetControllerBase::BindCallbacksToDependiencies()
{
}
