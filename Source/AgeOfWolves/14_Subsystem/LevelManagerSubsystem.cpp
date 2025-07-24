#include "LevelManagerSubsystem.h"
#include "Logging/StructuredLog.h"

#include "17_GameMode/AOWGameState.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogLevelManager)

//@Default Setting
#pragma region Default Setting
ULevelManagerSubsystem::ULevelManagerSubsystem()
{}

void ULevelManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    //@레벨 데이터 에셋 로드
    LevelDataInfos = LoadObject<ULevelDataInfos>(nullptr, TEXT("/Game/Blueprints/10_Level/DA_LevelDataInfos"));

    if (LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Log, "레벨 데이터 정보 로드 성공 - 총 {0}개의 레벨 데이터 확인됨",
            LevelDataInfos->GetLevelCount());

        //@데이터 무결성 검증
        if (IsLevelDataValid())
        {
            UE_LOGFMT(LogLevelManager, Log, "레벨 데이터 무결성 검증 완료");

            //@기본 레벨 설정
            const FLevelData* DefaultLevel = LevelDataInfos->GetDefaultLevel();
            if (DefaultLevel && DefaultLevel->bIsEnabled)
            {
                CurrentLevelTag = DefaultLevel->LevelTag;
                UE_LOGFMT(LogLevelManager, Log, "기본 레벨 설정 완료: {0}", *CurrentLevelTag.ToString());
            }
            else
            {
                UE_LOGFMT(LogLevelManager, Warning, "기본 레벨을 찾을 수 없거나 비활성화됨");
            }
        }
        else
        {
            UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터에 무결성 문제 발견 - 런타임 오류 가능성 있음");
        }
    }
    else
    {
        UE_LOGFMT(LogLevelManager, Error, "레벨 데이터 정보 로드 실패 - 경로 확인 필요: /Game/Blueprints/10_Level/DA_LevelDataInfos");
    }

    //@Begin Play
    if (UWorld* World = GetWorld())
    {
        WorldBeginPlayHandle = World->OnWorldBeginPlay.AddUObject(this, &ULevelManagerSubsystem::OnWorldBeginPlay);
        UE_LOGFMT(LogLevelManager, Log, "World BeginPlay 델리게이트에 바인딩 완료");
    }
}

void ULevelManagerSubsystem::ExternalBindinToGameState()
{
    //@AOW Game State
    auto GameState = UGameplayStatics::GetGameState(GetGameInstance());
    if (!GameState)
    {
        return;
    }

    auto AOWGameState = CastChecked<AAOWGameState>(GameState);

    //@외부 바인딩...
    AOWGameState->RequestStartLevelTransition.BindUFunction(this, "OnRequestStartLevelTransition");
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void ULevelManagerSubsystem::PerformLevelStreamingOperations(const FLevelData& TargetLevelData)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogLevelManager, Error, "World를 찾을 수 없습니다");

        //@실패 시 상태 정리
        FScopeLock Lock(&LevelTransitionLock);
        bIsLevelTransitionInProgress = false;
        PendingLevelTag = FGameplayTag();
        return;
    }

    //@현재 레벨 언로드
    if (CurrentLevelTag.IsValid())
    {
        FLevelData CurrentLevelData;
        if (LevelDataInfos->GetLevelByTag(CurrentLevelTag, CurrentLevelData))
        {
            FString CurrentLevelName = CurrentLevelData.LevelAsset.GetAssetName();
            UE_LOGFMT(LogLevelManager, Log, "현재 레벨 언로드: {0}", *CurrentLevelName);

            UGameplayStatics::UnloadStreamLevel(World, FName(*CurrentLevelName),
                FLatentActionInfo(), true);
        }
    }

    //@새 레벨 로드
    FString NewLevelName = TargetLevelData.LevelAsset.GetAssetName();
    UE_LOGFMT(LogLevelManager, Log, "새 레벨 로드: {0}", *NewLevelName);

    UGameplayStatics::LoadStreamLevel(World, FName(*NewLevelName),
        true, true, FLatentActionInfo());

    CurrentLevelTag = PendingLevelTag;
    bIsLevelTransitionInProgress = false;
    PendingLevelTag = FGameplayTag();

    //@완료 이벤트 호출
    LevelTransitionCompleted.ExecuteIfBound(CurrentLevelTag);

    UE_LOGFMT(LogLevelManager, Log, "레벨 전환 완료: {0}", *CurrentLevelTag.ToString());
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void ULevelManagerSubsystem::OnWorldBeginPlay()
{
    //@외부 바인딩...
    ExternalBindinToGameState();
}

