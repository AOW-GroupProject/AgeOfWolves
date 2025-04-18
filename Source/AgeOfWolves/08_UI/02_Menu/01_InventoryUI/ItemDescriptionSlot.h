// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "08_UI/ItemSlot.h"
#include "GameplayTagContainer.h"

#include "ItemDescriptionSlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemDescription, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UInventoryUIContent;
class UItemSlots;
class UVerticalBox;
class UEditableTextBox;
class UMultiLineEditableTextBox;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 완료 이벤트(초기화 작업 비동기화)
DECLARE_DELEGATE(FItemDescriptionSlotInitFinished);
#pragma endregion

/**
 * @UItemDescriptionSlot
 *
 * 현재 선택된 Item에 대한 간략한 정보를 나타내는 UI
 */
UCLASS()
class AGEOFWOLVES_API UItemDescriptionSlot : public UItemSlot
{

//@친추 클래스
#pragma region Friend Class
    friend class UInventoryUIContent;
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UItemDescriptionSlot(const FObjectInitializer& ObjectInitializer);

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
    void ExternalBindToInventoryComp();

protected:
    //@내부 바인딩

public:
    //@초기화
    UFUNCTION()
        void InitializeItemDescriptionSlot();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    void ResetItemDescriptionSlot();

public:
    //@퀵슬롯에 새로운 아이템을 할당합니다.
    virtual void AssignNewItem_Implementation(const FGuid& ID, FItemInformation ItemInformation, int32 ItemCount = -1) override;
    //@퀵슬롯에 할당된 기존 아이템을 제거합니다.
    void ClearAssignedItem(bool bForceClear = false);

protected:
    FString ArrangeItemDescriptionStringToText(FString String);

protected:
    //@수직 상자
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UVerticalBox* ItemDescriptionBox;

protected:
    //@아이템 이름 표시하는 Text Block
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UEditableTextBox* ItemNameTextBox;
    //@아이템 설명 표시하는 Text Block
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UMultiLineEditableTextBox* ItemDescriptionTextBox;

protected:
    //@FGuid에 대응되는 FTemporaryItemInformation를 갖는 TMap
    TMap<FGuid, FItemInformation> MItemsInInventoryItemSlots;

protected:
    //@Blend-In 애니메이션
    UPROPERTY(Transient, BlueprintReadWrite, meta = (BindWidgetAnim))
        TObjectPtr<UWidgetAnimation> BlendInAnimation;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 완료 이벤트
    FItemDescriptionSlotInitFinished ItemDescriptionSlotInitFinished;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@Item Slots의 바인딩 준비 완료 이벤트
    UFUNCTION()
        void OnItemSlotsReadyForBinding(const UInventoryUIContent* InventoryUIContent);

protected:
    //@Inventory Comp의 아이템 할당 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnItemAssignedToInventory(const FGuid& ID, EItemType Type, const FGameplayTag& ItemTag);
    //@Inventory Comp의 아이템 제거 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnItemRemovedFromInventory(const FGuid& ID);

protected:
    UFUNCTION(BlueprintNativeEvent)
        void OnItemSlotButtonHovered(const FGuid& ID, EInteractionMethod InteractionMethodType);
    virtual void OnItemSlotButtonHovered_Implementation(const FGuid& ID, EInteractionMethod InteractionMethodType);
    UFUNCTION(BlueprintNativeEvent)
        void OnItemSlotButtonUnhovered(const FGuid& ID);
    virtual void OnItemSlotButtonUnhovered_Implementation(const FGuid& ID);
    UFUNCTION(BlueprintNativeEvent)
        void OnItemSlotButtonCanceled(const FGuid& ID, EInteractionMethod InteractionMethodType);
    virtual void OnItemSlotButtonCanceled_Implementation(const FGuid& ID, EInteractionMethod InteractionMethodType);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

};