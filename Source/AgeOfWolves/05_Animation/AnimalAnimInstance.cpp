#include "AnimalAnimInstance.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

//@Defualt Setting
#pragma region Default Setting
UAnimalAnimInstance::UAnimalAnimInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 피벗 관련 변수 초기화
    PivotThreshold60 = 45.0f;  // 45도 이상 차이 나면 60도 피벗
    PivotThreshold90 = 75.0f;  // 75도 이상 차이 나면 90도 피벗
    PivotThreshold120 = 105.0f; // 105도 이상 차이 나면 120도 피벗

    PivotCooldownTime = 1.0f;  // 피벗 후 쿨다운 시간
    PivotDuration = 0.5f;      // 피벗 애니메이션 재생 시간

    CurrentPivotTime = 0.0f;
    CurrentPivotCooldown = 0.0f;
    RotationDeltaAngle = 0.0f;

    PivotDirection = EPivotDirection::None;
    PivotState = EPivotState::None;

    bIsPivoting = false;
    bIsPivotCooldown = false;
}

void UAnimalAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

    // 추가 초기화 작업이 필요하면 여기에 추가
}

void UAnimalAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    // 추가 초기화 작업이 필요하면 여기에 추가
}

void UAnimalAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    // 피벗 각도 계산
    CalculatePivotAngle();

    // 피벗 방향 업데이트
    UpdatePivotDirection();

    // 피벗 상태 업데이트
    UpdatePivotState(DeltaSeconds);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UAnimalAnimInstance::FindMovementState()
{
    // 피벗 중인 경우
    if (bIsPivoting)
    {
        // MovementState를 Idle로 설정하여 이동 애니메이션이 재생되지 않도록 함
        LastMovementState = MovementState;
        MovementState = EMovementState::Idle;
        return;
    }

    // 루트 모션 재생 중인 경우
    if (bIsPlayingRootMotionMontage)
    {
        MovementState = EMovementState::Idle;
        return;
    }

    // 이전 이동 상태 저장
    LastMovementState = MovementState;

    // 현재 속도와 최대 걷기 속도
    float CurrentSpeed = Speed;
    float MaxWalkSpeed = OwnerCharacterBaseRef->GetCharacterMovement()->MaxWalkSpeed;
    bool bIsSprinting = MaxWalkSpeed >= 650.f;

    // 이동 상태 결정
    if (CurrentSpeed < 0.05f)
    {
        MovementState = EMovementState::Idle;
    }
    else
    {
        MovementState = bIsSprinting ? EMovementState::Sprinting : EMovementState::Walking;
    }

    // 이동 상태가 변경된 경우
    if (LastMovementState != MovementState)
    {
        // 이동 상태 변경 로그 출력
        UE_LOGFMT(LogAnimInstance, Log, "{0} - 이동 상태 변경: {1} -> {2}",
            *OwnerCharacterBaseRef->GetName(),
            *UEnum::GetValueAsString(LastMovementState),
            *UEnum::GetValueAsString(MovementState));

        // Idle로 전환 시 이전 상태에 따라 StopMotion 결정
        if (LastMovementState == EMovementState::Walking || LastMovementState == EMovementState::Sprinting)
        {
            LastMovementState == EMovementState::Walking ?
                UpdateStopMotionType(EStopMotionType::WalkStop)
                : UpdateStopMotionType(EStopMotionType::SprintStop);
        }

        // 이동 설정 업데이트
        UpdateMovementSettings();
    }
}

void UAnimalAnimInstance::CalculatePivotAngle()
{
    if (!OwnerCharacterBaseRef.IsValid() || bIsPivoting || bIsPivotCooldown)
    {
        return;
    }

    // 현재 캐릭터의 회전
    FRotator CurrentRotation = OwnerCharacterBaseRef->GetActorRotation();

    // 캐릭터가 향하고자 하는 방향 (속도 방향)
    FVector Vel = OwnerCharacterBaseRef->GetVelocity();

    // 속도가 너무 낮으면 계산하지 않음
    if (Vel.SizeSquared() < 25.0f)
    {
        RotationDeltaAngle = 0.0f;
        return;
    }

    // 속도 방향으로의 회전 계산
    FRotator VelocityRotation = UKismetMathLibrary::MakeRotFromX(Vel.GetSafeNormal());

    // 현재 회전과 목표 회전 간의 각도 차이 계산
    RotationDeltaAngle = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, VelocityRotation.Yaw));

    // 디버그 로그 (Log 레벨)
    UE_LOGFMT(LogAnimInstance, Log, "회전 차이 각도: {0}", RotationDeltaAngle);
}

