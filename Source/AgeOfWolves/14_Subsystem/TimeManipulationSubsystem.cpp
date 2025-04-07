#include "TimeManipulationSubsystem.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(LogTimeManipulation);

//@Default Setting
#pragma region Default Setting
void UTimeManipulationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    //@Active Dilation Owner
    ActiveDilationOwner = nullptr;

    Super::Initialize(Collection);
}

void UTimeManipulationSubsystem::Deinitialize()
{
    // 모든 타임 딜레이션 정리
    TArray<AActor*> ActorsToReset;
    ActiveDilations.GetKeys(ActorsToReset);

    for (AActor* Actor : ActorsToReset)
    {
        StopTimeDilation(Actor, false, 0.0f);
    }

    // 타이머 핸들 정리
    if (UWorld* World = GetWorld())
    {
        for (auto& Pair : TimeDilationTimerHandles)
        {
            World->GetTimerManager().ClearTimer(Pair.Value);
        }
    }
    TimeDilationTimerHandles.Empty();

    // 애니메이션 인스턴스 바인딩 정리
    for (auto& Pair : AnimInstanceOwners)
    {
        if (IsValid(Pair.Key))
        {
            Pair.Key->OnMontageBlendingOut.RemoveAll(this);
        }
    }

    AnimInstanceOwners.Empty();

    Super::Deinitialize();
}

