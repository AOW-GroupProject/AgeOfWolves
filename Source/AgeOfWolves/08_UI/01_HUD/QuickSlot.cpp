// Fill out your copyright notice in the Description page of Project Settings.


#include "QuickSlot.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogQuickSlot)

//UE_LOGFMT(LogQuickSlot, Log, "");

#pragma region Default Setting
UQuickSlot::UQuickSlot(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
    SlotOverlay = nullptr;
	SlotImage = nullptr;
	SlotItemNum = nullptr;
	bStackable = false;
}

void UQuickSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UQuickSlot::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UQuickSlot::NativeConstruct()
{
	Super::NativeConstruct();

}

void UQuickSlot::NativeDestruct()
{
	Super::NativeDestruct();

}
#pragma endregion

#pragma region SubWidgets
void UQuickSlot::AssignNewItem(const FGuid& ID, UTexture2D* ItemImage, bool bIsStackable, int32 ItemCount, bool bIsRemovable)
{
    //@Unique Item ID
    this->UniqueItemID = UniqueItemID;
    //@bRemovable
    this->bRemovable = bIsRemovable;
    //@Slot Itme Image
    SetSlotImage(ItemImage);
    //@bStackable
    SetIsStackable(bIsStackable);
    //@ItemCount
    SetSlotItemNum(ItemCount);

    UE_LOGFMT(LogQuickSlot, Log, "새 아이템이 퀵슬롯에 할당되었습니다. ID: {0}, 갯수: {1}, 제거가능: {2}",
        UniqueItemID.ToString(), ItemCount, bIsRemovable ? TEXT("Yes") : TEXT("No"));
}

void UQuickSlot::UpdateItemCount(int32 NewCount)
{
    //@Item Count
    SetSlotItemNum(NewCount);

    UE_LOGFMT(LogQuickSlot, Log, "퀵슬롯 아이템 갯수가 업데이트되었습니다. ID: {0}, 새 갯수: {1}",
        UniqueItemID.ToString(), NewCount);
}

void UQuickSlot::ClearAssignedItem(bool bForceClear)
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
        SetSlotItemNum(0);
        //@bRemovable
        bRemovable = true;  // 기본값으로 리셋
        UE_LOGFMT(LogQuickSlot, Log, "퀵슬롯의 아이템이 제거되었습니다. 강제 제거: {0}, 원래 제거 가능 여부: {1}",
            bForceClear ? TEXT("Yes") : TEXT("No"),
            bRemovable ? TEXT("Yes") : TEXT("No"));
    }
    else
    {
        UE_LOGFMT(LogQuickSlot, Warning, "퀵슬롯의 아이템을 제거할 수 없습니다. 제거 불가능한 아이템이며 강제 제거가 지정되지 않았습니다.");
    }
}

void UQuickSlot::SetSlotImage(TSoftObjectPtr<UTexture2D> InTexture)
{
    //Texture2D
    if (InTexture.IsValid())
    {
        //SoftObjectPtr, LoadSynchronous
        UTexture2D* LoadedTexture = InTexture.LoadSynchronous();
        if (LoadedTexture)
        {
            //@Opacity
            SlotImage->SetColorAndOpacity(FLinearColor(1.0f, 1.0f, 1.0f, 1.0f));
            //@Texture
            SlotImage->SetBrushFromTexture(LoadedTexture);
            UE_LOGFMT(LogQuickSlot, Log, "{0}: 슬롯 이미지가 설정되었습니다. 텍스처 이름: {1}", __FUNCTION__, *LoadedTexture->GetName());
        }
        else
        {
            UE_LOGFMT(LogQuickSlot, Warning, "{0}: 텍스처 로딩 실패", __FUNCTION__);
        }
    }
    else
    {
        UE_LOGFMT(LogQuickSlot, Warning, "{0}: 유효하지 않은 텍스처 참조", __FUNCTION__);
    }
}

void UQuickSlot::SetSlotItemNum(int32 InNum)
{
    if (InNum > 0)
    {
        if (SlotItemNum->GetVisibility() == ESlateVisibility::Collapsed)
        {
            SlotItemNum->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
        SlotItemNum->SetText(FText::AsNumber(InNum));
    }
    else
    {
        SlotItemNum->SetVisibility(ESlateVisibility::Collapsed);
        SlotItemNum->SetText(FText::AsNumber(InNum));
    }
}

FSlateBrush UQuickSlot::GetSlotImage() const
{
    if (SlotImage)
    {
        return SlotImage->GetBrush();
    }
    return FSlateBrush();
}

int32 UQuickSlot::GetSlotItemNum() const
{
    if (SlotItemNum)
    {
        FString NumText = SlotItemNum->GetText().ToString();
        if (NumText.IsNumeric())
        {
            return FCString::Atoi(*NumText);
        }
    }
    return 0;
}
#pragma endregion
