// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "08_UI/HorizontalDotGauge.h"

#include "HUD_HPToolItemDotGauge.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHPToolItemDotGauge, Log, All)

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UHUD_HPDotGague
 * 
 *	HUD 구성 중 HP Potion 관련 Dot Gague UI입니다.
 */
UCLASS()
class AGEOFWOLVES_API UHUD_HPToolItemDotGauge: public UHorizontalDotGauge
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UHUD_HPToolItemDotGauge(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized() override;
	//~ End UUserWidget Interface

protected:
	//@내부 바인딩

protected:
	//@외부 바인딩
	void ExternalBindToInventoryComp();

public:
	//@초기화
	virtual void InitializeHorizontalDotGauge() override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@Inventory Comp의 아이템 할당 이벤트에 등록되는 콜백
	UFUNCTION()
		void OnItemAssignedToInventory(const FGuid& UniqueItemID, EItemType Type, const FGameplayTag& ItemTag);
	//@Inventory Comp의 아이템 제거 이벤트에 등록되는 콜백
	UFUNCTION()
		void OnItemRemovedFromInventory(const FGuid& UniqueItemID, EItemType Type);
	//@Inventory Comp의 아이테 업데이트 이벤트에 등록되는 콜백
	UFUNCTION()
		void OnInventoryItemUpdated(const FGuid& UniqueItemID, EItemType Type, const FGameplayTag& ItemTag, int32 UpdatedItemCount);
#pragma endregion

	//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
	
};
