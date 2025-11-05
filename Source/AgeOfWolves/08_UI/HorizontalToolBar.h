#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HorizontalToolBar.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHorizontalToolBar, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UHorizontalBox;
class UCustomButton;
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_DELEGATE(FToolBarInitFinished)
#pragma endregion

/**
 * UHorizontalToolBar
 *
 * 수평 배치 버튼 툴바의 추상 기본 클래스
 * 파생 클래스에서 버튼 생성 및 선택 로직 구현 필요
 */
UCLASS(Abstract)
class AGEOFWOLVES_API UHorizontalToolBar : public UUserWidget
{
    GENERATED_BODY()

    //@Default Setting
#pragma region Default Setting
public:
    UHorizontalToolBar(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;

public:
    //@초기화
    UFUNCTION()
    virtual void InitializeToolBar();
#pragma endregion

    //@Property/Info...etc
#pragma region SubWidgets
protected:
    //@방향키 좌우 이동 편의 함수 (오버라이드 가능)
    virtual void MoveLeft();
    virtual void MoveRight();

    //@툴바 초기화 (파생 클래스에서 구현 필수)
    UFUNCTION(BlueprintCallable, Category = "Horizontal Tool Bar")
    virtual void ResetToolBar() PURE_VIRTUAL(UHorizontalToolBar::ResetToolBar, );

protected:
    //@버튼 생성 (파생 클래스에서 구현 필수)
    virtual void CreateButtons() PURE_VIRTUAL(UHorizontalToolBar::CreateButtons, );

    //@방향키 입력 시 선택 이동 (파생 클래스에서 구현 필수)
    virtual void MoveSelection(int32 Direction) PURE_VIRTUAL(UHorizontalToolBar::MoveSelection, );

protected:
    //@HorizontalBox (블루프린트 바인딩)
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
    UHorizontalBox* ButtonBox;

    //@기본 선택 인덱스
    uint8 DefaultButtonIndex;

    //@현재 선택된 인덱스
    uint8 CurrentSelectedIndex;
#pragma endregion

    //@Delegates
#pragma region Delegates
public:
    FToolBarInitFinished ToolBarInitFinished;
#pragma endregion

    //@Callbacks
#pragma region Callbacks
protected:
    //@버튼 이벤트 콜백 (파생 클래스에서 오버라이드)
    UFUNCTION(BlueprintNativeEvent)
    void OnToolBarButtonClicked(EInteractionMethod InteractionMethodType, uint8 ButtonIndex);
    virtual void OnToolBarButtonClicked_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex);

    UFUNCTION(BlueprintNativeEvent)
    void OnToolBarButtonHovered(EInteractionMethod InteractionMethodType, uint8 ButtonIndex);
    virtual void OnToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex);

    UFUNCTION(BlueprintNativeEvent)
    void OnToolBarButtonUnhovered(uint8 ButtonIndex);
    virtual void OnToolBarButtonUnhovered_Implementation(uint8 ButtonIndex);

    UFUNCTION(BlueprintNativeEvent)
    void CancelToolBarButtonSelected(uint8 PreviousIndex);
    virtual void CancelToolBarButtonSelected_Implementation(uint8 PreviousIndex);
#pragma endregion

    //@Utility
#pragma region Utility
protected:
    //@인덱스 유효성 검사 (파생 클래스에서 구현 필수)
    virtual bool IsValidButtonIndex(uint8 Index) const PURE_VIRTUAL(UHorizontalToolBar::IsValidButtonIndex, return false;);

    //@특정 인덱스의 버튼 가져오기 (파생 클래스에서 구현 필수)
    virtual UCustomButton* GetButtonByIndex(uint8 Index) const PURE_VIRTUAL(UHorizontalToolBar::GetButtonByIndex, return nullptr;);
#pragma endregion
};