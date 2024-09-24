#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "09_Item/Item.h"

#include "InventoryToolBar.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryToolBar, Log, All)

#pragma region Delegates
//@초기화 완료 이벤트
DECLARE_DELEGATE(FInventoryToolBarInitFinished);
//@Item Type 버튼 선택 이벤트
DECLARE_DELEGATE_OneParam(FInventoryToolBarButtonClicked, EItemType);
#pragma endregion

#pragma region Forward Declaration
class UHorizontalBox;
class UCustomButton;
#pragma endregion

UCLASS()
class AGEOFWOLVES_API UInventoryToolBar : public UUserWidget
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UInventoryToolBar(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
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

#pragma region Widgets
public:
    //@강제로 Default Setting으로 리셋합니다.
    UFUNCTION(BlueprintCallable, Category = "Inventory Tool Bar")
        void ResetToolBar();

protected:
    void CreateButtons();
    void CreateAndAddButton(EItemType ButtonType, float Scale);

protected:
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UHorizontalBox* ItemTypeButtonBox;

    const EItemType DefaultItemType = EItemType::Tool;
    EItemType CurrentItemType = EItemType::MAX;
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

#pragma region Delegates
public:
    //@초기화 완료 이벤트
    FInventoryToolBarInitFinished InventoryToolBarInitFinished;

public:
    //@버튼 클릭 이벤트
    FInventoryToolBarButtonClicked InventoryToolBarButtonClicked;
#pragma endregion

#pragma region Callbacks
protected:
    //@Inventory Tool Bar 버튼 클릭 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnInventoryToolBarButtonClicked(EItemType ItemType);
    virtual void OnInventoryToolBarButtonClicked_Implementation(EItemType ItemType);
    //@Inventory Tool Bar 버튼 Hover 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnInventoryToolBarButtonHovered(EItemType ItemType);
    virtual void OnInventoryToolBarButtonHovered_Implementation(EItemType ItemType);
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
};