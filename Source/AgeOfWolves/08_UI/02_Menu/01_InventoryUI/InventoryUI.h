#pragma once

#include "CoreMinimal.h"
#include "08_UI/02_Menu/MenuUIContent.h"
#include "GameplayTagContainer.h"

#include "InventoryUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUI, Log, All)

class UOverlay;
class UScaleBox;
class UImage;
class UInventoryUIContent;

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInventoryUI)
//@초기화 완료 이벤트
DECLARE_DELEGATE(FInventoryUIInitFinished);

UCLASS()
class AGEOFWOLVES_API UInventoryUI : public UMenuUIContent
{
    GENERATED_BODY()
        
#pragma region Default Setting
public:
    UInventoryUI(const FObjectInitializer& ObjectInitializer);

protected:
    //Interface of UUserWidget
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~End UUserWidget
protected:
    //@외부 바인딩
    void ExternalBindingToInventoryComp();
    void ExternalBindingToInputComp();
protected:
    //@내부 바인딩
    void InternalBindingToInventoryUIContent(UInventoryUIContent* InventoryUIContent);
protected:
    //@초기화 함수
    virtual void InitializeMenuUIContent(EMenuCategory Category) override;
protected:
    bool bInventoryUIContentReady= false;
    virtual void CheckMenuUIContentInitFinished() const override;

#pragma endregion

#pragma region SubWidgets
protected:
    //@Item Slots
    void CreateInventoryContent();

protected:
    //UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    //    UImage* TitleImage;

    //@Item Slots를 담을 Scale Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* InventoryUIContentOverlay;
    //@Item Slots 블루프린트 클래스(WBP)
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI | Item Slot")
        TSubclassOf<UInventoryUIContent> InventoryUIContentClass;
#pragma endregion

#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByInventoryUI RequestStartInitByInventoryUI;
    //@초기화 완료 이벤트
    FInventoryUIInitFinished InventoryUIInitFinished;
#pragma endregion

#pragma region Callbacks
protected:
    //@Item Slots의 초기화 완료 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnInventoryUIContentInitFinished();

protected:
    //@Input Tag 활성화 이벤트에 등록하는 콜백
    UFUNCTION()
        void OnUIInputTagTriggered(const FGameplayTag& InputTag);
    //@Input Tag 해제 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnUIInputTagReleased(const FGameplayTag& InputTag);

protected:
    //@Inventory Comp의 아이템 할당 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnItemAssignedToInventory(const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag);
    //@Inventory Comp의 아이템 제거 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnItemRemovedFromInventory(const FGuid& UniqueItemID);
    //@Inventory Comp의 아이테 업데이트 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnInventoryItemUpdated(const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 UpdatedItemCount);
#pragma endregion

};