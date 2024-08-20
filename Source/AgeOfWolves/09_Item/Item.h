// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "AttributeSet.h"

#include "Item.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItem, Log, All);

class UBaseGameplayAbility;
class UItemManagerSubsystem;

/*
* EItemRank
*
* @목적: Item 랭크 목록을 정의합니다.
*/
UENUM(BlueprintType)
enum class EItemRank : uint8
{
	Normal = 0		UMETA(DisplayName = "Normal"),
	Epic			UMETA(DisplayName = "Epic"),
	Legendary		UMETA(DisplayName = "Legendary"),
	MAX,
};

/*
* EItemType
*
* @목적: Item 유형 목록을 정의합니다.
*/
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Tool = 0		UMETA(DisplayName = "Tools"),
	Equipment		UMETA(DisplayName = "Equipment"),
	Material		UMETA(DisplayName = "Material"),
	Memory			UMETA(DisplayName = "Memory"),
	MAX,
};

/*
* EToolItemType
*
* @목적: Tool Item 유형 목록을 정의합니다.
* @참고: 추가 예정
*/
UENUM(BlueprintType)
enum class EToolItemType : uint8
{
	Potion = 0		UMETA(DisplayName = "Potion"),
	MAX,
};

/*
* EEquipmentItemType
*
* @목적: Equipment Item 유형 목록을 정의합니다.
* @참고: 추가 예정
*/
UENUM(BlueprintType)
enum class EEquipmentItemType : uint8
{
	Weapon	= 0		UMETA(DisplayName = "Weapon"),
	Talisman 		UMETA(DisplayName = "Talisman"),
	MAX,
};
/*
* FEnhancementInfo
* 
* @목적: 아이템 제작을 위해 필요한 Material 정보를 담고 있습니다.
* @설명: bOriginal(고유 아이템)여부 확인 후 필요 재료 정보를 추가할 수 있습니다.
*/
USTRUCT(BlueprintType)
struct FEnhancementInformation : public FTableRowBase
{
	GENERATED_BODY()

#pragma region Default setting
public:
	FEnhancementInformation() {}
#pragma endregion

#pragma region Materials
public:
	//@고유 아이템 여부(하위 Material이 없는 아이템)
	UPROPERTY(EditAnywhere, Category = "Enhancement | Materials")
		bool bOriginal = true;
	//@하위 재료의 Tag와 갯수
	UPROPERTY(EditAnywhere, Category = "Enhancement | Materials", meta = (EditCondition = "bOriginal == false"))
		TMap<FGameplayTag, int32> RequiredMaterials;
#pragma endregion

#pragma region Info
protected:
	//@강화 최대 스택
	UPROPERTY(EditAnywhere, Category = "Enhancement | Stack")
		int32 MaxEnhancementStack = 5;
	//@현재 강화 스택
	UPROPERTY(EditAnywhere, Category = "Enhancement | Stack", meta = (EditCondition = "CurrentEnhancementStack <=  MaxEnhancementStack"))
		int32 CurrentEnhancementStack = 0;
public:
	FORCEINLINE int32 GetMaxEnhancementStack() { return MaxEnhancementStack; }
	//@강화 가능 여부
	bool CanEnhance()
	{
		return CurrentEnhancementStack < MaxEnhancementStack;
	}
#pragma endregion

#pragma region GA
public:
	//@TODO: Enhance

protected:
	//@강화 어빌리티(주로, GE를 통한 Attribute 수치 값 변화)
	UPROPERTY(EditAnywhere, Category = "Enhancement | Enhancement Ability")
		TSubclassOf<UBaseGameplayAbility> EnhancementAbilityClass;
#pragma endregion

#pragma region Records
protected:
	UPROPERTY(VisibleAnywhere, Category = "Enhancement | Enhancement Ability")
		TMap<FGameplayAttribute, int32> EnhancementResults;
#pragma endregion

};

