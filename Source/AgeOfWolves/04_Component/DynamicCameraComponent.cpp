// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Component/DynamicCameraComponent.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogDynamicCamera)

UDynamicCameraComponent::UDynamicCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
	PlayerCharacterRef.Reset();
	CameraComponentRef.Reset();
}

void UDynamicCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bIsTransitioning)
	{
		ProcessCurrentTransition(DeltaTime);
	}
}

void UDynamicCameraComponent::AddCameraTransition(const FCameraTransitionInfo& NewTransition)
{
	// 유효성 검사 
	if (!IsAngleValid(NewTransition.StartAngle))
	{
		UE_LOGFMT(LogDynamicCamera, Warning, "유효하지 않은 시작 카메라 앵글: {0}", 
			*UEnum::GetValueAsString(NewTransition.StartAngle));
		return;
	}

	if (!IsAngleValid(NewTransition.TargetAngle))
	{
		UE_LOGFMT(LogDynamicCamera, Warning, "유효하지 않은 타겟 카메라 앵글: {0}", 
			*UEnum::GetValueAsString(NewTransition.TargetAngle));
		return;
	}

	if (NewTransition.TransitionDuration <= 0.f)
	{
		UE_LOGFMT(LogDynamicCamera, Warning, "전환 지속 시간이 설정되어있지 않습니다.");
		return;
	}
	
	// 작업 큐에 추가
	FCurrentCameraTransitionInfo Forward;
	Forward.bIsRestoreTransition = false;
	Forward.StartSpringArm       = FindSpringArm(NewTransition.StartAngle);
	Forward.TargetSpringArm      = FindSpringArm(NewTransition.TargetAngle);
	Forward.StartRotate          = NewTransition.StartRotate;
	Forward.EndRotate			 = NewTransition.EndRotate;
	Forward.TransitionDuration   = NewTransition.TransitionDuration;
	Forward.BlendCurve           = NewTransition.BlendCurve;
	Forward.RestoreDelay         = 0.0f;
	Forward.TransitionPriority   = NewTransition.TransitionPriority;
	Forward.bInterruptible       = NewTransition.bInterruptible;
	TransitionQueue.Enqueue(Forward);

	// 복원 작업 큐에 추가
	if (NewTransition.bShouldRestoreAfterDuration)
	{
		FCurrentCameraTransitionInfo Restore;
		Restore.bIsRestoreTransition = true;
		Restore.StartSpringArm       = FindSpringArm(NewTransition.TargetAngle);
		Restore.TargetSpringArm      = FindSpringArm(NewTransition.StartAngle);
		Restore.StartRotate          = NewTransition.EndRotate;
		Restore.EndRotate			 = NewTransition.StartRotate;
		Restore.TransitionDuration   = (NewTransition.RestoreDuration > 0.f) ? NewTransition.RestoreDuration : NewTransition.TransitionDuration;
		Restore.BlendCurve           = NewTransition.BlendCurve;
		Restore.RestoreDelay         = FMath::Max(0.0f, NewTransition.RestoreDelay);
		Restore.TransitionPriority   = NewTransition.TransitionPriority;
		Restore.bInterruptible       = NewTransition.bInterruptible;
		TransitionQueue.Enqueue(Restore);
	}
	
	// 전환 시작
	if (!bIsTransitioning)
	{
		StartTransition();
	}
	
	UE_LOGFMT(LogDynamicCamera, Log, 
		"카메라 전환이 큐에 추가됨 - {0} -> {1}, Duration: {2}s, 복원 유무 : {3}",
		*UEnum::GetValueAsString(NewTransition.StartAngle),
		*UEnum::GetValueAsString(NewTransition.TargetAngle),
		NewTransition.TransitionDuration,
		NewTransition.bShouldRestoreAfterDuration ? TEXT("Restore") : TEXT("NoRestore"));
}

