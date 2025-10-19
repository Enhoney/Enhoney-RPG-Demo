// Copyright Enhoney.


#include "InventoryItemInfo.h"
#include "EnhoneyGameplayTags.h"

FInventoryItem_Task UInventoryItemInfo::FindItemInfoByTag_Task(FGameplayTag InItemTag) const
{
    FInventoryItem_Task OutItemInfo;

    if (!TaskItemsInfo.IsEmpty())
    {
        for (const FInventoryItem_Task& ItemInfo : TaskItemsInfo)
        {
            if (ItemInfo.InventoryItemBaseInfo.InventoryItemNameTag.MatchesTagExact(InItemTag))
            {
                OutItemInfo = ItemInfo;
                break;
            }
        }
    }

    return OutItemInfo;
}

FInventoryItem_Economic UInventoryItemInfo::FindItemInfoByTag_Economic(FGameplayTag InItemTag) const
{
    FInventoryItem_Economic OutItemInfo;

    if (!EconomicItemsInfo.IsEmpty())
    {
        for (const FInventoryItem_Economic& ItemInfo : EconomicItemsInfo)
        {
            if (ItemInfo.InventoryItemBaseInfo.InventoryItemNameTag.MatchesTagExact(InItemTag))
            {
                OutItemInfo = ItemInfo;
                break;
            }
        }
    }

    return OutItemInfo;
}

FInventoryItem_Food UInventoryItemInfo::FindItemInfoByTag_Food(FGameplayTag InItemTag) const
{
    FInventoryItem_Food OutItemInfo;

    if (!FoodItemsInfo.IsEmpty())
    {
        for (const FInventoryItem_Food& ItemInfo : FoodItemsInfo)
        {
            if (ItemInfo.InventoryItemBaseInfo.InventoryItemNameTag.MatchesTagExact(InItemTag))
            {
                OutItemInfo = ItemInfo;
                break;
            }
        }
    }

    return OutItemInfo;
}

FInventoryItem_Valuable UInventoryItemInfo::FindItemInfoByTag_Valuable(FGameplayTag InItemTag) const
{
    FInventoryItem_Valuable OutItemInfo;

    if (!ValuableItemsInfo.IsEmpty())
    {
        for (const FInventoryItem_Valuable& ItemInfo : ValuableItemsInfo)
        {
            if (ItemInfo.InventoryItemBaseInfo.InventoryItemNameTag.MatchesTagExact(InItemTag))
            {
                OutItemInfo = ItemInfo;
                break;
            }
        }
    }

    return OutItemInfo;
}

TSubclassOf<UGameplayEffect> UInventoryItemInfo::GetFoodEffectClassByTag(FGameplayTag InItemTag) const
{
    if (!FoodItemsInfo.IsEmpty())
    {
        for (const FInventoryItem_Food& ItemInfo : FoodItemsInfo)
        {
            if (ItemInfo.InventoryItemBaseInfo.InventoryItemNameTag.MatchesTagExact(InItemTag))
            {
                if (!ItemInfo.FoodEffectClasses.IsEmpty())
                {
                    if (InItemTag.MatchesTagExact(FEnhoneyGameplayTags::Get().Material_Food_ForbiddenPotion))
                    {
                        // ����ҩˮ�������һ��Ч����
                        int32 RandomIndex = FMath::RandRange(0, ItemInfo.FoodEffectClasses.Num() - 1);
                        if (ItemInfo.FoodEffectClasses.IsValidIndex(RandomIndex))
                        {
                            return ItemInfo.FoodEffectClasses[RandomIndex];
                        }
                    }
					// ��ͨʳ�ﷵ�ص�һ��Ч����
                    return ItemInfo.FoodEffectClasses[0];
                }
            }
        }
	}
    return TSubclassOf<UGameplayEffect>();
}
