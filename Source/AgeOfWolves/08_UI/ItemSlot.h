#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ItemSlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemSlot, Log, All)

//@초기화 완료 이벤트
DECLARE_DELEGATE(FItemSlotInitFinished);

class UOverlay;
class UImage;
class UVerticalBox;
class UEditableText;

/**
 * @UItemSlot
 *
 * 아이템을 표시하는 UI입니다.
 */
UCLASS()
class AGEOFWOLVES_API UItemSlot : public UUserWidget
{
	GENERATED_BODY()

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
#pragma endregion

#pragma region SubWidgets
public:
	//@퀵슬롯에 새로운 아이템을 할당합니다.
	void AssignNewItem(const FGuid& ID, UTexture2D* ItemImage, bool bIsStackable, int32 ItemCount, bool bIsRemovable);
	//@퀵슬롯에 할당된 기존 아이템 정보를 업데이트 합니다.
	void UpdateItemCount(int32 NewCount);
	//@퀵슬롯에 할당된 기존 아이템을 제거합니다.
	void ClearAssignedItem(bool bForceClear = false);
protected:
	/*
	* @목적 : HUD의 Quick Slot들 중 단일 Slot을 구성하는 Widget입니다.
	* @설명 : Quick Slot 목록 중 한 Slot을 나타내며, Overlay를 통해 UImage 와 UEditableText를 하나로 묶어줍니다.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
		UOverlay* SlotOverlay;
	FGuid UniqueItemID;
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
		UImage* SlotImage;
	bool bStackable;
	bool bRemovable;
	UPROPERTY(BlueprintReadWrite, Category = "Quick Slot", meta = (BindWidget))
		UEditableText* SlotItemNum;
public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetUniqueItemID(const FGuid& ItemID) { UniqueItemID = ItemID; }
	UFUNCTION(BlueprintCallable)
		void SetSlotImage(TSoftObjectPtr<UTexture2D> InTexture);
	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetIsStackable(bool InBool) { bStackable = InBool; }
	UFUNCTION(BlueprintCallable, meta = (EditCondition = "bStackable == true"))
		void SetSlotItemNum(int32 InNum);
public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE FGuid GetUniqueItemID() { return UniqueItemID; }
	UFUNCTION(BlueprintCallable)
		FSlateBrush GetSlotImage() const;
	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool GetIsStackable() const { return bStackable; }
	FORCEINLINE bool IsRemovable() const { return bRemovable; }
	UFUNCTION(BlueprintCallable)
		int32 GetSlotItemNum() const;
#pragma endregion


};

