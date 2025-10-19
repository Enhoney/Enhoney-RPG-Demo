// Copyright Enhoney.


#include "WeaponUpgradeMatListWidget.h"

void UWeaponUpgradeMatListWidget::Enable(UTexture2D* InMatIcon, const FString& InMatName, int32 InCurrentMatCount, int32 InNeedMatCount)
{
	bIsInUse = true;
	MatIcon = InMatIcon;
	MatName = InMatName;
	CurrentMatCount = InCurrentMatCount;
	NeedMatCount = InNeedMatCount;
}

void UWeaponUpgradeMatListWidget::Recycle()
{
	// ���ӿ��Ƴ�
	RemoveFromParent();

	bIsInUse = false;

	MatIcon = nullptr;
	MatName = TEXT("");
	CurrentMatCount = 0;
}