void UDynamicCameraComponent::StartTransition()
{
	// 큐가 비어있으면 종료
	if (TransitionQueue.IsEmpty())
	{
		bIsTransitioning = false;
		return;
	}

	// 큐 꺼내서 CurrentTransition에 넣기
	if (!TransitionQueue.Dequeue(CurrentTransition))
	{
		bIsTransitioning = false;
		return;
	}
	
	if (!IsValid(CurrentTransition.TargetSpringArm))
	{
		UE_LOGFMT(LogDynamicCamera, Warning, "유효하지 않은 타겟 스프링암");
		return;
	}

	if (IsMontagePlaying())
	{
		UE_LOGFMT(LogDynamicCamera, Log, "애님 몽타주가 실행 중이 아니므로 모든 카메라 전환을 중단합니다.");
					
		TransitionQueue.Empty();
		bIsTransitioning = false;
		TransitionElapsedTime = 0.0f;

		if (USpringArmComponent* BackArm = FindSpringArm(ECameraAngle::BackView))
		{
			if (CameraComponentRef.IsValid())
			{
				CameraComponentRef->AttachToComponent(
					BackArm,
					FAttachmentTransformRules::SnapToTargetIncludingScale,
					USpringArmComponent::SocketName
				);
				CameraComponentRef->SetRelativeTransform(FTransform::Identity);
				CameraComponentRef->SetRelativeRotation(FRotator(-25.f, 0.f, 0.f));
				
				BackArm->bUsePawnControlRotation = true;
			
				UE_LOGFMT(LogDynamicCamera, Log, "기본 카메라(SA_Back)로 복귀 완료");
			}
		}
		
		return;
	}

	// bUsePawnControlRotation에 false 적용, bUsePawnControlRotation가 true였던 경우를 위한 이전 옵션 캐싱
	if (IsValid(CurrentTransition.TargetSpringArm))
	{
		bCachedUsePawnControlRotation = CurrentTransition.TargetSpringArm->bUsePawnControlRotation;
		CurrentTransition.TargetSpringArm->bUsePawnControlRotation = false;
	}

	// SpringArm을 Target SpringArm에 부착하되 위치와 회전 값은 원래 있던 자리에 그대로 있음
	CameraComponentRef->AttachToComponent(CurrentTransition.TargetSpringArm, FAttachmentTransformRules::KeepWorldTransform,	USpringArmComponent::SocketName);

	// 보간에 사용할 Transform 캐싱, Transform 값은 Camera가 TargetSpringArm에 부착된 것을 이용해 계산한 원래 위치와의 차이값임
	CachedStartRelativeTransform = CameraComponentRef->GetRelativeTransform();
	bIsTransitioning = true;
	TransitionElapsedTime = 0.f;
	
	UE_LOGFMT(LogDynamicCamera, Log, "카메라 전환 시작: {0} -> {1}, Duration: {2}",
		CurrentTransition.StartSpringArm->GetFName(),
		CurrentTransition.TargetSpringArm->GetFName(),
		CurrentTransition.TransitionDuration);
}


void UDynamicCameraComponent::CompleteCurrentTransition()
{
	// 최종 스냅(남은 오차 보정)
	if (CameraComponentRef.IsValid() && IsValid(CurrentTransition.TargetSpringArm))
	{
		CameraComponentRef->AttachToComponent(
			CurrentTransition.TargetSpringArm,
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			USpringArmComponent::SocketName
		);

		CurrentTransition.TargetSpringArm->bUsePawnControlRotation = bCachedUsePawnControlRotation;
	}

	bIsTransitioning      = false;
	TransitionElapsedTime = 0.0f;

	// 다음 큐 전환 시작
	StartTransition();
}

void UDynamicCameraComponent::ProcessCurrentTransition(float DeltaTime)
{
	if (!PlayerCharacterRef.IsValid() || !CameraComponentRef.IsValid())
	{
		CompleteCurrentTransition();
		return;
	}

	if (!IsValid(CurrentTransition.StartSpringArm) || !IsValid(CurrentTransition.TargetSpringArm))
	{
		CompleteCurrentTransition();
		return;
	}

	TransitionElapsedTime += DeltaTime;

	// 복귀 큐에 대한 지연 시간 처리
	if (CurrentTransition.bIsRestoreTransition && TransitionElapsedTime < CurrentTransition.RestoreDelay)
	{
		return;
	}

	// RestoreDelay를 고려해 로직에 사용할 변수값 계산 
	const float Duration  = FMath::Max(KINDA_SMALL_NUMBER, CurrentTransition.TransitionDuration);
	const float Elapsed   = CurrentTransition.bIsRestoreTransition
		? FMath::Max(0.0f, TransitionElapsedTime - CurrentTransition.RestoreDelay)
		: TransitionElapsedTime;

	const float Alpha      = FMath::Clamp(Elapsed / Duration, 0.0f, 1.0f);
	const float EasedAlpha = ApplyEaseFunction(Alpha, CurrentTransition.BlendCurve);

	// SpringArm간의 보간에 사용할 Transform값
	const FTransform BlendedRelTransform = UKismetMathLibrary::TLerp(
	CachedStartRelativeTransform,  
	FTransform::Identity,          
	EasedAlpha
	);

	// Camera 자체에 사용되는 회전 값 계산 
	const FQuat AdditionalRotQuat = FQuat::Slerp(
		FQuat(CurrentTransition.StartRotate),
		FQuat(CurrentTransition.EndRotate),
		EasedAlpha
	);

	// 최종 계산
	const FQuat FinalRelativeRotation = AdditionalRotQuat * BlendedRelTransform.GetRotation();
	
	const FTransform FinalRelTransform = FTransform(
	FinalRelativeRotation,
	BlendedRelTransform.GetLocation(), 
	BlendedRelTransform.GetScale3D()   
	);
	
	CameraComponentRef->SetRelativeTransform(FinalRelTransform);

	if (Alpha >= 1.0f)
	{
		CompleteCurrentTransition();
	}
}

