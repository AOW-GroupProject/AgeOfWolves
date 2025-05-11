#include "AnimalAnimInstance.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogAnimalAnim)

//@Default Setting
#pragma region Default Setting
UAnimalAnimInstance::UAnimalAnimInstance(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , PivotThreshold60(60.f)
    , PivotThreshold90(150.0f)
    , PivotThreshold120(180.0f)
    , PivotCooldownTime(1.0f)
    , PivotDuration(0.5f)
    , CurrentPivotTime(0.0f)
    , CurrentPivotCooldown(0.0f)
    , RotationDeltaAngle(0.0f)
    , PivotDirection(EPivotDirection::None)
    , PivotState(EPivotState::None)
    , bIsPivoting(false)
    , bIsPivotCooldown(false)
{
}

void UAnimalAnimInstance::NativeBeginPlay()
{
    Super::NativeBeginPlay();

    UE_LOGFMT(LogAnimalAnim, Log, "동물형 애님 인스턴스 초기화 - 피벗 임계값: 60({0}), 90({1}), 120({2})",
        PivotThreshold60, PivotThreshold90, PivotThreshold120);
}

void UAnimalAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();
}

void UAnimalAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    // 기존 코드에서는 Super::NativeUpdateAnimation(DeltaSeconds)가 호출되어
    // 이미 FindMovementState()가 실행된 후에 피벗 계산을 함

    // Super 호출 전에 필요한 변수 업데이트
    if (OwnerCharacterBaseRef.IsValid())
    {
        bFalling = OwnerCharacterBaseRef->GetMovementComponent()->IsFalling();
        Velocity = OwnerCharacterBaseRef->GetVelocity();
        Speed = Velocity.Length();

        // 피벗 계산을 먼저 수행
        if (Speed > 0.05f && !bFalling)
        {
            CalculatePivotAngle();
            UpdatePivotDirection();
        }
    }

    // 피벗 상태 업데이트
    UpdatePivotState(DeltaSeconds);

    // 이후 상위 클래스 호출 (FindMovementState 포함)
    Super::NativeUpdateAnimation(DeltaSeconds);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UAnimalAnimInstance::FindMovementState()
{
    if (bIsPlayingRootMotionMontageWithFullBodySlot)
    {
        Super::FindMovementState();
        return;
    }

    //@피벗 상태 감지 및 처리
    if (!bIsPivoting && !bIsPivotCooldown && Speed > 0.05f && !bFalling)
    {
        // 피벗 임계값 이상의 회전이 발생했는지 확인
        if (RotationDeltaAngle >= PivotThreshold60 && PivotDirection != EPivotDirection::None)
        {
            // 이전 상태 저장
            LastMovementState = MovementState;

            // MovementState를 Pivoting으로 설정 (EMovementState에 Pivoting 상태가 추가되었다고 가정)
            static const UEnum* MovementStateEnum = StaticEnum<EMovementState>();
            if (MovementStateEnum->GetIndexByName("Pivoting") != INDEX_NONE)
            {
                //@Pivoting 상태가 존재하면 설정
                MovementState = static_cast<EMovementState>(MovementStateEnum->GetIndexByName("Pivoting"));

                UE_LOGFMT(LogAnimalAnim, Log, "{0} - 이동 상태 변경: {1} -> Pivoting, 각도({2}), 방향({3})",
                    *OwnerCharacterBaseRef->GetName(),
                    *UEnum::GetValueAsString(LastMovementState),
                    RotationDeltaAngle,
                    *UEnum::GetValueAsString(PivotDirection));

                //@피벗 시작
                bIsPivoting = true;
                PivotState = EPivotState::Starting;
                CurrentPivotTime = 0.0f;

                //@이동 설정 업데이트
                UpdateMovementSettings();
                return;
            }
            else
            {
                // Pivoting 상태가 없으면 기존 상태 유지하며 피벗 플래그만 설정
                UE_LOGFMT(LogAnimalAnim, Warning, "Pivoting 상태가 EMovementState에 정의되지 않았습니다. bIsPivoting 플래그만 설정합니다.");
                bIsPivoting = true;
                PivotState = EPivotState::Starting;
                CurrentPivotTime = 0.0f;
            }
        }
    }

    // 피벗 중이면 MovementState 변경 억제
    if (bIsPivoting)
    {
        return;
    }

    // 기본 이동 상태 처리
    Super::FindMovementState();
}

