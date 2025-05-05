#include "AnimalAIController.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogAnimalAIC)

//@Defualt Setting
#pragma region Default Setting
AAnimalAIController::AAnimalAIController(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    bIsFocusingOnTarget = false;
    FocusRotationSpeed = 180.0f; // 기본 회전 속도보다 느리게 설정
    MinFocusAngle = 30.0f; // 30도 이상 차이가 있을 때만 회전
}

void AAnimalAIController::BeginPlay()
{
    Super::BeginPlay();

    // 초기 회전 설정 적용
    UpdateRotationSettings();
}

void AAnimalAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Super::Tick에서는 UpdateControlRotation이 호출됨 (오버라이드 됨)
}

void AAnimalAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
    // 베이스 AI 컨트롤러의 UpdateControlRotation을 호출하지 않고 재정의합니다.

    // 포커스 모드가 아니면 회전하지 않음
    if (!bIsFocusingOnTarget)
    {
        return;
    }

    APawn* const AgentPawn = GetPawn();
    if (!AgentPawn) return;

    //@BB의 타겟 액터 확인
    AActor* TargetActor = nullptr;
    if (BBComponent)
    {
        TargetActor = Cast<AActor>(BBComponent->GetValueAsObject("TargetActor"));
    }

    //@타겟이 없으면 회전 업데이트 하지 않음
    if (!TargetActor) return;

    //@현재 위치와 회전
    FVector Start = AgentPawn->GetActorLocation();
    FRotator CurrentRotation = GetControlRotation();

    //@타겟을 향한 회전 계산
    TargetRotation = UKismetMathLibrary::FindLookAtRotation(Start, TargetActor->GetActorLocation());

    //@Yaw만 사용
    TargetRotation.Pitch = 0.0f;
    TargetRotation.Roll = 0.0f;

    // 현재 회전과 목표 회전 사이의 델타 각도 계산
    float DeltaAngle = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw));

    // 각도 차이가 최소 각도보다 작으면 회전하지 않음
    if (DeltaAngle < MinFocusAngle)
    {
        return;
    }

    //@보간된 최종 회전 계산 - 포커스 모드에서는 더 느리게 회전
    FRotator FinalRotation = UKismetMathLibrary::RInterpTo(
        CurrentRotation,
        TargetRotation,
        DeltaTime,
        FocusRotationSpeed
    );

    //@컨트롤러 회전 설정
    SetControlRotation(FinalRotation);

    UE_LOGFMT(LogAnimalAIC, Verbose, "타겟({0})을 향해 회전 - 델타 각도: {1}, 회전 속도: {2}",
        *TargetActor->GetName(), DeltaAngle, FocusRotationSpeed);
}

void AAnimalAIController::InitializeAIController(APawn* InPawn)
{
    Super::InitializeAIController(InPawn);

    // 캐릭터 무브먼트 컴포넌트 설정
    ACharacter* AgentCharacter = Cast<ACharacter>(InPawn);
    if (AgentCharacter && AgentCharacter->GetCharacterMovement())
    {
        // 기본적으로 bOrientRotationToMovement만 활성화
        AgentCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
        AgentCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;

        UE_LOGFMT(LogAnimalAIC, Log, "동물형 AI({0})의 이동 설정 초기화 - OrientRotationToMovement: true",
            *InPawn->GetName());
    }
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void AAnimalAIController::SetFocusToTargetActor(bool bEnable)
{
    if (bIsFocusingOnTarget == bEnable)
    {
        return; // 변경 없음
    }

    bIsFocusingOnTarget = bEnable;
    UpdateRotationSettings();

    // 변경 로그 출력
    UE_LOGFMT(LogAnimalAIC, Log, "타겟 포커스 모드 변경: {0}",
        bIsFocusingOnTarget ? TEXT("활성화") : TEXT("비활성화"));
}

void AAnimalAIController::UpdateRotationSettings()
{
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        return;
    }

    ACharacter* AgentCharacter = Cast<ACharacter>(ControlledPawn);
    if (!AgentCharacter || !AgentCharacter->GetCharacterMovement())
    {
        return;
    }

    UCharacterMovementComponent* MovementComp = AgentCharacter->GetCharacterMovement();

    // 포커스 모드에 따라 회전 설정 변경
    if (bIsFocusingOnTarget)
    {
        // 포커스 모드 - 컨트롤러가 회전을 관리
        MovementComp->bUseControllerDesiredRotation = true;
        MovementComp->bOrientRotationToMovement = false;
    }
    else
    {
        // 일반 모드 - 이동 방향으로 자연스럽게 회전
        MovementComp->bUseControllerDesiredRotation = false;
        MovementComp->bOrientRotationToMovement = true;
    }

    UE_LOGFMT(LogAnimalAIC, Log, "회전 설정 업데이트 - ControllerDesiredRotation: {0}, OrientRotationToMovement: {1}",
        MovementComp->bUseControllerDesiredRotation ? "true" : "false",
        MovementComp->bOrientRotationToMovement ? "true" : "false");
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void AAnimalAIController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
    // 베이스 AI 컨트롤러의 OnPerception 호출
    Super::OnPerception(Actor, Stimulus);

    // 타겟이 감지되면 포커스 모드 활성화
    if (Stimulus.WasSuccessfullySensed())
    {
        // 타겟에 포커스 설정 (필요에 따라 조건 추가 가능)
        SetFocusToTargetActor(true);

        UE_LOGFMT(LogAnimalAIC, Log, "타겟({0}) 감지 - 포커스 모드 활성화",
            *Actor->GetName());
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
void AAnimalAIController::SetFocusToTarget(bool bEnable)
{
    SetFocusToTargetActor(bEnable);
}
#pragma endregion