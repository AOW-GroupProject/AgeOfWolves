#include "ANS_ApplyDilation.h"
#include "Logging/StructuredLog.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY(LogANS_TimeDilation)

//@Defualt Setting
#pragma region Default Setting
UANS_ApplyDilation::UANS_ApplyDilation()
{
    //@기본값 설정
    bApplyGlobalDilation = true;

    //@타임 딜레이션 설정 초기화
    DilationSettings.DilationMode = ETimeDilationMode::SlowMotion;
    DilationSettings.DilationIntensity = ETimeDilationIntensity::Medium;
    DilationSettings.CustomDilationValue = 1.0f;
    DilationSettings.bSmoothTransition = true;
    DilationSettings.TransitionDuration = 0.2f;

    //@초기 타임 딜레이션 값 설정
    OriginalTimeDilation = 1.0f;
    TargetTimeDilation = 1.0f;
}

void UANS_ApplyDilation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    //@유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyBegin 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyBegin 실패 - 사유: 소유 액터가 유효하지 않음");
        return;
    }

    //@목표 타임 딜레이션 값 계산
    float TargetValue = CalculateTimeDilationValue();

    //@타임 딜레이션 적용
    ApplyTimeDilation(MeshComp, TargetValue);
}

void UANS_ApplyDilation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //@유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyEnd 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyEnd 실패 - 사유: 소유 액터가 유효하지 않음");
        return;
    }

    //@타임 딜레이션 복원
    RestoreTimeDilation(MeshComp);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
float UANS_ApplyDilation::CalculateTimeDilationValue() const
{
    //@커스텀 값이 설정되어 있다면 해당 값 사용
    if (DilationSettings.CustomDilationValue != 1.0f)
    {
        return DilationSettings.CustomDilationValue;
    }

    //@모드에 따른 값 계산
    float DilationValue = 1.0f;

    switch (DilationSettings.DilationMode)
    {
    case ETimeDilationMode::SlowMotion:
        //@슬로우 모션 - 1.0 미만 값
        switch (DilationSettings.DilationIntensity)
        {
        case ETimeDilationIntensity::Low:
            DilationValue = 0.7f;
            break;
        case ETimeDilationIntensity::Medium:
            DilationValue = 0.5f;
            break;
        case ETimeDilationIntensity::High:
            DilationValue = 0.25f;
            break;
        }
        break;

    case ETimeDilationMode::FastMotion:
        //@빠른 모션 - 1.0 초과 값
        switch (DilationSettings.DilationIntensity)
        {
        case ETimeDilationIntensity::Low:
            DilationValue = 1.5f;
            break;
        case ETimeDilationIntensity::Medium:
            DilationValue = 2.0f;
            break;
        case ETimeDilationIntensity::High:
            DilationValue = 3.0f;
            break;
        }
        break;
    }

    return DilationValue;
}

void UANS_ApplyDilation::ApplyTimeDilation(USkeletalMeshComponent* MeshComp, float TargetValue)
{
    //@유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogANS_TimeDilation, Error, "타임 딜레이션 적용 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    //@소유 액터 가져오기
    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogANS_TimeDilation, Error, "타임 딜레이션 적용 실패 - 사유: 소유 Actor가 유효하지 않음");
        return;
    }

    //@월드 가져오기
    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogANS_TimeDilation, Error, "타임 딜레이션 적용 실패 - 사유: World가 유효하지 않음");
        return;
    }

    //@현재 타임 딜레이션 저장
    if (bApplyGlobalDilation)
    {
        OriginalTimeDilation = UGameplayStatics::GetGlobalTimeDilation(World);
    }
    else
    {
        OriginalTimeDilation = Owner->CustomTimeDilation;
    }

    //@목표 타임 딜레이션 저장
    TargetTimeDilation = TargetValue;

    //@로그 출력
    UE_LOGFMT(LogANS_TimeDilation, Log,
        "타임 딜레이션 적용 시작 - Owner: {0}, 모드: {1}, 현재값: {2}, 목표값: {3}, 전환시간: {4}",
        *Owner->GetName(),
        bApplyGlobalDilation ? TEXT("글로벌") : TEXT("로컬"),
        OriginalTimeDilation,
        TargetTimeDilation,
        DilationSettings.bSmoothTransition ? DilationSettings.TransitionDuration : 0.0f
    );

    //@부드러운 전환 사용 여부에 따라 적용
    if (DilationSettings.bSmoothTransition)
    {
        //@타이머 설정 전 기존 타이머 정리
        if (World->GetTimerManager().IsTimerActive(TransitionTimerHandle))
        {
            World->GetTimerManager().ClearTimer(TransitionTimerHandle);
        }

        //@전환 시작
        OnTransitionTick(MeshComp, OriginalTimeDilation, TargetTimeDilation, DilationSettings.TransitionDuration);
    }
    else
    {
        //@즉시 적용
        if (bApplyGlobalDilation)
        {
            UGameplayStatics::SetGlobalTimeDilation(World, TargetTimeDilation);
        }
        else
        {
            Owner->CustomTimeDilation = TargetTimeDilation;
        }
    }
}

