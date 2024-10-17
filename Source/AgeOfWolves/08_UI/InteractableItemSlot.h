
#pragma once

#include "CoreMinimal.h"
#include "08_UI/ItemSlot.h"
#include "CustomButton.h"

#include "InteractableItemSlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteractableItemSlot, Log, All)

//@전방 선언
#pragma region Forward Declaration
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
//@친추 클래스
#pragma region Friend Class
    friend class UItemSlots;
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UInteractableItemSlot(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
    virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    //~ End UUserWidget Interface

protected:
    //@내부 바인딩
    void InternalBindToItemSlotButton(UCustomButton* InItemSlotButton);

public:
    //@초기화 함수
    virtual void InitializeItemSlot() override;
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
protected:
    //@CustomButton 생성
    void CreateButton();

protected:
    virtual void AssignNewItem_Implementation(const FGuid& ID, FItemInformation ItemInformation, int32 ItemCount = -1) override;
    virtual void UpdateItemCount_Implementation(int32 NewCount) override;
    virtual void ClearAssignedItem_Implementation(bool bForceClear = false) override;

public:
    //@아이템 슬롯 버튼 활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Item Slot | Button")
        void ActivateItemSlotInteraction();
    //@아이템 슬롯 버튼 비활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Item Slot | Button")
        void DeactivateItemSlotInteraction();

protected:
    //@Slot Overlay에 추가할 사용자 상호작용 가능한 CustomButton
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot | Button", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<UCustomButton> ItemSlotButtonClass;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByInteractableItemSlot RequestStartInitByInteractableItemSlot;

public:
    //@아이템 슬롯 버튼 호버 이벤트
    FItemSlotButtonHovered ItemSlotButtonHovered;
    //@아이템 슬롯 버튼 언호버 이벤트
    FItemSlotButtonUnhovered ItemSlotButtonUnhovered;
    //@아이템 슬롯 버튼 클릭 이벤트
    FItemSlotButtonClicked ItemSlotButtonClicked;

public:
    //@선택된 아이템 슬롯 버튼 선택 취소 알림 이벤트
    FNotifyItemSlotButtonCanceled NotifyItemSlotButtonCanceled;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@Button Hovered 이벤트에 등록되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnItemSlotButtonHovered(EInteractionMethod InteractionMethodType);
    virtual void OnItemSlotButtonHovered_Implementation(EInteractionMethod InteractionMethodType);
    //@Button Unhovered 이벤트에 등록되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnItemSlotButtonUnhovered();
    virtual void OnItemSlotButtonUnhovered_Implementation();
    //@Button Clicked 이벤트에 등록되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnItemSlotButtonClicked(EInteractionMethod InteractionMethodType);
    virtual void OnItemSlotButtonClicked_Implementation(EInteractionMethod InteractionMethodType);

protected:
    //@Button의 선택 취소 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void ItemSlotButtonCanceledNotified(const FGuid& ItemID);
    virtual void ItemSlotButtonCanceledNotified_Implementation(const FGuid& ItemID);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    UFUNCTION(BlueprintCallable, Category = "Item Slot | Button")
        UCustomButton* GetItemSlotButton() const;
#pragma endregion

};