void UAnimalAnimInstance::UpdatePivotDirection()
{
    if (bIsPivoting || bIsPivotCooldown)
    {
        UE_LOGFMT(LogAnimalAnim, Warning, "피벗 방향 업데이트 무시: 이미 피벗 중({0}) 또는 쿨다운 중({1})",
            bIsPivoting, bIsPivotCooldown);
        return;
    }

    // 이전 피벗 방향 저장
    EPivotDirection PrevDirection = PivotDirection;
    PivotDirection = EPivotDirection::None;

    // 속도 체크
    if (Velocity.SizeSquared() < 25.0f)
    {
        return;
    }

    // 회전 각도가 임계값 미만이면 피벗 없음
    if (RotationDeltaAngle < PivotThreshold60)
    {
        return;
    }

    // 현재 회전과 속도 방향의 회전 차이 계산
    FVector CurrentForward = OwnerCharacterBaseRef->GetActorForwardVector();
    FVector DesiredDirection = Velocity.GetSafeNormal();

    // 회전 방향 결정 (외적 사용)
    FVector CrossProduct = FVector::CrossProduct(CurrentForward, DesiredDirection);
    bool bIsLeft = CrossProduct.Z < 0.0f;

    // Idle 상태일 때는 90도 피벗도 고려
    if (MovementState == EMovementState::Idle)
    {
        // 90도 임계값 확인
        if (RotationDeltaAngle >= PivotThreshold90)
        {
            // 120도 피벗
            PivotDirection = bIsLeft ? EPivotDirection::Left120 : EPivotDirection::Right120;
            UE_LOGFMT(LogAnimalAnim, Log, "120도 피벗 선택 - 방향: {0}, 각도: {1}°",
                bIsLeft ? "Left" : "Right", RotationDeltaAngle);
        }
        else if (RotationDeltaAngle >= PivotThreshold60 + 15.0f) // 90도 중간 범위 (75도 이상)
        {
            // 90도 피벗
            PivotDirection = bIsLeft ? EPivotDirection::Left90 : EPivotDirection::Right90;
            UE_LOGFMT(LogAnimalAnim, Log, "90도 피벗 선택(Idle) - 방향: {0}, 각도: {1}°",
                bIsLeft ? "Left" : "Right", RotationDeltaAngle);
        }
        else
        {
            // 60도 피벗
            PivotDirection = bIsLeft ? EPivotDirection::Left60 : EPivotDirection::Right60;
            UE_LOGFMT(LogAnimalAnim, Log, "60도 피벗 선택 - 방향: {0}, 각도: {1}°",
                bIsLeft ? "Left" : "Right", RotationDeltaAngle);
        }
    }
    else // Walking/Sprinting 상태에서는 60/120도만 사용
    {
        if (RotationDeltaAngle >= PivotThreshold90)
        {
            // 120도 피벗
            PivotDirection = bIsLeft ? EPivotDirection::Left120 : EPivotDirection::Right120;
            UE_LOGFMT(LogAnimalAnim, Log, "120도 피벗 선택 - 방향: {0}, 각도: {1}°",
                bIsLeft ? "Left" : "Right", RotationDeltaAngle);
        }
        else
        {
            // 60도 피벗
            PivotDirection = bIsLeft ? EPivotDirection::Left60 : EPivotDirection::Right60;
            UE_LOGFMT(LogAnimalAnim, Log, "60도 피벗 선택 - 방향: {0}, 각도: {1}°",
                bIsLeft ? "Left" : "Right", RotationDeltaAngle);
        }
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
            UE_LOGFMT(LogAnimalAnim, Log, "피벗 쿨다운 종료");
        }
        return;
    }

    // 피벗 중인 경우 상태 업데이트
    if (bIsPivoting)
    {
        // 목표 회전 각도 설정 (처음 한 번만)
        static float TargetRotationAngle = 0.0f;
        static FRotator StartRotation;

        if (PivotState == EPivotState::Starting && CurrentPivotTime == 0.0f)
        {
            // 피벗 방향에 따른 목표 회전 각도 설정
            switch (PivotDirection)
            {
            case EPivotDirection::Left60:
            case EPivotDirection::Right60:
                TargetRotationAngle = 60.0f;
                break;
            case EPivotDirection::Left90:
            case EPivotDirection::Right90:
                TargetRotationAngle = 90.0f;
                break;
            case EPivotDirection::Left120:
            case EPivotDirection::Right120:
                TargetRotationAngle = 120.0f;
                break;
            default:
                TargetRotationAngle = 0.0f;
                break;
            }

            // 시작 회전값 저장
            StartRotation = OwnerCharacterBaseRef->GetActorRotation();
            UE_LOGFMT(LogAnimalAnim, Log, "피벗 시작: 목표 회전각({0}°), 방향({1})",
                TargetRotationAngle, *UEnum::GetValueAsString(PivotDirection));
        }

        // 시간 업데이트
        CurrentPivotTime += DeltaSeconds;

        // 현재 회전과 시작 회전 간의 차이 계산
        float CurrentRotationDelta = 0.0f;
        if (OwnerCharacterBaseRef.IsValid())
        {
            FRotator CurrentRotation = OwnerCharacterBaseRef->GetActorRotation();
            CurrentRotationDelta = FMath::Abs(FMath::FindDeltaAngleDegrees(StartRotation.Yaw, CurrentRotation.Yaw));
        }

        // 상태 전이 처리 - 회전 각도 기반
        switch (PivotState)
        {
        case EPivotState::Starting:
            // 시작 단계에서 피봇팅 단계로 전환 (20% 회전 완료)
            if (CurrentRotationDelta >= TargetRotationAngle * 0.2f || CurrentPivotTime >= PivotDuration * 0.3f)
            {
                PivotState = EPivotState::Pivoting;
                UE_LOGFMT(LogAnimalAnim, Log, "피벗 상태 변경: Starting -> Pivoting (회전: {0}°/{1}°)",
                    CurrentRotationDelta, TargetRotationAngle);
            }
            break;

        case EPivotState::Pivoting:
            // 목표 회전의 80% 이상 완료 또는 시간 종료 시 종료 단계로
            if (CurrentRotationDelta >= TargetRotationAngle * 0.8f || CurrentPivotTime >= PivotDuration * 0.7f)
            {
                PivotState = EPivotState::Ending;
                UE_LOGFMT(LogAnimalAnim, Log, "피벗 상태 변경: Pivoting -> Ending (회전: {0}°/{1}°)",
                    CurrentRotationDelta, TargetRotationAngle);
            }
            break;

        case EPivotState::Ending:
            // 목표 회전에 도달하거나 시간 종료 시 피벗 완료
            if (CurrentRotationDelta >= TargetRotationAngle || CurrentPivotTime >= PivotDuration)
            {
                // 피벗 상태 초기화
                PivotState = EPivotState::None;
                bIsPivoting = false;

                // 이전 이동 상태로 복원
                MovementState = LastMovementState;

                UE_LOGFMT(LogAnimalAnim, Log, "피벗 종료: 회전({0}°/{1}°), 이동 상태 복원({2})",
                    CurrentRotationDelta, TargetRotationAngle, *UEnum::GetValueAsString(MovementState));

                // 쿨다운 시작
                bIsPivotCooldown = true;
                CurrentPivotCooldown = 0.0f;

                // 회전 델타 초기화
                RotationDeltaAngle = 0.0f;
                PivotDirection = EPivotDirection::None;
                TargetRotationAngle = 0.0f;

                // 이동 설정 업데이트
                UpdateMovementSettings();
            }
            break;

        default:
            break;
        }
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
void UAnimalAnimInstance::CalculatePivotAngle()
{
    if (!OwnerCharacterBaseRef.IsValid() || bIsPivoting || bIsPivotCooldown)
    {
        return;
    }

    // 속도 계산
    if (Velocity.SizeSquared() < 25.0f)
    {
        // 속도가 너무 낮으면 피벗 감지 안함
        RotationDeltaAngle = 0.0f;
        return;
    }

    // 현재 방향과 목표 방향의 각도 차이 직접 계산
    FVector CurrentForward = OwnerCharacterBaseRef->GetActorForwardVector();
    FVector DesiredDirection = Velocity.GetSafeNormal();

    float DotProduct = FVector::DotProduct(CurrentForward, DesiredDirection);
    DotProduct = FMath::Clamp(DotProduct, -1.0f, 1.0f);
    float DirectAngle = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

    // 현재 회전 각도를 임계값의 두 배까지만 허용 (과도한 감지 방지)
    RotationDeltaAngle = FMath::Min(DirectAngle, PivotThreshold120 * 1.5f);
}
#pragma endregion
