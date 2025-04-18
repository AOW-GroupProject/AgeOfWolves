#include "AT_UpdateMotionWarpTarget.h"
#include "Logging/StructuredLog.h"

#include "Kismet/KismetMathLibrary.h"

#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "01_Character/PlayerCharacter.h"
#include "04_Component/LockOnComponent.h"
#include "10_AI/BaseAIController.h"

DEFINE_LOG_CATEGORY(LOGAT_UpdateMotionWarpTarget)

//@Defualt Setting
#pragma region Default Setting
UAT_UpdateMotionWarpTarget::UAT_UpdateMotionWarpTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;

	WarpDirection = EMotionWarpDirection::Normal;
	WarpProximity = EMotionWarpProximity::Normal;
	InteractionType = EInteractionType::None;
}

void UAT_UpdateMotionWarpTarget::Activate()
{
	Super::Activate();

	//@Target
	if (!TargetToTrack.IsValid())
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "Activate 실패 - 타겟 액터가 유효하지 않음");
		EndTask();
		return;
	}

	//@Motion Warp Component
	UMotionWarpingComponent* MotionWarpingComp = GetMotionWarpingComponent();
	if (!MotionWarpingComp)
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "Activate 실패 - MotionWarpingComponent를 찾을 수 없음");
		EndTask();
		return;
	}

	//@충돌 설정 변경
	SetupCollisionResponse();

	//@Fit 모드에서는 회전 제어 중지
	if (WarpProximity == EMotionWarpProximity::Fit)
	{
		SuspendRotationControl();
	}

	//@초기 타겟 위치로 모션 워프 타겟 설정
	UpdateWarpTarget();
}

// 수정된 OnDestroy 함수:
void UAT_UpdateMotionWarpTarget::OnDestroy(bool bInOwnerFinished)
{
	//@충돌 설정 복원
	RestoreCollisionResponse();

	//@회전 제어 복원 (Fit 모드인 경우에만)
	if (WarpProximity == EMotionWarpProximity::Fit)
	{
		RestoreRotationControl();
	}

	//@태스크 종료 시 워프 타겟 제거
	if (UMotionWarpingComponent* MotionWarpingComp = GetMotionWarpingComponent())
	{
		MotionWarpingComp->RemoveWarpTarget(MotionWarpTargetName);
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "워프 타겟 제거: {0}", *MotionWarpTargetName.ToString());
	}

	//@참조 초기화
	PlayerCharacterRef.Reset();
	LockOnComponentRef.Reset();
	AIControllerRef.Reset();

	//@상호작용 타입 초기화
	InteractionType = EInteractionType::None;

	Super::OnDestroy(bInOwnerFinished);
}

void UAT_UpdateMotionWarpTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	//@Target
	if (!TargetToTrack.IsValid())
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "TickTask 실패 - 타겟 액터가 유효하지 않음");
		EndTask();
		return;
	}

	//@Warp Target 업데이트
	UpdateWarpTarget();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
UAT_UpdateMotionWarpTarget* UAT_UpdateMotionWarpTarget::CreateMotionWarpTargetTracker(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	AActor* TargetActor,
	FName WarpTargetName,
	EMotionWarpDirection WarpDirection,
	EMotionWarpProximity WarpProximity,
	EInteractionType InteractionType)
{
	//@상호작용 타입이 지정된 경우 Proximity를 Fit으로 강제 설정
	if (InteractionType != EInteractionType::None)
	{
		WarpProximity = EMotionWarpProximity::Fit;
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "상호작용 타입 지정됨: {0} - 근접도를 Fit으로 자동 설정",
			UEnum::GetValueAsString(InteractionType));
	}

	//@AT
	UAT_UpdateMotionWarpTarget* MyTask = NewAbilityTask<UAT_UpdateMotionWarpTarget>(OwningAbility, TaskInstanceName);
	if (MyTask)
	{
		MyTask->TargetToTrack = TargetActor;
		MyTask->MotionWarpTargetName = WarpTargetName;
		MyTask->WarpDirection = WarpDirection;
		MyTask->WarpProximity = WarpProximity;
		MyTask->InteractionType = InteractionType;
	}
	return MyTask;
}

