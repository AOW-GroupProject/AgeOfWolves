// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"

/**
 * 
 */

namespace AOWGameplayTags
{
	// Native (C++) Gameplay Tags 선언 및 정의

	AGEOFWOLVES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	AGEOFWOLVES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	AGEOFWOLVES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	AGEOFWOLVES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	AGEOFWOLVES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	AGEOFWOLVES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	AGEOFWOLVES_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	AGEOFWOLVES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_EventTag_Montage_Start);
	AGEOFWOLVES_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_EventTag_Montage_End);

}