void ULevelManagerSubsystem::OnRequestStartLevelTransition(const FGameplayTag& NextLevelTag)
{
    UE_LOGFMT(LogLevelManager, Log, "레벨 전환 요청: {0}", *NextLevelTag.ToString());

    if (bIsLevelTransitionInProgress)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 전환이 이미 진행 중입니다. 현재 목표: {0}", *PendingLevelTag.ToString());
        return;
    }

    //@크리티컬 섹션으로 동시 접근 방지
    FScopeLock Lock(&LevelTransitionLock);

    //@기본 유효성 검사
    if (!NextLevelTag.IsValid())
    {
        UE_LOGFMT(LogLevelManager, Error, "유효하지 않은 레벨 태그: {0}", *NextLevelTag.ToString());
        return;
    }

    if (bIsLevelTransitionInProgress)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 전환이 이미 진행 중입니다. 현재 목표: {0}", *PendingLevelTag.ToString());
        return;
    }

    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Error, "레벨 데이터 정보가 로드되지 않았습니다");
        return;
    }

    //@대상 레벨 데이터 검색 및 검증
    FLevelData TargetLevelData;
    if (!LevelDataInfos->GetLevelByTag(NextLevelTag, TargetLevelData))
    {
        UE_LOGFMT(LogLevelManager, Error, "레벨 태그를 찾을 수 없습니다: {0}", *NextLevelTag.ToString());
        return;
    }

    if (!TargetLevelData.bIsEnabled || !TargetLevelData.HasValidAsset())
    {
        UE_LOGFMT(LogLevelManager, Error, "레벨을 로드할 수 없습니다: {0}", *NextLevelTag.ToString());
        return;
    }

    //@전환 상태 설정
    bIsLevelTransitionInProgress = true;
    PendingLevelTag = NextLevelTag;

    //@이미 GameThread에서 실행 중이므로 직접 레벨 스트리밍 수행
    PerformLevelStreamingOperations(TargetLevelData);
}
#pragma endregion

//@Utility Functions
#pragma region Utility

TArray<FLevelData> ULevelManagerSubsystem::GetLevelsByType(const ELevelType& LevelType) const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return TArray<FLevelData>();
    }

    TArray<FLevelData> Result = LevelDataInfos->GetLevelsByType(LevelType);
    UE_LOGFMT(LogLevelManager, Log, "{0} 타입의 레벨 {1}개를 반환합니다.",
        *UEnum::GetValueAsString(LevelType), Result.Num());

    return Result;
}

bool ULevelManagerSubsystem::GetLevelByTag(const FGameplayTag& LevelTag, FLevelData& OutLevelData) const
{
    //@Level Manager 서브시스템의 기본 상태 확인
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return false;
    }

    //@전달받은 태그의 유효성 사전 검증
    if (!LevelTag.IsValid())
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 검색 실패: 유효하지 않은 레벨 태그입니다 - {0}",
            *LevelTag.ToString());
        return false;
    }

    //@LevelDataInfos의 새로운 태그 기반 검색 함수 호출
    bool bFound = LevelDataInfos->GetLevelByTag(LevelTag, OutLevelData);

    if (bFound)
    {
        //@검색 성공 시 상세 정보 로깅 (디버깅에 유용)
        UE_LOGFMT(LogLevelManager, Log,
            "레벨 검색 성공: 태그 '{0}' -> 이름 '{1}', 카테고리 '{2}', 활성화 상태 '{3}'",
            *LevelTag.ToString(),
            *OutLevelData.LevelName.ToString(),
            *UEnum::GetValueAsString(OutLevelData.LevelCategory),
            OutLevelData.bIsEnabled ? TEXT("활성화") : TEXT("비활성화"));
    }
    else
    {
        //@검색 실패 시 명확한 오류 메시지 제공
        UE_LOGFMT(LogLevelManager, Warning,
            "레벨 태그 '{0}'에 해당하는 레벨을 찾지 못했습니다. 태그가 올바른지 확인하세요.",
            *LevelTag.ToString());
    }

    return bFound;
}

bool ULevelManagerSubsystem::GetLevelByName(const FText& LevelName, FLevelData& OutLevelData) const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return false;
    }

    bool bFound = LevelDataInfos->GetLevelByName(LevelName, OutLevelData);

    if (!bFound)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 이름 '{0}'을 찾지 못했습니다.", *LevelName.ToString());
    }

    return bFound;
}

