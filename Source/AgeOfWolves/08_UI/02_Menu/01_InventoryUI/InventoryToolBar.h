#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "09_Item/Item.h"

#include "InventoryToolBar.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryToolBar, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UHorizontalBox;
class UCustomButton;
class UInventoryUIContent;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 완료 이벤트
DECLARE_DELEGATE(FInventoryToolBarInitFinished)

//@Item Type 버튼 호버 이벤트
DECLARE_DELEGATE_OneParam(FInventoryToolBarButtonHovered, EItemType);
//@Item Type 버튼 선택 이벤트
DECLARE_DELEGATE_OneParam(FInventoryToolBarButtonClicked, EItemType)
#pragma endregion


UCLASS()
class AGEOFWOLVES_API UInventoryToolBar : public UUserWidget
{
//@친추 클래스
#pragma region Friend Class
    friend class UInventoryUIContent;
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UInventoryToolBar(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
    //~ End UUserWidget Interface

protected:
    //@외부 바인딩

protected:
    //@내부 바인딩
    void InternalBindToButton(UCustomButton* Button, EItemType ItemType);

public:
    //@초기화
    UFUNCTION()
        void InitializeInventoryToolBar();
#pragma endregion

//@Property/Info...etc
#pragma region Widgets
public:
    //@강제로 Default Setting으로 리셋합니다.
    UFUNCTION(BlueprintCallable, Category = "Inventory Tool Bar")
        void ResetToolBar();

protected:
    void CreateButtons();
    void CreateAndAddButton(EItemType ButtonType, float Scale);

protected:
    void MoveLeft();
    void MoveRight();

private:
    void MoveSelection(int32 Direction);

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UHorizontalBox* ItemTypeButtonBox;

    const EItemType DefaultItemType = EItemType::Tool;
    EItemType CurrentSelectedItemType = EItemType::MAX;
    TMap<EItemType, UCustomButton*> MItemTypeButtons;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot | Button", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<UCustomButton> ToolTypeButtonClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot | Button", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<UCustomButton> MaterialTypeButtonClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot | Button", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<UCustomButton> MemoryTypeButtonClass;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Slot | Button", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<UCustomButton> EquipmentTypeButtonClass;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 완료 이벤트
    FInventoryToolBarInitFinished InventoryToolBarInitFinished;

public:
    //@버튼 호버 이벤트
    FInventoryToolBarButtonHovered InventoryToolBarButtonHovered;
    //@버튼 클릭 이벤트
    FInventoryToolBarButtonClicked InventoryToolBarButtonClicked;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@Inventory Tool Bar 버튼 클릭 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnInventoryToolBarButtonClicked(EInteractionMethod InteractionMethodType, EItemType ItemType);
    virtual void OnInventoryToolBarButtonClicked_Implementation(EInteractionMethod InteractionMethodType, EItemType ItemType);
    //@Inventory Tool Bar 버튼 Hover 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnInventoryToolBarButtonHovered(EInteractionMethod InteractionMethodType, EItemType ItemType);
    virtual void OnInventoryToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, EItemType ItemType);
    //@Inventory Tool Bar 버튼 Unhover 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnInventoryToolBarButtonUnhovered(EItemType ItemType);
    virtual void OnInventoryToolBarButtonUnhovered_Implementation(EItemType ItemType);

protected:
    //@Inventory Tool Bar 버튼 선택 취소 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void CancelInventoryToolBarButtonSelected(EItemType PreviousItemType);
    virtual void CancelInventoryToolBarButtonSelected_Implementation(EItemType PreviousItemType);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    int32 GetCurrentButtonIndex() const;
#pragma endregion

};