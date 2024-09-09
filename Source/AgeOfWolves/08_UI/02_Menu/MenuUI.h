#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"

#include "MenuUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMenuUI, Log, All)

#pragma region Forward Declaration
class UImage;
class UTextBlock;
class UOverlay;

class UMenuUIContent;
class UMenuUIToolBar;
#pragma endregion

#pragma region Delegates 
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByMenuUI);
//@초기화 완료 이벤트
DECLARE_DELEGATE(FMenuUIInitFinished)

//@Inventory UI 초기화 완료 알림 이벤트
DECLARE_DELEGATE(FNotifyInventoryUIInitFinished);
#pragma endregion

#pragma region Enums
/*
* @EMenuCategory
* 
* Menu에서 제공하는 총 4개의 카테고리를 정의하는 열거형
*/
UENUM(BlueprintType)
enum class EMenuCategory : uint8
{
    Inventory = 0	UMETA(DisplayName = "Inventory"),
    Level		    UMETA(DisplayName = "Level"),
    Map             UMETA(DisplayName = "Map"),
    System          UMETA(DisplayName = "System"),
    MAX,
};

/*
* @FMenuContentUI
* 
* Menu UI 내부 콘텐츠의 정보를 담고 있는 구조체
*/
USTRUCT(BlueprintType)
struct FMenuUIContentInfo: public FTableRowBase
{
  GENERATED_BODY()
public:
    //@해당 컨텐츠를 나타낼 메뉴 카테고리
    UPROPERTY(EditAnywhere, Category = "Menu Content UI | Category")
        EMenuCategory MenuCategory;
    //@블루프린트 클래스
    UPROPERTY(EditAnywhere, Category = "Menu Content UI | Content")
        TSubclassOf<UMenuUIContent> MenuUIContentClass;
};
#pragma endregion

/**
 * UMenuUI
 *
 * Menu 관련 UI들의 프레임을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UMenuUI : public UUserWidget
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UMenuUI(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~ End UUserWidget Interface

protected:
    //@외부 바인딩
    void ExternalBindToInputComponent();
    void ExternalBindToUIComponent();

protected:
    // 내부 바인딩 함수
    void InternalBindingToToolBar(UMenuUIToolBar* ToolBar);
    void InternalBindingToMenuUIContent(UMenuUIContent* MenuUIContent);

public:
    //@초기화
    UFUNCTION()
        void InitializeMenuUI();

protected:
    //@Menu UI의 모든 구성 UI들의 초기화 완료 체크 함수
    void CheckAllUIsInitFinsiehd();

    bool bMenuToolBarInitFinished = false;
    void CheckMenuToolBarInitFinished();

    //@Inventory UI 초기화 완료 체크 함수
    bool bInventoryUIInitFinished= false;
    void CheckInventoryUIInitFinished();

    //@Level UI 초기화 완료 체크 함수
    bool bLevelUIInitFinished = false;
    void CheckLevelUIInitFinished();
    
    //@Map UI 초기화 완료 체크 함수
    bool bMapUIInitFinished = false;
    void CheckMapUIInitFinished();

    //#System UI 초기화 완료 체크 함수
    bool bSystemUIInitFinished = false;
    void CheckSystemUIInitFinished();
#pragma endregion

#pragma region SubWidgets
protected:
    //@Menu UI 설정 리셋
    void ResetMenuUI();
    //@Tool Bar 생성
    void CreateToolBar();
    //@Menu UI 컨텐츠 생성
    void CreateAllCategoryUIs();

protected:
    //@BG 이미지
    UPROPERTY(BlueprintReadWrite, Category = "Menu", meta = (BindWidget))
        UImage* MenuUI_Outer_BG;
    UPROPERTY(BlueprintReadWrite, Category = "Menu", meta = (BindWidget))
        UImage* MenuUI_Inner_BG;

protected:
    //@Tool Bar UI
    UPROPERTY(BlueprintReadWrite, Category = "Menu | Tool Bar UI", meta = (BindWidget))
        UOverlay* ToolBarOverlay;
    UPROPERTY(EditDefaultsOnly, Category = "Menu | Tool Bar")
        TSubclassOf<UMenuUIToolBar> ToolBarClass;
    
protected:
    //@Menu UI Content를 담을 Overlay
    UPROPERTY(BlueprintReadWrite, Category = "Menu", meta = (BindWidget))
        UOverlay* MenuUIContentOverlay;

    //@초기 Menu UI가 나타낼 Menu Category
    EMenuCategory DefaultCategory = EMenuCategory::Inventory;
    //@현재 보여지고 있는 Menu UI의 자식 UI
    EMenuCategory CurrentCategory = EMenuCategory::MAX;
    //@Menu UI 내부 컨텐츠를 표시할 UI
    UPROPERTY(EditDefaultsOnly, Category = "Menu | Content")
        TArray<FMenuUIContentInfo> MenuContent;
    //@카테고리와 위젯을 매핑하는 맵
    TMap<EMenuCategory, TObjectPtr<UUserWidget>> MMenuContents;
#pragma endregion

#pragma region Utility
public:
    //@가시성 관리
    void SetCategoryVisibility(EMenuCategory Category, bool bVisible);
    //@ToolBar UI 가져오기
    UFUNCTION(BlueprintCallable, Category = "Menu")
        UMenuUIToolBar* GetToolBarUI() const;
    //@카테고리별 UI 가져오기
    UFUNCTION(BlueprintCallable, Category = "Menu")
        UUserWidget* GetCategoryUI(EMenuCategory Category) const;
#pragma endregion

#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByMenuUI RequestStartInitByMenuUI;
    //@초기화 완료 이벤트
    FMenuUIInitFinished MenuUIInitFinished;

public:
    //@Inventory UI의 초기화 완료 이벤트
    FNotifyInventoryUIInitFinished NotifyInventoryUIInitFinished;
#pragma endregion

#pragma region Callbacks
protected:
    //@UI의 가시성 변화 이벤트에 바인딩 되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnUIVisibilityChanged(UUserWidget* Widget, bool bVisible);
    virtual void OnUIVisibilityChanged_Implementation(UUserWidget* Widget, bool bVisible);

protected:
    //@Input Tag 활성화 이벤트에 등록하는 콜백
    UFUNCTION()
        void OnUIInputTagTriggered(const FGameplayTag& InputTag);
    //@Input Tag 해제 이벤트에 등록되는 콜백
    UFUNCTION()
        void OnUIInputTagReleased(const FGameplayTag& InputTag);

protected:
    //@툴바 초기화 완료 이벤트 구독
    UFUNCTION()
        void OnToolBarInitFinished();
    //@Menu UI Content 초기화 완료 이벤트 구독
    UFUNCTION()
        void OnMenuUIContentInitFinished(EMenuCategory Category);

protected:
    //@Menu Tool Bar의 Menu Category 선택 이벤트에 바인딩 되는 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnMenuCategoryButtonClikced(EMenuCategory MenuCategory);
    virtual void OnMenuCategoryButtonClikced_Implementation(EMenuCategory MenuCategory);
#pragma endregion
};