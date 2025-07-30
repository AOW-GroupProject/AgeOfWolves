#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "LoadingUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLoading, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UImage;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBlendAnimationCompleted);
#pragma endregion

/**
 *	@Loading UI
 * 
 *	레벨 전환에서 활용되는 대기 화면 연출
 */
UCLASS()
class AGEOFWOLVES_API ULoadingUI : public UUserWidget
{
	//@친추 클래스
#pragma region Friend Class
	friend class UUIManagerSubsystem;
#pragma endregion

	GENERATED_BODY()

	//@Defualt Setting
#pragma region Default Setting
public:
	ULoadingUI(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct()override;
	virtual void NativeDestruct()override;
	//~ End UUserWidget Interface
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
	UFUNCTION(BlueprintCallable, Category = "Loading UI",
		meta = (DisplayName = "Set Loading UI Visibility"))
	void SetLoadingUIVisibility(ESlateVisibility NewVisibility);

protected:
	// 이전 가시성 상태를 추적하여 의미있는 변화만 감지합니다
	ESlateVisibility PreviousVisibility = ESlateVisibility::Collapsed;

protected:
	//@Overlay
	UPROPERTY(BlueprintReadWrite, Category = "Loading UI | Overlay", meta = (BindWidget))
	UOverlay* LoadingUIOverlay;
	//@Loading UI 이미지
	UPROPERTY(BlueprintReadWrite, Category = "Loading UI | Image", meta = (BindWidget))
	UImage* LoadingImage;

protected:
	bool bIsWaitingForBlendOutAnimation = false;  // Blend Out 애니메이션 대기 중인지
	ESlateVisibility PendingVisibilityAfterAnimation = ESlateVisibility::Collapsed;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	// 블루프린트에서 애니메이션 완료를 알릴 수 있는 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "Loading UI Animation Events")
	FBlendAnimationCompleted OnBlendAnimationCompleted;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	UFUNCTION()
	void HandleVisibilityChanged(ESlateVisibility InVisibility);

	UFUNCTION(BlueprintImplementableEvent, Category = "Loading UI Events",
		meta = (DisplayName = "On Loading UI Shown"))
	void OnLoadingUIShown();

	UFUNCTION(BlueprintImplementableEvent, Category = "Loading UI Events",
		meta = (DisplayName = "On Loading UI Hidden"))
	void OnLoadingUIHidden();

protected:
	// 애니메이션 완료 후 실제 가시성 변경을 처리하는 콜백
	UFUNCTION()
	void OnBlendOutAnimationCompleted();
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	UFUNCTION(BlueprintPure, Category = "Loading UI",
		meta = (DisplayName = "Get Loading UI Visibility"))
	ESlateVisibility GetLoadingUIVisibility() const;

	// 특정 상태인지 확인하는 편의 함수들
	// 이러한 함수들은 코드의 가독성을 크게 향상시킵니다
	UFUNCTION(BlueprintPure, Category = "Loading UI",
		meta = (DisplayName = "Is Loading UI Visible"))
	bool IsLoadingUIVisible() const;

	UFUNCTION(BlueprintPure, Category = "Loading UI",
		meta = (DisplayName = "Is Loading UI Hidden"))
	bool IsLoadingUIHidden() const;
#pragma endregion
	
};
