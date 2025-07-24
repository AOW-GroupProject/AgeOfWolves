#include "UIManagerSubsystem.h"
#include "Logging/StructuredLog.h"

#include "17_GameMode/AOWGameState.h"
#include "Blueprint/UserWidget.h"
#include "Components/Widget.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogUIManager)
// UE_LOGFMT(LogUIManager, Log, "");

//@Default Setting
#pragma region Default Settings
UUIManagerSubsystem::UUIManagerSubsystem()
{}

void UUIManagerSubsystem::ExternalBindinToGameState()
{
    //@AOW Game State
    auto GameState = UGameplayStatics::GetGameState(GetGameInstance());
    if (!GameState)
    {
        return;
    }

    auto AOWGameState = CastChecked<AAOWGameState>(GameState);
    
    //@외부 바인딩...
    AOWGameState->RequestShowLoadingUI.BindUFunction(this, "OnRequestShowLoadingUI");
}

void UUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    //@Load UICollection
    UICollection = LoadObject<UUICollection>(nullptr, TEXT("/Game/Blueprints/08_UI/01_AkaOni/UC_AkaOni"));
    if (UICollection)
    {
        UE_LOGFMT(LogUIManager, Log, "UI Collection 정보 로드 성공");
    }
    else
    {
        UE_LOGFMT(LogUIManager, Error, "UI Collection 로드 실패");
    }

    //@Begin Play
    if (UWorld* World = GetWorld())
    {
        WorldBeginPlayHandle = World->OnWorldBeginPlay.AddUObject(this, &UUIManagerSubsystem::OnWorldBeginPlay);
        UE_LOGFMT(LogUIManager, Log, "World BeginPlay 델리게이트에 바인딩 완료");
    }

    //@System UI 생성
    CreateAndCacheSystemUIs();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UUIManagerSubsystem::CreateAndCacheSystemUIs()
{
    //@UI Collection 유효성 검사
    if (!UICollection)
    {
        UE_LOGFMT(LogUIManager, Error, "UI Collection이 없어 System UI 생성을 건너뜁니다");
        return;
    }

    //@System UI 정보들 가져오기
    const TArray<FUIInformation>* SystemUIInfos = GetUICategoryInformations(EUICategory::System);
    if (!SystemUIInfos || SystemUIInfos->Num() == 0)
    {
        UE_LOGFMT(LogUIManager, Warning, "System UI 정보가 없습니다");
        return;
    }

    UE_LOGFMT(LogUIManager, Log, "System UI 생성 시작 - 총 {0}개의 UI를 처리합니다", SystemUIInfos->Num());

    //@각 System UI를 생성하고 캐싱
    int32 SuccessfullyCreated = 0;
    for (const FUIInformation& UIInfo : *SystemUIInfos)
    {
        //@UI 정보 유효성 검사
        if (!UIInfo.UIClass || !UIInfo.UITag.IsValid())
        {
            UE_LOGFMT(LogUIManager, Warning, "유효하지 않은 System UI 정보를 건너뜁니다: {0}",
                *UIInfo.UITag.ToString());
            continue;
        }

        //@이미 캐싱된 UI인지 확인 (중복 방지)
        if (CachedSystemUIs.Contains(UIInfo.UITag))
        {
            UE_LOGFMT(LogUIManager, Warning, "이미 캐싱된 System UI입니다: {0}",
                *UIInfo.UITag.ToString());
            continue;
        }

        //@UI 위젯 생성 및 초기 설정
        UUserWidget* CreatedWidget = CreateSystemUIWidget(UIInfo);
        if (CreatedWidget)
        {
            CachedSystemUIs.Add(UIInfo.UITag, CreatedWidget);
            SuccessfullyCreated++;

            UE_LOGFMT(LogUIManager, Log, "System UI 생성 및 캐싱 완료: {0}",
                *UIInfo.UITag.ToString());
        }
        else
        {
            UE_LOGFMT(LogUIManager, Error, "System UI 생성 실패: {0}",
                *UIInfo.UITag.ToString());
        }
    }

    UE_LOGFMT(LogUIManager, Log, "System UI 생성 완료 - {0}/{1}개 성공적으로 생성됨",
        SuccessfullyCreated, SystemUIInfos->Num());

    //@캐시 상태 검증
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
    //@캐시에서 UI 위젯 찾기
    TObjectPtr<UUserWidget>* FoundWidget = CachedSystemUIs.Find(UITag);
    if (!FoundWidget || !IsValid(*FoundWidget))
    {
        UE_LOGFMT(LogUIManager, Warning, "표시하려는 System UI를 찾을 수 없습니다: {0}",
            *UITag.ToString());
        return false;
    }

    UUserWidget* TargetWidget = *FoundWidget;

    //@이미 표시되고 있는지 확인
    if (TargetWidget->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
    {
        UE_LOGFMT(LogUIManager, Warning, "System UI가 이미 표시되고 있습니다: {0}",
            *UITag.ToString());
        return true;
    }

    //@UI를 Visible 상태로 변경
    TargetWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

    UE_LOGFMT(LogUIManager, Log, "System UI 표시 완료: {0}", *UITag.ToString());
    return true;
}

bool UUIManagerSubsystem::HideSystemUI(const FGameplayTag& UITag)
{
    //@캐시에서 UI 위젯 찾기
    TObjectPtr<UUserWidget>* FoundWidget = CachedSystemUIs.Find(UITag);
    if (!FoundWidget || !IsValid(*FoundWidget))
    {
        UE_LOGFMT(LogUIManager, Warning, "숨기려는 System UI를 찾을 수 없습니다: {0}",
            *UITag.ToString());
        return false;
    }

    UUserWidget* TargetWidget = *FoundWidget;

    //@이미 숨겨져 있는지 확인
    if (TargetWidget->GetVisibility() == ESlateVisibility::Collapsed)
    {
        UE_LOGFMT(LogUIManager, Warning, "System UI가 이미 숨겨져 있습니다: {0}",
            *UITag.ToString());
        return true;
    }

    //@UI를 Collapsed 상태로 변경 (렌더링에서 완전히 제외)
    TargetWidget->SetVisibility(ESlateVisibility::Collapsed);

    UE_LOGFMT(LogUIManager, Log, "System UI 숨김 완료: {0}", *UITag.ToString());
    return true;
}

void UUIManagerSubsystem::HideAllSystemUIs()
{
    UE_LOGFMT(LogUIManager, Log, "모든 System UI 숨김 시작 - 총 {0}개 처리",
        CachedSystemUIs.Num());

    int32 HiddenCount = 0;
    for (auto& UIPair : CachedSystemUIs)
    {
        if (IsValid(UIPair.Value))
        {
            UIPair.Value->SetVisibility(ESlateVisibility::Collapsed);
            HiddenCount++;
        }
    }

    UE_LOGFMT(LogUIManager, Log, "모든 System UI 숨김 완료 - {0}개 처리됨", HiddenCount);
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UUIManagerSubsystem::OnWorldBeginPlay()
{
    //@외부 바인딩...
    ExternalBindinToGameState();
}

void UUIManagerSubsystem::OnRequestShowLoadingUI()
{
    //@로딩 UI 렌더링 시작
    UE_LOGFMT(LogUIManager, Log, "로딩 UI 렌더링 요청 받음");

    //@로딩 UI 태그 생성
    FGameplayTag LoadingUITag = FGameplayTag::RequestGameplayTag(FName("UI.System.LoadingUI"));

    //@캐시에서 로딩 UI를 찾아서 활성화
    if (!ShowSystemUI(LoadingUITag))
    {
        UE_LOGFMT(LogUIManager, Error, "로딩 UI 활성화 실패 - UI를 찾을 수 없거나 이미 활성화됨");

        //@실패했어도 이벤트는 호출해서 게임 플로우가 멈추지 않도록 함
        LoadingUIFadeInComplete.Broadcast();
        UE_LOGFMT(LogUIManager, Warning, "로딩 UI 활성화 실패했지만 Fade-In 완료 이벤트 호출");
    }

    UE_LOGFMT(LogUIManager, Log, "로딩 UI 활성화 성공");

    //@Fade-In 완료 이벤트 호출 (실제 페이드 인 애니메이션이 없으므로 즉시 호출)
    LoadingUIFadeInComplete.Broadcast();
    UE_LOGFMT(LogUIManager, Log, "로딩 UI Fade-In 완료 이벤트 호출");

}
#pragma endregion

//@Utility
#pragma region Utility
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
    return (*FoundWidget)->GetVisibility() == ESlateVisibility::Visible;
}

const TArray<FUIInformation>* UUIManagerSubsystem::GetUICategoryInformations(const EUICategory& UICategory) const
{
    if (!UICollection)
    {
        UE_LOGFMT(LogUIManager, Warning, "UI Collection이 정상적으로 Load되지 않았습니다.");
        return nullptr;
    }

    switch (UICategory)
    {
    case EUICategory::HUD:
        return &UICollection->GetUICategoryInformations(EUICategory::HUD);
    case EUICategory::Menu:
        return &UICollection->GetUICategoryInformations(EUICategory::Menu);
    case EUICategory::Interaction:
        return &UICollection->GetUICategoryInformations(EUICategory::Interaction);
    default:
        UE_LOGFMT(LogUIManager, Warning, "유효하지 않은 UI Category입니다.");
        return nullptr;
    }
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
    UE_LOGFMT(LogUIManager, Warning, "{0} UI Category의 {1} UI를 찾지 못했습니다.",
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
#pragma endregion