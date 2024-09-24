#pragma once

#include "CoreMinimal.h"
#include "08_UI/ItemSlot.h"
#include "CustomButton.h"

#include "InteractableItemSlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteractableItemSlot, Log, All)

#pragma region Forward Declaration
class UDropDownMenu;
#pragma endregion

#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInteractableItemSlot);

//@아이템 슬롯 버튼 호버 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FItemSlotButtonHovered, const FGuid&)
//@아이템 슬롯 버튼 언호버 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FItemSlotButtonUnhovered, const FGuid&)
//@아이템 슬롯 버튼 클릭 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FItemSlotButtonClicked, const FGuid&)

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
    void InternalBindToDropDownMenu(UDropDownMenu* DropDownMenu);

public:
    //@초기화 함수
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
    //@Slot Overlay에 추가할 사용자 상호작용 가능한 CustomButton
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot | Button", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<UCustomButton> ItemSlotButtonClass;

protected:
    //@Drop Down Menu Overlay
    UDropDownMenu* DropDownMenu;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot | Drop Down Menu", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<UDropDownMenu> DropDownMenuClass;
#pragma endregion

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

#pragma region Callbacks
protected:
    UFUNCTION()
        void OnDropDownMenuInitFinished();

protected:
    //@Button Hovered 이벤트에 등록되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnItemSlotButtonHovered();
    virtual void OnItemSlotButtonHovered_Implementation();
    //@Button Unhovered 이벤트에 등록되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnItemSlotButtonUnhovered();
    virtual void OnItemSlotButtonUnhovered_Implementation();
    //@Button Clicked 이벤트에 등록되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnItemSlotButtonClicked();
    virtual void OnItemSlotButtonClicked_Implementation();

protected:
    //@Button의 선택 취소 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void ItemSlotButtonCanceledNotified(const FGuid& ItemID);
    virtual void ItemSlotButtonCanceledNotified_Implementation(const FGuid& ItemID);
#pragma endregion

#pragma region Utility
public:
    UFUNCTION(BlueprintCallable, Category = "Item Slot | Button")
        UCustomButton* GetItemSlotButton() const;
#pragma endregion
};