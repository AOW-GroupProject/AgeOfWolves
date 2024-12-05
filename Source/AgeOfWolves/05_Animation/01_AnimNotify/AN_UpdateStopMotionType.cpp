// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_UpdateStopMotionType.h"
#include "Logging/StructuredLog.h"

#include "05_Animation/BaseAnimInstance.h"

DEFINE_LOG_CATEGORY(LogAN_UpdateStopMotionType)

void UAN_UpdateStopMotionType::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp)
        return;

    UBaseAnimInstance* BaseAnimInstance = Cast<UBaseAnimInstance>(MeshComp->GetAnimInstance());
    if (!BaseAnimInstance)
        return;

    //@Update Stop Motion Type
    BaseAnimInstance->UpdateStopMotionType(StopMotionType);

    UE_LOGFMT(LogAN_UpdateStopMotionType, Log, "Sprint Stop 종료 - Stop Motion 업데이트: {0}",
        *UEnum::GetValueAsString(StopMotionType));

}