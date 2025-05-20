#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"

#include "DropDownMenu.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDropDownMenu, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UVerticalBox;
class UImage;
class USizeBox;
class UOverlay;
class UDropDownMenuOption;
class UConfirmationMenu;
#pragma endregion

//@열거형
#pragma region Enums
/*
* @EHotKey
* 
* Drop Down Menu 옵션 선택 시 활용 가능한 단축키 목록
*/
UENUM(BlueprintType)
enum class EHotKey : uint8
{
    None    UMETA(DisplayName = "None"),
    Q       UMETA(DisplayName = "Q"),
    W       UMETA(DisplayName = "W"),
    E       UMETA(DisplayName = "E"),
    R       UMETA(DisplayName = "R"),
    T       UMETA(DisplayName = "T"),
};
#pragma endregion

//@구조체
#pragma region Structs
/*
* @FDropDownMenuOptionInformation
*
* Drop Down Menu가 가질 Option의 정보를 담고 있는 구조체
*/
USTRUCT(BlueprintType)
struct FDropDownMenuOptionInformation : public FTableRowBase
{
    GENERATED_BODY()

public:
    FDropDownMenuOptionInformation() {}
    FDropDownMenuOptionInformation(TSubclassOf<UDropDownMenuOption> InOptionClass, const FName& InOptionName, EHotKey InHotKey, TSoftObjectPtr<UTexture2D> InOptionHotKeyInfoBGImage)
        : OptionClass(InOptionClass)
        , OptionName(InOptionName)
        , HotKey(InHotKey)
        , OptionHotKeyInfoBGImage(InOptionHotKeyInfoBGImage)
    {}

    FORCEINLINE bool CompareOptionName(const FName& OtherName) const
    {
        return OptionName == OtherName;
    }

public:
    FORCEINLINE TSubclassOf<UDropDownMenuOption> GetOptionClass() const { return OptionClass; }
    FORCEINLINE const FName& GetOptionName() const { return OptionName; }
    FORCEINLINE EHotKey GetHotKey() const { return HotKey; }
    FORCEINLINE FText GetOptionHotKeyText() const { return GetHotKeyAsText(HotKey); }
    FORCEINLINE TSoftObjectPtr<UTexture2D> GetOptionHotKeyInfoBGImage() const { return OptionHotKeyInfoBGImage; }
    FORCEINLINE const FText& GetConfirmationMenuDialogueText() const { return ConfirmationMenuDialogueText; }

private:
    //@Option 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<UDropDownMenuOption> OptionClass;

    //@Option 명
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        FName OptionName;

    //@Option의 단축키
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        EHotKey HotKey;

    //@Option과 상호작용 가능한 키를 나타내는 배경 이미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        TSoftObjectPtr<UTexture2D> OptionHotKeyInfoBGImage;

    //@Option 클릭 시 열리는 Confirmation Menu의 Dialogue Box에 나타낼 설명 문
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        FText ConfirmationMenuDialogueText;

    //@EHotKey -> FText
    static FText GetHotKeyAsText(EHotKey Key)
    {
        switch (Key)
        {
        case EHotKey::Q: return FText::FromString("Q");
        case EHotKey::W: return FText::FromString("W");
        case EHotKey::E: return FText::FromString("E");
        case EHotKey::R: return FText::FromString("R");
        case EHotKey::T: return FText::FromString("T");
        default: return FText::FromString("None");
        }
    }
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByDropDownMenu)
//@초기화 완료 이벤트(초기화 작업 비동기화)
DECLARE_DELEGATE(FDropDownMenuInitFinished)

//@옵션 버튼 선택 알림 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FDropDownMenuOptionButtonClicked, FName)

//@버튼 취소 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FCancelDropDownMenuOptionButton, FName)
#pragma endregion

/**
 * @UDropDownMenu
 *
 * Drop Down Menu를 구현합니다.
 */
