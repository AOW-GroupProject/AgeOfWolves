#include "InventoryUI.h"
#include "Logging/StructuredLog.h"

#include "04_Component/InventoryComponent.h"
#include "04_Component/BaseInputComponent.h"

DEFINE_LOG_CATEGORY(LogInventoryUI)

#pragma region Default Setting
UInventoryUI::UInventoryUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UInventoryUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    ExternalBindingToInventoryComp();
    ExternalBindingToInputComp();
}

void UInventoryUI::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UInventoryUI::NativeConstruct()
{
    Super::NativeConstruct();
}

void UInventoryUI::NativeDestruct()
{
    Super::NativeDestruct();
}

void UInventoryUI::ExternalBindingToInventoryComp()
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: World is null", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: PlayerController is null", __FUNCTION__);
        return;
    }
    //@Pawns
    APawn* OwningPawn = PC->GetPawn();
    if (!OwningPawn)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: Pawn을 찾을 수 없습니다", __FUNCTION__);
        return;
    }
    //@Inven Comp
    UInventoryComponent* InvenComp = OwningPawn->FindComponentByClass<UInventoryComponent>();
    if (!InvenComp)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: Inventory Component를 찾을 수 없습니다", __FUNCTION__);
        return;
    }

    //@TODO: Binding
}

void UInventoryUI::ExternalBindingToInputComp()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: World is null", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: PlayerController is null", __FUNCTION__);
        return;
    }
    //@Pawn
    APawn* OwningPawn = PC->GetPawn();
    if (!OwningPawn)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: Pawn을 찾을 수 없습니다", __FUNCTION__);
        return;
    }
    //@Input Comp
    UBaseInputComponent* BaseInputComp = Cast<UBaseInputComponent>(PC->InputComponent);
    if (!BaseInputComp)
    {
        UE_LOGFMT(LogInventoryUI, Error, "{0}: Input Component를 찾을 수 없습니다", __FUNCTION__);
        return;
    }

    //@TODO: Binding
}

void UInventoryUI::InitializeInventoryUI()
{
    //@TODO: Inventory UI의 Subwidget들의 생성 작업
    //@TODO: Internal Binding

    InventoryUIInitFinished.ExecuteIfBound();
}
#pragma endregion