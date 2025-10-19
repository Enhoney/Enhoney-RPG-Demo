// Copyright Enhoney.


#include "EnhoneyUserWidgetBase.h"

void UEnhoneyUserWidgetBase::SetWidgetController(UEnhoneyWidgetControllerBase* InWidgetController)
{
	WidgetController = InWidgetController;

	OnWidgetControllerSet();
}

void UEnhoneyUserWidgetBase::HandleEnhoneyWidgetVisibility_Implementation(bool bIsVisibile)
{
	SetVisibility(bIsVisibile ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
