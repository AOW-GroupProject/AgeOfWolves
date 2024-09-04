// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDescriptionSlot.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"

#include "08_UI/02_Menu/01_InventoryUI/InventoryUIContent.h"
#include "08_UI/02_Menu/01_InventoryUI/ItemSlots.h"
#include "08_UI/InteractableItemSlot.h"

#include "14_Subsystem/ItemManagerSubsystem.h"
#include "04_Component/InventoryComponent.h"

#include "Components/EditableTextBox.h"
#include "Components/MultiLineEditableTextBox.h"

DEFINE_LOG_CATEGORY(LogItemDescription)


#pragma region Default Settings
UItemDescriptionSlot::UItemDescriptionSlot(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{}

void UItemDescriptionSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();
    //@외부 바인딩
    ExternalBindToInventoryComp();
}

void UItemDescriptionSlot::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UItemDescriptionSlot::NativeConstruct()
{
    Super::NativeConstruct();
}

void UItemDescriptionSlot::NativeDestruct()
{
    Super::NativeDestruct();
}

void UItemDescriptionSlot::ExternalBindToInventoryComp()
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogItemSlots, Error, "{0}: World is null", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogItemSlots, Error, "{0}: PlayerController is null", __FUNCTION__);
        return;
    }
    //@Pawns
    APawn* OwningPawn = PC->GetPawn();
    if (!OwningPawn)
    {
        UE_LOGFMT(LogItemSlots, Error, "{0}: Pawn을 찾을 수 없습니다", __FUNCTION__);
        return;
    }
    //@Inven Comp
    UInventoryComponent* InvenComp = OwningPawn->FindComponentByClass<UInventoryComponent>();
    if (!InvenComp)
    {
        UE_LOGFMT(LogItemSlots, Error, "{0}: Inventory Component를 찾을 수 없습니다", __FUNCTION__);
        return;
    }

    //@External Binding
    InvenComp->ItemAssignedToInventory.AddUFunction(this, "OnItemAssignedToInventory");
    InvenComp->ItemRemovedFromInventory.AddUFunction(this, "OnItemRemovedFromInventory");
}

void UItemDescriptionSlot::InitializeItemDescriptionSlot()
{
    //@TODO: Widget 생성 및 초기화 작업 진행...

    //@초기화 완료 이벤트 호출
    ItemDescriptionSlotInitFinished.ExecuteIfBound();
}
#pragma endregion

#pragma region SubWidgets
void UItemDescriptionSlot::ResetItemDescriptionSlot()
{
    //@FGuid
    if (UniqueItemID.IsValid())
    {
        //@Clear Assigned Item;
        ClearAssignedItem();

        UE_LOGFMT(LogItemSlot, Log, "아이템 설명 슬롯이 초기화되었습니다. 이전 아이템 ID: {0}", UniqueItemID.ToString());
    }
    else
    {
        UE_LOGFMT(LogItemSlot, Log, "아이템 설명 슬롯 초기화 시도. 이미 비어있는 상태입니다.");
    }
}

void UItemDescriptionSlot::AssignNewItem(const FGuid& ID, const FItemInformation* ItemInformation, int32 ItemCount)
{
    //@Unique Item ID
    UniqueItemID = ID;
    //@Slot Itme Image
    SetSlotImage(ItemInformation->ItemImage);
    //@Item Name Text Block
    ItemNameTextBox->SetText(FText::FromString(ItemInformation->ItemName));
    //@Item Descrption Text Block
    ItemDescriptionTextBox->SetText(FText::FromString(ArrangeItemDescriptionStringToText(ItemInformation->ItemDescription)));

    UE_LOGFMT(LogItemSlot, Log, "새 아이템이 아이템 슬롯에 할당되었습니다. ID: {0}", UniqueItemID.ToString());
}

