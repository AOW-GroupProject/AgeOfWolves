#include "ItemSlot.h"
#include "Logging/StructuredLog.h"

#include "Components/Overlay.h"
#include "Components/Image.h"
#include "Components/VerticalBox.h"
#include "Components/EditableText.h"

DEFINE_LOG_CATEGORY(LogItemSlot)

#pragma region Default Setting
UItemSlot::UItemSlot(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    UniqueItemID = FGuid::FGuid();
    SlotOverlay = nullptr;
    SlotBGImage = nullptr;
    SlotImage = nullptr;
    SlotItemNum = nullptr;
    bStackable = false;
    bRemovable = true;
}

void UItemSlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();

}

void UItemSlot::NativePreConstruct()
{
    Super::NativePreConstruct();

}

void UItemSlot::NativeConstruct()
{
    Super::NativeConstruct();

    //@기본 가시성 설정은 HitTestInvisible입니다
    auto RootWidget = GetRootWidget();
    if (RootWidget)
    {
        RootWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
        UE_LOG(LogTemp, Log, TEXT("ItemSlot: RootWidget 가시성이 HitTestInvisible로 설정되었습니다."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemSlot: RootWidget이 null입니다. 가시성을 설정할 수 없습니다."));
    }
}

void UItemSlot::NativeDestruct()
{
    Super::NativeDestruct();

}

void UItemSlot::InitializeItemSlot()
{
    //아이템 슬롯 관련 초기화 작업들 아래에서 수행...

    //@초기화 완료 이벤트 호출
    ItemSlotInitFinished.ExecuteIfBound();
}
#pragma endregion

#pragma region SubWidgets
void UItemSlot::AssignNewItem(const FGuid& ID, const FItemInformation* ItemInformation, int32 ItemCount)
{
    //@Unique Item ID
    UniqueItemID = ID;
    //@bRemovable
    bRemovable = ItemInformation->bRemovable;
    //@Slot Itme Image
    SetSlotImage(ItemInformation->ItemSlotImage);
    //@bStackable
    SetIsStackable(ItemInformation->bStackable);
    //@ItemCount
    UpdateItemCount(ItemCount);

    UE_LOGFMT(LogItemSlot, Log, "새 아이템이 아이템 슬롯에 할당되었습니다. ID: {0}, 갯수: {1}, 제거가능: {2}, 스택가능: {3}",
        UniqueItemID.ToString(), ItemCount, ItemInformation->bRemovable ? TEXT("Yes") : TEXT("No"), ItemInformation->bStackable ? TEXT("Yes") : TEXT("No"));
}

void UItemSlot::UpdateItemCount(int32 NewCount)
{
    //@Item Count
    SetSlotItemNum(NewCount);

    UE_LOGFMT(LogItemSlot, Log, "아이템 슬롯의 아이템 갯수가 업데이트되었습니다. ID: {0}, 새 갯수: {1}",
        UniqueItemID.ToString(), NewCount);
}

void UItemSlot::ClearAssignedItem(bool bForceClear)
{
    if (bForceClear || bRemovable)
    {
        //@FGuid
        UniqueItemID.Invalidate();
        //@Slot Item Image
        SetSlotImage(nullptr);
        //@bStackable
        SetIsStackable(false);
        //@ItemNum
        UpdateItemCount(0);
        //@bRemovable
        bRemovable = true;  // 기본값으로 리셋
        UE_LOGFMT(LogItemSlot, Log, "아이템 슬롯의 아이템이 제거되었습니다. 강제 제거: {0}, 원래 제거 가능 여부: {1}",
            bForceClear ? TEXT("Yes") : TEXT("No"),
            bRemovable ? TEXT("Yes") : TEXT("No"));
    }
    else
    {
        UE_LOGFMT(LogItemSlot, Warning, "아이템 슬롯의 아이템을 제거할 수 없습니다. 제거 불가능한 아이템이며 강제 제거가 지정되지 않았습니다.");
    }
}

void UItemSlot::SetSlotImage(TSoftObjectPtr<UTexture2D> InTexture)
{
    if (InTexture.IsValid())
    {
        UTexture2D* LoadedTexture = InTexture.LoadSynchronous();
        if (LoadedTexture)
        {
            // 텍스처 설정 최적화
            LoadedTexture->MipGenSettings = TMGS_NoMipmaps;
            LoadedTexture->CompressionSettings = TC_VectorDisplacementmap;
            LoadedTexture->Filter = TF_Bilinear;
            LoadedTexture->UpdateResource();

            SlotImage->SetBrushTintColor(FLinearColor::White);
            SlotImage->SetColorAndOpacity(FLinearColor::White);
            SlotImage->SetBrushFromTexture(LoadedTexture);

            // 이미지 크기를 원본 텍스처 크기로 설정
            SlotImage->SetDesiredSizeOverride(FVector2D(LoadedTexture->GetSizeX(), LoadedTexture->GetSizeY()));

            UE_LOGFMT(LogItemSlot, Log, "{0}: 슬롯 이미지가 설정되었습니다. 텍스처 이름: {1}", __FUNCTION__, *LoadedTexture->GetName());
        }
        else
        {
            UE_LOGFMT(LogItemSlot, Warning, "{0}: 텍스처 로딩 실패", __FUNCTION__);
        }
    }
    else
    {
        SlotImage->SetBrushFromTexture(nullptr);
        SlotImage->SetBrushTintColor(FLinearColor::Transparent);
        UE_LOGFMT(LogItemSlot, Warning, "{0}: 유효하지 않은 텍스처 참조", __FUNCTION__);
    }
}

void UItemSlot::SetSlotItemNum(int32 InNum)
{
    if (InNum > 0)
    {
        SlotItemNum->SetVisibility(ESlateVisibility::HitTestInvisible);
        SlotItemNum->SetText(FText::AsNumber(InNum));
        
        UE_LOGFMT(LogItemSlot, Log, "아이템 개수가 설정되었습니다. 개수: {0}", InNum);
    }
    else
    {
        SlotItemNum->SetVisibility(ESlateVisibility::Collapsed);
        SlotItemNum->SetText(FText::AsNumber(InNum));

        UE_LOGFMT(LogItemSlot, Log, "아이템 개수가 0이하로 설정되어 숨겨집니다. 개수: {0}", InNum);
    }
}

FSlateBrush UItemSlot::GetSlotImage() const
{
    if (SlotImage)
    {
        UE_LOGFMT(LogItemSlot, Verbose, "슬롯 이미지를 반환합니다.");
        return SlotImage->GetBrush();
    }
    UE_LOGFMT(LogItemSlot, Warning, "슬롯 이미지가 유효하지 않습니다. 빈 FSlateBrush를 반환합니다.");
    return FSlateBrush();
}

int32 UItemSlot::GetSlotItemNum() const
{
    if (SlotItemNum)
    {
        FString NumText = SlotItemNum->GetText().ToString();
        if (NumText.IsNumeric())
        {
            int32 Num = FCString::Atoi(*NumText);
            UE_LOGFMT(LogItemSlot, Verbose, "슬롯 아이템 개수를 반환합니다. 개수: {0}", Num);
            return Num;
        }
    }
    UE_LOGFMT(LogItemSlot, Warning, "슬롯 아이템 개수를 가져올 수 없습니다. 0을 반환합니다.");
    return 0;
}
#pragma endregion
