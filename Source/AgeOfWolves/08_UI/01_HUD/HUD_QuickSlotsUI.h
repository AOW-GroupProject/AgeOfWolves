// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTags.h" 

#include "HUD_QuickSlotsUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuickSlotsUI, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UHorizontalBox;
class UVerticalBox;
class UItemSlot;
class UAbilitySlot;
class UHUD_QuickSlotsUI_AbilitySlot;
class UBaseAbilitySystemComponent;
class UItemManagerSubsystem;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
USTRUCT(BlueprintType)
struct FAbilitySlotSizeInformation
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 슬롯 | 배경 이미지 사이즈")
		FVector2D BGImageSize;
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 슬롯 | 어벌리티 이미지 사이즈")
		FVector2D AbilityImageSize;
};

/*
*	@FJujutsuQuickSlotInformation
*	
*	주술 어빌리티 슬롯의 정보
*/
USTRUCT(BlueprintType)
struct FJujutsuQuickSlotInformation
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, Category = "퀵슬롯 | 주술 슬롯 BP 클래스")
		TSubclassOf<UHUD_QuickSlotsUI_AbilitySlot> JujutsuAbilitySlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "퀵슬롯 | 대기 주술 슬롯 BP 클래스")
		TSubclassOf<UAbilitySlot> ExtraJujutsuAbilitySlotClass;

	UPROPERTY(EditDefaultsOnly, Category = "퀵슬롯 | 대기 주술 슬롯 개수")
		int ExtraJujutsuAbilitySlotNum;

	UPROPERTY(EditDefaultsOnly, Category = "퀵슬롯 | 대기 주술 슬롯 사이즈")
		TArray<FAbilitySlotSizeInformation> ExtraJujutsuAbilitySlotsSizeInfo;
};

/*
*	@FJujutsuAbilitySlotInfo
* 
*	Jujutsu Ability Slot 에 필요한 기본 정보
*/
USTRUCT()
struct FJujutsuAbilitySlotInfo
{
	GENERATED_BODY()

	FGameplayTag AbilityTag;
	bool bIsFilled = false;

	void Clear()
	{
		AbilityTag = FGameplayTag();
		bIsFilled = false;
	}
};

/*
*	@FJujutsuRotationState
* 
*	주술 어빌리티 슬롯 회전 상태 정보
*/
USTRUCT(BlueprintType)
struct FJujutsuRotationState
{
public:

	GENERATED_BODY()
	
	float AccumulatedValue = 0.0f;
	double LastInputTime = 0.0;
	bool bIsAccumulating = false;

	void Reset()
	{
		AccumulatedValue = 0.0f;
		LastInputTime = 0.0;
		bIsAccumulating = false;
	}
};

/*
*	@FJujutsuSlots
* 
*	주술 어빌리티 슬롯 목록 정보
*/
USTRUCT(BlueprintType)
struct FJujutsuSlots {
public:

	GENERATED_BODY()

	TObjectPtr<UHUD_QuickSlotsUI_AbilitySlot> MainSlot;
	TArray<UAbilitySlot*> ExtraSlots;                      
	TArray<FJujutsuAbilitySlotInfo> AbilityInfos;
	int32 CurrentMainIndex = 0;                            

	void RotateSlots(bool bForward)
	{
		if (AbilityInfos.Num() >= 3)
		{
			if (bForward)
			{
				CurrentMainIndex = (CurrentMainIndex + 1) % AbilityInfos.Num();
			}
			else
			{
				CurrentMainIndex = (CurrentMainIndex + 1 + AbilityInfos.Num()) % AbilityInfos.Num();
			}
		}
	}
};

#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByQuickSlots);
//@초기화 완료 이벤트
DECLARE_DELEGATE(FQuickSlotsInitFinished);

DECLARE_MULTICAST_DELEGATE(FRequestHPToolItemActivation);
#pragma endregion


/**
 *	@UHUD_QuickSlotsUI
 * 
 *	HUD 구성 중 활용 가능한 퀵슬롯 UI
 */
