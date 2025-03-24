#include "AT_UpdateMotionWarpTarget.h"
#include "Logging/StructuredLog.h"

#include "Kismet/KismetMathLibrary.h"

#include "MotionWarpingComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LOGAT_UpdateMotionWarpTarget)

//@Defualt Setting
#pragma region Default Setting
UAT_UpdateMotionWarpTarget::UAT_UpdateMotionWarpTarget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
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

	// 초기 타겟 위치로 모션 워프 타겟 설정
	UpdateWarpTarget();
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

	// 매 틱마다 워프 타겟 업데이트
	UpdateWarpTarget();
}

void UAT_UpdateMotionWarpTarget::OnDestroy(bool bInOwnerFinished)
{
	// 태스크 종료 시 워프 타겟 제거
	if (UMotionWarpingComponent* MotionWarpingComp = GetMotionWarpingComponent())
	{
		MotionWarpingComp->RemoveWarpTarget(MotionWarpTargetName);
		UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "워프 타겟 제거: {0}", *MotionWarpTargetName.ToString());
	}

	Super::OnDestroy(bInOwnerFinished);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
UAT_UpdateMotionWarpTarget* UAT_UpdateMotionWarpTarget::CreateMotionWarpTargetTracker(
	UGameplayAbility* OwningAbility,
	FName TaskInstanceName,
	AActor* TargetActor,
	FName WarpTargetName,
	bool bFollowRotation)
{
	UAT_UpdateMotionWarpTarget* MyTask = NewAbilityTask<UAT_UpdateMotionWarpTarget>(OwningAbility, TaskInstanceName);
	if (MyTask)
	{
		MyTask->TargetToTrack = TargetActor;
		MyTask->MotionWarpTargetName = WarpTargetName;
		MyTask->bTrackRotation = bFollowRotation;
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
	WarpTarget.Location = TargetToTrack->GetActorLocation();

	// Avatar의 Controller 회전 값 또는 LookAt 회전 계산
	if (bTrackRotation)
	{
		// Avatar에서 Target을 바라보는 회전 계산
		WarpTarget.Rotation = UKismetMathLibrary::FindLookAtRotation(
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
			WarpTarget.Rotation = AvatarController->GetControlRotation();
		}
		else
		{
			// Controller가 없는 경우 기본 회전값 사용
			WarpTarget.Rotation = Avatar->GetActorRotation();
		}
	}

	// 워프 타겟 업데이트
	MotionWarpingComp->AddOrUpdateWarpTarget(WarpTarget);

	UE_LOGFMT(LOGAT_UpdateMotionWarpTarget, Log, "WarpTarget 업데이트 - 타겟: {0} | 위치: {1} | 회전: {2}",
		*TargetToTrack->GetName(),
		*WarpTarget.Location.ToString(),
		*WarpTarget.Rotation.ToString());
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
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