void UANS_ApplyDilation::RestoreTimeDilation(USkeletalMeshComponent* MeshComp)
{
    //@유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogANS_TimeDilation, Error, "타임 딜레이션 복원 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    //@소유 액터 가져오기
    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogANS_TimeDilation, Error, "타임 딜레이션 복원 실패 - 사유: 소유 Actor가 유효하지 않음");
        return;
    }

    //@월드 가져오기
    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogANS_TimeDilation, Error, "타임 딜레이션 복원 실패 - 사유: World가 유효하지 않음");
        return;
    }

    //@현재 값 가져오기
    float CurrentValue = 0.0f;
    if (bApplyGlobalDilation)
    {
        CurrentValue = UGameplayStatics::GetGlobalTimeDilation(World);
    }
    else
    {
        CurrentValue = Owner->CustomTimeDilation;
    }

    //@로그 출력
    UE_LOGFMT(LogANS_TimeDilation, Log,
        "타임 딜레이션 복원 시작 - Owner: {0}, 모드: {1}, 현재값: {2}, 복원값: {3}, 전환시간: {4}",
        *Owner->GetName(),
        bApplyGlobalDilation ? TEXT("글로벌") : TEXT("로컬"),
        CurrentValue,
        OriginalTimeDilation,
        DilationSettings.bSmoothTransition ? DilationSettings.TransitionDuration : 0.0f
    );

    //@부드러운 전환 사용 여부에 따라 복원
    if (DilationSettings.bSmoothTransition)
    {
        //@타이머 설정 전 기존 타이머 정리
        if (World->GetTimerManager().IsTimerActive(TransitionTimerHandle))
        {
            World->GetTimerManager().ClearTimer(TransitionTimerHandle);
        }

        //@전환 시작
        OnTransitionTick(MeshComp, CurrentValue, OriginalTimeDilation, DilationSettings.TransitionDuration);
    }
    else
    {
        //@즉시 복원
        if (bApplyGlobalDilation)
        {
            UGameplayStatics::SetGlobalTimeDilation(World, OriginalTimeDilation);
        }
        else
        {
            Owner->CustomTimeDilation = OriginalTimeDilation;
        }
    }
}
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UANS_ApplyDilation::OnTransitionTick(USkeletalMeshComponent* MeshComp, float StartValue, float EndValue, float Duration)
{
    //@유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogANS_TimeDilation, Error, "타임 딜레이션 전환 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    //@소유 액터 가져오기
    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogANS_TimeDilation, Error, "타임 딜레이션 전환 실패 - 사유: 소유 Actor가 유효하지 않음");
        return;
    }

    //@월드 가져오기
    UWorld* World = Owner->GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogANS_TimeDilation, Error, "타임 딜레이션 전환 실패 - 사유: World가 유효하지 않음");
        return;
    }

    //@정적 변수로 경과 시간과 총 소요 시간 관리
    static float ElapsedTime = 0.0f;
    static float TotalDuration = 0.0f;

    //@첫 호출 시 초기화
    if (ElapsedTime == 0.0f || FMath::IsNearlyEqual(ElapsedTime, TotalDuration, 0.01f))
    {
        ElapsedTime = 0.0f;
        TotalDuration = Duration;
    }

    //@경과 시간 증가
    ElapsedTime += World->GetDeltaSeconds();

    //@알파값 계산 (0.0 ~ 1.0)
    float Alpha = FMath::Clamp(ElapsedTime / TotalDuration, 0.0f, 1.0f);

    //@현재 타임 딜레이션 값 계산 (보간)
    float CurrentDilation = FMath::Lerp(StartValue, EndValue, Alpha);

    //@타임 딜레이션 적용
    if (bApplyGlobalDilation)
    {
        UGameplayStatics::SetGlobalTimeDilation(World, CurrentDilation);
    }
    else
    {
        Owner->CustomTimeDilation = CurrentDilation;
    }

    //@전환 완료 확인
    if (Alpha >= 1.0f)
    {
        UE_LOGFMT(LogANS_TimeDilation, Log, "타임 딜레이션 전환 완료 - 최종값: {0}", CurrentDilation);
        World->GetTimerManager().ClearTimer(TransitionTimerHandle);
        return;
    }

    //@다음 틱 예약
    World->GetTimerManager().SetTimer(
        TransitionTimerHandle,
        [this, MeshComp, StartValue, EndValue, Duration]()
        {
            this->OnTransitionTick(MeshComp, StartValue, EndValue, Duration);
        },
        World->GetDeltaSeconds(),
            false
            );
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
FString UANS_ApplyDilation::GetNotifyName_Implementation() const
{
    //@모드와 강도에 따른 표시 이름 생성
    FString ModeName;
    FString IntensityName;

    switch (DilationSettings.DilationMode)
    {
    case ETimeDilationMode::SlowMotion:
        ModeName = TEXT("슬로우 모션");
        break;
    case ETimeDilationMode::FastMotion:
        ModeName = TEXT("빠른 모션");
        break;
    }

    switch (DilationSettings.DilationIntensity)
    {
    case ETimeDilationIntensity::Low:
        IntensityName = TEXT("약");
        break;
    case ETimeDilationIntensity::Medium:
        IntensityName = TEXT("중");
        break;
    case ETimeDilationIntensity::High:
        IntensityName = TEXT("강");
        break;
    }

    return FString::Printf(TEXT("타임 딜레이션: %s (%s)"), *ModeName, *IntensityName);
}
#pragma endregion