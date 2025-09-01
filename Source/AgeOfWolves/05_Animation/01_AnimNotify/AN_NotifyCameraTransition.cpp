// Fill out your copyright notice in the Description page of Project Settings.


#include "05_Animation/01_AnimNotify/AN_NotifyCameraTransition.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"

DEFINE_LOG_CATEGORY(LogAN_NotifyCameraTransition)

void UAN_NotifyCameraTransition::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	// Owner 유효성 검사
	if (!MeshComp)
	{
		UE_LOGFMT(LogAN_NotifyCameraTransition, Error, "MeshComp가 유효하지 않습니다.");
		return;
	}

	// Owner에서 Character 캐스팅
	AActor* Owner = MeshComp->GetOwner();
	ACharacterBase* CharacterBase = Cast<ACharacterBase>(Owner);
	if (!CharacterBase)
	{
		UE_LOGFMT(LogAN_NotifyCameraTransition, Error, "Character Base 캐스팅 실패.");
		return;
	}

	// DynamicCameraComponent 가져오기
	UDynamicCameraComponent* DynamicCameraComp = CharacterBase->FindComponentByClass<UDynamicCameraComponent>();
	if (!DynamicCameraComp)
	{
		UE_LOGFMT(LogAN_NotifyCameraTransition, Error, "DynamicCameraComponent를 찾을 수 없습니다.");
		return;
	}

	// DynamicCameraComp의 큐에 FCameraTransitionInfo 추가
	DynamicCameraComp->AddCameraTransition(TransitionInfo);

	UE_LOGFMT(LogAN_NotifyCameraTransition, Log, "카메라 전환 요청 완료");
}