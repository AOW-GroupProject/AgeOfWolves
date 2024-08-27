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

protected:
    //@내부 바인딩
    void InternalBindingToInventoryUIContent(UInventoryUIContent* InventoryUIContent);
protected:
    //~UMenuUIContent Interface
    //@초기화 함수
    virtual void InitializeMenuUIContent() override;
    //~End UMenuUIContent Interface

protected:
    bool bInventoryUIContentReady= false;
    virtual void CheckMenuUIContentInitFinished() override;
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
    UPROPERTY(EditDefaultsOnly, Category = "Inventory UI | Inventory UI Content")
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
    //~UMenuUIContent Interface
    virtual void OnUIVisibilityChanged(ESlateVisibility VisibilityType) override;
    //~End UMenuUIContent Interface
#pragma endregion

};