UCLASS()
class AGEOFWOLVES_API UDropDownMenu : public UUserWidget
{
//@친추 클래스
#pragma region Friend Class
    friend class UItemSlots;
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UDropDownMenu(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
    virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
    //~ End UUserWidget Interface

protected:
    //@외부 바인딩

protected:
    //@내부 바인딩
    virtual void InternalBindToOptions(UDropDownMenuOption* Option, const FName& OptionName, bool bIsLastOption);

public:
    //@초기화
    UFUNCTION()
        virtual void InitializeDropDownMenu();

protected:
    //@초기화 완료 체크
    bool bOptionsInitFinished = false;
    void CheckAllUIsInitFinished();
#pragma endregion

//@Property/Info...etc
#pragma region Subwidgets
protected:
    //@Reset
    virtual void ResetDropDownMenu();
    //@Drop Down Menu Option 생성
    virtual void CreateDropDownMenuOptions();

protected:
    //@Open Drop Down Menu
    UFUNCTION(BlueprintNativeEvent, Category = "Drop Down Menu")
        void OpenDropDownMenu();
    virtual void OpenDropDownMenu_Implementation();

    //@Close Drop Down Menu
    UFUNCTION(BlueprintNativeEvent, Category = "Drop Down Menu")
        void CloseDropDownMenu();
    virtual void CloseDropDownMenu_Implementation();

    //@BG
protected:
    //@BG Image
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UImage* DropDownMenuBGImage;

protected:
    //@Vertical Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UVerticalBox* DropDownMenuOptionBox;
    
protected:
    //@Drop Down Menu Option 목록
    UPROPERTY()
        TArray<UDropDownMenuOption*> DropDownMenuOptions;

    //@Drop Down Menu Option 정보를 담고 있는 구조체 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option | Option", meta = (AllowPrivateAccess = "true"))
        TArray<FDropDownMenuOptionInformation> OptionInformations;

protected:
    //@현재 선택된 Drop Down Menu Option
    FName CurrentSelectedOptionName;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByDropDownMenu RequestStartInitByDropDownMenu;
    //@초기화 완료 이벤트
    FDropDownMenuInitFinished DropDownMenuInitFinished;

public:
    //@Drop Down Menu Option의 버튼 클릭 이벤트
    FDropDownMenuOptionButtonClicked DropDownMenuOptionButtonClicked;

public:
    //@Drop Down Menu Option 버튼 선택 취소 이벤트
    FCancelDropDownMenuOptionButton CancelDropDownMenuOptionButton;
#pragma endregion

//@Callbacks
#pragma region Callbacks;
protected:
    //@가시성 변화 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnUIVisibilityChanged(ESlateVisibility VisibilityType);
    virtual void OnUIVisibilityChanged_Implementation(ESlateVisibility VisibilityType);

protected:
    //@초기화 완료 이벤트
    UFUNCTION()
        void OnDropDownMenuOptionInitFinished();

protected:
    //@Option 선택 이벤트에 등록되는 콜백, 각 Drop Down Menu 에서 제공하는 옵션들에 대한 기능들 오버라이딩 필수!
    UFUNCTION(BlueprintNativeEvent)
        void OnDropDownMenuOptionSelected(FName SelectedOptionName, EInteractionMethod InteractionMethodType);
    virtual void OnDropDownMenuOptionSelected_Implementation(FName SelectedOptionName, EInteractionMethod InteractionMethodType);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility Functions
public:
    //@첫 번째 Drop Down Menu Option을 반환
    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu")
        UDropDownMenuOption* GetFirstDropDownMenuOption() const;

    //@특정 이름의 Drop Down Menu Option을 반환하는 함수
    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu")
        UDropDownMenuOption* GetDropDownMenuOptionByName(const FName& OptionName) const;

    //@Option의 이름을 가져오는 함수 (DropDownMenuOption 클래스에 추가)
    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu Option")
        FName GetOptionName() const;
public:
    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu")
        const FText GetConfirmationMenuDialogueText(const FName& Name) const;
#pragma endregion

};