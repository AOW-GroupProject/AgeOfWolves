#include "UIManagerSubsystem.h"
#include "Logging/StructuredLog.h"

#include "17_GameMode/AOWGameState.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

#include "08_UI/03_System/LoadingUI.h"

DEFINE_LOG_CATEGORY(LogUIManager)

//@Default Setting
#pragma region Default Settings
UUIManagerSubsystem::UUIManagerSubsystem()
    : Super()
{}

void UUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    UE_LOGFMT(LogUIManager, Log, "UIManagerSubsystem 초기화 시작 - 지연 로딩 모드");

    // UI Collection 데이터 에셋만 로드 (UI 생성은 필요시에 수행)
    UICollection = LoadObject<UUICollection>(nullptr, TEXT("/Game/Blueprints/08_UI/01_AkaOni/UC_AkaOni"));
    if (UICollection)
    {
        UE_LOGFMT(LogUIManager, Log, "UI Collection 데이터 에셋 로드 성공 - {0}개 System UI 정보 확인됨",
            UICollection->GetUICategoryInformations(EUICategory::System).Num());
    }
    else
    {
        UE_LOGFMT(LogUIManager, Error, "UI Collection 데이터 에셋 로드 실패 - System UI 생성 불가");
    }

    // UI 생성은 실제 요청 시점까지 연기
    UE_LOGFMT(LogUIManager, Log, "UIManagerSubsystem 초기화 완료 - UI는 필요시 생성됨");
}

void UUIManagerSubsystem::Deinitialize()
{
    // 모든 미완료 Promise들을 완료 상태로 만들어줍니다
    for (auto& Pair : UIMinimumTimePromises)
    {
        if (Pair.Value.IsValid())
        {
            TSharedPtr<TPromise<void>> Promise = Pair.Value;
            if (Promise.IsValid())
            {
                UE_LOGFMT(LogUIManager, Warning, "강제로 미완료 UI 작업을 정리합니다: {0}",
                    *Pair.Key.ToString());

                Promise->SetValue();
            }
        }
    }

    // 모든 맵을 완전히 비워줍니다
    UIMinimumTimePromises.Empty();
    UIMinimumTimeFutures.Empty(); // 새로 추가

    Super::Deinitialize();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UUIManagerSubsystem::CreateAndCacheSystemUIs()
{
    UE_LOGFMT(LogUIManager, Log, "System UI 일괄 생성 시작 (선택적 기능)");

    // UI Collection 유효성 검사
    if (!UICollection)
    {
        UE_LOGFMT(LogUIManager, Error, "UI Collection이 없어 System UI 생성을 건너뜁니다");
        return;
    }

    // 전제 조건 검사 (지연 초기화와 동일한 로직)
    UWorld* World = GetWorld();
    if (!World || !World->GetFirstPlayerController())
    {
        UE_LOGFMT(LogUIManager, Warning, "PlayerController가 준비되지 않아 UI 생성을 건너뜁니다");
        return;
    }

    // System UI 정보들 가져오기
    const TArray<FUIInformation>* SystemUIInfos = GetUICategoryInformations(EUICategory::System);
    if (!SystemUIInfos || SystemUIInfos->Num() == 0)
    {
        UE_LOGFMT(LogUIManager, Warning, "System UI 정보가 없습니다");
        return;
    }

    UE_LOGFMT(LogUIManager, Log, "System UI 일괄 생성 - 총 {0}개의 UI를 처리합니다", SystemUIInfos->Num());

    // 각 System UI를 개별적으로 생성 (지연 초기화 로직 재사용)
    int32 SuccessfullyCreated = 0;
    int32 AlreadyExists = 0;

    for (const FUIInformation& UIInfo : *SystemUIInfos)
    {
        if (!UIInfo.UIClass || !UIInfo.UITag.IsValid())
        {
            UE_LOGFMT(LogUIManager, Warning, "유효하지 않은 System UI 정보를 건너뜁니다: {0}", *UIInfo.UITag.ToString());
            continue;
        }

        // 이미 존재하는 UI는 건너뛰기
        if (CachedSystemUIs.Contains(UIInfo.UITag))
        {
            AlreadyExists++;
            continue;
        }

        // 개별 UI 생성 (지연 초기화 로직 재사용)
        if (EnsureUICreatedOnDemand(UIInfo.UITag))
        {
            SuccessfullyCreated++;
        }
    }

    UE_LOGFMT(LogUIManager, Log, "System UI 일괄 생성 완료 - 신규 생성: {0}개, 기존 존재: {1}개",
        SuccessfullyCreated, AlreadyExists);

    // 캐시 상태 검증
    if (!ValidateSystemUICache())
    {
        UE_LOGFMT(LogUIManager, Warning, "System UI 캐시 무결성 검사에서 문제가 발견되었습니다");
    }
}

UUserWidget* UUIManagerSubsystem::CreateSystemUIWidget(const FUIInformation& UIInfo)
{
    //@UI 클래스 유효성 재확인
    if (!UIInfo.UIClass)
    {
        UE_LOGFMT(LogUIManager, Error, "UI 클래스가 null입니다: {0}", *UIInfo.UITag.ToString());
        return nullptr;
    }

    //@World 및 PlayerController 가져오기
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogUIManager, Error, "World가 유효하지 않습니다");
        return nullptr;
    }

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogUIManager, Error, "PlayerController를 찾을 수 없습니다");
        return nullptr;
    }

    //@UI 위젯 생성
    UUserWidget* CreatedWidget = CreateWidget<UUserWidget>(PC, UIInfo.UIClass);
    if (!CreatedWidget)
    {
        UE_LOGFMT(LogUIManager, Error, "UI 위젯 생성 실패: {0}", *UIInfo.UITag.ToString());
        return nullptr;
    }

    //@Viewport에 추가 (Z-Order는 기본값 사용)
    CreatedWidget->AddToViewport();

    //@초기 가시성을 Collapsed로 설정 (렌더링 파이프라인에서 완전히 제외)
    CreatedWidget->SetVisibility(ESlateVisibility::Collapsed);

    UE_LOGFMT(LogUIManager, Log, "UI 위젯이 Viewport에 추가되고 Collapsed 상태로 설정됨: {0}",
        *UIInfo.UITag.ToString());

    return CreatedWidget;
}

