#include "AT_UpdateMotionWarpTarget.h"
#include "Logging/StructuredLog.h"

#include "Kismet/KismetMathLibrary.h"

#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "01_Character/PlayerCharacter.h"
#include "04_Component/LockOnComponent.h"

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

	if (!TargetToTrack.IsValid())
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "Activate 실패 - 타겟 액터가 유효하지 않음");
		EndTask();
		return;
	}

	UMotionWarpingComponent* MotionWarpingComp = GetMotionWarpingComponent();
	if (!MotionWarpingComp)
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "Activate 실패 - MotionWarpingComponent를 찾을 수 없음");
		EndTask();
		return;
	}

	// Closest 근접도에서만 충돌 설정 변경
	if (WarpProximity == EMotionWarpProximity::Closest || WarpProximity == EMotionWarpProximity::Fit)
	{
		AActor* Avatar = GetAvatarActor();
		if (Avatar)
		{
			UCapsuleComponent* CapsuleComp = Avatar->FindComponentByClass<UCapsuleComponent>();
			if (CapsuleComp)
			{
				// 현재 Pawn 채널에 대한 충돌 응답 저장
				PreviousPawnResponse = CapsuleComp->GetCollisionResponseToChannel(ECC_Pawn);

				// Pawn 채널에 대해서만 Overlap으로 변경
				CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

				UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "Closest 근접도 - Pawn 채널 충돌 응답 변경: {0} -> Overlap",
					PreviousPawnResponse == ECR_Block ? TEXT("Block") :
					PreviousPawnResponse == ECR_Overlap ? TEXT("Overlap") : TEXT("Ignore"));
			}
		}
	}

	//@초기 타겟 위치로 모션 워프 타겟 설정
	UpdateWarpTarget();
}

void UAT_UpdateMotionWarpTarget::OnDestroy(bool bInOwnerFinished)
{
	//@Closest 근접도인 경우에만 충돌 응답 복원
	if (WarpProximity == EMotionWarpProximity::Closest || WarpProximity == EMotionWarpProximity::Fit)
	{
		AActor* Avatar = GetAvatarActor();
		if (Avatar)
		{
			UCapsuleComponent* CapsuleComp = Avatar->FindComponentByClass<UCapsuleComponent>();
			if (CapsuleComp)
			{
				//@Pawn 채널에 대한 이전 충돌 응답으로 복원
				CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, PreviousPawnResponse);

				UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "Closest 근접도 종료 - Pawn 채널 충돌 응답 복원: Overlap -> {0}",
					PreviousPawnResponse == ECR_Block ? TEXT("Block") :
					PreviousPawnResponse == ECR_Overlap ? TEXT("Overlap") : TEXT("Ignore"));
			}
		}
	}

	//@태스크 종료 시 워프 타겟 제거
	if (UMotionWarpingComponent* MotionWarpingComp = GetMotionWarpingComponent())
	{
		MotionWarpingComp->RemoveWarpTarget(MotionWarpTargetName);

		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "워프 타겟 제거: {0}", *MotionWarpTargetName.ToString());
	}

	//@상호작용 타입 초기화
	InteractionType = EInteractionType::None;

	Super::OnDestroy(bInOwnerFinished);
}

void UAT_UpdateMotionWarpTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (!TargetToTrack.IsValid())
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "TickTask 실패 - 타겟 액터가 유효하지 않음");
		EndTask();
		return;
	}

	//@매 틱마다 워프 타겟 업데이트
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
	bool bFollowRotation,
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

	UAT_UpdateMotionWarpTarget* MyTask = NewAbilityTask<UAT_UpdateMotionWarpTarget>(OwningAbility, TaskInstanceName);
	if (MyTask)
	{
		MyTask->TargetToTrack = TargetActor;
		MyTask->MotionWarpTargetName = WarpTargetName;
		MyTask->bTrackRotation = bFollowRotation;
		MyTask->WarpDirection = WarpDirection;
		MyTask->WarpProximity = WarpProximity;
		MyTask->InteractionType = InteractionType;
	}
	return MyTask;
}

