#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/DataTable.h"

#include "DropDownMenu.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDropDownMenu, Log, All)

#pragma region Forward Declaration
class UVerticalBox;
class UImage;
class USizeBox;
class UOverlay;
class UDropDownMenuOption;
class UConfirmationMenu;
#pragma endregion

#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByDropDownMenu)
//@초기화 완료 이벤트(초기화 작업 비동기화)
DECLARE_DELEGATE(FDropDownMenuInitFinished)

//@옵션 버튼 선택 알림 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FDropDownMenuOptionButtonClicked, const FName&)

//@버튼 취소 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FCancelDropDownMenuOptionButton, FName)
#pragma endregion

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
    FDropDownMenuOptionInformation(TSubclassOf<UDropDownMenuOption> InOptionClass, const FName& InOptionName, const FText& InOptionHotKeyText, TSoftObjectPtr<UTexture2D> InOptionHotKeyInfoBGImage)
        : OptionClass(InOptionClass)
        , OptionName(InOptionName)
        , OptionHotKeyText(InOptionHotKeyText)
        , OptionHotKeyInfoBGImage(InOptionHotKeyInfoBGImage)
    {}

    FORCEINLINE bool CompareOptionName(const FName& OtherName) const
    {
        return OptionName == OtherName;
    }

public:
    FORCEINLINE TSubclassOf<UDropDownMenuOption> GetOptionClass() const { return OptionClass; }
    FORCEINLINE const FName& GetOptionName() const { return OptionName; }
    FORCEINLINE const FText& GetOptionHotKeyText() const { return OptionHotKeyText; }
    FORCEINLINE TSoftObjectPtr<UTexture2D> GetOptionHotKeyInfoBGImage() const { return OptionHotKeyInfoBGImage; }
    FORCEINLINE const FText& GetConfirmationMenuDialogueText() const { return ConfirmationMenuDialogueText; }


private:
    //@Option 위젯 클래스
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        TSubclassOf<UDropDownMenuOption> OptionClass;

    //@Option 명
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        FName OptionName;

    //@Option의 단축키 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        FText OptionHotKeyText;

    //@Option과 상호작용 가능한 키를 나타내는 배경 이미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        TSoftObjectPtr<UTexture2D> OptionHotKeyInfoBGImage;

    //@Option 클릭 시 열리는 Confirmation Menu의 Dialogue Box에 나타낼 설명 문
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        FText ConfirmationMenuDialogueText;
};
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
    //~ End UUserWidget Interface

protected:
    //@외부 바인딩

protected:
    //@내부 바인딩
    void InternalBindToOptions(UDropDownMenuOption* Option, const FName& OptionName, bool bIsLastOption);

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
    void ResetDropDownMenu();
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
    //@Drop Down Menu Option 정보를 담고 있는 구조체 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option | Option", meta = (AllowPrivateAccess = "true"))
        TArray<FDropDownMenuOptionInformation> OptionInformations;

protected:
    //@현재 선택된 Drop Down Menu Option
    FName CurrentSelectedOption;

#pragma endregion

//@Delegates
#pragma region Delegates
//@초기화
public:
    //@초기화 요청 이벤트
    FRequestStartInitByDropDownMenu RequestStartInitByDropDownMenu;
    //@초기화 완료 이벤트
    FDropDownMenuInitFinished DropDownMenuInitFinished;

public:
    //@Drop Down Menu Option의 버튼 클릭 이벤트
    FDropDownMenuOptionButtonClicked DropDownMenuOptionButtonClicked;


    //@Option Button
public:
    //@Drop Down Menu Option 버튼 선택 취소 이벤트
    FCancelDropDownMenuOptionButton CancelDropDownMenuOptionButton;
#pragma endregion

//@Callbacks
#pragma region Callbacks;
protected:
    //@초기화 완료 이벤트
    UFUNCTION()
        void OnDropDownMenuOptionInitFinished();

protected:
    //@Option 선택 이벤트에 등록되는 콜백, 각 Drop Down Menu 에서 제공하는 옵션들에 대한 기능들 오버라이딩 필수!
    UFUNCTION()
        virtual void OnDropDownMenuOptionSelected(FName SelectedOptionName);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility Functions
public:
    const FText GetConfirmationMenuDialogueText(const FName& Name) const;
#pragma endregion
};