bool UUIManagerSubsystem::ShowSystemUI(const FGameplayTag& UITag)
{
    UE_LOGFMT(LogUIManager, Log, "System UI 표시 요청: {0}", *UITag.ToString());

    // 첫 번째 단계: 이미 생성된 UI가 있는지 확인
    TObjectPtr<UUserWidget>* FoundWidget = CachedSystemUIs.Find(UITag);

    if (!FoundWidget || !IsValid(*FoundWidget))
    {
        // 두 번째 단계: UI가 없으면 지금 생성 시도
        UE_LOGFMT(LogUIManager, Log, "UI가 존재하지 않음 - 지연 생성 시도: {0}", *UITag.ToString());

        if (!EnsureUICreatedOnDemand(UITag))
        {
            UE_LOGFMT(LogUIManager, Warning, "UI 지연 생성 실패: {0}", *UITag.ToString());
            return false;
        }

        // 생성 후 다시 검색
        FoundWidget = CachedSystemUIs.Find(UITag);

        if (!FoundWidget || !IsValid(*FoundWidget))
        {
            UE_LOGFMT(LogUIManager, Error, "UI 생성 후에도 찾을 수 없음: {0}", *UITag.ToString());
            return false;
        }

        UE_LOGFMT(LogUIManager, Log, "UI 지연 생성 성공: {0}", *UITag.ToString());
    }

    // 세 번째 단계: LoadingUI 특별 처리 (OnRequestShowLoadingUI 로직 통합)
    bool bIsLoadingUI = (UITag == FGameplayTag::RequestGameplayTag("UI.System.LoadingUI"));
    if (bIsLoadingUI)
    {
        // LoadingUI 전용 로직: 최소 표시 시간 설정
        const FUIInformation* UIInfo = GetUIInformation(EUICategory::System, UITag);
        if (UIInfo)
        {
            SetupMinimumDisplayTimeForUI(UITag, *UIInfo);
            UE_LOGFMT(LogUIManager, Log, "LoadingUI 최소 표시 시간 설정 완료");
        }
        else
        {
            UE_LOGFMT(LogUIManager, Warning, "LoadingUI 정보를 찾을 수 없음 - 기본 동작으로 진행");
        }
    }

    // 네 번째 단계: UI 표시 (기존 로직과 동일)
    UUserWidget* TargetWidget = *FoundWidget;

    // LoadingUI 특별 처리 로직
    if (ULoadingUI* LoadingUIWidget = Cast<ULoadingUI>(TargetWidget))
    {
        if (LoadingUIWidget->IsLoadingUIVisible())
        {
            UE_LOGFMT(LogUIManager, Warning, "LoadingUI가 이미 표시되고 있습니다: {0}", *UITag.ToString());

            // 이미 표시된 경우에도 LoadingUI라면 이벤트 브로드캐스트 스케줄링
            if (bIsLoadingUI)
            {
                ScheduleLoadingUIShownEvent();
            }
            return true;
        }

        LoadingUIWidget->SetLoadingUIVisibility(ESlateVisibility::SelfHitTestInvisible);
        UE_LOGFMT(LogUIManager, Log, "LoadingUI 전용 인터페이스로 표시 완료: {0}", *UITag.ToString());
    }
    else
    {
        if (TargetWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
        {
            UE_LOGFMT(LogUIManager, Warning, "System UI가 이미 표시되고 있습니다: {0}", *UITag.ToString());

            // 이미 표시된 경우에도 LoadingUI라면 이벤트 브로드캐스트 스케줄링
            if (bIsLoadingUI)
            {
                ScheduleLoadingUIShownEvent();
            }
            return true;
        }

        TargetWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        UE_LOGFMT(LogUIManager, Log, "일반 위젯으로 표시 완료: {0}", *UITag.ToString());
    }

    // 다섯 번째 단계: LoadingUI 전용 이벤트 브로드캐스트 스케줄링
    if (bIsLoadingUI)
    {
        ScheduleLoadingUIShownEvent();
        UE_LOGFMT(LogUIManager, Log, "LoadingUI 표시 완료 이벤트 스케줄링됨");
    }

    return true;
}

bool UUIManagerSubsystem::HideSystemUI(const FGameplayTag& UITag)
{
    UE_LOGFMT(LogUIManager, Log, "System UI 숨김 요청: {0}", *UITag.ToString());

    // 첫 번째 단계: LoadingUI 특별 처리 (OnRequestHideLoadingUI 로직 통합)
    bool bIsLoadingUI = (UITag == FGameplayTag::RequestGameplayTag("UI.System.LoadingUI"));
    if (bIsLoadingUI)
    {
        // LoadingUI 전용 로직: 최소 표시 시간 체크
        if (!CanHideUIImmediately(UITag))
        {
            // 최소 시간이 경과하지 않았으므로 숨기기 요청을 대기 상태로 설정
            if (FMinimumDisplayTimeInfo* TimeInfo = MinimumDisplayTimeInfoMap.Find(UITag))
            {
                TimeInfo->bHasPendingHideRequest = true;
                UE_LOGFMT(LogUIManager, Log, "LoadingUI 최소 표시 시간이 경과하지 않아 숨기기 요청을 pending으로 설정 - 추적 정보: {0}",
                    *TimeInfo->ToString());

                UE_LOGFMT(LogUIManager, Log, "Promise 완료시 자동 처리 예정");
                return true; // pending 상태도 성공으로 간주
            }
            else
            {
                UE_LOGFMT(LogUIManager, Warning, "LoadingUI 시간 추적 정보를 찾을 수 없음 - 즉시 숨기기로 진행");
            }
        }
    }

    // 두 번째 단계: 캐시에서 UI 위젯 찾기
    TObjectPtr<UUserWidget>* FoundWidget = CachedSystemUIs.Find(UITag);
    if (!FoundWidget || !IsValid(*FoundWidget))
    {
        UE_LOGFMT(LogUIManager, Warning, "숨기려는 System UI를 찾을 수 없습니다: {0}", *UITag.ToString());

        // LoadingUI의 경우 실패해도 이벤트 브로드캐스트
        if (bIsLoadingUI)
        {
            LoadingUIHidden.Broadcast();
            UE_LOGFMT(LogUIManager, Warning, "LoadingUI 숨김 실패했지만 이벤트는 브로드캐스트됨");
        }
        return false;
    }

    // 세 번째 단계: UI 숨김 처리
    UUserWidget* TargetWidget = *FoundWidget;

    // LoadingUI인지 확인하고 타입에 맞는 처리를 수행
    if (ULoadingUI* LoadingUIWidget = Cast<ULoadingUI>(TargetWidget))
    {
        // LoadingUI의 경우: 전용 인터페이스를 사용
        if (LoadingUIWidget->IsLoadingUIHidden())
        {
            UE_LOGFMT(LogUIManager, Warning, "LoadingUI가 이미 숨겨져 있습니다: {0}", *UITag.ToString());

            // 이미 숨겨진 경우에도 LoadingUI라면 이벤트 브로드캐스트
            if (bIsLoadingUI)
            {
                LoadingUIHidden.Broadcast();
            }
            return true;
        }

        LoadingUIWidget->SetLoadingUIVisibility(ESlateVisibility::Collapsed);
        UE_LOGFMT(LogUIManager, Log, "LoadingUI 전용 인터페이스로 숨김 완료: {0}", *UITag.ToString());
    }
    else
    {
        // 일반 위젯의 경우: 기존 방식을 유지
        if (TargetWidget->GetVisibility() == ESlateVisibility::Collapsed)
        {
            UE_LOGFMT(LogUIManager, Warning, "System UI가 이미 숨겨져 있습니다: {0}", *UITag.ToString());

            // 이미 숨겨진 경우에도 LoadingUI라면 이벤트 브로드캐스트
            if (bIsLoadingUI)
            {
                LoadingUIHidden.Broadcast();
            }
            return true;
        }

        TargetWidget->SetVisibility(ESlateVisibility::Collapsed);
        UE_LOGFMT(LogUIManager, Log, "일반 위젯으로 숨김 완료: {0}", *UITag.ToString());
    }

    // 네 번째 단계: LoadingUI 전용 후처리
    if (bIsLoadingUI)
    {
        // 시간 추적 정보 정리
        CleanupUITimeTrackingInfo(UITag);

        // LoadingUI 숨김 완료 이벤트 브로드캐스트
        LoadingUIHidden.Broadcast();
        UE_LOGFMT(LogUIManager, Log, "LoadingUI 숨김 완료 이벤트 브로드캐스트 완료");
    }

    return true;
}

void UUIManagerSubsystem::HideAllSystemUIs()
{
    UE_LOGFMT(LogUIManager, Log, "모든 System UI 숨김 시작 - 총 {0}개 처리",
        CachedSystemUIs.Num());

    int32 HiddenCount = 0;
    int32 LoadingUICount = 0;

    for (auto& UIPair : CachedSystemUIs)
    {
        if (IsValid(UIPair.Value))
        {
            // 각 위젯의 타입에 맞는 처리를 수행합니다
            if (ULoadingUI* LoadingUIWidget = Cast<ULoadingUI>(UIPair.Value))
            {
                // LoadingUI의 경우: 전용 인터페이스 사용
                LoadingUIWidget->SetLoadingUIVisibility(ESlateVisibility::Collapsed);
                LoadingUICount++;
                UE_LOGFMT(LogUIManager, Verbose, "LoadingUI 전용 방식으로 숨김: {0}",
                    *UIPair.Key.ToString());
            }
            else
            {
                // 일반 위젯의 경우: 기존 방식 사용
                UIPair.Value->SetVisibility(ESlateVisibility::Collapsed);
            }
            HiddenCount++;
        }
    }

    // 모든 최소 표시 시간 추적 정보도 정리
    MinimumDisplayTimeInfoMap.Empty();
    UIMinimumTimePromises.Empty();

    UE_LOGFMT(LogUIManager, Log,
        "모든 System UI 숨김 완료 - 총 {0}개 처리됨 (LoadingUI: {1}개, 일반: {2}개), 시간 추적 정보 정리됨",
        HiddenCount, LoadingUICount, HiddenCount - LoadingUICount);
}

void UUIManagerSubsystem::ScheduleLoadingUIShownEvent()
{
    // 기존에 스케줄된 타이머가 있다면 정리 (중복 방지)
    if (UWorld* World = GetWorld())
    {
        static FTimerHandle LoadingUIShownTimerHandle;

        // 기존 타이머 클리어 (중복 스케줄링 방지)
        if (LoadingUIShownTimerHandle.IsValid())
        {
            World->GetTimerManager().ClearTimer(LoadingUIShownTimerHandle);
        }

        // 3초 후 LoadingUIShown 이벤트 브로드캐스트
        World->GetTimerManager().SetTimer(
            LoadingUIShownTimerHandle,
            [this]()
            {
                LoadingUIShown.Broadcast();
                UE_LOGFMT(LogUIManager, Log, "LoadingUI 표시 완료 이벤트 브로드캐스트 완료");
            },
            3.0f,
            false
        );

        UE_LOGFMT(LogUIManager, Log, "LoadingUI 표시 완료 이벤트 3초 후 브로드캐스트 예약됨");
    }
}

void UUIManagerSubsystem::SetupMinimumDisplayTimeForUI(const FGameplayTag& UITag, const FUIInformation& UIInfo)
{
    UE_LOGFMT(LogUIManager, Log, "UI '{0}'의 최소 표시 시간 설정 시작", *UITag.ToString());

    // 최소 표시 시간이 설정되지 않은 경우 기존 정보 정리하고 종료
    if (!UIInfo.bUseMinimumDisplayTime)
    {
        CleanupUITimeTrackingInfo(UITag);
        UE_LOGFMT(LogUIManager, Log, "UI '{0}'는 최소 표시 시간을 사용하지 않음", *UITag.ToString());
        return;
    }

    // 기존 Promise/Future 정리 (재시작의 경우)
    CleanupUITimeTrackingInfo(UITag);

    UE_LOGFMT(LogUIManager, Log, "UI '{0}'의 최소 표시 시간 설정: {1}초",
        *UITag.ToString(), UIInfo.MinimumDisplayTimeSeconds);

    // 최소 표시 시간 정보 설정
    FMinimumDisplayTimeInfo& TimeInfo = MinimumDisplayTimeInfoMap.FindOrAdd(UITag);
    TimeInfo.bIsActive = true;
    TimeInfo.MinimumTimeSeconds = UIInfo.MinimumDisplayTimeSeconds;
    TimeInfo.ShowStartTime = GetWorld()->GetTimeSeconds();
    TimeInfo.bHasPendingHideRequest = false;

    // Promise 생성 및 저장
    TSharedPtr<TPromise<void>> Promise = MakeShared<TPromise<void>>();
    UIMinimumTimePromises.Add(UITag, Promise);

    // Future를 한 번만 가져와서 별도로 저장 (핵심 수정 사항)
    TSharedPtr<TFuture<void>> Future = MakeShared<TFuture<void>>(Promise->GetFuture());
    UIMinimumTimeFutures.Add(UITag, Future);

    UE_LOGFMT(LogUIManager, Log, "Promise와 Future가 안전하게 생성되어 별도 저장됨: {0}", *UITag.ToString());

    // 설정된 시간 후에 Promise를 완료하는 Timer 설정
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, UITag, Promise]()
        {
            UE_LOGFMT(LogUIManager, Log, "UI '{0}'의 최소 표시 시간 경과 - Promise 완료 시그널 발송", *UITag.ToString());

            // Promise 완료 - 이제 이 UI는 숨길 수 있습니다
            Promise->SetValue();

            // 만약 대기 중인 숨기기 요청이 있다면 즉시 처리
            ProcessPendingHideRequestIfExists(UITag);

        }, UIInfo.MinimumDisplayTimeSeconds, false);

    UE_LOGFMT(LogUIManager, Log, "최소 표시 시간 추적 시작 - {0}초 후 자동 확인 예약됨, 추적 정보: {1}",
        UIInfo.MinimumDisplayTimeSeconds, *TimeInfo.ToString());
}

