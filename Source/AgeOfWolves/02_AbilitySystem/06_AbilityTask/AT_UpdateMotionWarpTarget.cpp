#include "AT_UpdateMotionWarpTarget.h"
#include "Logging/StructuredLog.h"

#include "Kismet/KismetMathLibrary.h"

#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LOGAT_UpdateMotionWarpTarget)

//@Defualt Setting
#pragma region Default Setting
UAT_UpdateMotionWarpTarget::UAT_UpdateMotionWarpTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	WarpDirection = EMotionWarpDirection::Normal;
	WarpProximity = EMotionWarpProximity::Normal;
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
	if (WarpProximity == EMotionWarpProximity::Closest)
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
	// Closest 근접도인 경우에만 충돌 응답 복원
	if (WarpProximity == EMotionWarpProximity::Closest)
	{
		AActor* Avatar = GetAvatarActor();
		if (Avatar)
		{
			UCapsuleComponent* CapsuleComp = Avatar->FindComponentByClass<UCapsuleComponent>();
			if (CapsuleComp)
			{
				// Pawn 채널에 대한 이전 충돌 응답으로 복원
				CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, PreviousPawnResponse);

				UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "Closest 근접도 종료 - Pawn 채널 충돌 응답 복원: Overlap -> {0}",
					PreviousPawnResponse == ECR_Block ? TEXT("Block") :
					PreviousPawnResponse == ECR_Overlap ? TEXT("Overlap") : TEXT("Ignore"));
			}
		}
	}

	// 태스크 종료 시 워프 타겟 제거
	if (UMotionWarpingComponent* MotionWarpingComp = GetMotionWarpingComponent())
	{
		MotionWarpingComp->RemoveWarpTarget(MotionWarpTargetName);
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "워프 타겟 제거: {0}", *MotionWarpTargetName.ToString());
	}

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
	EMotionWarpProximity WarpProximity)
{
	UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "CreateMotionWarpTargetTracker - 입력된 Proximity: {0}",
		UEnum::GetValueAsString(WarpProximity));

	UAT_UpdateMotionWarpTarget* MyTask = NewAbilityTask<UAT_UpdateMotionWarpTarget>(OwningAbility, TaskInstanceName);
	if (MyTask)
	{
		MyTask->TargetToTrack = TargetActor;
		MyTask->MotionWarpTargetName = WarpTargetName;
		MyTask->bTrackRotation = bFollowRotation;
		MyTask->WarpDirection = WarpDirection;
		MyTask->WarpProximity = WarpProximity;
	}
	return MyTask;
}

void UAT_UpdateMotionWarpTarget::UpdateWarpTarget()
{
	UMotionWarpingComponent* MotionWarpingComp = GetMotionWarpingComponent();
	if (!MotionWarpingComp || !TargetToTrack.IsValid())
	{
		return;
	}

	AActor* Avatar = GetAvatarActor();
	if (!Avatar)
	{
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Warning, "UpdateWarpTarget 실패 - 아바타가 유효하지 않음");
		return;
	}

	// 워프 타겟 생성
	FMotionWarpingTarget WarpTarget;
	WarpTarget.Name = MotionWarpTargetName;

	// 기본 위치에 방향 및 근접도 오프셋 적용
	FVector BaseLocation = TargetToTrack->GetActorLocation();
	FVector LocationOffset = CalculatePositionOffset(TargetToTrack.Get());
	WarpTarget.Location = BaseLocation + LocationOffset;

	// 회전값 계산
	FRotator CalculatedRotation;

	if (bTrackRotation)
	{
		// Avatar에서 Target을 바라보는 회전 계산
		CalculatedRotation = UKismetMathLibrary::FindLookAtRotation(
			Avatar->GetActorLocation(),
			TargetToTrack->GetActorLocation()
		);
	}
	else
	{
		// Avatar의 Controller 회전값 사용
		AController* AvatarController = nullptr;
		if (APawn* AvatarPawn = Cast<APawn>(Avatar))
		{
			AvatarController = AvatarPawn->GetController();
		}

		if (AvatarController)
		{
			CalculatedRotation = AvatarController->GetControlRotation();
		}
		else
		{
			//@Controller가 없는 경우 기본 회전값 사용
			CalculatedRotation = Avatar->GetActorRotation();
		}
	}

	// Z축(Yaw) 회전만 사용하고 X(Pitch), Y(Roll)는 0으로 설정
	WarpTarget.Rotation = FRotator(0.0f, CalculatedRotation.Yaw, 0.0f);

	// 워프 타겟 업데이트
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
	// Normal 방향일 경우 오프셋 없음
	if (WarpDirection == EMotionWarpDirection::Normal || WarpProximity == EMotionWarpProximity::Normal)
	{
		return FVector::ZeroVector;
	}

	// 근접도에 따른 스칼라 값 가져오기
	float OffsetScalar = GetProximityScalar();

	// 타겟의 Forward, Right 벡터 가져오기
	FVector ForwardVector = Target->GetActorForwardVector();
	FVector RightVector = Target->GetActorRightVector();

	// 방향에 따른 위치 오프셋 계산
	// 각 방향은 "내가 Target의 어느 방향에서 접근하는지"를 의미
	switch (WarpDirection)
	{
	case EMotionWarpDirection::Front:
		// Target의 앞에서 접근 - Proximity가 증가할수록(Closest) Target에 더 가까워짐
		return ForwardVector * OffsetScalar;  // 앞쪽에서 접근하므로 앞쪽으로 배치

	case EMotionWarpDirection::Back:
		// Target의 뒤에서 접근 - Proximity가 증가할수록(Closest) Target에 더 가까워짐
		return -ForwardVector * OffsetScalar;  // 뒤쪽에서 접근하므로 뒤쪽으로 배치

	case EMotionWarpDirection::Left:
		// Target의 왼쪽에서 접근 - Proximity가 증가할수록(Closest) Target에 더 가까워짐
		return -RightVector * OffsetScalar;  // 왼쪽에서 접근하므로 왼쪽으로 배치

	case EMotionWarpDirection::Right:
		// Target의 오른쪽에서 접근 - Proximity가 증가할수록(Closest) Target에 더 가까워짐
		return RightVector * OffsetScalar;  // 오른쪽에서 접근하므로 오른쪽으로 배치

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
	// 근접도에 따른 스칼라 값 반환
	switch (WarpProximity)
	{
	case EMotionWarpProximity::Close:
		return 70.f;
	case EMotionWarpProximity::Closer:
		return 60.0f;
	case EMotionWarpProximity::Closest:
		return 50.f;
	case EMotionWarpProximity::Normal:
	default:
		return 100.0f;
	}
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