void UAnimalAnimInstance::UpdatePivotDirection()
{
    if (bIsPivoting || bIsPivotCooldown)
    {
        return;
    }

    // 이전 피벗 방향 저장
    EPivotDirection PrevDirection = PivotDirection;

    // 회전 차이가 임계값 이상인 경우에만 피벗 방향 결정
    if (RotationDeltaAngle < PivotThreshold60)
    {
        // 각도 차이가 임계값보다 작으면 피벗하지 않음
        PivotDirection = EPivotDirection::None;
    }
    else
    {
        // 현재 회전과 속도 방향의 회전 차이 계산
        FRotator CurrentRotation = OwnerCharacterBaseRef->GetActorRotation();
        FVector Vel = OwnerCharacterBaseRef->GetVelocity();
        FRotator VelocityRotation = UKismetMathLibrary::MakeRotFromX(Vel.GetSafeNormal());

        // 왼쪽/오른쪽 방향 결정
        float DeltaAngle = FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, VelocityRotation.Yaw);
        bool bIsLeft = DeltaAngle < 0.0f; // 음수면 왼쪽, 양수면 오른쪽

        // 절대 각도에 따른 피벗 방향 결정
        if (RotationDeltaAngle >= PivotThreshold120)
        {
            PivotDirection = bIsLeft ? EPivotDirection::Left120 : EPivotDirection::Right120;
        }
        else if (RotationDeltaAngle >= PivotThreshold90)
        {
            PivotDirection = bIsLeft ? EPivotDirection::Left90 : EPivotDirection::Right90;
        }
        else // 여기서는 이미 PivotThreshold60 이상임이 보장됨
        {
            PivotDirection = bIsLeft ? EPivotDirection::Left60 : EPivotDirection::Right60;
        }
    }

    // 피벗 방향이 변경된 경우 로그 출력
    if (PrevDirection != PivotDirection && PivotDirection != EPivotDirection::None)
    {
        UE_LOGFMT(LogAnimInstance, Log, "피벗 방향 변경: {0}, 회전 차이 각도: {1}",
            *UEnum::GetValueAsString(PivotDirection), RotationDeltaAngle);
    }
}

void UAnimalAnimInstance::UpdatePivotState(float DeltaSeconds)
{
    // 피벗 쿨다운 처리
    if (bIsPivotCooldown)
    {
        CurrentPivotCooldown += DeltaSeconds;
        if (CurrentPivotCooldown >= PivotCooldownTime)
        {
            bIsPivotCooldown = false;
            CurrentPivotCooldown = 0.0f;
            UE_LOGFMT(LogAnimInstance, Log, "피벗 쿨다운 종료");
        }
        return;
    }

    // 피벗 중이 아니고, 피벗 방향이 있으면 피벗 시작
    if (!bIsPivoting && PivotDirection != EPivotDirection::None)
    {
        bIsPivoting = true;
        PivotState = EPivotState::Starting;
        CurrentPivotTime = 0.0f;
        UE_LOGFMT(LogAnimInstance, Log, "피벗 시작: {0}", *UEnum::GetValueAsString(PivotDirection));
    }

    // 피벗 중인 경우 상태 업데이트
    if (bIsPivoting)
    {
        CurrentPivotTime += DeltaSeconds;

        // 피벗 상태 전환 (시작 -> 피벗 중 -> 종료)
        if (PivotState == EPivotState::Starting && CurrentPivotTime >= PivotDuration * 0.2f)
        {
            PivotState = EPivotState::Pivoting;
            UE_LOGFMT(LogAnimInstance, Log, "피벗 상태 변경: Starting -> Pivoting");
        }
        else if (PivotState == EPivotState::Pivoting && CurrentPivotTime >= PivotDuration * 0.8f)
        {
            PivotState = EPivotState::Ending;
            UE_LOGFMT(LogAnimInstance, Log, "피벗 상태 변경: Pivoting -> Ending");
        }
        else if (PivotState == EPivotState::Ending && CurrentPivotTime >= PivotDuration)
        {
            // 피벗 종료
            bIsPivoting = false;
            PivotState = EPivotState::None;
            PivotDirection = EPivotDirection::None;
            bIsPivotCooldown = true;
            CurrentPivotCooldown = 0.0f;
            UE_LOGFMT(LogAnimInstance, Log, "피벗 종료, 쿨다운 시작");
        }
    }
}
#pragma endregion