void UAT_UpdateMotionWarpTarget::UpdateWarpTarget()
{
	//@Motion Warp
	UMotionWarpingComponent* MotionWarpingComp = GetMotionWarpingComponent();
	if (!MotionWarpingComp || !TargetToTrack.IsValid())
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "UpdateWarpTarget 실패 - MotionWarpingComponent 또는 타겟이 유효하지 않음");
		return;
	}

	//@Avatar
	AActor* Avatar = GetAvatarActor();
	if (!Avatar)
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "UpdateWarpTarget 실패 - 아바타가 유효하지 않음");
		return;
	}

	FMotionWarpingTarget WarpTarget;
	WarpTarget.Name = MotionWarpTargetName;

	//@근접도
	bool bIsFitMode = (WarpProximity == EMotionWarpProximity::Fit);

	//@근접도 'Fit' 설정일 경우
	if (bIsFitMode && InteractionType != EInteractionType::None)
	{
		//@상호작용 소켓 위치 사용
		WarpTarget.Location = GetFitLocationFromTargetToTrack();

		//@Fit 모드에서는 소켓의 회전값 사용 (Motion Warping 컴포넌트에서 회전 활성화되어 있을 경우)
		WarpTarget.Rotation = GetFitRotationFromTargetToTrack();

		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "상호작용 Fit 모드 - 소켓 위치/회전 적용됨");
	}
	else
	{
		//@일반 위치 계산 (타겟 위치 + 방향/근접도 오프셋)
		FVector BaseLocation = TargetToTrack->GetActorLocation();
		FVector LocationOffset = CalculatePositionOffset(TargetToTrack.Get());
		WarpTarget.Location = BaseLocation + LocationOffset;

		//@타겟의 회전 값
		WarpTarget.Rotation = TargetToTrack->GetActorRotation();

		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "일반 모드 - 타겟의 회전 값을 그대로 사용");
	}

	//@워프 타겟 업데이트
	MotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);

	UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "WarpTarget 업데이트 - 타겟: {0} | 위치: {1} | 회전: {2} | 근접도: {3}",
		*TargetToTrack->GetName(),
		*WarpTarget.Location.ToString(),
		*WarpTarget.Rotation.ToString(),
		UEnum::GetValueAsString(WarpProximity));
}

void UAT_UpdateMotionWarpTarget::SetupCollisionResponse()
{
	//@근접도가 Closest 혹은 Fit일 경우 충돌 설정 변경
	if (WarpProximity == EMotionWarpProximity::Closest || WarpProximity == EMotionWarpProximity::Fit)
	{
		AActor* Avatar = GetAvatarActor();
		if (!Avatar)
		{
			UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "충돌 설정 변경 실패 - 아바타가 유효하지 않음");
			return;
		}

		UCapsuleComponent* CapsuleComp = Avatar->FindComponentByClass<UCapsuleComponent>();
		if (!CapsuleComp)
		{
			UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "충돌 설정 변경 실패 - 캡슐 컴포넌트를 찾을 수 없음");
			return;
		}

		//@기존 충돌 응답
		PreviousPawnResponse = CapsuleComp->GetCollisionResponseToChannel(ECC_Pawn);

		//@Block -> Overlap
		CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "Closest/Fit 근접도 - Pawn 채널 충돌 응답 변경: {0} -> Overlap",
			PreviousPawnResponse == ECR_Block ? TEXT("Block") :
			PreviousPawnResponse == ECR_Overlap ? TEXT("Overlap") : TEXT("Ignore"));
	}
}