void UTimeManipulationSubsystem::Tick(float DeltaTime)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    // 전환 중인 모든 타임 딜레이션 처리
    TArray<AActor*> ActorsToRemove;

    for (auto& Pair : TransitioningDilations)
    {
        AActor* Actor = Pair.Key;
        FTimeDilationInfo& DilationInfo = Pair.Value;

        if (!IsValid(Actor))
        {
            ActorsToRemove.Add(Actor);
            continue;
        }

        // 타임 딜레이션 업데이트
        UpdateTimeDilation(Actor, DilationInfo, DeltaTime);

        // 전환 완료 여부 확인
        float ElapsedTime = World->GetTimeSeconds() - DilationInfo.StartTime;
        if (ElapsedTime >= DilationInfo.TransitionDuration)
        {
            // 전환 완료, 제거 목록에 추가
            ActorsToRemove.Add(Actor);

            // 최종 값 설정
            float FinalValue = ActiveDilations.Contains(Actor) ?
                DilationInfo.TargetDilation : DilationInfo.OriginalDilation;

            SetTimeDilation(Actor, DilationInfo, FinalValue);
        }
    }

    // 완료된 전환 제거
    for (AActor* Actor : ActorsToRemove)
    {
        TransitioningDilations.Remove(Actor);
    }
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
float UTimeManipulationSubsystem::CalculateTimeDilationValue(const FTimeDilationSettings& Settings) const
{
    //@CustomDilationValue
    if (Settings.CustomDilationValue != 1.0f)
    {
        return Settings.CustomDilationValue;
    }

    float DilationValue = 1.0f;
    //@Dilation Mode
    switch (Settings.DilationMode)
    {
        //@슬로우 모션
    case ETimeDilationMode::SlowMotion:
        switch (Settings.DilationIntensity)
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
        //@패스트 모션
    case ETimeDilationMode::FastMotion:
        switch (Settings.DilationIntensity)
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
        //@완전 정지 - 추가
    case ETimeDilationMode::Stop:
        switch (Settings.DilationIntensity)
        {
        case ETimeDilationIntensity::Low:
            DilationValue = 0.05f;
            break;
        case ETimeDilationIntensity::Medium:
            DilationValue = 0.02f;
            break;
        case ETimeDilationIntensity::High:
            DilationValue = 0.01f;
            break;
        }
        break;
        //@히트 스톱 - 강도에 따라 다른 값 사용
    case ETimeDilationMode::HitStop:
        switch (Settings.DilationIntensity)
        {
        case ETimeDilationIntensity::Low:
            DilationValue = 0.005f; 
            break;
        case ETimeDilationIntensity::Medium:
            DilationValue = 0.002f; 
            break;
        case ETimeDilationIntensity::High:
            DilationValue = 0.001f; 
            break;
        }
        break;
    }

    return DilationValue;
}
void UTimeManipulationSubsystem::StartTimeDilation(AActor* Owner, const FTimeDilationSettings& Settings, bool bGlobal)
{
    //@Owner
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "타임 딜레이션 시작 실패 - 유효하지 않은 액터");
        return;
    }

    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "타임 딜레이션 시작 실패 - 유효하지 않은 월드");
        return;
    }

    // 이미 활성화된 시간 조작이 있고 다른 Actor가 요청하는 경우 무시
    if (ActiveDilationOwner.IsValid() && ActiveDilationOwner.Get() != Owner && bIgnorePreviousRequests)
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "타임 딜레이션 시작 무시 - 사유: 다른 액터가 이미 활성화 중 - 현재 액터: {0}, 요청 액터: {1}",
            *ActiveDilationOwner.Get()->GetName(), *Owner->GetName());
        return;
    }

    //@목표 Dilation Value
    float TargetValue = CalculateTimeDilationValue(Settings);

    //@Already Active?
    if (!ActiveDilations.Contains(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Log, "타임 딜레이션 시작 - 액터: {0}, 목표값: {1}, 모드: {2}",
            *Owner->GetName(), TargetValue, bGlobal ? TEXT("글로벌") : TEXT("로컬"));
    }
    else
    {
        UE_LOGFMT(LogTimeManipulation, Log, "타임 딜레이션 업데이트 - 액터: {0}, 새 목표값: {1}", *Owner->GetName(), TargetValue);
    }

    //@기존에 활성화된 다른 액터가 있다면 중지
    if (ActiveDilationOwner.IsValid() && ActiveDilationOwner.Get() != Owner)
    {
        StopTimeDilation(ActiveDilationOwner.Get(), false, 0.0f);
    }

    // 실행 중인 타이머가 있다면 제거
    if (TimeDilationTimerHandles.Contains(Owner))
    {
        World->GetTimerManager().ClearTimer(TimeDilationTimerHandles[Owner]);
        TimeDilationTimerHandles.Remove(Owner);
    }

    //@Remove
    ActiveDilations.Remove(Owner);

    //@Remove
    if (TransitioningDilations.Contains(Owner))
    {
        TransitioningDilations.Remove(Owner);
    }

    //@Dilation Info
    FTimeDilationInfo DilationInfo;

    //@Glboal: 게임 전체에 속도 조절?
    if (bGlobal)
    {
        // 현재 글로벌 타임 딜레이션 값 저장
        DilationInfo.OriginalDilation = UGameplayStatics::GetGlobalTimeDilation(World);
    }
    else
    {
        DilationInfo.OriginalDilation = Owner->CustomTimeDilation;
    }

    //@Dilation Info 구성
    DilationInfo.TargetDilation = TargetValue;
    DilationInfo.bGlobal = bGlobal;

    // HitStop 모드일 경우 부드러운 전환 무시
    if (Settings.DilationMode == ETimeDilationMode::HitStop)
    {
        DilationInfo.bSmoothTransition = false;
        DilationInfo.TransitionDuration = 0.0f;
    }
    else
    {
        DilationInfo.bSmoothTransition = Settings.bSmoothTransition;
        DilationInfo.TransitionDuration = Settings.TransitionDuration;
    }

    DilationInfo.StartTime = World->GetTimeSeconds();

    //@Active Dilations
    ActiveDilations.Add(Owner, DilationInfo);

    // 현재 활성화된 소유자 설정
    ActiveDilationOwner = Owner;

    //@Anim Instance 등록
    RegisterAnimInstance(Owner);

    //@@지연 실행/즉시 실행
    bool bAllowSmoothTransition =
        (Settings.DilationMode == ETimeDilationMode::SlowMotion ||
            Settings.DilationMode == ETimeDilationMode::FastMotion);

    if (DilationInfo.bSmoothTransition &&
        DilationInfo.TransitionDuration > 0.0f &&
        bAllowSmoothTransition)
    {
        TransitioningDilations.Add(Owner, DilationInfo);
    }
    else
    {
        SetTimeDilation(Owner, DilationInfo, TargetValue);
    }

    //@타임 딜레이션 모드에 따른 자동 해제 처리
    switch (Settings.DilationMode)
    {
    case ETimeDilationMode::Stop:
    {
        // 완전 정지 모드일 경우 자동 해제 타이머 설정
        if (Settings.StopDuration > 0.0f)
        {
            UE_LOGFMT(LogTimeManipulation, Log, "완전 정지 모드 타이머 설정 - 액터: {0}, 지속 시간: {1}초",
                *Owner->GetName(), Settings.StopDuration);

            // 타이머 핸들 생성 및 저장
            FTimerHandle& StopModeTimerHandle = TimeDilationTimerHandles.FindOrAdd(Owner);

            World->GetTimerManager().SetTimer(
                StopModeTimerHandle,
                FTimerDelegate::CreateUObject(this, &UTimeManipulationSubsystem::OnStopModeTimerExpired, Owner, Settings.bSmoothTransition, Settings.TransitionDuration),
                Settings.StopDuration,
                false
            );
        }
        break;
    }

    case ETimeDilationMode::HitStop:
    {
        UE_LOGFMT(LogTimeManipulation, Log, "히트 스톱 모드 타이머 설정 - 액터: {0}, 예상 지속 시간: {1}초",
            *Owner->GetName(), TargetValue);

        //@타이머 핸들 생성 및 저장
        FTimerHandle& HitStopTimerHandle = TimeDilationTimerHandles.FindOrAdd(Owner);

        //@계산된 지속 시간 후 자동 해제
        World->GetTimerManager().SetTimer(
            HitStopTimerHandle,
            FTimerDelegate::CreateUObject(this, &UTimeManipulationSubsystem::OnHitStopTimerExpired, Owner),
            TargetValue,
            false
        );
        break;
    }

    default:
        break;
    }
}