void UUIManagerSubsystem::ProcessPendingHideRequestIfExists(const FGameplayTag& UITag)
{
    UE_LOGFMT(LogUIManager, Log, "대기 중인 숨기기 요청 확인 시작: {0}", *UITag.ToString());

    //@최소 표시 시간 정보 확인
    FMinimumDisplayTimeInfo* TimeInfo = MinimumDisplayTimeInfoMap.Find(UITag);
    if (!TimeInfo)
    {
        UE_LOGFMT(LogUIManager, Log, "시간 추적 정보가 없음 - 확인 종료: {0}", *UITag.ToString());
        return;
    }

    //@대기 중인 요청이 있는지 확인
    if (!TimeInfo->bHasPendingHideRequest)
    {
        UE_LOGFMT(LogUIManager, Log, "대기 중인 숨기기 요청이 없음: {0}", *UITag.ToString());
        return;
    }

    UE_LOGFMT(LogUIManager, Log, "대기 중인 숨기기 요청 발견 - 즉시 처리 시작: {0}", *UITag.ToString());

    //@UI 숨기기 실행
    if (HideSystemUI(UITag))
    {
        UE_LOGFMT(LogUIManager, Log, "대기 중인 숨기기 요청 성공적으로 처리됨: {0}", *UITag.ToString());

        //@추적 정보 정리
        CleanupUITimeTrackingInfo(UITag);

        //@특별 처리: 로딩 UI인 경우 이벤트 브로드캐스트
        if (UITag == FGameplayTag::RequestGameplayTag("UI.System.LoadingUI"))
        {
            LoadingUIHidden.Broadcast();
            UE_LOGFMT(LogUIManager, Log, "대기된 로딩 UI 숨김 이벤트 브로드캐스트 완료");
        }
    }
    else
    {
        UE_LOGFMT(LogUIManager, Error, "대기 중인 숨기기 요청 처리 실패: {0}", *UITag.ToString());
    }
}

