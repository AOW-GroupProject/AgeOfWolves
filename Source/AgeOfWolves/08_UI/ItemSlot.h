#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "09_Item/Item.h"

#include "ItemSlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemSlot, Log, All)

//@Forward Declaration
#pragma region Forward Decalration
class UOverlay;
class UImage;
class UVerticalBox;
class UEditableTextBox;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@Delegates
#pragma region Delegates
//@초기화 완료 이벤트
DECLARE_DELEGATE(FItemSlotInitFinished);
#pragma endregion

/**
 * @UItemSlot
 *
 * 아이템을 표시하는 UI입니다.
 */
UCLASS()
class AGEOFWOLVES_API UItemSlot : public UUserWidget
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Default Setting
#pragma region Default Setting
public:
	UItemSlot(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
public:
	UFUNCTION()
		virtual void InitializeItemSlot();
#pragma endregion

//@Property/Info...etc
#pragma region SubWidgets
public:
	//@퀵슬롯에 새로운 아이템을 할당합니다.
	UFUNCTION(BlueprintNativeEvent)
		void AssignNewItem(const FGuid& ID, FItemInformation ItemInformation, int32 ItemCount = -1);
	virtual void AssignNewItem_Implementation(const FGuid& ID, FItemInformation ItemInformation, int32 ItemCount=-1);

	// 다른 ItemSlot으로부터 정보를 복사하여 할당
	UFUNCTION(BlueprintNativeEvent)
		void AssignNewItemFromSlot(UItemSlot* FromSlot);
	virtual void AssignNewItemFromSlot_Implementation(UItemSlot* FromSlot);

	//@퀵슬롯에 할당된 기존 아이템 정보를 업데이트 합니다.
	UFUNCTION(BlueprintNativeEvent)
		void UpdateItemCount(int32 NewCount);
	virtual void UpdateItemCount_Implementation(int32 NewCount);

	//@퀵슬롯에 할당된 기존 아이템을 제거합니다.
	UFUNCTION(BlueprintNativeEvent)
		void ClearAssignedItem(bool bForceClear = false);
	virtual void ClearAssignedItem_Implementation(bool bForceClear = false);

protected:
	//@Item Slot에 할당된 아이템의 고유 아이디(Inventory Component에서 발급)
	FGuid UniqueItemID;
	/*
	* @목적 : HUD의 Quick Slot들 중 단일 Slot을 구성하는 Widget입니다.
	* @설명 : Quick Slot 목록 중 한 Slot을 나타내며, Overlay를 통해 UImage 와 UEditableText를 하나로 묶어줍니다.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Item Slot", meta = (BindWidget))
		UOverlay* SlotOverlay;
	UPROPERTY(BlueprintReadWrite, Category = "Item Slot", meta = (BindWidget))
		UImage* SlotBGImage;
	UPROPERTY(BlueprintReadWrite, Category = "Item Slot", meta = (BindWidget))
		UImage* SlotImage;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Item Slot", meta = (BindWidget))
		UOverlay* SlotItemNumOverlay;
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
		UEditableTextBox* SlotItemNum;

protected:
	bool bStackable = false;
	bool bRemovable = false;

public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetUniqueItemID(const FGuid& ItemID) { UniqueItemID = ItemID; }
	UFUNCTION(BlueprintCallable)
		void SetSlotImage(TSoftObjectPtr<UTexture2D> InTexture);
	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetIsStackable(bool InBool) { bStackable = InBool; }
	FORCEINLINE void SetIsRemovable(bool InBool){ bRemovable = InBool; }
	UFUNCTION(BlueprintCallable, meta = (EditCondition = "bStackable == true"))
		void SetSlotItemNum(int32 InNum);

public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE FGuid GetUniqueItemID() const { return UniqueItemID; } 
	UFUNCTION(BlueprintCallable)
		FSlateBrush GetSlotImage() const;
	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool GetIsStackable() const { return bStackable; }
	FORCEINLINE bool IsRemovable() const { return bRemovable; }
	UFUNCTION(BlueprintCallable)
		int32 GetSlotItemNum() const;
#pragma endregion

//@Delegates
#pragma region Delegates 
public:
	FItemSlotInitFinished ItemSlotInitFinished;
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

};