void UTimeManipulationSubsystem::StopTimeDilation(AActor* Owner, bool bSmoothTransition, float TransitionDuration)
{
    //@Owner
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "타임 딜레이션 종료 실패 - 유효하지 않은 액터");
        return;
    }

    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "타임 딜레이션 종료 실패 - 유효하지 않은 월드");
        return;
    }

    //@Already Active?
    if (!ActiveDilations.Contains(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "타임 딜레이션 종료 실패 - 적용되지 않은 액터: {0}", *Owner->GetName());
        return;
    }

    // 실행 중인 타이머가 있다면 제거
    if (TimeDilationTimerHandles.Contains(Owner))
    {
        World->GetTimerManager().ClearTimer(TimeDilationTimerHandles[Owner]);
        TimeDilationTimerHandles.Remove(Owner);
    }

    //@Dilation Info
    FTimeDilationInfo DilationInfo = ActiveDilations[Owner];

    UE_LOGFMT(LogTimeManipulation, Log, "타임 딜레이션 종료 - 액터: {0}, 복원값: {1}",
        *Owner->GetName(), DilationInfo.OriginalDilation);

    //@부드러운 전환
    bool bAllowSmoothTransition = false;

    if (ActiveDilations.Contains(Owner))
    {
        ETimeDilationMode CurrentMode = ETimeDilationMode::SlowMotion; // 기본값

        // 현재 딜레이션 값으로 모드 추정
        float CurrentDilation = DilationInfo.TargetDilation;
        if (CurrentDilation > 1.0f)
        {
            CurrentMode = ETimeDilationMode::FastMotion;
        }
        else if (CurrentDilation < 0.01f)
        {
            CurrentMode = ETimeDilationMode::Stop;
        }

        bAllowSmoothTransition =
            (CurrentMode == ETimeDilationMode::SlowMotion ||
                CurrentMode == ETimeDilationMode::FastMotion);
    }

    if (bSmoothTransition && TransitionDuration > 0.0f && bAllowSmoothTransition)
    {
        FTimeDilationInfo TransitionInfo = DilationInfo;
        TransitionInfo.StartTime = World->GetTimeSeconds();
        TransitionInfo.TransitionDuration = TransitionDuration;
        TransitionInfo.bSmoothTransition = true;

        // TransitioningDilations에 추가하여 Tick에서 처리
        TransitioningDilations.Add(Owner, TransitionInfo);
    }
    else
    {
        // 부드러운 전환이 아닐 경우 즉시 적용
        SetTimeDilation(Owner, DilationInfo, DilationInfo.OriginalDilation);
    }

    //@Remove
    ActiveDilations.Remove(Owner);

    // 현재 활성화된 소유자가 Owner인 경우 초기화
    if (ActiveDilationOwner.Get() == Owner)
    {
        ActiveDilationOwner = nullptr;
    }

    //@Anim Instance 등록 해제
    UnregisterAnimInstance(Owner);
}