void UUIManagerSubsystem::CleanupUITimeTrackingInfo(const FGameplayTag& UITag)
{
    // UI 관련 모든 시간 추적 정보를 정리하는 함수
    bool bHadTimeInfo = MinimumDisplayTimeInfoMap.Remove(UITag) > 0;
    bool bHadPromise = UIMinimumTimePromises.Remove(UITag) > 0;
    bool bHadFuture = UIMinimumTimeFutures.Remove(UITag) > 0; // 새로 추가

    if (bHadTimeInfo || bHadPromise || bHadFuture)
    {
        UE_LOGFMT(LogUIManager, VeryVerbose, "UI '{0}'의 시간 추적 정보 정리 완료 (TimeInfo: {1}, Promise: {2}, Future: {3})",
            *UITag.ToString(),
            bHadTimeInfo ? TEXT("제거됨") : TEXT("없음"),
            bHadPromise ? TEXT("제거됨") : TEXT("없음"),
            bHadFuture ? TEXT("제거됨") : TEXT("없음"));
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UUIManagerSubsystem::OnWorldBeginPlay()
{
    UE_LOGFMT(LogUIManager, Log, "World BeginPlay 콜백 호출됨");

    //@GameState와의 외부 바인딩 수행

    //ExternalBindinToGameState();


    UE_LOGFMT(LogUIManager, Log, "World BeginPlay 처리 완료");
}
#pragma endregion

//@Utility
#pragma region Utility
bool UUIManagerSubsystem::EnsureUICreatedOnDemand(const FGameplayTag& UITag)
{
    // 전제 조건 검사: UI Collection이 로드되어 있는가?
    if (!UICollection)
    {
        UE_LOGFMT(LogUIManager, Error, "UI Collection이 로드되지 않아 UI 생성 불가: {0}", *UITag.ToString());
        return false;
    }

    // 전제 조건 검사: PlayerController가 준비되어 있는가?
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogUIManager, Warning, "World가 유효하지 않음 - UI 생성 연기: {0}", *UITag.ToString());
        return false;
    }

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogUIManager, Warning, "PlayerController가 아직 준비되지 않음 - UI 생성 연기: {0}", *UITag.ToString());
        return false;
    }

    // 중복 생성 방지: 이미 생성 중이거나 생성된 UI인가?
    if (CachedSystemUIs.Contains(UITag))
    {
        UE_LOGFMT(LogUIManager, VeryVerbose, "UI가 이미 캐시에 존재함: {0}", *UITag.ToString());
        return true;
    }

    // UI 정보 검색
    const FUIInformation* UIInfo = GetUIInformation(EUICategory::System, UITag);
    if (!UIInfo)
    {
        UE_LOGFMT(LogUIManager, Error, "UI 정보를 찾을 수 없음: {0}", *UITag.ToString());
        return false;
    }

    // 실제 UI 생성 (기존 CreateSystemUIWidget 함수 재활용)
    UUserWidget* CreatedWidget = CreateSystemUIWidget(*UIInfo);
    if (!CreatedWidget)
    {
        UE_LOGFMT(LogUIManager, Error, "UI 위젯 생성 실패: {0}", *UITag.ToString());
        return false;
    }

    // 캐시에 추가
    CachedSystemUIs.Add(UITag, CreatedWidget);

    UE_LOGFMT(LogUIManager, Log, "UI 지연 생성 및 캐싱 완료: {0}", *UITag.ToString());
    return true;
}

