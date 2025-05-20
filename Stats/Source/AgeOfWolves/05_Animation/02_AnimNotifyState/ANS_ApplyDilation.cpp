#include "ANS_ApplyDilation.h"
#include "Logging/StructuredLog.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "14_Subsystem/TimeManipulationSubsystem.h"

DEFINE_LOG_CATEGORY(LogANS_TimeDilation)

UANS_ApplyDilation::UANS_ApplyDilation()
{
    // 기본값 설정
    bApplyGlobalDilation = true;

    // 타임 딜레이션 설정 초기화
    DilationSettings.DilationMode = ETimeDilationMode::SlowMotion;
    DilationSettings.DilationIntensity = ETimeDilationIntensity::Medium;
    DilationSettings.CustomDilationValue = 1.0f;
    DilationSettings.bSmoothTransition = true;
    DilationSettings.TransitionDuration = 0.2f;
}

void UANS_ApplyDilation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
    //@MeshComp 유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyBegin 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    //@Owner 유효성 검사
    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyBegin 실패 - 사유: 소유 액터가 유효하지 않음");
        return;
    }

    //@World 유효성 검사
    UWorld* World = Owner->GetWorld();
    if (!IsValid(World))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyBegin 실패 - 사유: World가 유효하지 않음");
        return;
    }

    //@GameInstance 유효성 검사
    UGameInstance* GameInstance = World->GetGameInstance();
    if (!IsValid(GameInstance))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyBegin 실패 - 사유: GameInstance가 유효하지 않음");
        return;
    }

    //@시간 조작 서브시스템 가져오기
    UTimeManipulationSubsystem* TimeSystem = GameInstance->GetSubsystem<UTimeManipulationSubsystem>();
    if (!TimeSystem)
    {
        UE_LOGFMT(LogANS_TimeDilation, Error, "시간 조작 서브시스템을 찾을 수 없음 - 타임 딜레이션을 적용할 수 없습니다.");
        return;
    }

    //@서브시스템을 통한 타임 딜레이션 시작
    UE_LOGFMT(LogANS_TimeDilation, Log, "시간 조작 서브시스템을 통한 타임 딜레이션 시작 - 액터: {0}", *Owner->GetName());
    TimeSystem->StartTimeDilation(Owner, DilationSettings, bApplyGlobalDilation);
}

void UANS_ApplyDilation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    //@MeshComp 유효성 검사
    if (!IsValid(MeshComp))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyEnd 실패 - 사유: MeshComp가 유효하지 않음");
        return;
    }

    //@Owner 유효성 검사
    AActor* Owner = MeshComp->GetOwner();
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyEnd 실패 - 사유: 소유 액터가 유효하지 않음");
        return;
    }

    //@World 유효성 검사
    UWorld* World = Owner->GetWorld();
    if (!IsValid(World))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyBegin 실패 - 사유: World가 유효하지 않음");
        return;
    }

    //@GameInstance 유효성 검사
    UGameInstance* GameInstance = World->GetGameInstance();
    if (!IsValid(GameInstance))
    {
        UE_LOGFMT(LogANS_TimeDilation, Warning, "NotifyBegin 실패 - 사유: GameInstance가 유효하지 않음");
        return;
    }

    //@시간 조작 서브시스템 가져오기
    UTimeManipulationSubsystem* TimeSystem = GameInstance->GetSubsystem<UTimeManipulationSubsystem>();
    if (!TimeSystem)
    {
        UE_LOGFMT(LogANS_TimeDilation, Error, "시간 조작 서브시스템을 찾을 수 없음 - 타임 딜레이션을 적용할 수 없습니다.");
        return;
    }

    //@서브시스템을 통한 타임 딜레이션 종료
    UE_LOGFMT(LogANS_TimeDilation, Log, "시간 조작 서브시스템을 통한 타임 딜레이션 종료 - 액터: {0}", *Owner->GetName());
    TimeSystem->StopTimeDilation(Owner, DilationSettings.bSmoothTransition, DilationSettings.TransitionDuration);
}

FString UANS_ApplyDilation::GetNotifyName_Implementation() const
{
    // 모드와 강도에 따른 표시 이름 생성
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