void UTimeManipulationSubsystem::StartGlobalTimeDilation(const FTimeDilationSettings& Settings)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "글로벌 타임 딜레이션 시작 실패 - 유효하지 않은 월드");
        return;
    }

    // 월드 세팅 액터를 글로벌 타임 딜레이션 소유자로 사용
    AActor* WorldActor = World->GetWorldSettings();
    if (!IsValid(WorldActor))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "글로벌 타임 딜레이션 시작 실패 - WorldSettings를 찾을 수 없음");
        return;
    }

    // 기존 함수를 이용해 글로벌 타임 딜레이션 적용
    StartTimeDilation(WorldActor, Settings, true);
}

void UTimeManipulationSubsystem::StopGlobalTimeDilation(bool bSmoothTransition, float TransitionDuration)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "글로벌 타임 딜레이션 종료 실패 - 유효하지 않은 월드");
        return;
    }

    // 글로벌 타임 딜레이션 소유자 찾기
    AActor* GlobalDilationActor = nullptr;

    for (auto& Pair : ActiveDilations)
    {
        if (Pair.Value.bGlobal)
        {
            GlobalDilationActor = Pair.Key;
            break;
        }
    }

    if (!GlobalDilationActor)
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "글로벌 타임 딜레이션 종료 - 적용된 글로벌 타임 딜레이션 없음");
        return;
    }

    // 기존 함수를 이용해 글로벌 타임 딜레이션 종료
    StopTimeDilation(GlobalDilationActor, bSmoothTransition, TransitionDuration);
}

void UTimeManipulationSubsystem::StartLocalTimeDilation(AActor* Owner, const FTimeDilationSettings& Settings)
{
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "로컬 타임 딜레이션 시작 실패 - 유효하지 않은 액터");
        return;
    }

    // 기존 함수를 이용해 로컬 타임 딜레이션 적용
    StartTimeDilation(Owner, Settings, false);
}

void UTimeManipulationSubsystem::StopLocalTimeDilation(AActor* Owner, bool bSmoothTransition, float TransitionDuration)
{
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "로컬 타임 딜레이션 종료 실패 - 유효하지 않은 액터");
        return;
    }

    // 로컬 타임 딜레이션 확인
    if (!ActiveDilations.Contains(Owner) || ActiveDilations[Owner].bGlobal)
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "로컬 타임 딜레이션 종료 - 적용된 로컬 타임 딜레이션 없음 - 액터: {0}", *Owner->GetName());
        return;
    }

    // 기존 함수를 이용해 로컬 타임 딜레이션 종료
    StopTimeDilation(Owner, bSmoothTransition, TransitionDuration);
}

void UTimeManipulationSubsystem::ApplyHitStop(AActor* Owner, const FTimeDilationSettings& Settings, bool bGlobal)
{
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "히트 스톱 적용 실패 - 유효하지 않은 액터");
        return;
    }

    UE_LOGFMT(LogTimeManipulation, Log, "히트 스톱 적용 시작 - 액터: {0}, 강도: {1}, 글로벌: {2}",
        *Owner->GetName(),
        static_cast<int32>(Settings.DilationIntensity),
        bGlobal ? TEXT("예") : TEXT("아니오"));

    //@히트 스톱 적용
    StartTimeDilation(Owner, Settings, bGlobal);
}

void UTimeManipulationSubsystem::ApplyHitStop(AActor* Owner, int32 FrameCount, bool bGlobal)
{
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "히트 스톱 적용 실패 - 유효하지 않은 액터");
        return;
    }

    //@FTimeDilationSettings
    FTimeDilationSettings HitStopSettings;
    HitStopSettings.DilationMode = ETimeDilationMode::HitStop;
    HitStopSettings.bSmoothTransition = false;

    //@히트 스톱 적용
    ApplyHitStop(Owner, HitStopSettings, bGlobal);
}

void UTimeManipulationSubsystem::UpdateTimeDilation(AActor* Owner, FTimeDilationInfo& DilationInfo, float DeltaTime)
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    //@경과 시간
    float ElapsedTime = World->GetTimeSeconds() - DilationInfo.StartTime;
    //@보간 
    float Alpha = FMath::Clamp(ElapsedTime / FMath::Max(DilationInfo.TransitionDuration, 0.01f), 0.0f, 1.0f);

    //@Start/Target Vlaue
    float StartValue = DilationInfo.bGlobal ?
        UGameplayStatics::GetGlobalTimeDilation(World) :
        Owner->CustomTimeDilation;

    float TargetValue = ActiveDilations.Contains(Owner) ?
        DilationInfo.TargetDilation :
        DilationInfo.OriginalDilation;

    //@Final Value
    float FinalValue = FMath::Lerp(StartValue, TargetValue, Alpha);

    // 타임 딜레이션 적용
    SetTimeDilation(Owner, DilationInfo, FinalValue);
}