bool UUIManagerSubsystem::CanCreateUICurrently() const
{
    // UI 생성에 필요한 모든 조건이 충족되었는지 확인
    if (!UICollection)
    {
        return false;
    }

    UWorld* World = GetWorld();
    if (!World || !World->GetFirstPlayerController())
    {
        return false;
    }

    return true;
}

bool UUIManagerSubsystem::CanHideUIImmediately(const FGameplayTag& UITag) const
{
    // Promise가 아닌 Future 맵에서 검색 (핵심 변경사항)
    const TSharedPtr<TFuture<void>>* FoundFuture = UIMinimumTimeFutures.Find(UITag);

    if (!FoundFuture || !FoundFuture->IsValid())
    {
        // Future가 없거나 무효한 경우 즉시 숨기기 가능
        UE_LOGFMT(LogUIManager, VeryVerbose, "UI '{0}': Future 없음 - 즉시 숨기기 가능", *UITag.ToString());
        return true;
    }

    // 이미 저장된 Future를 사용하여 상태 확인 (GetFuture() 재호출 없음)
    bool bCanHide = (*FoundFuture)->IsReady();

    // 추가 정보를 위한 시간 정보 확인
    if (const FMinimumDisplayTimeInfo* TimeInfo = MinimumDisplayTimeInfoMap.Find(UITag))
    {
        double ElapsedTime = GetWorld()->GetTimeSeconds() - TimeInfo->ShowStartTime;
        UE_LOGFMT(LogUIManager, VeryVerbose, "UI '{0}' 즉시 숨기기 가능 여부: {1} (경과 시간: {2:.2f}초, 최소 시간: {3}초)",
            *UITag.ToString(),
            bCanHide ? TEXT("가능") : TEXT("불가능"),
            ElapsedTime,
            TimeInfo->MinimumTimeSeconds);
    }

    return bCanHide;
}