void UAT_UpdateMotionWarpTarget::RestoreCollisionResponse()
{
	//@Closest 또는 Fit 근접도인 경우에만 충돌 응답 복원
	if (WarpProximity == EMotionWarpProximity::Closest || WarpProximity == EMotionWarpProximity::Fit)
	{
		//@Avatar
		AActor* Avatar = GetAvatarActor();
		if (!Avatar)
		{
			UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "충돌 설정 복원 실패 - 아바타가 유효하지 않음");
			return;
		}

		//@Capsule Component
		UCapsuleComponent* CapsuleComp = Avatar->FindComponentByClass<UCapsuleComponent>();
		if (!CapsuleComp)
		{
			UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "충돌 설정 복원 실패 - 캡슐 컴포넌트를 찾을 수 없음");
			return;
		}

		//@Pawn 채널에 대한 이전 충돌 응답으로 복원
		CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, PreviousPawnResponse);

		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "Closest/Fit 근접도 종료 - Pawn 채널 충돌 응답 복원: Overlap -> {0}",
			PreviousPawnResponse == ECR_Block ? TEXT("Block") :
			PreviousPawnResponse == ECR_Overlap ? TEXT("Overlap") : TEXT("Ignore"));
	}
}

void UAT_UpdateMotionWarpTarget::SuspendRotationControl()
{
	//@회전 값 변경 지연
	if (bRotationControlSuspended)
	{
		return;
	}

	//@Avatar
	AActor* Avatar = GetAvatarActor();
	if (!Avatar)
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "회전 제어 중지 실패 - 아바타가 유효하지 않음");
		return;
	}

	//@Player
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Avatar);
	if (PlayerCharacter)
	{
		ULockOnComponent* LockOnComp = PlayerCharacter->GetLockOnComponent();
		if (LockOnComp && LockOnComp->GetbLockOn())
		{
			PlayerCharacterRef = PlayerCharacter;
			LockOnComponentRef = LockOnComp;

			//@Lock On 해제
			LockOnComp->CancelLockOn();

			UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "플레이어 락온 일시 중지");
		}
	}
	//@AI
	else
	{
		//ABaseAIController* AIController = Cast<ABaseAIController>(Avatar->GetInstigatorController());
		//if (AIController)
		//{
		//	AIControllerRef = AIController;

		//	// AI 컨트롤러의 타겟 저장 및 타겟 제거
		//	if (UBlackboardComponent* BBComp = AIController->GetBlackboardComponent())
		//	{
		//		StoredAITarget = BBComp->GetValueAsObject("TargetActor");
		//		BBComp->ClearValue("TargetActor");
		//	}

		//	UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "AI 컨트롤러 타겟 추적 일시 중지");
		//}
	}

	bRotationControlSuspended = true;
}

void UAT_UpdateMotionWarpTarget::RestoreRotationControl()
{
	//@회전 값 설정 지연
	if (!bRotationControlSuspended)
	{
		return;
	}

	//@Player
	if (LockOnComponentRef.IsValid() && PlayerCharacterRef.IsValid())
	{
		//@Lock On 활성화
		LockOnComponentRef->StartLockOn();

		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "플레이어 락온 상태 복원");
	}

	//@AI
	//if (AIControllerRef.IsValid())
	//{
	//	// AI 타겟 복원
	//	if (UBlackboardComponent* BBComp = AIControllerRef->GetBlackboardComponent())
	//	{
	//		BBComp->SetValueAsObject("TargetActor", StoredAITarget);
	//	}

	//	UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "AI 컨트롤러 타겟 추적 복원");
	//}

	bRotationControlSuspended = false;
}

