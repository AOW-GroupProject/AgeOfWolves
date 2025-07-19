#include "UIManagerSubsystem.h"
#include "Logging/StructuredLog.h"

#include "17_GameMode/AOWGameState.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogUIManager)
// UE_LOGFMT(LogUIManager, Log, "");

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
    AOWGameState->RequestLoadingUIRender.BindUFunction(this, "OnRequestLoadingUIRender");

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
}

void UUIManagerSubsystem::OnWorldBeginPlay()
{
    //@외부 바인딩...
    ExternalBindinToGameState();
}

void UUIManagerSubsystem::OnRequestLoadingUIRender()
{
    //@로딩 UI 렌더링 시작
    UE_LOGFMT(LogUIManager, Log, "로딩 UI 렌더링 요청 받음");

    //@Fade-In 완료 이벤트 호출 (임시로 즉시 호출)
    OnLoadingUIFadeInComplete.Broadcast();
    UE_LOGFMT(LogUIManager, Log, "로딩 UI Fade-In 완료 이벤트 호출");
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