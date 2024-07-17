#include "UIManagerSubsystem.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogUIManager)
// UE_LOGFMT(LogUIManager, Log, "");

UUIManagerSubsystem::UUIManagerSubsystem()
{}

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
    case EUICategory::System:
        return &UICollection->GetUICategoryInformations(EUICategory::System);
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

const FUIInformation* UUIManagerSubsystem::GetUIInformaitonByInputTag(const FGameplayTag& InputTag) const
{
    if (!InputTag.IsValid())
    {
        UE_LOGFMT(LogUIManager, Error, "GetUIInformaitonByInputTag: 유효하지 않은 InputTag입니다.");
        return nullptr;
    }

    FString InputTagString = InputTag.ToString();
    EUICategory Category = EUICategory::MAX;

    // Input Tag로부터 UI 카테고리 추론
    if (InputTagString.StartsWith("Input.UI.HUD"))
    {
        Category = EUICategory::HUD;
    }
    else if (InputTagString.StartsWith("Input.UI.System"))
    {
        Category = EUICategory::System;
    }
    else if (InputTagString.StartsWith("Input.UI.Interaction"))
    {
        Category = EUICategory::Interaction;
    }
    else
    {
        UE_LOGFMT(LogUIManager, Warning, "GetUIInformaitonByInputTag: 알 수 없는 UI 카테고리입니다. InputTag: {0}", InputTagString);
        return nullptr;
    }

    const TArray<FUIInformation>* CategoryInfos = GetUICategoryInformations(Category);
    if (!CategoryInfos)
    {
        UE_LOGFMT(LogUIManager, Error, "GetUIInformaitonByInputTag: 해당 카테고리의 UI 정보를 찾을 수 없습니다. Category: {0}", static_cast<uint8>(Category));
        return nullptr;
    }

    // 해당 카테고리 내에서 InputTag와 일치하는 UI 정보 찾기
    for (const FUIInformation& UIInfo : *CategoryInfos)
    {
        if (UIInfo.bInputBinded && UIInfo.InputTags.Contains(InputTag))
        {
            return &UIInfo;
        }
    }

    UE_LOGFMT(LogUIManager, Warning, "GetUIInformaitonByInputTag: 해당 InputTag에 대응하는 UI 정보를 찾을 수 없습니다. InputTag: {0}", InputTagString);
    return nullptr;
}