bool UUIManagerSubsystem::IsUICreated(const FGameplayTag& UITag) const
{
    const TObjectPtr<UUserWidget>* FoundWidget = CachedSystemUIs.Find(UITag);
    return FoundWidget && IsValid(*FoundWidget);
}

bool UUIManagerSubsystem::ValidateSystemUICache() const
{
    //@캐시 무결성 검사
    bool bIsValid = true;
    int32 InvalidWidgets = 0;

    for (const auto& UIPair : CachedSystemUIs)
    {
        if (!IsValid(UIPair.Value))
        {
            UE_LOGFMT(LogUIManager, Error, "무효한 위젯이 캐시에 발견됨: {0}",
                *UIPair.Key.ToString());
            InvalidWidgets++;
            bIsValid = false;
        }
    }

    if (InvalidWidgets > 0)
    {
        UE_LOGFMT(LogUIManager, Error, "System UI 캐시 무결성 검사 실패: {0}개의 무효한 위젯 발견",
            InvalidWidgets);
    }
    else if (CachedSystemUIs.Num() > 0)
    {
        UE_LOGFMT(LogUIManager, Log, "System UI 캐시 무결성 검사 통과: {0}개 위젯 모두 유효",
            CachedSystemUIs.Num());
    }

    return bIsValid;
}

