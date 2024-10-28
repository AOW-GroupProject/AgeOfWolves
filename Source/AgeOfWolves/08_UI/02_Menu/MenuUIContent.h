#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuUI.h"

#include "MenuUIContent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMenuUIContent, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UImage;
class UMenuUI;
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
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByMenuUIContent)
//@초기화 완료 이벤트
DECLARE_DELEGATE_OneParam(FMenuUIContentInitFinished, EMenuCategory);
#pragma endregion

/**
 * @UMenuContentUI
 * 
 * Menu UI 내부 컨텐츠를 나타내는 UI를 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UMenuUIContent : public UUserWidget
{

//@친추 클래스
#pragma region Friend Class
    friend class UMenuUI;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UMenuUIContent(const FObjectInitializer& ObjectInitializer);

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

public:
    //@초기화
    UFUNCTION()
        virtual void InitializeMenuUIContent() {}

protected:
    //@초기화 완료 확인 함수
    virtual void CheckMenuUIContentInitFinished();
#pragma endregion

//@Property/Info...etc
#pragma region Info
protected:
    virtual void ResetMenuUIContent() {}

protected:
    //@Menu Content의 타이틀 이미지
    UPROPERTY(BlueprintReadWrite, Category = "Menu Content | Title Image", meta = (BindWidget))
        UImage* TitleImage;

protected:
    //@Menu Content의 Category를 설정합니다.
    //@참조: InitializeMenuUIContent()
    EMenuCategory MenuCategory = EMenuCategory::MAX;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByMenuUIContent RequestStartInitByMenuUIContent;
    //@초기화 완료 이벤트
    FMenuUIContentInitFinished MenuUIContentInitFinished;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@가시성 변화 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnUIVisibilityChanged(ESlateVisibility VisibilityType);
    virtual void OnUIVisibilityChanged_Implementation(ESlateVisibility VisibilityType);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
	
};
