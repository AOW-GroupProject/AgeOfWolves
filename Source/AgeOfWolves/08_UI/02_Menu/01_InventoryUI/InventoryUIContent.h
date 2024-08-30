#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "09_Item/Item.h"

#include "InventoryUIContent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUIContent, Log, All)

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInventoryUIContent)
//@초기화 완료 이벤트
DECLARE_DELEGATE(FInventoryUIContentInitFinished);

class UOverlay;
class UInventoryToolBar;
class UItemSlots;

/*
* @FItemSlotsInfo
*
* Inventory UI 내부 Item Slots UI 정보를 담고 있는 구조체
*/
USTRUCT(BlueprintType)
struct FItemSlotsInfo : public FTableRowBase
{
    GENERATED_BODY()
public:
    //@해당 컨텐츠를 나타낼 아이템 타입
    UPROPERTY(EditAnywhere, Category = "Item Slots | Category")
        EItemType ItemType;
    //@블루프린트 클래스
    UPROPERTY(EditAnywhere, Category = "Item Slots | Item Slots")
        TSubclassOf<UItemSlots> ItemSlotsClass;
};

/**
 * @UInventoryUIContent
 *
 * Inventory UI 내부 Content를 보여주는 UI
 *
 * 1. Inventory Tool Bar: Item Slots에 나타낼 아이템 카테고리를 선택할 수 있는 Tool Bar
 * 2. Item Slots: 현재 Inventory에 저장된 Item 목록을 나타내는 UI
 * 3. Item Description: 현재 선택된 Item 정보를 나타내는 UI
 */
UCLASS()
class AGEOFWOLVES_API UInventoryUIContent : public  UUserWidget
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UInventoryUIContent(const FObjectInitializer& ObjectInitializer);

protected:
    //Interface of UUserWidget
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~End Interface

protected:
    //@내부바인딩
    void InternalBindingToInventoryToolBar(UInventoryToolBar* ToolBar);
    void InternalBindingToItemSlots(UItemSlots* ItemSlotsWidget);

public:
    //@초기화
    UFUNCTION()
        void InitializeInventoryUIContent();

protected:
    //@Inventory 초기화 체크
    bool bInventoryToolBarReady = false;
    bool bInventoryItemSlotsReady = false;
    bool bInventoryItemDescriptionReady = false;
    void CheckInventoryUIContentInitialization();
#pragma endregion

#pragma region SubWidgets
protected:
    //@Inventory Tool Bar 생성
    void CreateToolBar();
    //@Item Slots 생성
    void CreateAllItemSlots();
    //@Item Description 생성
    void CreateItemDescription();

protected:
    //@Item Slots의 가시성 설정
    void SetItemTypeVisibility(EItemType ItemType, bool bVisible);
    // 모든 ItemSlots의 가시성을 설정하는 함수
    void UpdateAllItemSlotsVisibility();
    //@ItemType을 통해 이에 대응되는 Item Slots 반환
    UUserWidget* GetItemSlotsUI(EItemType ItemType) const;

protected:
    //@Tool Bar Overlay
    UPROPERTY(BlueprintReadWrite, Category = "Inventory Content UI | Tool Bar", meta = (BindWidget))
        UOverlay* ToolBarOverlay;
    //@Tool Bar Blueprint Class
    UPROPERTY(EditDefaultsOnly, category = "Inventory Content UI | Tool Bar")
        TSubclassOf<UInventoryToolBar> InventoryToolBarClass;

protected:
    //@Item Slot 목록을 담을 Overlay
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* ItemSlotsOverlay;
    //@Item Slots UI 정보
    UPROPERTY(EditDefaultsOnly, Category = "Inventory Content UI | Item Slots")
        TArray<FItemSlotsInfo> ItemSlots;
    //@현재 보여지고 있는 Item Slots UI의 아이템 타입
    EItemType CurrentItemType = EItemType::Tool;
    //@아이템 타입 별 아이템 슬롯을 TMap 자료구조에서 관리
    TMap<EItemType, TObjectPtr<UUserWidget>> MItemSlots;

protected:
    //@Item Description Overlay
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* ItemDescriptionOverlay;
    //@TODO: Item Description 구현에 필요한 정보들...

#pragma endregion

#pragma region Delegate
public:
    //@초기화 요청 이벤트
    FRequestStartInitByInventoryUIContent RequestStartInitByInventoryUIContent;
    //@초기화 완료 이벤트
    FInventoryUIContentInitFinished InventoryUIContentInitFinished;
#pragma endregion

#pragma region Callbacks
public:
    //@Inventory UI의 가시성 변화 이벤트 구독
    UFUNCTION()
        void InventoryUIVisibilityChangedNotified(bool bIsOpened);

protected:
    //@Inventory Tool Bar 초기화 완료 이벤트에 등록하는 콜백
    UFUNCTION()
        void OnInventoryToolBarInitFinished();
    //@Item Slots 초기화 완료 이벤트에 등록하는 콜백
    UFUNCTION()
        void OnInventoryItemSlotsInitFinished();

protected:
    //@Item Type 버튼 클릭 이벤트에 등록하는 콜백
    UFUNCTION()
        void OnInventoryToolBarButtonClicked(EItemType ItemType);
#pragma endregion
};