void UItemDescriptionSlot::ClearAssignedItem(bool bForceClear)
{
    //@FGuid
    UniqueItemID.Invalidate();
    //@Slot Item Image
    SetSlotImage(TSoftObjectPtr<UTexture2D>());
    //@Item Name Text Block
    ItemNameTextBox->SetText(FText());
    //@Item Descrption Text Block
    ItemDescriptionTextBox->SetText(FText());

    UE_LOGFMT(LogItemSlot, Log, "기존 아이템이 아이템 슬롯으로부터 제거되었습니다.");

}

FString UItemDescriptionSlot::ArrangeItemDescriptionStringToText(FString String)
{
    //@'.'을 기준으로 문자열 분리
    TArray<FString> Sentences;
    String.ParseIntoArray(Sentences, TEXT("."), true);

    //@각 문장에 개행 추가 및 재결합
    FString FormattedDescription;
    for (int32 i = 0; i < Sentences.Num(); ++i)
    {
        FormattedDescription += Sentences[i].TrimStart().TrimEnd();
        if (i < Sentences.Num() - 1)  // 마지막 문장이 아니면 '.'과 개행 추가
        {
            FormattedDescription += TEXT(".\n");
        }
        else if (i == Sentences.Num() - 1 && !Sentences[i].IsEmpty())  // 마지막 문장이면서 비어있지 않으면 '.'만 추가
        {
            FormattedDescription += TEXT(".");
        }
    }

    return FormattedDescription;
}
#pragma endregion

#pragma region Callbacks
void UItemDescriptionSlot::OnItemSlotsReadyForBinding(const UInventoryUIContent* InventoryUIContent)
{
    if (!InventoryUIContent)
    {
        UE_LOGFMT(LogItemDescription, Error, "InventoryUIContent가 유효하지 않습니다.");
        return;
    }

    TArray<EItemType> ItemTypes = { EItemType::Tool, EItemType::Material, EItemType::Equipment, EItemType::Memory };
    int32 TotalSlots = 0;
    int32 TotalBindCount = 0;

    for (EItemType ItemType : ItemTypes)
    {
        UUserWidget* ItemSlotsWidget = InventoryUIContent->GetItemSlotsUI(ItemType);
        if (!ItemSlotsWidget)
        {
            UE_LOGFMT(LogItemDescription, Warning, "{0} 타입의 ItemSlots 위젯을 찾을 수 없습니다.", *UEnum::GetValueAsString(ItemType));
            continue;
        }

        UItemSlots* ItemSlots = Cast<UItemSlots>(ItemSlotsWidget);
        if (!ItemSlots)
        {
            UE_LOGFMT(LogItemDescription, Error, "{0} 타입의 ItemSlots 위젯을 UItemSlots로 캐스팅할 수 없습니다.", *UEnum::GetValueAsString(ItemType));
            continue;
        }

        TArray<UInteractableItemSlot*> TypeItemSlots = ItemSlots->GetAllItemSlots();
        TotalSlots += TypeItemSlots.Num();

        int32 TypeBindCount = 0;
        for (UInteractableItemSlot* ItemSlot : TypeItemSlots)
        {
            if (ItemSlot)
            {
                ItemSlot->ItemSlotButtonHovered.AddUObject(this, &UItemDescriptionSlot::OnItemSlotButtonHovered);
                ItemSlot->ItemSlotButtonUnhovered.AddUObject(this, &UItemDescriptionSlot::OnItemSlotButtonUnhovered);
                ItemSlot->NotifyItemSlotButtonCanceled.AddUObject(this, &UItemDescriptionSlot::OnItemSlotButtonCanceled);
                TypeBindCount++;
            }
        }

        TotalBindCount += TypeBindCount;
        UE_LOGFMT(LogItemDescription, Log, "{0} 타입: 총 {1}개의 Item Slot 중 {2}개의 Hover 이벤트가 성공적으로 바인딩되었습니다.",
            *UEnum::GetValueAsString(ItemType), TypeItemSlots.Num(), TypeBindCount);

        if (TypeBindCount != TypeItemSlots.Num())
        {
            UE_LOGFMT(LogItemDescription, Warning, "{0} 타입: {1}개의 Item Slot에 대해 Hover 이벤트 바인딩에 실패했습니다.",
                *UEnum::GetValueAsString(ItemType), TypeItemSlots.Num() - TypeBindCount);
        }
    }

    UE_LOGFMT(LogItemDescription, Log, "모든 타입: 총 {0}개의 Item Slot 중 {1}개의 Hover 이벤트가 성공적으로 바인딩되었습니다.",
        TotalSlots, TotalBindCount);

    if (TotalBindCount != TotalSlots)
    {
        UE_LOGFMT(LogItemDescription, Warning, "모든 타입: {0}개의 Item Slot에 대해 Hover 이벤트 바인딩에 실패했습니다.",
            TotalSlots - TotalBindCount);
    }
}

