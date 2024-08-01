#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "SystemUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSystemUI, Log, All)

//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitBySystemUI);
//@Inventory UI 초기화 완료 알림 이벤트
DECLARE_DELEGATE(FNotifyInventoryUIInitFinished);

class UScaleBox;
class UInventoryUI;

/**
 * USystemUI
 * 
 * System 관련 UI들의 프레임을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API USystemUI : public UUserWidget
{
	GENERATED_BODY()

#pragma region Default Setting
public:
	USystemUI(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
protected:
	//@내부 바인딩
	void InternalBindingToInventoryUI(UInventoryUI* InventoryUI);
public:
	//@초기화
	UFUNCTION()
		void InitializeSystemUI();
#pragma endregion

#pragma region Sub Widgets
protected:
	//@Inventory UI를 생성합니다.
	void CreateInventoryUI();
protected:
	UPROPERTY(BlueprintReadWrite, Category = "System | Inventory UI", meta = (BindWidget))
		UScaleBox* InventoryUIBox;
	UPROPERTY(EditDefaultsOnly, category = "System | Inventory UI")
		TSubclassOf<UInventoryUI> InventoryUIClass;
public:
	UFUNCTION(BlueprintCallable, Category = "System | Inventory UI")
		UInventoryUI* GetInventoryUI() const;
#pragma endregion

#pragma region Delegates
public:
	//@초기화 요청 이벤트
	FRequestStartInitBySystemUI RequestStartInitBySystemUI;
	//@초기화 완료
	FNotifyInventoryUIInitFinished NotifyInventoryUIInitFinished;
#pragma endregion

#pragma region Callbacks
protected:
	UFUNCTION()
		void OnInventoryUIInitFinished();
#pragma endregion
	
};