bool UUIManagerSubsystem::IsSystemUIVisible(const FGameplayTag& UITag) const
{
    //@캐시에서 UI 위젯 찾기
    const TObjectPtr<UUserWidget>* FoundWidget = CachedSystemUIs.Find(UITag);
    if (!FoundWidget || !IsValid(*FoundWidget))
    {
        return false;
    }

    //@Visible 상태인지 확인
    return (*FoundWidget)->GetVisibility() == ESlateVisibility::SelfHitTestInvisible;
}

const TArray<FUIInformation>* UUIManagerSubsystem::GetUICategoryInformations(const EUICategory& UICategory) const
{
    if (!UICollection)
    {
        UE_LOGFMT(LogUIManager, Warning, "UI Collection이 정상적으로 로드되지 않았습니다.");
        return nullptr;
    }

    return &UICollection->GetUICategoryInformations(UICategory);
}

const FUIInformation* UUIManagerSubsystem::GetUIInformation(const EUICategory& UICategory, const FGameplayTag& UITag) const
{
    const TArray<FUIInformation>* CategoryInformations = GetUICategoryInformations(UICategory);
    if (CategoryInformations)
    {
        for (const FUIInformation& UIInfo : *CategoryInformations)
        {
            if (UIInfo.UITag == UITag)
            {
                return &UIInfo;
            }
        }
    }
    UE_LOGFMT(LogUIManager, VeryVerbose, "{0} UI Category의 {1} UI를 찾지 못했습니다.",
        static_cast<int32>(UICategory), *UITag.ToString());
    return nullptr;
}

