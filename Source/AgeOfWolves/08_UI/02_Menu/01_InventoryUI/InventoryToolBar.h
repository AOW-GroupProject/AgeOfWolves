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
    //@내부 바인딩
    void InternalBindToButton(UCustomButton* Button, EItemType ItemType);

public:
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
private:
    UFUNCTION()
        void HandleButtonHover(EItemType ItemType);
    UFUNCTION()
        void HandleButtonUnhover(EItemType ItemType);
    UFUNCTION()
        void HandleButtonClick(EItemType ItemType);
    void HandleButtonCanceled(EItemType PreviousItemType);
#pragma endregion
};