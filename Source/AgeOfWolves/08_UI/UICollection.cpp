
#include "08_UI/UICollection.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogUICollection)
// UE_LOGFMT(LogUICollection, Log, "");

UUICollection::UUICollection(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    HUDUIInformations.Empty();
    SystemUIInformations.Empty();
    InteractionUIInformations.Empty();
}

const TArray<FUIInformation>& UUICollection::GetUICategoryInformations(const EUICategory& UICategory) const
{
    switch (UICategory)
    {
    case EUICategory::HUD:
        return HUDUIInformations;
    case EUICategory::System:
        return SystemUIInformations;
    case EUICategory::Interaction:
        return InteractionUIInformations;
    default:
        UE_LOGFMT(LogUICollection, Warning, "유효하지 않은 UI Category 입니다!");
        static const TArray<FUIInformation> EmptyArray;
        return EmptyArray;
    }
}