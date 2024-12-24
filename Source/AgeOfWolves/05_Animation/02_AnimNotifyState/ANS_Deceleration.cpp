// Fill out your copyright notice in the Description page of Project Settings.

#include "ANS_Deceleration.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/Character.h"
#include "05_Animation/BaseAnimInstance.h"

DEFINE_LOG_CATEGORY(LogANS_Deceleration)

UANS_Deceleration::UANS_Deceleration()
{
	bIsDecelerating = false;
}

void UANS_Deceleration::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	if (!MeshComp || !MeshComp->GetOwner())
	{
		UE_LOGFMT(LogANS_Deceleration, Warning, "유효하지 않은 메시 컴포넌트 또는 오너");
		return;
	}

	// 애님 인스턴스를 찾아서 이벤트 바인딩
	if (UBaseAnimInstance* AnimInstance = Cast<UBaseAnimInstance>(MeshComp->GetAnimInstance()))
	{
		OnDecelerationStateChanged.AddDynamic(AnimInstance, &UBaseAnimInstance::OnDecelerationStateChanged);
		UE_LOGFMT(LogANS_Deceleration, Log, "감속 상태 이벤트 바인딩 성공");
	}
	else
	{
		UE_LOGFMT(LogANS_Deceleration, Warning, "애님 인스턴스 찾기 실패");
	}

	// 감속 상태 시작
	bIsDecelerating = true;
	OnDecelerationStateChanged.Broadcast(true);

	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	UE_LOGFMT(LogANS_Deceleration, Log, "감속 상태 시작");
}

void UANS_Deceleration::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// 감속 상태 종료
	bIsDecelerating = false;
	OnDecelerationStateChanged.Broadcast(false);

	// 애님 인스턴스와의 바인딩 해제
	if (UBaseAnimInstance* AnimInstance = Cast<UBaseAnimInstance>(MeshComp->GetAnimInstance()))
	{
		OnDecelerationStateChanged.RemoveDynamic(AnimInstance, &UBaseAnimInstance::OnDecelerationStateChanged);
		UE_LOGFMT(LogANS_Deceleration, Log, "감속 상태 이벤트 바인딩 해제");
	}

	Super::NotifyEnd(MeshComp, Animation);

	if (MeshComp && MeshComp->GetOwner())
	{
		ACharacter* OwnerCharacter = Cast<ACharacter>(MeshComp->GetOwner());
		if (OwnerCharacter)
		{
			UE_LOGFMT(LogANS_Deceleration, Log, "감속 종료 - 최종 속도: {0}",
				OwnerCharacter->GetVelocity().Size());
		}
	}
}