void UAT_UpdateMotionWarpTarget::UpdateWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComp = GetMotionWarpingComponent();
	if (!MotionWarpingComp || !TargetToTrack.IsValid()) return;

	AActor* Avatar = GetAvatarActor();
	if (!Avatar) {
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "UpdateWarpTarget 실패 - 아바타가 유효하지 않음");
		return;
	}

	FMotionWarpingTarget WarpTarget;
	WarpTarget.Name = MotionWarpTargetName;

	// 위치 계산 로직
	bool bIsFitMode = (InteractionType != EInteractionType::None && WarpProximity == EMotionWarpProximity::Fit);

	if (bIsFitMode)
	{
		WarpTarget.Location = GetFitLocationFromTargetToTrack();

		// Fit 모드에서는 소켓의 회전값을 사용
		if (bTrackRotation)
		{
			WarpTarget.Rotation = GetFitRotationFromTargetToTrack();
			UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "Fit 모드 - 소켓 회전값 적용됨: {0}",
				*WarpTarget.Rotation.ToString());
		}
	}
	else
	{
		FVector BaseLocation = TargetToTrack->GetActorLocation();
		FVector LocationOffset = CalculatePositionOffset(TargetToTrack.Get());
		WarpTarget.Location = BaseLocation + LocationOffset;
	}

	// Fit 모드가 아니거나, Fit 모드지만 bTrackRotation이 false인 경우에만 기존 회전 로직 사용
	if (!bIsFitMode || (bIsFitMode && !bTrackRotation))
	{
		if (bTrackRotation)
		{
			//@기존 회전 로직 (타겟의 회전을 따라감)
			WarpTarget.Rotation = TargetToTrack->GetActorRotation();
		}
		else
		{
			APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(Avatar);
			if (PlayerChar && PlayerChar->GetLockOnComponent() && PlayerChar->GetLockOnComponent()->GetbLockOn())
			{
				WarpTarget.Rotation = PlayerChar->GetLockOnComponent()->GetFinalRotation();

				UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "LockOn 회전 적용됨: {0}",
					*WarpTarget.Rotation.ToString());
			}
			else
			{
				FVector LookAtDirection = TargetToTrack->GetActorLocation() - Avatar->GetActorLocation();
				if (!LookAtDirection.IsNearlyZero())
				{
					WarpTarget.Rotation = LookAtDirection.Rotation();

					UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "타겟 방향 회전 계산됨: {0}",
						*WarpTarget.Rotation.ToString());
				}
			}
		}
	}

	MotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);

	UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "WarpTarget 업데이트 - 타겟: {0} | 위치: {1} | 오프셋: {2} | 회전: {3} | 접근방향: {4} | 근접도: {5}",
		*TargetToTrack->GetName(),
		*WarpTarget.Location.ToString(),
		*(WarpTarget.Location - TargetToTrack->GetActorLocation()).ToString(),
		*WarpTarget.Rotation.ToString(),
		UEnum::GetValueAsString(WarpDirection),
		UEnum::GetValueAsString(WarpProximity));
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
	if (!TargetToTrack.IsValid() || InteractionType == EInteractionType::None)
	{
		return FVector::ZeroVector;
	}

	// Character로 캐스팅
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetToTrack.Get());
	if (!TargetCharacter || !TargetCharacter->GetMesh())
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "타겟을 Character로 캐스팅할 수 없거나 메시가 없음 - 타겟: {0}",
			*TargetToTrack->GetName());
		return FVector::ZeroVector;
	}

	FName SocketName;

	// 상호작용 타입에 따라 소켓 이름 결정
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

	//@소켓 위치 가져오기
	FVector SocketLocation = TargetCharacter->GetMesh()->GetSocketLocation(SocketName);

	UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "소켓 위치 가져옴 - 상호작용: {0}, 소켓: {1}, 위치: {2}",
		*UEnum::GetValueAsString(InteractionType),
		*SocketName.ToString(),
		*SocketLocation.ToString());

	return SocketLocation;
}

FRotator UAT_UpdateMotionWarpTarget::GetFitRotationFromTargetToTrack() const
{
	if (!TargetToTrack.IsValid() || InteractionType == EInteractionType::None)
	{
		return FRotator::ZeroRotator;
	}

	// Character로 캐스팅
	ACharacter* TargetCharacter = Cast<ACharacter>(TargetToTrack.Get());
	if (!TargetCharacter || !TargetCharacter->GetMesh())
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "타겟을 Character로 캐스팅할 수 없거나 메시가 없음 - 타겟: {0}",
			*TargetToTrack->GetName());
		return FRotator::ZeroRotator;
	}

	FName SocketName;

	// 상호작용 타입에 따라 소켓 이름 결정
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

	FRotator SocketRotation = TargetCharacter->GetMesh()->GetSocketRotation(SocketName);

	FRotator MeshRotation = TargetCharacter->GetMesh()->GetComponentRotation();
	FRotator WorldRotation = FRotator(MeshRotation.Quaternion() * SocketRotation.Quaternion());

	//@회전 값 보정: 상대 캐릭터의 기본 회전 값에 보정 값 추가
	WorldRotation.Yaw += RotationOffset;

	UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "소켓 회전값 가져옴 (Yaw 보정) - 소켓: {0}, 최종회전: {1}",
		*SocketName.ToString(), *WorldRotation.ToString());

	return WorldRotation;
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