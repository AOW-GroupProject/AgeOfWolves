#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTags.h" 

#include "AbilitySlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAbilitySlot, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UVerticalBox;
class UImage;
class UEditableTextBox;
class UAbilityManagerSubsystem;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_DELEGATE(FAbilitySlotInitFinished)
#pragma endregion

/**
 *	@UAbilitySlot
 * 
 *	Gameplay Ability 관련 정보를 나타내는 Slot 형식의 UI
 */
UCLASS()
class AGEOFWOLVES_API UAbilitySlot : public UUserWidget
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UAbilitySlot(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
public:
	UFUNCTION()
		virtual void InitializeAbilitySlot();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@새로운 Ability 할당
	UFUNCTION(BlueprintNativeEvent)
		void AssignNewAbility(FGameplayTag Tag);
	virtual void AssignNewAbility_Implementation(FGameplayTag Tag);

	//@다른 Ability Slot으로부터 Ability 할당
	UFUNCTION(BlueprintNativeEvent)
		void AssignNewAbilityFromSlot(UAbilitySlot* FromSlot);
	virtual void AssignNewAbilityFromSlot_Implementation(UAbilitySlot* FromSlot);

	//@퀵슬롯에 할당된 기존 아이템을 제거합니다.
	UFUNCTION(BlueprintNativeEvent)
		void ClearAssignedAbility(bool bForceClear = false);
	virtual void ClearAssignedAbility_Implementation(bool bForceClear = false);

protected:
	//@세로 형식의 박스
	UPROPERTY(BlueprintReadWrite, Category = "어빌리티 슬롯", meta = (BindWidget))
		UVerticalBox* AbilitySlotBox;

protected:
	//@배경 이미지
	UPROPERTY(BlueprintReadWrite, Category = "어빌리티 슬롯", meta = (BindWidget))
		UImage* SlotBGImage;
	//@어빌리티 이미지
	UPROPERTY(BlueprintReadWrite, Category = "어빌리티 슬롯", meta = (BindWidget))
		UImage* AbilityImage;

protected:
	//@어빌리티 태그
	FGameplayTag AbilityTag;

protected:
	//@채워져 있는지 여부
	bool bIsFilled;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	FAbilitySlotInitFinished AbilitySlotInitFinished;
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	//@어빌리티 매니저 서브시스템 약한 참조 캐싱
	UPROPERTY()
		UAbilityManagerSubsystem* AbilityManagerCache;

public:
	FORCEINLINE void SetAbilityTag(FGameplayTag Tag) { AbilityTag = Tag; }
	FORCEINLINE FGameplayTag GetAbilityTag() const { return AbilityTag; }
public:
	FORCEINLINE void SetIsFilled(bool bNewIsFilled) { bIsFilled = bNewIsFilled; }
	FORCEINLINE bool GetIsFilled() const { return bIsFilled; }
#pragma endregion
	
};
