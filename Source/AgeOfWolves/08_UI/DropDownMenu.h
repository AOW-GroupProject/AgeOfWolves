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
class UBaseGameplayAbility; 
#pragma endregion

#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByDropDownMenu)
//@초기화 완료 이벤트(초기화 작업 비동기화)
DECLARE_DELEGATE(FDropDownMenuInitFinished);
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
    FDropDownMenuOptionInformation(const FText& InOptionName, const FText& InOptionHotKeyText, TSoftObjectPtr<UTexture2D> InOptionHotKeyInfoBGImage, UBaseGameplayAbility* InOptionAbility = nullptr)
        : OptionName(InOptionName)
        , OptionHotKeyText(InOptionHotKeyText)
        , OptionHotKeyInfoBGImage(InOptionHotKeyInfoBGImage)
        , OptionAbility(InOptionAbility)
    {
        bHavingGA = (OptionAbility != nullptr);
    }

public:
    // Get 함수들
    FORCEINLINE const FText& GetOptionName() const { return OptionName; }
    FORCEINLINE const FText& GetOptionHotKeyText() const { return OptionHotKeyText; }
    FORCEINLINE TSoftObjectPtr<UTexture2D> GetOptionHotKeyInfoBGImage() const { return OptionHotKeyInfoBGImage; }
    FORCEINLINE UBaseGameplayAbility* GetOptionAbility() const { return OptionAbility; }
    FORCEINLINE bool IsHavingGA() const { return bHavingGA; }

private:
    //@Option 명
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        FText OptionName;

    //@Option의 단축키 텍스트
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        FText OptionHotKeyText;

    //@Option과 상호작용 가능한 키를 나타내는 배경 이미지
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        TSoftObjectPtr<UTexture2D> OptionHotKeyInfoBGImage;

    //@Option 클릭 시 활성화되는 GA
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        UBaseGameplayAbility* OptionAbility;

    //@Option의 GA 활성화 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option", meta = (AllowPrivateAccess = "true"))
        bool bHavingGA;
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
    //@friend class
    friend class UInteractableItemSlot;

    GENERATED_BODY()

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
    // 내부 바인딩 함수
    virtual void InternalBindToOptions(UDropDownMenuOption* Option);

public:
    //@초기화
    UFUNCTION()
        virtual void InitializeDropDownMenu();

#pragma endregion

#pragma region Subwidgets
protected:
    //@Reset
    void ResetDropDownMenu();
    //@Drop Down Menu Option 생성
    virtual void CreateDropDownMenuOptions();

protected:
    //@Open Drop Down Menu
    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu")
        void OpenDropDownMenu();
    //@Close Drop Down Menu
    UFUNCTION(BlueprintCallable, Category = "Drop Down Menu")
        void CloseDropDownMenu();

protected:
    //@BG Image
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UImage* DropDownMenuBGImage;
    //@Vertical Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UVerticalBox* DropDownMenuOptionBox;


protected:
    //@Drop Down Menu Option 정보를 담고 있는 구조체 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Down Menu Option | Option", meta = (AllowPrivateAccess = "true"))
        TArray<FDropDownMenuOptionInformation> OptionInformations;

    //@Drop Down Menu Option 블루프린트 클래스
    UPROPERTY(EditDefaultsOnly, category = "Drop Down Menu | Options")
        TSubclassOf<UDropDownMenuOption> DropDownMenuOptionClass;
#pragma endregion

#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByDropDownMenu RequestStartInitByDropDownMenu;
    //@초기화 완료 이벤트
    FDropDownMenuInitFinished DropDownMenuInitFinished;
#pragma endregion

#pragma region Callbacks;
protected:
    //@초기화 완료 이벤트
    UFUNCTION()
        void OnDropDownMenuOptionInitFinished();

protected:
    //@Option 선택 이벤트에 등록되는 콜백
    UFUNCTION()
        virtual void OnDropDownMenuOptionSelected(const FText& SelectedOptionText);
#pragma endregion

};