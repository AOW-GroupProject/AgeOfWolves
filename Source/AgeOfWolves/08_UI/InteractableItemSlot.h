#pragma once

#include "CoreMinimal.h"
#include "08_UI/ItemSlot.h"
#include "CustomButton.h"
#include "InteractableItemSlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteractableItemSlot, Log, All)

//@아이템 슬롯 버튼 클릭 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FItemSlotButtonClicked, const FGuid&)

//@선택된 아이템 슬롯 버튼 선택 취소 이벤트
DECLARE_MULTICAST_DELEGATE(FNotifyItemSlotButtonCanceled);

/**
 * @UInteractableItemSlot
 *
 * Item Slot 중 사용자와 상호작용 가능한 Item Slot을 정의합니다.
 */
    UCLASS()
    class AGEOFWOLVES_API UInteractableItemSlot : public UItemSlot
{
    GENERATED_BODY()

        //@Friend Class 설정
        friend class UItemSlots;

#pragma region Default Setting
public:
    UInteractableItemSlot(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override; // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override; // 화면에 렌더되기 직전에 호출됨
    virtual void NativeDestruct() override;
    //~ End UUserWidget Interface

protected:
    //@내부 바인딩
    void InternalBindToItemSlotButton(UCustomButton* InItemSlotButton);

public:
    virtual void InitializeItemSlot() override;
#pragma endregion

#pragma region SubWidgets
protected:
    //@CustomButton 생성
    void CreateButton();
    //@드롭다운 메뉴 생성
    void CreateDropDownMenu();

public:
    //@아이템 슬롯 버튼 활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Item Slot | Button")
        void ActivateItemSlotInteraction();

    //@아이템 슬롯 버튼 비활성화 함수
    UFUNCTION(BlueprintCallable, Category = "Item Slot | Button")
        void DeactivateItemSlotInteraction();

protected:
    //@Slot Overlay에 사용자 상호작용 가능한 CustomButton
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot | Button", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<UCustomButton> ItemSlotButtonClass;
#pragma endregion

#pragma region Delegates
public:
    //@아이템 슬롯 버튼 클릭 이벤트
    FItemSlotButtonClicked ItemSlotButtonClicked;

public:
    //@선택된 아이템 슬롯 버튼 선택 취소 알림 이벤트
    FNotifyItemSlotButtonCanceled NotifyItemSlotButtonCanceled;
#pragma endregion

#pragma region Callbacks
protected:
    //@Button Hovered 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnItemSlotButtonHovered();
    //@Button Unhovered 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnItemSlotButtonUnhovered();
    //@Button Pressed 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnItemSlotButtonPressed();
    //@Button Clicked 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnItemSlotButtonClicked();

protected:
    //@Button의 선택 취소 이벤트 구독
    UFUNCTION()
        void OnItemSlotButtonCanceled(const FGuid& ItemID);
#pragma endregion

#pragma region Utility
public:
    UFUNCTION(BlueprintCallable, Category = "Item Slot | Button")
        UCustomButton* GetItemSlotButton() const;
#pragma endregion
};