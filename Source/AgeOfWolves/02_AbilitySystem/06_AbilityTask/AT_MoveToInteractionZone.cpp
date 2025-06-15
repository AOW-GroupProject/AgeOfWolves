#include "AT_MoveToInteractionZone.h"
#include "Logging/StructuredLog.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY(LogAT_InteractionZone)

UAT_MoveToInteractionZone::UAT_MoveToInteractionZone(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	SocketName = FName::FName();
	Duration = 1.0f;
	ElapsedTime = 0.0f;
}

UAT_MoveToInteractionZone* UAT_MoveToInteractionZone::MoveToInteractionZone(
	UGameplayAbility* OwningAbility, AActor* TargetActor, FName SocketName, float Duration)
{
	if (!OwningAbility || !TargetActor)
	{
		UE_LOGFMT(LogAT_InteractionZone, Warning, "Task 생성 실패 - 유효하지 않은 파라미터");
		return nullptr;
	}

	UAT_MoveToInteractionZone* MyTask = NewAbilityTask<UAT_MoveToInteractionZone>(OwningAbility);
	MyTask->TargetActor = TargetActor;
	MyTask->SocketName = SocketName;
	MyTask->Duration = Duration;

	if (!MyTask->CalculateSocketTransform())
	{
		UE_LOGFMT(LogAT_InteractionZone, Warning, "Socket Transform 계산 실패");
		MyTask->EndTask();
		return nullptr;
	}

	return MyTask;
}

void UAT_MoveToInteractionZone::Activate()
{
	Super::Activate();

	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor)
	{
		UE_LOGFMT(LogAT_InteractionZone, Warning, "Avatar Actor가 유효하지 않음");
		EndTask();
		return;
	}

	SetupCollisionResponse();

	StartLocation = AvatarActor->GetActorLocation();
	StartRotation = AvatarActor->GetActorRotation();
	ElapsedTime = 0.0f;

	UE_LOGFMT(LogAT_InteractionZone, Log, "상호작용 영역으로 이동 시작");
}

void UAT_MoveToInteractionZone::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor)
	{
		EndTask();
		return;
	}

	ElapsedTime += DeltaTime;
	float Progress = FMath::Clamp(ElapsedTime / Duration, 0.0f, 1.0f);

	FVector CurrentLocation = FMath::Lerp(StartLocation, TargetLocation, Progress);
	FRotator CurrentRotation = FMath::RInterpTo(StartRotation, TargetRotation, DeltaTime, 1.0f / Duration);

	AvatarActor->SetActorLocation(CurrentLocation, true);
	AvatarActor->SetActorRotation(CurrentRotation);

	if (Progress >= 1.0f)
	{
		AvatarActor->SetActorLocation(TargetLocation, true);
		AvatarActor->SetActorRotation(TargetRotation);

		RestoreCollisionResponse();

		UE_LOGFMT(LogAT_InteractionZone, Log, "상호작용 영역 도달 완료");
		OnInteractionZoneReached.Broadcast();
		EndTask();
	}
}

void UAT_MoveToInteractionZone::SetupCollisionResponse()
{
	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor)
	{
		return;
	}

	UCapsuleComponent* CapsuleComp = AvatarActor->FindComponentByClass<UCapsuleComponent>();
	if (!CapsuleComp)
	{
		return;
	}

	PreviousPawnResponse = CapsuleComp->GetCollisionResponseToChannel(ECC_Pawn);
	CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	UE_LOGFMT(LogAT_InteractionZone, Log, "충돌 응답 변경: Block -> Overlap");
}

void UAT_MoveToInteractionZone::RestoreCollisionResponse()
{
	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor)
	{
		return;
	}

	UCapsuleComponent* CapsuleComp = AvatarActor->FindComponentByClass<UCapsuleComponent>();
	if (!CapsuleComp)
	{
		return;
	}

	CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, PreviousPawnResponse);

	UE_LOGFMT(LogAT_InteractionZone, Log, "충돌 응답 복원: Overlap -> Block");
}

bool UAT_MoveToInteractionZone::CalculateSocketTransform()
{
	if (!TargetActor.IsValid())
	{
		return false;
	}

	USkeletalMeshComponent* TargetMesh = TargetActor->FindComponentByClass<USkeletalMeshComponent>();
	if (!TargetMesh || !TargetMesh->DoesSocketExist(SocketName))
	{
		UE_LOGFMT(LogAT_InteractionZone, Warning, "Socket '{0}' 찾을 수 없음", *SocketName.ToString());
		return false;
	}

	FTransform SocketTransform = TargetMesh->GetSocketTransform(SocketName);
	TargetLocation = SocketTransform.GetLocation();
	TargetRotation = SocketTransform.Rotator();

	return true;
}