#pragma once

#include "CoreMinimal.h"
#include "08_UI/ItemSlot.h"

#include "InteractableItemSlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteractableItemSlot, Log, All)

UENUM(BlueprintType)
enum class EItemSlotButtonState : uint8
{
	Normal = 0,
	Hovered,
	Selected,
	MAX
};

class UButton;

//@아이템 슬롯 버튼 클릭 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FItemSlotButtonClicked,  const FGuid&)

/**
 * @UInteractableItemSlot
 * 
 * Item Slot 중 사용자와 상호작용 가능한 Item Slot을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UInteractableItemSlot : public UItemSlot
{
	GENERATED_BODY()

		//@Friend Class 설정
		friend class UItemSlots;

#pragma region Default Setting
public:
	UInteractableItemSlot(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface

protected:
	//@내부 바인딩
	void InternalBindToItemSlotButton(UButton* Button);

public:
	virtual void InitializeItemSlot() override;
#pragma endregion

#pragma region SubWidgets
protected:
	//@버튼 생성
	void CreateButton();
	//@드롭다운 메뉴 생성
	void CreateDropDownMenu();

protected:
	//@Slot Overlay에 사용자 상호작용 가능한 버튼 UI
	UPROPERTY(BlueprintReadWrite, Category = "Item Slot", meta = (BindWidget))
		UButton* ItemSlotButton;
	UPROPERTY(EditDefaultsOnly, Category = "Item Slot")
		TSoftObjectPtr<UTexture2D> ButtonFocusImage;

	//@현재 Item Slot Button의 상태
	EItemSlotButtonState CurrentButtonState;

	//@각 상태별 ButtonStyle을 저장할 변수들
	FButtonStyle NormalStyle;
	FButtonStyle HoveredStyle;
	FButtonStyle SelectedStyle;

private:
	//@현재 버튼의 상태를 설정합니다.
	UFUNCTION(BlueprintCallable, Category = "Item Slot | Button Style")
		void SetButtonState(EItemSlotButtonState NewState);

	//@현재 선택된 버튼의 스타일을 업데이트합니다.
	UFUNCTION(BlueprintCallable, Category = "Item Slot | Button Style")
		void UpdateButtonStyle(EItemSlotButtonState NewState);

protected:
	//@Item Slot Button의 상호작용 활성화
	void ActivateItemSlotInteraction();
	//@Item Slot Button의 상호작용 비 활성화
	void DeactivateItemSlotInteraction();
#pragma endregion

#pragma region Delegates
public:
	//@아이템 슬롯 버튼 클릭 이벤트
	FItemSlotButtonClicked ItemSlotButtonClicked;
#pragma endregion

#pragma region Callbacks
protected:
    //@Button Clicked 이벤트에 등록되는 콜백
	UFUNCTION(BlueprintNativeEvent, Category = "Item Slots | Item Slot Button")
		void OnItemSlotButtonSelected();
	virtual void OnItemSlotButtonSelected_Implementation();
	//@Button Hovered 이벤트에 등록되는 콜백
	UFUNCTION(BlueprintNativeEvent, Category = "Item Slots | Item Slot Button")
		void OnItemSlotButtonHovered();
	virtual void OnItemSlotButtonHovered_Implementation();
	//Butotn Unhovered 이벤트에 등록되는 콜백
	UFUNCTION(BlueprintNativeEvent, Category = "Item Slots | Item Slot Button")
		void OnItemSlotButtonUnhovered();
	virtual void OnItemSlotButtonUnhovered_Implementation();
	//Butotn Canceled 이벤트에 등록되는 콜백
	UFUNCTION(BlueprintNativeEvent, Category = "Item Slots | Item Slot Button")
		void OnItemSlotButtonCanceled(const FGuid& ItemID);
	virtual void OnItemSlotButtonCanceled_Implementation(const FGuid& ItemID);
#pragma endregion
};
