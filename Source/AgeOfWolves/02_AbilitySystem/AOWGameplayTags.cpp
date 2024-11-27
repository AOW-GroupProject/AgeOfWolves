// Fill out your copyright notice in the Description page of Project Settings.


#include "AOWGameplayTags.h"

namespace AOWGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_IsDead, "Ability.ActivateFail.IsDead", "Ability failed to activate because its owner is dead.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cooldown, "Ability.ActivateFail.Cooldown", "Ability failed to activate because it is on cool down.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Cost, "Ability.ActivateFail.Cost", "Ability failed to activate because it did not pass the cost checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsBlocked, "Ability.ActivateFail.TagsBlocked", "Ability failed to activate because tags are blocking it.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_TagsMissing, "Ability.ActivateFail.TagsMissing", "Ability failed to activate because tags are missing.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_Networking, "Ability.ActivateFail.Networking", "Ability failed to activate because it did not pass the network checks.");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_ActivateFail_ActivationGroup, "Ability.ActivateFail.ActivationGroup", "Ability failed to activate because of its activation group.");


	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Active_Dodge, "Ability.Active.Dodge", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Active_Sprint, "Ability.Active.Sprint", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Active_HitReact, "Ability.Active.HitReact", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Active_Attack_Next, "Ability.Active.Attack.Next", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Active_Attack_Normal_1, "Ability.Active.Attack.Normal.1", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Active_Attack_Normal_2, "Ability.Active.Attack.Normal.2", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Active_Attack_Normal_3, "Ability.Active.Attack.Normal.3", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Active_Attack_Normal_4, "Ability.Active.Attack.Normal.4", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Active_Attack_DodgeAttack, "Ability.Active.Attack.DodgeAttack", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Ready_Attack_Normal_2, "Ability.Ready.Attack.Normal.2", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Ready_Attack_Normal_3, "Ability.Ready.Attack.Normal.3", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Ready_Attack_Normal_4, "Ability.Ready.Attack.Normal.4", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_Ready_Attack_DodgeAttack, "Ability.Ready.Attack.DodgeAttack", "");

	UE_DEFINE_GAMEPLAY_TAG(TAG_EventTag_HitReact, "EventTag.HitReact");
	UE_DEFINE_GAMEPLAY_TAG(TAG_EventTag_HitReact_Forward, "EventTag.HitReact.Forward");
	UE_DEFINE_GAMEPLAY_TAG(TAG_EventTag_HitReact_Backward, "EventTag.HitReact.Backward");
	UE_DEFINE_GAMEPLAY_TAG(TAG_EventTag_HitReact_Left, "EventTag.HitReact.Left");
	UE_DEFINE_GAMEPLAY_TAG(TAG_EventTag_HitReact_Right, "EventTag.HitReact.Right");

	UE_DEFINE_GAMEPLAY_TAG(TAG_EventTag_Montage_Start, "EventTag.Montage.Start");
	UE_DEFINE_GAMEPLAY_TAG(TAG_EventTag_Montage_End, "EventTag.Montage.End");
	
}
