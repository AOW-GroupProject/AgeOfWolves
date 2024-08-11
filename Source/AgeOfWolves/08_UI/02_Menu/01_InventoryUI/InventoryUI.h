#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"

#include "InventoryUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUI, Log, All)

class UOverlay;
class UScaleBox;
class UImage;
class UItemSlots;
class UItemDescription;

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInventoryUI)
//@초기화 완료 이벤트
DECLARE_DELEGATE(FInventoryUIInitFinished);

UCLASS()
class AGEOFWOLVES_API UInventoryUI : public UUserWidget
{
    GENERATED_BODY()
        
#pragma region Default Setting

public:
    UInventoryUI(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
protected:
    //@외부 바인딩
    void ExternalBindingToInventoryComp();
    void ExternalBindingToInputComp();
protected:
    void InternalBindingToItemSlots(UItemSlots* ItemSlots);
    void InternalBindingToItemDescription(UItemDescription* ItemDescription);

public:
    //@초기화 함수
    UFUNCTION()
        void InitializeInventoryUI();
protected:
    void CheckInventoryUIInitialization();

#pragma endregion

#pragma region SubWidgets
protected:
    //@Item Slots
    bool bItemSlotsInitialized = false;
    void CreateItemSlots();
    //@Item Descrption
    bool bItemDescriptionInitialized = false;
    void CreateItemDescription();

protected:
    //@Inventory UI Overlay
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* InventoryUIOverlay;

    //@Title Image를 담을 Scale Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UScaleBox* TitleImageBox;
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UImage* TitleImage;

    //@Item Slots를 담을 Scale Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UScaleBox* ItemSlotsBox;
    //@Item Slots 블루프린트 클래스(WBP)
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI")
        TSubclassOf<UItemSlots> ItemSlotsClass;
    
    //@Item Decription을 담을 Scale Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UScaleBox* ItemDescriptionBox;
    //@Item Decription 블루프린트 클래스(WBP)
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI")
        TSubclassOf<UItemDescription> ItemDescriptionClass;
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
        void OnItemSlotsInitFinished();
    //@Item Descrption의 초기화 완료 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnItemDescriptionInitFinished();

protected:
    UFUNCTION()
        void OnUIInputTagTriggered(const FGameplayTag& InputTag);

    UFUNCTION()
        void OnUIInputTagReleased(const FGameplayTag& InputTag);
protected:
    UFUNCTION()
        void OnItemAssignedToInventory(const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag);

    UFUNCTION()
        void OnItemRemovedFromInventory(const FGuid& UniqueItemID);

    UFUNCTION()
        void OnInventoryItemUpdated(const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 UpdatedItemCount);
#pragma endregion

};