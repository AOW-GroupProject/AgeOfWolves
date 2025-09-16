#include "04_Component/DynamicCameraComponent.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "04_Component/LockOnComponent.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogDynamicCamera)

//@Defualt Setting
#pragma region Default Settings
UDynamicCameraComponent::UDynamicCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	PlayerCharacterRef.Reset();
	CameraComponentRef.Reset();
}

void UDynamicCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	CheckHitInterrupt();
	
	if (bIsTransitioning)
	{
		ProcessCurrentTransition(DeltaTime);
	}
}
#pragma endregion

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

	if (LockOnComponentRef.IsValid() && LockOnComponentRef->GetbLockOn())
	{
		LockOnComponentRef->CancelLockOn();
		UE_LOGFMT(LogDynamicCamera, Log, "카메라 전환으로 인해 Lock On 상태가 해제되었습니다.");
	}

	FCurrentCameraTransitionInfo TransitionInfo;
	TransitionInfo.StartSpringArm = FindSpringArm(NewTransition.StartAngle);
	TransitionInfo.TargetSpringArm = FindSpringArm(NewTransition.TargetAngle);
	TransitionInfo.TransitionDuration = NewTransition.TransitionDuration;
	TransitionInfo.BlendCurve = NewTransition.BlendCurve;
	TransitionInfo.TransitionPriority = NewTransition.TransitionPriority;
	TransitionInfo.bInterruptible = NewTransition.bInterruptible;
	TransitionInfo.FallbackPolicy = NewTransition.FallbackPolicy;
    
	// 복원 관련 정보 설정
	if (NewTransition.bShouldRestoreAfterDuration)
	{
		TransitionInfo.bShouldRestore = NewTransition.bShouldRestoreAfterDuration;
		TransitionInfo.RestoreDuration = NewTransition.RestoreDuration;
		TransitionInfo.RestoreBlendCurve = NewTransition.RestoreBlendCurve;
		TransitionInfo.RestoreDelay = FMath::Max(0.0f, NewTransition.RestoreDelay);
	}

	EnqueueTransition(TransitionInfo);

	UE_LOGFMT(LogDynamicCamera, Log,
	"카메라 전환이 큐에 추가됨 - {0} -> {1}, Duration: {2}s, 복원 유무: {3}",
	*UEnum::GetValueAsString(NewTransition.StartAngle),
	*UEnum::GetValueAsString(NewTransition.TargetAngle),
	NewTransition.TransitionDuration,
	NewTransition.bShouldRestoreAfterDuration ? TEXT("True") : TEXT("False"));
	
	// 전환 중이 아니라면 전환 시작
	if (!bIsTransitioning)
	{
		StartTransition();
	}
}

