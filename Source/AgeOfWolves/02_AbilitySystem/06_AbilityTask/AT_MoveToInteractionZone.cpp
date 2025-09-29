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

	//@ 상호작용 아바타가 pawn에 대해 충돌을 Overlap으로 지정
	UCapsuleComponent* CapsuleComp = AvatarActor->FindComponentByClass<UCapsuleComponent>();
	if (!CapsuleComp)
	{
		return;
	}

	PreviousPawnResponse = CapsuleComp->GetCollisionResponseToChannel(ECC_Pawn);
	PreviousWorldDynamicResponse = CapsuleComp->GetCollisionResponseToChannel(ECC_WorldDynamic);
	CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	CapsuleComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	//@ 상호작용 대상도 pawn에대해 충돌을 Overlap으로 지정

	AActor* TargetActorRef = TargetActor.Get();
	
	if (!TargetActorRef) return;

	TArray<UPrimitiveComponent*> Comps;
	TargetActorRef->GetComponents<UPrimitiveComponent>(Comps);

	for (UPrimitiveComponent* Comp : Comps)
	{
		if (!Comp) continue;

		ECollisionResponse PreviousResponse = Comp->GetCollisionResponseToChannel(ECC_Pawn);
		
		// Pawn 채널만 Overlap으로 변경
		Comp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

		FCollisionPawnOverrideBackup Backup;
		Backup.Comp = Comp;
		Backup.PrevPawnResponse = PreviousResponse;

		
		TargetActorPreviousPawnResponseBackupArray.Add(Backup);
	}

	UE_LOGFMT(LogAT_InteractionZone, Log, "충돌 응답 변경: Block -> Overlap");
}

void UAT_MoveToInteractionZone::RestoreCollisionResponse()
{
	AActor* AvatarActor = GetAvatarActor();
	if (!AvatarActor)
	{
		return;
	}

	//@ 상호작용 아바타가 충돌타입 복원
	UCapsuleComponent* CapsuleComp = AvatarActor->FindComponentByClass<UCapsuleComponent>();
	if (!CapsuleComp)
	{
		return;
	}

	CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, PreviousPawnResponse);
	CapsuleComp->SetCollisionResponseToChannel(ECC_WorldDynamic, PreviousWorldDynamicResponse);

	//@ 상호작용 대상  충돌타입 복원
	for (const FCollisionPawnOverrideBackup& Backup : TargetActorPreviousPawnResponseBackupArray)
	{
		if (!Backup.Comp.IsValid()) continue;

		UPrimitiveComponent* Comp = Backup.Comp.Get();
		Comp->SetCollisionResponseToChannel(ECC_Pawn, Backup.PrevPawnResponse);
	}

	TargetActorPreviousPawnResponseBackupArray.Empty();
	
	UE_LOGFMT(LogAT_InteractionZone, Log, "충돌 응답 복원: Overlap -> Block");
}

bool UAT_MoveToInteractionZone::CalculateSocketTransform()
{
	if (!TargetActor.IsValid())
	{
		return false;
	}

	UMeshComponent* TargetMesh = TargetActor->FindComponentByClass<UMeshComponent>(); //@ skelMesh, staticMesh 모두 범주되는 UMeshComponent로 찾음
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