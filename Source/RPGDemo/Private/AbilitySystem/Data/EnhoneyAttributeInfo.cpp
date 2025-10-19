// Copyright Enhoney.


#include "EnhoneyAttributeInfo.h"

#include "EnhoneyLogChannel.h"

FEnhoneyAttributeInformation UEnhoneyAttributeInfo::FindAttributeInfoByTag(const FGameplayTag& InAttributeTag, bool bLogNotFound) const
{
    FEnhoneyAttributeInformation OutAttributeInformation;

    bool bInfoFound = false;
    for (FEnhoneyAttributeInformation Info : AttributeInfo)
    {
        if (InAttributeTag.MatchesTagExact(Info.AttributeTag))
        {
            bInfoFound = true;
            OutAttributeInformation = Info;
        }
    }

    if (!bInfoFound && bLogNotFound)
    {
        UE_LOG(LogEnhoney, Error, TEXT("Can't find Attribute Tag : [%s] On %s"), *InAttributeTag.ToString(), *GetNameSafe(this));
    }

    return OutAttributeInformation;
}