void UDynamicCameraComponent::AddCameraInterruptTransition(const FCurrentCameraTransitionInfo& NewInterruptTransition)
{
	FCurrentCameraTransitionInfo InterruptTransitionInfo;
	InterruptTransitionInfo.FallbackPolicy = NewInterruptTransition.FallbackPolicy;
	ApplyFallbackPolicy(NewInterruptTransition, OUT InterruptTransitionInfo);
	InterruptTransitionInfo.TransitionPriority = 10;
	InterruptTransitionInfo.bInterruptible = false;
	InterruptTransitionInfo.RestoreDuration = 0.f;

	if (InterruptTransitionInfo.FallbackPolicy != ECameraFallbackPolicy::ContinueToTarget)
	{
		bIsTransitioning = false;
		EnqueueTransition(InterruptTransitionInfo);
		StartTransition();
	}
	else
	{
		// ContinueToTarget의 경우
		CurrentTransition.bInterruptible = false;
		EnqueueTransition(InterruptTransitionInfo);
	}
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
	if (!DequeueTransition(OUT CurrentTransition))
	{
		bIsTransitioning = false;
		return;
	}

	if (IsNotMontagePlaying())
	{
		UE_LOGFMT(LogDynamicCamera, Log, "애님 몽타주가 실행 중이 아니므로 모든 카메라 전환을 중단합니다.");
		TransitionQueue.Empty();
		AddCameraInterruptTransition(CurrentTransition);

		return;
	}

	// 카메라 회전 시 마우스 입력 비활성화
	if (APlayerController* PC = Cast<APlayerController>(PlayerCharacterRef->GetController()))
	{
		PC->SetIgnoreLookInput(true);
	}
	
	CameraComponentRef->AttachToComponent(CurrentTransition.TargetSpringArm, FAttachmentTransformRules::KeepWorldTransform, USpringArmComponent::SocketName);
	// TargetSpringArm부터의 상대 위치 값
	CachedStartRelativeTransform = CameraComponentRef->GetRelativeTransform();
	
	if (CurrentTransition.TargetSpringArm == FindSpringArm(ECameraAngle::BackView))
	{
		TargetRelativeTransform = FTransform::Identity;
		TargetRelativeTransform.SetRotation(FRotator(-25.f, 0.f, 0.f).Quaternion());
	}
	else
	{
		TargetRelativeTransform = FTransform::Identity;
	}

	bIsTransitioning = true;  
	CurrentTransition.TransitionElapsedTime = 0.f;
	
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
	}
	
	if (APlayerController* PC = Cast<APlayerController>(PlayerCharacterRef->GetController()))
	{
		PC->ResetIgnoreLookInput();
	}
	
	bIsTransitioning = false;
	CurrentTransition.TransitionElapsedTime = 0.0f;

	// 다음 큐 전환 시작
	if (!TransitionQueue.IsEmpty())
	{
		StartTransition();
	}
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

    CurrentTransition.TransitionElapsedTime += DeltaTime;

    // 복원이 필요한 경우 복원 대기 시간 처리
    if (!CurrentTransition.bIsInRestorePhase && CurrentTransition.bShouldRestore)
    {
        if (CurrentTransition.TransitionElapsedTime >= CurrentTransition.TransitionDuration)
        {
            // 첫 전환이 완료되고 복원 대기 시간 처리
            CurrentTransition.RestoreDelayElapsed += DeltaTime;
            if (CurrentTransition.RestoreDelayElapsed >= CurrentTransition.RestoreDelay)
            {
                // 복원 단계 시작
                CurrentTransition.bIsInRestorePhase = true;
                CurrentTransition.TransitionElapsedTime = 0.0f;

                // SpringArm 교체
                USpringArmComponent* TempArm = CurrentTransition.StartSpringArm;
                CurrentTransition.StartSpringArm = CurrentTransition.TargetSpringArm;
                CurrentTransition.TargetSpringArm = TempArm;
            	CameraComponentRef->AttachToComponent(CurrentTransition.TargetSpringArm, FAttachmentTransformRules::KeepWorldTransform, USpringArmComponent::SocketName);
            	// TargetSpringArm부터의 상대 위치 값
            	CachedStartRelativeTransform = CameraComponentRef->GetRelativeTransform();

            	if (CurrentTransition.TargetSpringArm == FindSpringArm(ECameraAngle::BackView))
            	{
            		TargetRelativeTransform = FTransform::Identity;
            		TargetRelativeTransform.SetRotation(FRotator(-25.f, 0.f, 0.f).Quaternion());
            	}
            	else
            	{
            		TargetRelativeTransform = FTransform::Identity;
            	}
            }
            return;
        }
    }

    // 현재 단계에 따른 진행 시간과 보간 곡선 선택
    const float Duration = CurrentTransition.bIsInRestorePhase ? CurrentTransition.RestoreDuration : CurrentTransition.TransitionDuration;
    const EBlendCurve CurrentBlendCurve = CurrentTransition.bIsInRestorePhase ? CurrentTransition.RestoreBlendCurve : CurrentTransition.BlendCurve;
	const float Alpha = FMath::Clamp(CurrentTransition.TransitionElapsedTime / Duration, 0.0f, 1.0f);
    const float EasedAlpha = ApplyEaseFunction(Alpha, CurrentBlendCurve);

    // Transform 보간
    const FTransform BlendedRelTransform = UKismetMathLibrary::TLerp(
        CachedStartRelativeTransform,
        TargetRelativeTransform,
        EasedAlpha
    );

    CameraComponentRef->SetRelativeTransform(BlendedRelTransform);
	
    // bIsInRestorePhase면 Comeplete하고 아니면 return 함
    if (Alpha >= 1.0f)
    {
        if (CurrentTransition.bShouldRestore && !CurrentTransition.bIsInRestorePhase) return;
        
        CompleteCurrentTransition();
    }
}

