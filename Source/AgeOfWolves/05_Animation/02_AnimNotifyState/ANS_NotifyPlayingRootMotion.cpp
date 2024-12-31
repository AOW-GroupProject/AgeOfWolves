// Fill out your copyright notice in the Description page of Project Settings.


#include "ANS_NotifyPlayingRootMotion.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/Character.h"
#include "05_Animation/BaseAnimInstance.h"

DEFINE_LOG_CATEGORY(LogANS_NotifyPlayingRootMotion)

UANS_NotifyPlayingRootMotion::UANS_NotifyPlayingRootMotion()
{
	AnimInstanceRef.Reset();
}

void UANS_NotifyPlayingRootMotion::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp || !MeshComp->GetOwner())
	{
		UE_LOGFMT(LogANS_NotifyPlayingRootMotion, Warning, "유효하지 않은 메시 컴포넌트 또는 오너");
		return;
	}

	// 애님 인스턴스를 찾아서 이벤트 바인딩
	if (UBaseAnimInstance* AnimInstance = Cast<UBaseAnimInstance>(MeshComp->GetAnimInstance()))
	{
		AnimInstance->SetIsPlayingRootMotionMontage(true);

		AnimInstanceRef = AnimInstance;
		UE_LOGFMT(LogANS_NotifyPlayingRootMotion, Log, "감속 상태 이벤트 바인딩 성공");
	}
	else
	{
		UE_LOGFMT(LogANS_NotifyPlayingRootMotion, Warning, "애님 인스턴스 찾기 실패");
	}
}

void UANS_NotifyPlayingRootMotion::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!AnimInstanceRef.IsValid())
	{
		UE_LOGFMT(LogANS_NotifyPlayingRootMotion, Warning, "애님 인스턴스 찾기 실패");
		return;
	}

	AnimInstanceRef->SetIsPlayingRootMotionMontage(false);
}