TArray<FLevelData> ULevelManagerSubsystem::GetEnabledLevels() const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return TArray<FLevelData>();
    }

    TArray<FLevelData> EnabledLevels = LevelDataInfos->GetEnabledLevels();
    UE_LOGFMT(LogLevelManager, Log, "활성화된 레벨 {0}개를 반환합니다.", EnabledLevels.Num());

    return EnabledLevels;
}

bool ULevelManagerSubsystem::IsLevelEnabled(const FGameplayTag& LevelTag) const
{
    //@Level Manager 서브시스템의 기본 상태 확인
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return false;
    }

    //@전달받은 태그의 유효성 사전 검증
    if (!LevelTag.IsValid())
    {
        UE_LOGFMT(LogLevelManager, Warning,
            "레벨 활성화 상태 확인 실패: 유효하지 않은 레벨 태그입니다 - {0}",
            *LevelTag.ToString());
        return false;
    }

    //@LevelDataInfos의 새로운 태그 기반 활성화 상태 확인 함수 호출
    bool bIsEnabled = LevelDataInfos->IsLevelEnabled(LevelTag);

    //@디버깅을 위한 상세 로깅 (성능에 영향 없는 Log 레벨)
    UE_LOGFMT(LogLevelManager, Log,
        "레벨 활성화 상태 확인: 태그 '{0}' -> {1}",
        *LevelTag.ToString(),
        bIsEnabled ? TEXT("활성화됨") : TEXT("비활성화됨"));

    return bIsEnabled;
}

bool ULevelManagerSubsystem::GetFirstLevelOfType(const ELevelType& LevelType, FLevelData& OutLevelData) const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return false;
    }

    bool bFound = LevelDataInfos->GetFirstLevelOfTypeBlueprint(LevelType, OutLevelData);

    if (bFound)
    {
        UE_LOGFMT(LogLevelManager, Log, "{0} 타입의 첫 번째 레벨 '{1}'을 반환합니다.",
            *UEnum::GetValueAsString(LevelType), *OutLevelData.LevelName.ToString());
    }
    else
    {
        UE_LOGFMT(LogLevelManager, Warning, "{0} 타입의 활성화된 레벨을 찾지 못했습니다.",
            *UEnum::GetValueAsString(LevelType));
    }

    return bFound;
}

int32 ULevelManagerSubsystem::GetTotalLevelCount() const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return 0;
    }

    return LevelDataInfos->GetLevelCount();
}

int32 ULevelManagerSubsystem::GetLevelCountByType(const ELevelType& LevelType) const
{
    if (!LevelDataInfos)
    {
        UE_LOGFMT(LogLevelManager, Warning, "레벨 데이터 정보가 정상적으로 로드되지 않았습니다.");
        return 0;
    }

    return LevelDataInfos->GetLevelCountByType(LevelType);
}

bool ULevelManagerSubsystem::IsLevelDataValid() const
{
    //@데이터 에셋 로드 상태 및 유효성 확인
    if (!LevelDataInfos)
    {
        return false;
    }

    return LevelDataInfos->GetLevelCount() > 0;
}

bool ULevelManagerSubsystem::GetDefaultPlayerStartTag(const FGameplayTag& LevelTag, FGameplayTag& OutPlayerStartTag) const
{
    if (!LevelDataInfos || !LevelTag.IsValid())
    {
        return false;
    }

    FLevelData LevelData;
    if (LevelDataInfos->GetLevelByTag(LevelTag, LevelData))
    {
        OutPlayerStartTag = LevelData.DefaultPlayerStartTag;
        return LevelData.DefaultPlayerStartTag.IsValid();
    }

    return false;
}

bool ULevelManagerSubsystem::GetAvailablePlayerStartTags(const FGameplayTag& LevelTag, TArray<FGameplayTag>& OutPlayerStartTags) const
{
    if (!LevelDataInfos || !LevelTag.IsValid())
    {
        return false;
    }

    FLevelData LevelData;
    if (LevelDataInfos->GetLevelByTag(LevelTag, LevelData))
    {
        OutPlayerStartTags = LevelData.AvailablePlayerStartTags;
        return OutPlayerStartTags.Num() > 0;
    }

    return false;
}

FGameplayTag ULevelManagerSubsystem::GetDefaultPlayerStartTagForLevel(const FGameplayTag& LevelTag) const
{
    FGameplayTag OutTag;
    GetDefaultPlayerStartTag(LevelTag, OutTag);
    return OutTag;
}

TArray<FGameplayTag> ULevelManagerSubsystem::GetAvailablePlayerStartTagsForLevel(const FGameplayTag& LevelTag) const
{
    TArray<FGameplayTag> OutTags;
    GetAvailablePlayerStartTags(LevelTag, OutTags);
    return OutTags;
}

#pragma endregion