// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "08_UI/AbilitySlot.h"

#include "HUD_QuickSlotsUI_AbilitySlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuickSlotsAbilitySlot, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UEditableTextBox;
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
 * 
 */
UCLASS()
class AGEOFWOLVES_API UHUD_QuickSlotsUI_AbilitySlot : public UAbilitySlot
{
//@친추 클래스
#pragma region Friend Class
	friend class UHUD_QuickSlotsUI;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UHUD_QuickSlotsUI_AbilitySlot(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized() override; // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	//~ End UUserWidget Interface

protected:
	//@내부 바인딩

protected:
	//@외부 바인딩

public:
	//@초기화
	virtual void InitializeAbilitySlot() override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@새로운 Ability 할당
	virtual void AssignNewAbility_Implementation(FGameplayTag Tag) override;

	//@다른 Ability Slot으로부터 Ability 할당
	virtual void AssignNewAbilityFromSlot_Implementation(UAbilitySlot* FromSlot) override;

	//@퀵슬롯에 할당된 기존 아이템을 제거합니다.
	virtual void ClearAssignedAbility_Implementation(bool bForceClear = false) override;

protected:
	//@Ability Slot의 세부 이름
	UPROPERTY(BlueprintReadWrite, Category = "어빌리티 슬롯 | 어빌리티 세부 기술 명", meta = (BindWidget))
		UEditableTextBox* AbilityName;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	//@GameplayTag로부터 Ability의 TypeName과 Name을 추출하는 함수
	bool ExtractAbilityNamesFromTag(const FGameplayTag& Tag, FString& OutTypeName, FString& OutAbilityName) const;
#pragma endregion
	
};
