// Fill out your copyright notice in the Description page of Project Settings.

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

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 완료 이벤트
DECLARE_DELEGATE(FToolBarInitFinished)
#pragma endregion

/**
 * UHorizontalToolBar
 *
 * 수평으로 배치된 버튼들을 관리하는 기본 툴바 클래스입니다.
 * 버튼의 생성, 이동, 선택 관리 등의 공통 기능을 제공합니다.
 */
    UCLASS(Abstract)
    class AGEOFWOLVES_API UHorizontalToolBar : public UUserWidget
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

        //@Default Setting
#pragma region Default Setting
public:
    UHorizontalToolBar(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interface
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FNavigationReply NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply) override;
    //~ End UUserWidget Interface

protected:
    //@내부 바인딩 - 파생 클래스에서 구현
    virtual void InternalBindToButton(UCustomButton* Button) {};

public:
    //@초기화
    UFUNCTION()
        virtual void InitializeToolBar();

protected:
    //@초기화 완료 체크
    void CheckToolBarInitialization();
#pragma endregion

    //@Property/Info...etc
#pragma region SubWidgets
public:
    //@툴바를 초기 상태로 리셋
    UFUNCTION(BlueprintCallable, Category = "Horizontal Tool Bar")
        virtual void ResetToolBar() {}

protected:
    //@버튼 생성
    virtual void CreateButtons();

protected:
    //@좌우 이동 함수
    virtual void MoveLeft();
    virtual void MoveRight();
    virtual void MoveSelection(int32 Direction) {}

protected:
    //@Tool Bar의 Horizontal Box
    UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
        UHorizontalBox* ButtonBox;

protected:
    //@Map: 버튼들을 담는 컨테이너. 파생 클래스에서 적절한 타입으로 정의
    UPROPERTY()
        TMap<uint8, UCustomButton*> MButtons;

    //@Default Button Index
    uint8 DefaultButtonIndex;
    //@Current Selected Button Index
    uint8 CurrentSelectedIndex;
#pragma endregion

    //@Delegates
#pragma region Delegates
public:
    //@Tool Bar의 초기화 완료 이벤트
    FToolBarInitFinished ToolBarInitFinished;
#pragma endregion

    //@Callbacks
#pragma region Callbacks
protected:
    //@버튼 클릭 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnToolBarButtonClicked(EInteractionMethod InteractionMethodType, uint8 ButtonIndex);
    virtual void OnToolBarButtonClicked_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex);

    //@버튼 호버 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnToolBarButtonHovered(EInteractionMethod InteractionMethodType, uint8 ButtonIndex);
    virtual void OnToolBarButtonHovered_Implementation(EInteractionMethod InteractionMethodType, uint8 ButtonIndex);

    //@버튼 언호버 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void OnToolBarButtonUnhovered(uint8 ButtonIndex);
    virtual void OnToolBarButtonUnhovered_Implementation(uint8 ButtonIndex);

protected:
    //@버튼 선택 취소 이벤트 구독
    UFUNCTION(BlueprintNativeEvent)
        void CancelToolBarButtonSelected(uint8 PreviousIndex);
    virtual void CancelToolBarButtonSelected_Implementation(uint8 PreviousIndex);
#pragma endregion

    //@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
    //@현재 선택된 버튼의 인덱스 반환
    virtual int32 GetCurrentButtonIndex() const;

    //@인덱스의 유효성 검사 (파생 클래스에서 재정의)
    virtual bool IsValidButtonIndex(uint8 Index) const { return false; }
#pragma endregion

};