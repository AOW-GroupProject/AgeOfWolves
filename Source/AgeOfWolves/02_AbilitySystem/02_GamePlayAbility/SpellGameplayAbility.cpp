// Fill out your copyright notice in the Description page of Project Settings.


#include "02_AbilitySystem/02_GamePlayAbility/SpellGameplayAbility.h"
#include "01_Character/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"




USpellGameplayAbility::USpellGameplayAbility(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bMoveNone(true)
    , bPlayMontageAndWait(true)
{
    ActivationPolicy = EAbilityActivationPolicy::OnInputTriggered;

    //static FName FuncName = FName(TEXT("K2_ActivateAbility"));
    //UFunction* ActivateFunction = GetClass()->FindFunctionByName(FuncName);
    //// FIXME: temp to work around crash
    //if (ActivateFunction && (HasAnyFlags(RF_ClassDefaultObject) || ActivateFunction->IsValidLowLevelFast()))
    //{
    //    if (ActivateFunction)
    //    {
    //        if (ensure(ActivateFunction->GetOuter()))
    //        {
    //            if (ActivateFunction->GetOuter()->IsA(UBlueprintGeneratedClass::StaticClass()))
    //            {
    //                bHasBlueprintActivate = true;
    //            }
    //        }
    //    }
    //}
}



void USpellGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
    Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    // ��Ÿ�� üũ
    check(ActivationSpellActionMontage);

    // ���� �� �������� �ʰ� ����
    if (bMoveNone)
    {
        APlayerCharacter* TargetCharacter = Cast<APlayerCharacter>(ActorInfo->AvatarActor);

        if (TargetCharacter)
        {
            TargetCharacter->GetCharacterMovement()->SetMovementMode(MOVE_None);
        }
    }

    // �𸮾� �������� �����Ǵ� ��Ÿ�� Task Class�� ����� ������
    if (bPlayMontageAndWait)
    {
        CreateTask_PlayMontageAndWait();
    }
}

void USpellGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
    // �����ٸ� �ٽ� ������ �� �ֵ��� ����
    if (bMoveNone)
    {
        APlayerCharacter* TargetCharacter = Cast<APlayerCharacter>(ActorInfo->AvatarActor);

        if (TargetCharacter)
        {
            TargetCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
        }
    }

    Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USpellGameplayAbility::OnCompleteCallback()
{
    bool bReplicateCancelAbility = true;
    bool bWasCancelled = false;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateCancelAbility, bWasCancelled);
}

void USpellGameplayAbility::OnInterruptedCallback()
{
    bool bReplicateCancelAbility = true;
    bool bWasCancelled = true;
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateCancelAbility, bWasCancelled);
}

void USpellGameplayAbility::ExecuteGameplayCueWithParams(FGameplayTag GameplayCueTag, const FGameplayCueParameters& GameplayCueParameters)
{
    K2_ExecuteGameplayCueWithParams(GameplayCueTag, GameplayCueParameters);
}

void USpellGameplayAbility::CreateTask_PlayMontageAndWait()
{
    UAbilityTask_PlayMontageAndWait* PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, TEXT("SkillMontage"), ActivationSpellActionMontage, MontageRate);
    PlayMontageTask->OnCompleted.AddDynamic(this, &USpellGameplayAbility::OnCompleteCallback);
    PlayMontageTask->OnInterrupted.AddDynamic(this, &USpellGameplayAbility::OnInterruptedCallback);
    PlayMontageTask->ReadyForActivation();
}
