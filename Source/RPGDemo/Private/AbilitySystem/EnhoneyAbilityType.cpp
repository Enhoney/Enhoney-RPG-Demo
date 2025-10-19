// Copyright Enhoney.

#include "EnhoneyAbilityType.h"

bool FEnhoneyGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	uint8 RepBits = 0;

	if (bIsCriticalHit)
	{
		RepBits |= 1 << 0;
	}
	if (KnockbackVector != FVector::ZeroVector)
	{
		RepBits |= 1 << 1;
	}
	if (AbilityDamageType.IsValid())
	{
		RepBits |= 1 << 2;
	}

	Ar.SerializeBits(&RepBits, 3);

	if (RepBits & (1 << 0))
	{
		Ar << bIsCriticalHit;
	}
	if (RepBits & (1 << 1))
	{
		KnockbackVector.NetSerialize(Ar, Map, bOutSuccess);
	}
	if (RepBits & (1 << 2))
	{
		AbilityDamageType.NetSerialize(Ar, Map, bOutSuccess);
	}

	bOutSuccess = true;
	return bOutSuccess;
}
