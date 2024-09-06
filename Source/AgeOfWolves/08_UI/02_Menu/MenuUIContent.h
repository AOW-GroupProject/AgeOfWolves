#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuUI.h"

#include "MenuUIContent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMenuUIContent, Log, All)

class UImage;
class UMenuUI;

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByMenuUIContent)
//@초기화 완료 이벤트
DECLARE_DELEGATE_OneParam(FMenuUIContentInitFinished, EMenuCategory);

/**
 * @UMenuContentUI
 * 
 * Menu UI 내부 컨텐츠를 나타내는 UI를 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UMenuUIContent : public UUserWidget
{
    //@Friend Class
    friend class UMenuUI;

	GENERATED_BODY()
#pragma region Default Setting
public:
    UMenuUIContent(const FObjectInitializer& ObjectInitializer);

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

public:
    //@초기화
    //@오버라이딩 시 UFUNCTION() 제외
    UFUNCTION()
        virtual void InitializeMenuUIContent();
protected:
    //@TODO: 각 멤버들의 초기화 완료 여불르 확인하는 boolean 변수 선언
    //@초기화 완료 확인 함수
    virtual void CheckMenuUIContentInitFinished();
#pragma endregion

#pragma region Info
protected:
    //@Menu Content의 Category를 설정합니다.
    //@참조: InitializeMenuUIContent()
    EMenuCategory MenuCategory = EMenuCategory::MAX;
#pragma endregion

#pragma region SubWidgets
protected:
    virtual void ResetMenuUIContent() {}

protected:
    //@Menu Content의 타이틀 이미지
    UPROPERTY(BlueprintReadWrite, Category = "Menu Content | Title Image", meta = (BindWidget))
        UImage* TitleImage;
#pragma endregion

#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByMenuUIContent RequestStartInitByMenuUIContent;
    //@초기화 완료 이벤트
    FMenuUIContentInitFinished MenuUIContentInitFinished;
#pragma endregion

#pragma region Callbacks
protected:
    UFUNCTION(BlueprintNativeEvent)
        void OnUIVisibilityChanged(ESlateVisibility VisibilityType);
    virtual void OnUIVisibilityChanged_Implementation(ESlateVisibility VisibilityType);


#pragma endregion
	
};
