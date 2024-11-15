// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"

#include "HUD_StatusUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStatusUI, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UVerticalBox;
class UHorizontalBox;
class UProgressBar;
class UAsyncTaskAttributeChanged;
class UAbilitySystemComponent;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_DELEGATE(FStatusUIInitFinished);
#pragma endregion

/**
 * @UHUD_StatusUI
 * 
 * HUD의 캐릭터 상태 창을 나타내는 UI입니다.
 * 
 * 2개의 Status Bar와 Memmory(Item) 관련 이미지로 구성합니다.
 */
UCLASS()
class AGEOFWOLVES_API UHUD_StatusUI : public UUserWidget
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UHUD_StatusUI(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    //@외부 바인딩
    void ExternalBindingToAttributeSet();

public:
    //@초기화
    UFUNCTION()
        void InitializeStatusUI();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    //@Memory 아이템 관련 이미지 생성 및 설정 작업
    void CreateAndSetupMemoryImage();
    void CreateManaCountImages();

protected:
    UPROPERTY(BlueprintReadWrite, Category = "Status UI", meta = (BindWidget))
        UVerticalBox* StateBarVerticalBox;

    UPROPERTY(BlueprintReadWrite, Category = "Status UI | Status Bar", meta = (BindWidget))
        UProgressBar* HP;

    UPROPERTY(BlueprintReadWrite, Category = "Status UI | Status Bar", meta = (BindWidget))
        UProgressBar* SP;

protected:
    UPROPERTY(BlueprintReadWrite, Category = "Status UI | Mana", meta = (BindWidget))
        UHorizontalBox* ManaCountBox;

protected:
    //@FGmaeplayAttirbute & ProgressBar 
    TMap<FString, UProgressBar*> MStateBars;

    //@능력치 속성 변화 이벤트 구독 Async Task 목록
    UPROPERTY()
        TArray<UAsyncTaskAttributeChanged*> AttributeListeners;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@Status UI의 초기화 완료 이벤트
    FStatusUIInitFinished StatusUIInitFinished;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    UFUNCTION()
        void OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
    //@ASC 캐시
    UPROPERTY()
        TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
#pragma endregion
	
};
