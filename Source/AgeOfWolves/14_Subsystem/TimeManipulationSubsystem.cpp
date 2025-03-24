#include "TimeManipulationSubsystem.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY(LogTimeManipulation);

//@Defualt Setting
#pragma region Default Setting
void UTimeManipulationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
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
    // 전환 중인 타임 딜레이션 업데이트
    TArray<AActor*> CompletedTransitions;

    for (auto& Pair : TransitioningDilations)
    {
        AActor* Owner = Pair.Key;
        if (!IsValid(Owner))
        {
            CompletedTransitions.Add(Owner);
            continue;
        }

        FTimeDilationInfo& DilationInfo = Pair.Value;
        UpdateTimeDilation(Owner, DilationInfo, DeltaTime);

        // 전환 완료 체크
        float ElapsedTime = GetWorld()->GetTimeSeconds() - DilationInfo.StartTime;
        if (ElapsedTime >= DilationInfo.TransitionDuration)
        {
            // 최종 값 설정
            if (ActiveDilations.Contains(Owner))
            {
                SetTimeDilation(Owner, DilationInfo, DilationInfo.TargetDilation);
            }
            else
            {
                SetTimeDilation(Owner, DilationInfo, DilationInfo.OriginalDilation);
            }

            CompletedTransitions.Add(Owner);
        }
    }

    // 완료된 전환 제거
    for (AActor* Owner : CompletedTransitions)
    {
        TransitioningDilations.Remove(Owner);
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

    //@목표 Dilation Value
    float TargetValue = CalculateTimeDilationValue(Settings);

    //Already Active?
    if (!ActiveDilations.Contains(Owner))
    {

        UE_LOGFMT(LogTimeManipulation, Log, "타임 딜레이션 시작 - 액터: {0}, 목표값: {1}, 모드: {2}",
            *Owner->GetName(), TargetValue, bGlobal ? TEXT("글로벌") : TEXT("로컬"));
    }

    UE_LOGFMT(LogTimeManipulation, Log, "타임 딜레이션 업데이트 - 액터: {0}, 새 목표값: {1}", *Owner->GetName(), TargetValue);

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
    DilationInfo.bSmoothTransition = Settings.bSmoothTransition;
    DilationInfo.TransitionDuration = Settings.TransitionDuration;
    DilationInfo.StartTime = World->GetTimeSeconds();

    //@Active Dilations
    ActiveDilations.Add(Owner, DilationInfo);

    //@Anim Instance 등록
    RegisterAnimInstance(Owner);

    //@@지연 실행/즉시 실행
    if (Settings.bSmoothTransition && Settings.TransitionDuration > 0.0f)
    {
        // 부드러운 전환 사용 시 전환 딜레이션 정보 추가
        TransitioningDilations.Add(Owner, DilationInfo);
    }
    else
    {
        // 즉시 적용
        SetTimeDilation(Owner, DilationInfo, TargetValue);
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

    //@Dilation Info
    FTimeDilationInfo DilationInfo = ActiveDilations[Owner];

    UE_LOGFMT(LogTimeManipulation, Log, "타임 딜레이션 종료 - 액터: {0}, 복원값: {1}",
        *Owner->GetName(), DilationInfo.OriginalDilation);

    //@부드러운 전환
    if (bSmoothTransition && TransitionDuration > 0.0f)
    {
        FTimeDilationInfo TransitionInfo = DilationInfo;
        TransitionInfo.StartTime = World->GetTimeSeconds();
        TransitionInfo.TransitionDuration = TransitionDuration;

        TransitioningDilations.Add(Owner, TransitionInfo);
    }
    //@즉시 섲ㄴ환
    else
    {
        SetTimeDilation(Owner, DilationInfo, DilationInfo.OriginalDilation);
    }

    //@Remove
    ActiveDilations.Remove(Owner);

    //@Anim Instance 등록 해제
    UnregisterAnimInstance(Owner);
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
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
bool UTimeManipulationSubsystem::IsActorTimeDilated(AActor* Owner) const
{
    return ActiveDilations.Contains(Owner);
}
#pragma endregion