void UTimeManipulationSubsystem::SetTimeDilation(AActor* Owner, const FTimeDilationInfo& DilationInfo, float Value)
{
    //@Wolrd
    UWorld* World = GetWorld();
    if (!World)
    {
        return;
    }

    //@Global?
    if (DilationInfo.bGlobal)
    {
        UGameplayStatics::SetGlobalTimeDilation(World, Value);
    }
    //@Local?
    else
    {
        Owner->CustomTimeDilation = Value;
    }
}

void UTimeManipulationSubsystem::RegisterAnimInstance(AActor* Owner)
{
    //@Owner 유효성 검사
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "애니메이션 인스턴스 등록 실패 - 사유: Owner가 유효하지 않음");
        return;
    }

    //@Character 캐스팅
    ACharacter* Character = Cast<ACharacter>(Owner);
    if (!Character)
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "애니메이션 인스턴스 등록 실패 - 사유: Owner를 Character로 캐스팅할 수 없음");
        return;
    }

    //@Mesh 유효성 검사
    USkeletalMeshComponent* Mesh = Character->GetMesh();
    if (!IsValid(Mesh))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "애니메이션 인스턴스 등록 실패 - 사유: Mesh 컴포넌트가 유효하지 않음");
        return;
    }

    //@AnimInstance 유효성 검사
    UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
    if (!IsValid(AnimInstance))
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "애니메이션 인스턴스 등록 실패 - 사유: AnimInstance가 유효하지 않음");
        return;
    }

    //@이미 등록된 경우 확인
    if (AnimInstanceOwners.Contains(AnimInstance))
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "애니메이션 인스턴스 등록 스킵 - 사유: 이미 등록된 AnimInstance");
        return;
    }

    //@애니메이션 인스턴스와 소유자 매핑
    AnimInstanceOwners.Add(AnimInstance, Owner);

    //@몽타주 블렌딩 아웃 이벤트 바인딩
    AnimInstance->OnMontageBlendingOut.AddDynamic(this, &UTimeManipulationSubsystem::OnMontageBlendingOut);

    UE_LOGFMT(LogTimeManipulation, Log, "애니메이션 인스턴스 등록 완료 - 액터: {0}", *Owner->GetName());
}