void UItemDescriptionSlot::OnItemAssignedToInventory(const FGuid& ID, EItemType Type, const FGameplayTag& ItemTag)
{
    UItemManagerSubsystem* ItemManager = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogItemDescription, Error, "ItemManagerSubsystem을 찾을 수 없습니다.");
        return;
    }

    const FItemInformation* ItemInfo = ItemManager->GetItemInformation<FItemInformation>(Type, ItemTag);
    if (!ItemInfo)
    {
        UE_LOGFMT(LogItemDescription, Error, "아이템 정보를 찾을 수 없습니다: Type {0}, Tag {1}",
            *UEnum::GetValueAsString(Type), ItemTag.ToString());
        return;
    }

    //@MItemsInInventoryItemSlots
    MItemsInInventoryItemSlots.Add(ID, *ItemInfo);

    UE_LOGFMT(LogItemDescription, Log, "아이템이 인벤토리에 추가됨: ID {0}, 타입 {1}, 태그 {2}",
        ID.ToString(), *UEnum::GetValueAsString(Type), ItemTag.ToString());
}

void UItemDescriptionSlot::OnItemRemovedFromInventory(const FGuid& ID)
{
    //@MItemsInInventoryItemSlots
    int32 NumRemoved = MItemsInInventoryItemSlots.Remove(ID);

    if (NumRemoved > 0)
    {
        UE_LOGFMT(LogItemDescription, Log, "아이템이 인벤토리에서 제거됨: ID {0}", ID.ToString());
    }
    else
    {
        UE_LOGFMT(LogItemDescription, Warning, "제거하려는 아이템을 찾을 수 없음: ID {0}", ID.ToString());
    }
}

void UItemDescriptionSlot::OnItemSlotButtonHovered_Implementation(const FGuid& ID)
{
    UE_LOGFMT(LogItemDescription, Log, "아이템 슬롯 호버: ID {0}", ID.ToString());

    const FItemInformation* ItemInfo = MItemsInInventoryItemSlots.Find(ID);
    if (ItemInfo)
    {
        // 새 아이템 할당
        AssignNewItem(ID, ItemInfo);

        // 가시성 활성화
        SetVisibility(ESlateVisibility::HitTestInvisible);

        UE_LOGFMT(LogItemDescription, Log, "아이템 설명 UI에 새 아이템 할당: ID {0}, 이름 {1}, 등급 {2}",
            ID.ToString(), *ItemInfo->ItemName, *UEnum::GetValueAsString(ItemInfo->ItemRank));
    }
    else
    {
        UE_LOGFMT(LogItemDescription, Warning, "호버된 아이템 정보를 찾을 수 없습니다: ID {0}", ID.ToString());
        SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UItemDescriptionSlot::OnItemSlotButtonUnhovered_Implementation(const FGuid& ID)
{
    UE_LOGFMT(LogItemDescription, Log, "아이템 슬롯 언호버: ID {0}", ID.ToString());

    //@필요 시 BP에서 해당 함수 오버라이딩...
    
    //@가시성 비활성화
    SetVisibility(ESlateVisibility::Collapsed);

}

void UItemDescriptionSlot::OnItemSlotButtonCanceled_Implementation()
{
    //@필요 시 BP에서 해당 함수 오버라이딩...


    //@가시성 비활성화
    SetVisibility(ESlateVisibility::Collapsed);
}
#pragma endregion