float UDynamicCameraComponent::ApplyEaseFunction(float Alpha, EBlendCurve BlendCurve)
{
	switch (BlendCurve)
	{
	case EBlendCurve::Snap:
		return Alpha >= 1.0f ? 1.0f : 0.0f;
            
	case EBlendCurve::Linear:
		return Alpha;
            
	case EBlendCurve::EaseIn:
		return UKismetMathLibrary::Ease(0.0f, 1.0f, Alpha, EEasingFunc::EaseIn, 2.0f);
            
	case EBlendCurve::EaseOut:
		return UKismetMathLibrary::Ease(0.0f, 1.0f, Alpha, EEasingFunc::EaseOut, 2.0f);
            
	case EBlendCurve::EaseInOut:
		return UKismetMathLibrary::Ease(0.0f, 1.0f, Alpha, EEasingFunc::EaseInOut, 2.0f);

	// 복잡한 바운스 함수, 현재 기획 단계에서는 미구현
	case EBlendCurve::Bounce:
		return Alpha;
            
	default:
		return Alpha;
	}
}

bool UDynamicCameraComponent::IsAngleValid(ECameraAngle Angle) const
{
	if (CameraAngleNameMap.Contains(Angle))
	{
		return true;
	}
	
	UE_LOGFMT(LogDynamicCamera, Warning, "유효성 검사 실패 : 해당 스프링암이 존재하지 않습니다.");
	return false;
}

bool UDynamicCameraComponent::IsMontagePlaying() 
{
	if (PlayerCharacterRef.IsValid())
	{
		if (USkeletalMeshComponent* Mesh = PlayerCharacterRef->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				if (!AnimInstance->IsAnyMontagePlaying())
				{
					return true;
				}
			}
		}
	}

	return false;
}

USpringArmComponent* UDynamicCameraComponent::FindSpringArm(ECameraAngle Angle) const
{
	// 유효성 검사
	if (USpringArmComponent* const* Arm = CameraAngleSpringArmMap.Find(Angle))
	{
		return *Arm;
	}
	UE_LOGFMT(LogDynamicCamera, Warning, "해당 Angle의 스프링암을 찾지 못함: {0}", *UEnum::GetValueAsString(Angle));
	
	return nullptr;
}

void UDynamicCameraComponent::InitializeDynamicCameraComp(const AController* Controller)
{
	const auto Owner = GetOwner();
	if (!Owner)
	{
		UE_LOGFMT(LogDynamicCamera, Warning, "Owner Actor가 유효하지 않습니다.");
		return;
	}

	PlayerCharacterRef = Cast<APlayerCharacter>(Owner);
	if (!PlayerCharacterRef.IsValid())
	{
		UE_LOGFMT(LogDynamicCamera, Warning, "컴포넌트 초기화 실패: 플레이어 캐릭터가 유효하지 않음");
		return;
	}

	CameraComponentRef = PlayerCharacterRef->GetCameraComponent();
	if (!CameraComponentRef.IsValid())
	{
		UE_LOGFMT(LogDynamicCamera, Warning, "컴포넌트 초기화 실패: 카메라가 유효하지 않음");
		return;
	}
	

	// FName에 해당하는 SpringArm이 있는지 유효성 검사 후 SpringArm 매칭
	if (!CameraAngleNameMap.IsEmpty())
	{
		CameraAngleSpringArmMap.Empty();

		TArray<USpringArmComponent*> SpringArmArray;
		PlayerCharacterRef->GetComponents<USpringArmComponent>(SpringArmArray);
		
		for (const TPair<ECameraAngle, FName> CameraAngleSocket : CameraAngleNameMap)
		{
			USpringArmComponent* FoundSpringArm = nullptr;
			
			for (USpringArmComponent* SpringArm : SpringArmArray)
			{
				if (SpringArm && SpringArm->GetFName() == CameraAngleSocket.Value)
				{
					FoundSpringArm = SpringArm;
					break; 
				}
			}
			
			if (FoundSpringArm)
			{
				CameraAngleSpringArmMap.Add(CameraAngleSocket.Key, FoundSpringArm);
			}
			else
			{
				UE_LOG(LogDynamicCamera, Warning, TEXT("이름에 맞는 SpringArm을 찾을 수 없습니다. ECameraAngle : %s, Name : %s"), 
					*UEnum::GetValueAsString(CameraAngleSocket.Key), 
					*CameraAngleSocket.Value.ToString());
			}
		}
	}

	
}