void UTimeManipulationSubsystem::UnregisterAnimInstance(AActor* Owner)
{
    //@Owner 유효성 검사
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "애니메이션 인스턴스 등록 해제 실패 - 사유: Owner가 유효하지 않음");
        return;
    }

    //@등록된 AnimInstance가 있는지 확인
    bool bHasRegisteredInstances = false;
    for (const auto& Pair : AnimInstanceOwners)
    {
        if (Pair.Value == Owner)
        {
            bHasRegisteredInstances = true;
            break;
        }
    }

    if (!bHasRegisteredInstances)
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "애니메이션 인스턴스 등록 해제 스킵 - 사유: 등록된 AnimInstance가 없음 - 액터: {0}", *Owner->GetName());
        return;
    }

    //@제거할 애니메이션 인스턴스 목록 생성
    TArray<UAnimInstance*> InstancesToRemove;

    //@소유자와 연결된 모든 애니메이션 인스턴스 처리
    for (auto& Pair : AnimInstanceOwners)
    {
        if (Pair.Value == Owner)
        {
            if (IsValid(Pair.Key))
            {
                //@몽타주 블렌딩 아웃 이벤트 바인딩 해제
                Pair.Key->OnMontageBlendingOut.RemoveAll(this);
                UE_LOGFMT(LogTimeManipulation, Log, "애니메이션 인스턴스 델리게이트 바인딩 해제 - 액터: {0}", *Owner->GetName());
            }

            InstancesToRemove.Add(Pair.Key);
        }
    }

    //@AnimInstance가 발견되지 않은 경우 체크
    if (InstancesToRemove.Num() == 0)
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "애니메이션 인스턴스 등록 해제 실패 - 사유: 유효한 AnimInstance가 없음 - 액터: {0}", *Owner->GetName());
        return;
    }

    //@애니메이션 인스턴스 매핑에서 제거
    for (UAnimInstance* AnimInstance : InstancesToRemove)
    {
        AnimInstanceOwners.Remove(AnimInstance);
    }

    UE_LOGFMT(LogTimeManipulation, Log, "애니메이션 인스턴스 등록 해제 완료 - 액터: {0}, 제거된 인스턴스 수: {1}", *Owner->GetName(), InstancesToRemove.Num());
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UTimeManipulationSubsystem::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
    //@Montage 유효성 검사
    if (!IsValid(Montage))
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "몽타주 블렌딩 아웃 처리 스킵 - 사유: Montage가 유효하지 않음");
        return;
    }

    //@AnimInstance 가져오기
    UAnimInstance* AnimInstance = Cast<UAnimInstance>(Montage->GetOuter());
    if (!IsValid(AnimInstance))
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "몽타주 블렌딩 아웃 처리 스킵 - 사유: AnimInstance를 가져올 수 없음");
        return;
    }

    //@등록된 AnimInstance인지 확인
    if (!AnimInstanceOwners.Contains(AnimInstance))
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "몽타주 블렌딩 아웃 처리 스킵 - 사유: 등록되지 않은 AnimInstance");
        return;
    }

    //@Owner 가져오기
    AActor* Owner = AnimInstanceOwners[AnimInstance];
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "몽타주 블렌딩 아웃 처리 실패 - 사유: Owner가 유효하지 않음");
        return;
    }

    //@중단된 경우만 처리
    if (!bInterrupted)
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "몽타주 블렌딩 아웃 처리 스킵 - 사유: 중단되지 않은 몽타주");
        return;
    }

    //@타임 딜레이션이 적용된 액터인지 확인
    if (!ActiveDilations.Contains(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "몽타주 블렌딩 아웃 처리 스킵 - 사유: 타임 딜레이션이 적용되지 않은 액터 - 액터: {0}", *Owner->GetName());
        return;
    }

    //@몽타주 중단으로 인한 타임 딜레이션 종료
    UE_LOGFMT(LogTimeManipulation, Log, "몽타주 중단으로 인한 타임 딜레이션 종료 - 액터: {0}, 몽타주: {1}", *Owner->GetName(), *Montage->GetName());

    //@부드러운 전환 없이 즉시 종료
    StopTimeDilation(Owner, false, 0.0f);
}

void UTimeManipulationSubsystem::OnHitStopTimerExpired(AActor* Owner)
{
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "히트 스톱 타이머 만료 - 유효하지 않은 액터");
        return;
    }

    // 타이머 핸들 제거
    TimeDilationTimerHandles.Remove(Owner);

    // 액터가 아직 타임 딜레이션 상태인지 확인
    if (!ActiveDilations.Contains(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "히트 스톱 타이머 만료 - 이미 해제된 액터: {0}", *Owner->GetName());
        return;
    }

    UE_LOGFMT(LogTimeManipulation, Log, "히트 스톱 타이머 만료 - 타임 딜레이션 종료 - 액터: {0}", *Owner->GetName());

    // 타임 딜레이션 즉시 종료
    StopTimeDilation(Owner, false, 0.0f);
}

void UTimeManipulationSubsystem::OnStopModeTimerExpired(AActor* Owner, bool bSmoothTransition, float TransitionDuration)
{
    if (!IsValid(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Warning, "정지 모드 타이머 만료 - 유효하지 않은 액터");
        return;
    }

    // 타이머 핸들 제거
    TimeDilationTimerHandles.Remove(Owner);

    // 액터가 아직 타임 딜레이션 상태인지 확인
    if (!ActiveDilations.Contains(Owner))
    {
        UE_LOGFMT(LogTimeManipulation, Verbose, "정지 모드 타이머 만료 - 이미 해제된 액터: {0}", *Owner->GetName());
        return;
    }

    UE_LOGFMT(LogTimeManipulation, Log, "정지 모드 타이머 만료 - 타임 딜레이션 종료 - 액터: {0}", *Owner->GetName());

    // 타임 딜레이션 종료
    StopTimeDilation(Owner, bSmoothTransition, TransitionDuration);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
bool UTimeManipulationSubsystem::IsActorTimeDilated(AActor* Owner) const
{
    return ActiveDilations.Contains(Owner);
}
#pragma endregion