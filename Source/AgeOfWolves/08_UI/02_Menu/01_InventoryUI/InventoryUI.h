#pragma once

#include "CoreMinimal.h"
#include "08_UI/02_Menu/MenuUIContent.h"
#include "GameplayTagContainer.h"
#include "09_Item/Item.h"

#include "InventoryUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventoryUI, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UImage;
class UInventoryUIContent;
class UItemSlots;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByInventoryUI)
//@초기화 완료 이벤트
DECLARE_DELEGATE(FInventoryUIInitFinished)
#pragma endregion

UCLASS()
class AGEOFWOLVES_API UInventoryUI : public UMenuUIContent
{
    //@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting        
#pragma region Default Setting
public:
    UInventoryUI(const FObjectInitializer& ObjectInitializer);

protected:
    //Interface of UUserWidget
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
    //~End UUserWidget

protected:
    //@외부 바인딩

protected:
    //@내부 바인딩
    void InternalBindingToInventoryUIContent(UInventoryUIContent* Content);

protected:
    //@초기화 함수
    virtual void InitializeMenuUIContent() override;

protected:
    //@Invenotry UI의 내부 컨텐츠 초기화 완료 체크
    bool bInventoryUIContentReady = false;
    virtual void CheckMenuUIContentInitFinished() override;
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
private:
    //@Inventory UI의 상태를 초기 상태로 되돌립니다.
    virtual void ResetMenuUIContent() override;

protected:
    //@Item Slots
    void CreateInventoryContent();

protected:
    //@Item Slots를 담을 Scale Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UOverlay* InventoryUIContentOverlay;

    //@Inventory UI Content
    TObjectPtr<UInventoryUIContent> InventoryUIContent;

    //@Inventory UI Content의 BP 클래스
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI | Inventory UI Content")
        TSubclassOf<UInventoryUIContent> InventoryUIContentClass;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByInventoryUI RequestStartInitByInventoryUI;
    //@초기화 완료 이벤트
    FInventoryUIInitFinished InventoryUIInitFinished;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@Inventory UI Content의 초기화 완료 이벤트 구독
    UFUNCTION()
        void OnInventoryUIContentInitFinished();

protected:
    //@UI의 가시성 변화 이벤트
    virtual void OnUIVisibilityChanged_Implementation(ESlateVisibility VisibilityType) override;
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    UItemSlots* GetItemSlotsByType(EItemType ItemType) const;
#pragma endregion

};