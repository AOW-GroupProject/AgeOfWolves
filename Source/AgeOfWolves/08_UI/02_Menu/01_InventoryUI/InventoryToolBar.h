#pragma once

#include "CoreMinimal.h"
#include "08_UI/HorizontalToolBar.h"
#include "09_Item/Item.h"

#include "InventoryToolBar.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryToolBar, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UInventoryUIContent;
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@Item Type 버튼 호버 이벤트
DECLARE_DELEGATE_OneParam(FInventoryToolBarButtonHovered, EItemType);
//@Item Type 버튼 선택 이벤트
DECLARE_DELEGATE_OneParam(FInventoryToolBarButtonClicked, EItemType)
#pragma endregion

/**
 * UInventoryToolBar
 *
 * 인벤토리 UI의 상단에 위치하는 아이템 타입 선택 툴바입니다.
 */
    UCLASS()
    class AGEOFWOLVES_API UInventoryToolBar : public UHorizontalToolBar
{

//@친추 클래스
#pragma region Friend Class
    friend class UInventoryUIContent;
#pragma endregion

    GENERATED_BODY()

        //@Default Setting
#pragma region Default Setting
public:
    UInventoryToolBar(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interface
    virtual void NativeOnInitialized() override;
    //~ End UUserWidget Interface

protected:
    //@내부 바인딩
    virtual void InternalBindToButton(UCustomButton* Button, EItemType ItemType);

public:
    //@초기화
    virtual void InitializeToolBar() override;
#pragma endregion

    //@Property/Info...etc
#pragma region SubWidgets
protected:
    virtual void ResetToolBar() override;

protected:
    //@버튼 생성
    virtual void CreateButtons() override;
    void CreateAndAddButton(EItemType ButtonType, float Scale);

protected:
    virtual void MoveSelection(int32 Direction) override;

protected:
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
    //@버튼 호버 이벤트
    FInventoryToolBarButtonHovered InventoryToolBarButtonHovered;
    //@버튼 클릭 이벤트
    FInventoryToolBarButtonClicked InventoryToolBarButtonClicked;
#pragma endregion

    //@Callbacks
#pragma region Callbacks
protected:
    //@버튼 이벤트 override
    virtual void OnToolBarButtonClicked_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex) override;
    virtual void OnToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex) override;
    virtual void OnToolBarButtonUnhovered_Implementation(uint8 ButtonIndex) override;
    virtual void CancelToolBarButtonSelected_Implementation(uint8 PreviousIndex) override;
#pragma endregion

    //@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
    //@인덱스의 유효성 검사 override
    virtual bool IsValidButtonIndex(uint8 Index) const override;

private:
    //@uint8 <-> EItemType 변환 유틸리티
    FORCEINLINE EItemType IndexToItemType(uint8 Index) const { return static_cast<EItemType>(Index); }
    FORCEINLINE uint8 ItemTypeToIndex(EItemType Type) const { return static_cast<uint8>(Type); }
#pragma endregion
};