UCLASS()
class AGEOFWOLVES_API UHUD_QuickSlotsUI : public UUserWidget
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UHUD_QuickSlotsUI(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized();
	//~ End UUserWidget Interface

protected:
	//@외부 바인딩
	void ExternalBindToASC();
	void ExternalBindToInputComp();

protected:
	//@내부 바인딩
	void InternalBindToToolItemSlots(UItemSlot* ItemSlot, bool bLast = false);
	void InternalBindToBattoujutsuAbilitySlot(UHUD_QuickSlotsUI_AbilitySlot* AbilitySlot);
	void InternalBindToJujutsuAbilitySlots(UAbilitySlot* AbilitySlot);

protected:
	//@외부 바인딩
	void ExternalBindToInventoryComp();

public:
	//@초기화
	UFUNCTION()
		void InitializeQuickSlotsUI();

protected:
	//@초기화 완료 체크
	bool bToolItemSlotInitFinished = false;
	bool bBattouJutsuAbilitySlotInitFinished = false;
	bool bJujutsuAbilitySlotsInitFinished = false;
	void CheckAllUIsInitFinished();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@Tool Item Slots 생성
	void CreateToolItemQuickSlots();
	//@발도술 슬롯 생성
	void CreateBattoujutsuAbilitySlot();
	//@주술 슬롯 생성
	void CreateJujutsuAbilitySlot();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "퀵슬롯", meta = (BindWidget))
		UVerticalBox* QuickSlotsBox;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "퀵슬롯 | 도구 아이템", meta = (BindWidget))
		UHorizontalBox* ToolItemQuickSlotBox;

	UPROPERTY(EditDefaultsOnly, Category = "퀵슬롯 | 도구 아이템")
		TSubclassOf<UItemSlot> ToolItemSlotClass;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "퀵슬롯 | 발도술", meta = (BindWidget))
		UOverlay* BattoujutsuQuickSlotOverlay;

	UPROPERTY(EditDefaultsOnly, Category = "퀵슬롯 | 발도술 슬롯 BP 클래스")
		TSubclassOf<UHUD_QuickSlotsUI_AbilitySlot> BattoujutsuAbilitySlotClass;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "퀵슬롯 | 주술", meta = (BindWidget))
		UOverlay* JujutsuQuickSlotsOverlay;

	UPROPERTY(BlueprintReadWrite, Category = "퀵슬롯 | 주술", meta = (BindWidget))
		UVerticalBox* JujutsuExtraQuickSlotBox;

	UPROPERTY(EditDefaultsOnly, Category = "퀵슬롯 | 주술 슬롯 BP 클래스")
		FJujutsuQuickSlotInformation JujutsuQuickSlotInformation;

protected:
	//@Tool Item Slot 목록
	UPROPERTY()
		TArray<UItemSlot*> ToolItemSlots;

protected:
		//@발도술 어빌리티 슬롯 캐싱
	UPROPERTY()
		TObjectPtr<UHUD_QuickSlotsUI_AbilitySlot> BattoujutsuAbilitySlotRef;

protected:
	//@마우스 휠 축 관련 기준 최대 값
	const float MAX_THRESHOLD = 1.0f;
    const float AXIS_ACCUMULATE_TIME = 1.0f; 

	//@주술 어빌리티 슬롯 목록
	UPROPERTY()
		FJujutsuSlots JujutsuSlots;
	//@주술 어빌리티 순환 상태 정보
	UPROPERTY()
		FJujutsuRotationState JujutsuRotationState;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@초기화 요청 이벤트
	FRequestStartInitByQuickSlots RequestStartInitByQuickSlots;

	//@초기화 완료 이벤트
	FQuickSlotsInitFinished QuickSlotsInitFinished;

public:
	//@HP 도구 아이템 활성화 요청 이벤트
	FRequestHPToolItemActivation RequestHPToolItemActivation;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	UFUNCTION()
		void OnToolItemSlotsInitFinished();
	UFUNCTION()
		void OnBattoujutsuAbilitySlotInitFinished();
	UFUNCTION()
		void OnJujutsuAbilitySlotsInitFinished();

protected:
	UFUNCTION()
		void OnAbilitySpecGiven(FGameplayAbilitySpec AbilitySpec);

protected:
	UFUNCTION()
		void OnUIInputTriggeredWithValue(const FGameplayTag& InputTag, const float AxisValue);

protected:
	//@Inventory Comp의 퀵슬롯 아이템 할당 이벤트에 등록되는 콜백
	UFUNCTION()
		void OnQuickSlotItemsLoaded(int32 QuickSlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount);
	//@Inventory Comp의 퀵슬롯 아이템 업데이트 이벤트에 등록되는 콜백
	UFUNCTION()
		void OnQuickSlotItemUpdated(int32 QuickSlotNum, const FGuid& UniqueItemID, int32 ItemNum);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	//@아이템 매니저 서브시스템 캐싱
	UPROPERTY()
		TWeakObjectPtr<UItemManagerSubsystem> ItemManagerRef;
#pragma endregion
	
};