UUserWidget* UUIManagerSubsystem::GetCachedSystemUI(const FGameplayTag& UITag) const
{
    //@캐시에서 UI 위젯 찾기
    const TObjectPtr<UUserWidget>* FoundWidget = CachedSystemUIs.Find(UITag);
    if (!FoundWidget || !IsValid(*FoundWidget))
    {
        UE_LOGFMT(LogUIManager, Error, "요청한 System UI를 찾을 수 없습니다: {0}",
            *UITag.ToString());
        return nullptr;
    }

    return *FoundWidget;
}

void UUIManagerSubsystem::PrintMinimumDisplayTimeDebugInfo() const
{
    UE_LOGFMT(LogUIManager, Log, "=== 최소 표시 시간 디버그 정보 ===");
    UE_LOGFMT(LogUIManager, Log, "추적 중인 UI 개수: {0}", MinimumDisplayTimeInfoMap.Num());
    UE_LOGFMT(LogUIManager, Log, "활성 Promise 개수: {0}", UIMinimumTimePromises.Num());
    UE_LOGFMT(LogUIManager, Log, "활성 Future 개수: {0}", UIMinimumTimeFutures.Num()); // 새로 추가

    double CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0;

    for (const auto& TimeInfoPair : MinimumDisplayTimeInfoMap)
    {
        const FGameplayTag& UITag = TimeInfoPair.Key;
        const FMinimumDisplayTimeInfo& TimeInfo = TimeInfoPair.Value;

        bool bHasPromise = UIMinimumTimePromises.Contains(UITag);
        bool bHasFuture = UIMinimumTimeFutures.Contains(UITag);
        bool bFutureReady = false;

        if (bHasFuture)
        {
            if (const TSharedPtr<TFuture<void>>* FoundFuture = UIMinimumTimeFutures.Find(UITag))
            {
                if (FoundFuture->IsValid())
                {
                    bFutureReady = (*FoundFuture)->IsReady();
                }
            }
        }

        UE_LOGFMT(LogUIManager, Log, "UI: {0} | {1} | Promise존재: {2} | Future존재: {3} | Future완료: {4}",
            *UITag.ToString(),
            *TimeInfo.ToString(),
            bHasPromise ? TEXT("Yes") : TEXT("No"),
            bHasFuture ? TEXT("Yes") : TEXT("No"),
            bFutureReady ? TEXT("Yes") : TEXT("No"));
    }

    UE_LOGFMT(LogUIManager, Log, "=== 디버그 정보 끝 ===");
}
#pragma endregion