float UDynamicCameraComponent::ApplyEaseFunction(float Alpha, EBlendCurve BlendCurve)
{
	switch (BlendCurve)
	{
	case EBlendCurve::Snap:
		return Alpha > 0.0f ? 1.0f : 0.0f; 

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

void UDynamicCameraComponent::ApplyFallbackPolicy(const FCurrentCameraTransitionInfo& NewInterruptTransition, FCurrentCameraTransitionInfo& OutInterruptTransitionInfo)
{
    // FallbackPolicy에 따른 설정
    switch (OutInterruptTransitionInfo.FallbackPolicy)
    {
    case ECameraFallbackPolicy::SnapToDefault:
        {
            OutInterruptTransitionInfo.StartSpringArm = NewInterruptTransition.TargetSpringArm; 
            OutInterruptTransitionInfo.TargetSpringArm = FindSpringArm(ECameraAngle::BackView);
            OutInterruptTransitionInfo.TransitionDuration = NewInterruptTransition.TransitionDuration;
            OutInterruptTransitionInfo.BlendCurve = EBlendCurve::Snap;
        }
        break;
 
    case ECameraFallbackPolicy::EaseInToDefault:
        {
            OutInterruptTransitionInfo.StartSpringArm = NewInterruptTransition.TargetSpringArm;
            OutInterruptTransitionInfo.TargetSpringArm = FindSpringArm(ECameraAngle::BackView);
    		OutInterruptTransitionInfo.TransitionDuration = NewInterruptTransition.TransitionDuration;
            OutInterruptTransitionInfo.BlendCurve = EBlendCurve::EaseIn;
        }
        break;
 
    case ECameraFallbackPolicy::EaseOutToDefault:
        {
            OutInterruptTransitionInfo.StartSpringArm = NewInterruptTransition.TargetSpringArm;
            OutInterruptTransitionInfo.TargetSpringArm = FindSpringArm(ECameraAngle::BackView);
    		OutInterruptTransitionInfo.TransitionDuration = NewInterruptTransition.TransitionDuration;
            OutInterruptTransitionInfo.BlendCurve = EBlendCurve::EaseOut;
        }
        break;
 
    case ECameraFallbackPolicy::EaseInOutToDefault:
        {
            OutInterruptTransitionInfo.StartSpringArm = NewInterruptTransition.TargetSpringArm;
            OutInterruptTransitionInfo.TargetSpringArm = FindSpringArm(ECameraAngle::BackView);
    		OutInterruptTransitionInfo.TransitionDuration = NewInterruptTransition.TransitionDuration;	
            OutInterruptTransitionInfo.BlendCurve = EBlendCurve::EaseInOut;
        }
        break;
 
    case ECameraFallbackPolicy::SnapToStartAngle:
        {
            OutInterruptTransitionInfo.StartSpringArm = NewInterruptTransition.TargetSpringArm;
            OutInterruptTransitionInfo.TargetSpringArm = NewInterruptTransition.StartSpringArm;
    		OutInterruptTransitionInfo.TransitionDuration = NewInterruptTransition.TransitionDuration;
            OutInterruptTransitionInfo.BlendCurve = EBlendCurve::Snap;
        }
        break;
 
    case ECameraFallbackPolicy::EaseInToStartAngle:
        {
            OutInterruptTransitionInfo.StartSpringArm = NewInterruptTransition.TargetSpringArm;
            OutInterruptTransitionInfo.TargetSpringArm = NewInterruptTransition.StartSpringArm;
    		OutInterruptTransitionInfo.TransitionDuration = NewInterruptTransition.TransitionDuration;
            OutInterruptTransitionInfo.BlendCurve = EBlendCurve::EaseIn;
        }
        break;
 
    case ECameraFallbackPolicy::EaseOutToStartAngle:
        {
            OutInterruptTransitionInfo.StartSpringArm = NewInterruptTransition.TargetSpringArm;
            OutInterruptTransitionInfo.TargetSpringArm = NewInterruptTransition.StartSpringArm;
    		OutInterruptTransitionInfo.TransitionDuration = NewInterruptTransition.TransitionDuration;
            OutInterruptTransitionInfo.BlendCurve = EBlendCurve::EaseOut;
        }
        break;
 
    case ECameraFallbackPolicy::EaseInOutToStartAngle:
        {
            OutInterruptTransitionInfo.StartSpringArm = NewInterruptTransition.TargetSpringArm;
            OutInterruptTransitionInfo.TargetSpringArm = NewInterruptTransition.StartSpringArm;
    		OutInterruptTransitionInfo.TransitionDuration = NewInterruptTransition.TransitionDuration;
            OutInterruptTransitionInfo.BlendCurve = EBlendCurve::EaseInOut;
        }
        break;
 
    case ECameraFallbackPolicy::UseRestoreSettings:
        {
            OutInterruptTransitionInfo.StartSpringArm = NewInterruptTransition.TargetSpringArm;
            OutInterruptTransitionInfo.TargetSpringArm = NewInterruptTransition.StartSpringArm;
            OutInterruptTransitionInfo.TransitionDuration = NewInterruptTransition.RestoreDuration;
            OutInterruptTransitionInfo.BlendCurve = NewInterruptTransition.RestoreBlendCurve;
        }
        break;
 
    	// 구현 추가 필요
    case ECameraFallbackPolicy::ContinueToTarget:
        {
    		OutInterruptTransitionInfo.StartSpringArm = NewInterruptTransition.TargetSpringArm;
    		OutInterruptTransitionInfo.TargetSpringArm = FindSpringArm(ECameraAngle::BackView);
    		OutInterruptTransitionInfo.TransitionDuration = NewInterruptTransition.TransitionDuration;
    		OutInterruptTransitionInfo.BlendCurve = NewInterruptTransition.BlendCurve;
        }
        break;
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
bool UDynamicCameraComponent::IsAngleValid(ECameraAngle Angle) const
{
	if (CameraAngleNameMap.Contains(Angle))
	{
		return true;
	}

	UE_LOGFMT(LogDynamicCamera, Warning, "유효성 검사 실패 : 해당 스프링암이 존재하지 않습니다.");
	return false;
}

bool UDynamicCameraComponent::IsNotMontagePlaying()
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

void UDynamicCameraComponent::CheckHitInterrupt()
{
	if (!PlayerCharacterRef.IsValid())
		return;
    
	USkeletalMeshComponent* Mesh = PlayerCharacterRef->GetMesh();
	if (!Mesh)
		return;
    
	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
		return;
    
	UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
	bool bCurrentlyHit = false;
    
	if (CurrentMontage)
	{
		FString MontageName = CurrentMontage->GetName();
        
		// 피격 애니메이션 감지
		bCurrentlyHit = MontageName.Contains("Hit");
	}
	
	if (bCurrentlyHit)
	{
		// 현재 전환이 진행 중이고 인터럽트 가능한 상태인지 확인
		if (bIsTransitioning && CurrentTransition.bInterruptible && !CurrentTransition.bIsInRestorePhase)
		{
			UE_LOGFMT(LogDynamicCamera, Log, "피격 애니메이션 인터럽트 발생 - CurrentTransition 인터럽트");
			
			AddCameraInterruptTransition(CurrentTransition);
		}
	}
}

void UDynamicCameraComponent::EnqueueTransition(const FCurrentCameraTransitionInfo& NewTransition)
{
	TransitionQueue.HeapPush(NewTransition, FTransitionPriorityPredicate());
}

bool UDynamicCameraComponent::DequeueTransition(FCurrentCameraTransitionInfo& OutTransition)
{
	if (TransitionQueue.Num() == 0)
	{
		return false;
	}

	TransitionQueue.HeapPop(OutTransition, FTransitionPriorityPredicate());
	return true;
}

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
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

	LockOnComponentRef = PlayerCharacterRef->FindComponentByClass<ULockOnComponent>();
	if (!LockOnComponentRef.IsValid())
	{
		UE_LOGFMT(LogDynamicCamera, Warning, "컴포넌트 초기화 실패: LockOnComponent가 유효하지 않음");
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