/*
* FItemInfo
*
* @목적: Item과 관련된 정보를 한 데 묶어놓습니다.
* @참고: AItem* 유형 뿐만 아니라, FGameplayTag 유형의 '태그'를 저장합니다.
*/
USTRUCT(BlueprintType)
struct FItemInformation : public FTableRowBase
{
	GENERATED_BODY()

public:
	//@AItem 유형의 객체
	UPROPERTY(EditAnywhere)
		TSubclassOf<AItem> ItemClass;
	//@Gameplay Tag
	UPROPERTY(EditAnywhere)
		FGameplayTag ItemTag;
	//@Item 이름
	UPROPERTY(EditAnywhere)
		FText ItemName;
	//@Item 유형
	UPROPERTY(EditAnywhere)
		EItemType ItemType = EItemType::MAX;
	//@Item 등급
	UPROPERTY(EditAnywhere)
		EItemRank ItemRank = EItemRank::MAX;
	//@Item Image(Inventory UI)
	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UTexture2D> ItemImage;
	//@Item Image2(Inventory Slot)
	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UTexture2D> ItemSlotImage;
	//@Item 이름
	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UTexture2D> ItemNameImage;
	//@Item 설명
	UPROPERTY(EditAnywhere)
		TSoftObjectPtr<UTexture2D> ItemDescriptionImage;
	//@되팔 수 있는가?
	UPROPERTY(EditAnywhere)
		bool bSellable = false;
	//@Item 가격
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bSellable == true"))
		int32 Item_Price = 0;
	//@고유 아이템 여부
	UPROPERTY(EditAnywhere)
		bool bStackable = false;
	//@사용 가능한가?
	UPROPERTY(EditAnywhere, meta = (EditCondition = "ItemType != EItemType::Material"))
		bool bConsumable = false;
	//@Quick Slot에 추가될 아이템인가?(현재 HP, MP 포션 예정)
	UPROPERTY(EditAnywhere, meta = (EditCondition = "ItemType == EItemType::Tool"))
		bool bObssessedToQuickSlots = false;
	//@Quick Slot에 추가될 아이템인가?(현재 HP, MP 포션 예정)
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bObssessedToQuickSlots == true"))
		int32 QuickSlotNum = -1;
	//@최대 가질 수 있는 갯수
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bStackable == true"))
		int32 MaxStack = -1;
	//@Default Item 여부
	UPROPERTY(EditAnywhere)
		bool bDefault = false;
	//@Default Item일 경우, 기본 제공되는 갯수
	UPROPERTY(EditAnywhere, meta = (EditCondition = "(bStackable == true)&&(bDefault == true)"))
		int32 DefaultGivenStack = 1;
	//@제거 가능한가?
	UPROPERTY(EditAnywhere, meta = (EditCondtion = "bObssessedToQuickSlots == false"))
		bool bRemovable = false;
	//@갯수가 0이되면 Inventory에서 바로 제거할 것인지
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bStackable == bRemovable"))
		bool bRemoveWhenCountZero = false;
	//@아이템 사용 시 활성화되는 GA
	UPROPERTY(EditAnywhere, meta = (EditCondtion = "ItemType != EItemType::Material"))
		TArray<UBaseGameplayAbility*> Abilities;
	//@강화 정보
	UPROPERTY(EditAnywhere)
		FEnhancementInformation EnhancementInfo;
};

/*
* FToolItemInforamtion : FItemInformation
*
* @목적: 도구 아이템 정보를 저장하는 구조체
* @설명: Item Type 설정, 강화 정보가 포함되어 있는 구조체
*/
USTRUCT(BlueprintType)
struct FToolItemInformation : public FItemInformation
{
	GENERATED_BODY()

public:
	EToolItemType ToolType = EToolItemType::MAX;
	FToolItemInformation()
	{
		//@Itme type
		ItemType = EItemType::Tool;
		//@별도의 매개변수 없을 경우, 첫 번째 타입으로 설정됨
		ToolType = EToolItemType::Potion;
	}
	FToolItemInformation(EToolItemType ToolItemType)
	{
		//@Itme type
		ItemType = EItemType::Tool;
		ToolType = ToolItemType;
	}
};

/*
* FEquipmentItemInforamtion : FItemInformation
*
* @목적: 장비 아이템 정보를 저장하는 구조체
* @설명: Item Type 설정, 강화 정보가 포함되어 있는 구조체
*/
USTRUCT(BlueprintType)
struct FEquipmentItemInformation : public FItemInformation
{
	GENERATED_BODY()

public:
	EEquipmentItemType EquipmentType = EEquipmentItemType::MAX;
	FEquipmentItemInformation()
	{
		ItemType = EItemType::Equipment;
	}
	FEquipmentItemInformation(EEquipmentItemType EquipmentItemType)
	{
		//@Itme type
		ItemType = EItemType::Equipment;
		//@별도의 매개변수 없을 경우, 첫 번째 타입으로 설정됨
		EquipmentType = EquipmentItemType;
	}
};

/*
* FMaterailItemInforamtion : FItemInformation
* 
* @목적: 재료 아이템 정보를 저장하는 구조체
* @설명: Item Type 설정, 강화 정보가 포함되어 있는 구조체
*/
USTRUCT(BlueprintType)
struct FMaterialItemInformation : public FItemInformation
{
	GENERATED_BODY()

public:
	FMaterialItemInformation()
	{
		//@Itme type
		ItemType = EItemType::Material;
	}
};

UCLASS()
class AGEOFWOLVES_API AItem : public AActor
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:	
	AItem(const FObjectInitializer& ObjectInitializer);

protected:
	// 	//~UObject interface
	virtual void PreRegisterAllComponents() override;
	virtual void PostRegisterAllComponents() override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	//~End Of UObject Interface

public:
	//@Item 초기화
	UFUNCTION()
		void InitializeItem(const FGuid& UniqueId);
#pragma endregion

#pragma region Item
public:
	//@아이템 활성화
	UFUNCTION(BlueprintNativeEvent)
		bool TryActivateItem();
	virtual bool TryActivateItem_Implementation();
#pragma endregion 

#pragma region Property
protected:
	//@Item Tag
	UPROPERTY(EditDefaultsOnly, Category = "Item | Item Tag")
		FGameplayTag ItemTag;
	//@Item Type
	UPROPERTY(VisibleAnywhere, Category = "Item | Item Type")
		EItemType ItemType;
public:
	FORCEINLINE const EItemType GetItemType() const { return ItemType; }
	FORCEINLINE const FGameplayTag& GetItemTag() const { return ItemTag; }
#pragma endregion

#pragma region CallBacks
public:
	//@Inventory의 아이템 제거 이벤트에 등록되는 콜백
	UFUNCTION()
		void OnItemRemovedFromInventory();
#pragma endregion


};