FVector UAT_UpdateMotionWarpTarget::CalculatePositionOffset(const AActor* Target) const
{
	float OffsetScalar = GetProximityScalar();

	FVector ForwardVector = Target->GetActorForwardVector();
	FVector RightVector = Target->GetActorRightVector();

	switch (WarpDirection)
	{
	case EMotionWarpDirection::Front:
		return ForwardVector * OffsetScalar;
	case EMotionWarpDirection::Back:
		return -ForwardVector * OffsetScalar;
	case EMotionWarpDirection::Left:
		return -RightVector * OffsetScalar;
	case EMotionWarpDirection::Right:
		return RightVector * OffsetScalar;
	default:
		return FVector::ZeroVector;
	}
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
float UAT_UpdateMotionWarpTarget::GetProximityScalar() const
{
	//@근접도에 따른 스칼라 값 반환
	switch (WarpProximity)
	{
	case EMotionWarpProximity::Close:
		return 70.f;
	case EMotionWarpProximity::Closer:
		return 60.0f;
	case EMotionWarpProximity::Closest:
		return 50.f;
	case EMotionWarpProximity::Fit:
	case EMotionWarpProximity::Normal:
	default:
		return 100.0f;
	}
}

FVector UAT_UpdateMotionWarpTarget::GetFitLocationFromTargetToTrack() const
{
	//@Target, Interaction 유형
	if (!TargetToTrack.IsValid() || InteractionType == EInteractionType::None)
	{
		return FVector::ZeroVector;
	}

	//@Character
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetToTrack.Get());
	if (!TargetCharacter || !TargetCharacter->GetMesh())
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "타겟을 Character로 캐스팅할 수 없거나 메시가 없음 - 타겟: {0}",
			*TargetToTrack->GetName());

		return FVector::ZeroVector;
	}

	FName SocketName;

	//@상호작용 타입에 따라 소켓 이름 결정
	switch (InteractionType)
	{
	case EInteractionType::Execution:
		SocketName = FName("ExecuterTarget");
		break;
	case EInteractionType::Ambush:
		SocketName = FName("AmbusherTarget");
		break;
	default:
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "지원되지 않는 상호작용 타입: {0}",
			*UEnum::GetValueAsString(InteractionType));

		return FVector::ZeroVector;
	}

	//@소켓 위치
	FVector SocketLocation = TargetCharacter->GetMesh()->GetSocketLocation(SocketName);

	UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "소켓 위치 가져옴 - 상호작용: {0}, 소켓: {1}, 위치: {2}",
		*UEnum::GetValueAsString(InteractionType),
		*SocketName.ToString(),
		*SocketLocation.ToString());

	return SocketLocation;
}

FRotator UAT_UpdateMotionWarpTarget::GetFitRotationFromTargetToTrack() const
{
	//@Target, Interaction 유형
	if (!TargetToTrack.IsValid() || InteractionType == EInteractionType::None)
	{
		return FRotator::ZeroRotator;
	}

	//@Character
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetToTrack.Get());
	if (!TargetCharacter || !TargetCharacter->GetMesh())
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "타겟을 Character로 캐스팅할 수 없거나 메시가 없음 - 타겟: {0}",
			*TargetToTrack->GetName());
		return FRotator::ZeroRotator;
	}

	FName SocketName;

	//@상호작용 타입에 따라 소켓 이름 결정
	switch (InteractionType)
	{
	case EInteractionType::Execution:
		SocketName = FName("ExecuterTarget");
		break;
	case EInteractionType::Ambush:
		SocketName = FName("AmbusherTarget");
		break;
	default:
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "지원되지 않는 상호작용 타입: {0}",
			*UEnum::GetValueAsString(InteractionType));
		return FRotator::ZeroRotator;
	}

	//@소켓 회전 값
	FRotator SocketRotation = TargetCharacter->GetMesh()->GetSocketRotation(SocketName);

	//@회전 값 보정: 상대 캐릭터의 기본 회전 값에 보정 값 추가
	SocketRotation.Yaw += RotationOffset;

	UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "소켓 회전값 가져옴 (Yaw 보정) - 소켓: {0}, 최종회전: {1}",
		*SocketName.ToString(), *SocketRotation.ToString());

	return SocketRotation;
}

UMotionWarpingComponent* UAT_UpdateMotionWarpTarget::GetMotionWarpingComponent() const
{
	AActor* Avatar = GetAvatarActor();
	if (!Avatar)
	{
		return nullptr;
	}
	return Avatar->FindComponentByClass<UMotionWarpingComponent>();
}
#pragma endregion