
#pragma once

#include "CoreMinimal.h"
#include "08_UI/ItemSlot.h"
#include "CustomButton.h"

#include "InteractableItemSlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteractableItemSlot, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UCustomButton;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInteractableItemSlot);

//@아이템 슬롯 버튼 호버 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FItemSlotButtonHovered, const FGuid&, EInteractionMethod)
//@아이템 슬롯 버튼 언호버 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FItemSlotButtonUnhovered, const FGuid&)
//@아이템 슬롯 버튼 클릭 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FItemSlotButtonClicked, const FGuid&, EInteractionMethod)

//@선택된 아이템 슬롯 버튼 선택 취소 이벤트
DECLARE_MULTICAST_DELEGATE(FNotifyItemSlotButtonCanceled);
#pragma endregion

/**
 * @UInteractableItemSlot
 *
 * Item Slot 중 사용자와 상호작용 가능한 Item Slot을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UInteractableItemSlot : public UItemSlot
{
    friend class UItemSlots;

    GENERATED_BODY()

    //@Default Setting
#pragma region Default Setting
public:
    UInteractableItemSlot(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;

protected:
    //@내부 바인딩
    void InternalBindToItemSlotButton();

public:
    virtual void InitializeItemSlot() override;
#pragma endregion

    //@Property/Info...etc
#pragma region SubWidgets
protected:
    virtual void AssignNewItem_Implementation(const FGuid& ID, FItemInformation ItemInformation, int32 ItemCount = -1) override;
    virtual void AssignNewItemFromSlot_Implementation(UItemSlot* FromSlot) override;
    virtual void UpdateItemCount_Implementation(int32 NewCount) override;
    virtual void ClearAssignedItem_Implementation(bool bForceClear = false) override;

public:
    //@아이템 슬롯 버튼 활성화/비활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Item Slot | Button")
    void ActivateItemSlotInteraction();
    UFUNCTION(BlueprintCallable, Category = "Item Slot | Button")
    void DeactivateItemSlotInteraction();

protected:
    //@블루프린트에서 배치된 CustomButton (동적 생성 X)
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UCustomButton* ItemSlotButton;
#pragma endregion

    //@Delegates
#pragma region Delegates
public:
    FRequestStartInitByInteractableItemSlot RequestStartInitByInteractableItemSlot;
    FItemSlotButtonHovered ItemSlotButtonHovered;
    FItemSlotButtonUnhovered ItemSlotButtonUnhovered;
    FItemSlotButtonClicked ItemSlotButtonClicked;
    FNotifyItemSlotButtonCanceled NotifyItemSlotButtonCanceled;
#pragma endregion

    //@Callbacks
#pragma region Callbacks
protected:
    UFUNCTION(BlueprintNativeEvent)
    void OnItemSlotButtonHovered(EInteractionMethod InteractionMethodType);
    virtual void OnItemSlotButtonHovered_Implementation(EInteractionMethod InteractionMethodType);

    UFUNCTION(BlueprintNativeEvent)
    void OnItemSlotButtonUnhovered();
    virtual void OnItemSlotButtonUnhovered_Implementation();

    UFUNCTION(BlueprintNativeEvent)
    void OnItemSlotButtonClicked(EInteractionMethod InteractionMethodType);
    virtual void OnItemSlotButtonClicked_Implementation(EInteractionMethod InteractionMethodType);

protected:
    UFUNCTION(BlueprintNativeEvent)
    void ItemSlotButtonCanceledNotified(const FGuid& ItemID);
    virtual void ItemSlotButtonCanceledNotified_Implementation(const FGuid& ItemID);
#pragma endregion

    //@Utility
#pragma region Utility
public:
    FORCEINLINE UCustomButton* GetItemSlotButton() const { return ItemSlotButton; }
#pragma endregion
};