// Fill out your copyright notice in the Description page of Project Settings.


#include "02_AbilitySystem/BaseGameplayEffectContext.h"

bool FBaseGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);
	return true;
}
