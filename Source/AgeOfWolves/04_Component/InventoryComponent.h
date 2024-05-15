// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "InventoryComponent.generated.h"

class AItem;

/*
* EItemType
*
* @목적: Item 유형 목록을 정의합니다.
*/
UENUM(BlueprintType)
enum class EItemType : uint8
{
	Consumable = 0		UMETA(DisplayName = "Consumable"),
	NonConsumable		UMETA(DisplayName = "NonConsumable"),
	MAX,
};

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
	Logendary		UMETA(DisplayName = "Legendary"),
	MAX,
};

/*
* FItemInfo
* 
* @목적: Item과 관련된 정보를 한 데 묶어놓습니다.
* @참고: AItem* 유형이 아니라, FGameplayTag 유형의 '태그'를 저장합니다.
*/
USTRUCT(BlueprintType)
struct FItemInformation
{
	GENERATED_BODY()

public:
	// AItem 유형의 객체
	UPROPERTY(EditAnywhere)
		AItem* Item;
	// Gameplay Tag
	UPROPERTY(EditAnywhere)
		FGameplayTag ItemTag;
	// Item 이름
	UPROPERTY(EditAnywhere)
		FText ItemName;
	// Item 유형
	UPROPERTY(EditAnywhere)
		EItemType ItemType = EItemType::MAX;
	// Item 등급
	UPROPERTY(EditAnywhere)
		EItemRank ItemRank = EItemRank::MAX;
	// Item 설명
	UPROPERTY(EditAnywhere)
		FText ItemDescription;
	// Item 가격
	UPROPERTY(EditAnywhere)
		int32 Item_Price = 0;
	// 되팔 수 있는가?
	UPROPERTY(EditAnywhere)
		bool bSellable = true;
	// 고유 아이템 여부
	UPROPERTY(EditAnywhere)
		bool bStackable = true;
	// 최대 가질 수 있는 갯수
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bStackable == true"))
		int32 MaxStack = -1;
	// 제거 가능한가?
	UPROPERTY(EditAnywhere)
		bool bRemovable = true;
	// 갯수가 0이되면 Inventory에서 바로 제거할 것인지
	UPROPERTY(EditAnywhere, meta = (EditCondition = "bStackable == bRemovable"))
		bool bRemoveWhenCountZero = false;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Default Setting
public:	
	// Sets default values for this component's properties
	UInventoryComponent();

protected:
	//~UActorComponent Interface
	// @설명: 구성 요소가 액터에 등록될 때 호출됩니다. 구성 요소 초기화 로직을 여기에 배치할 수 있습니다.
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	// @설명: OnRegister 후에 호출되어 구성 요소의 초기화를 완료합니다. 이 단계에서는 모든 초기 설정이 완료되어야 합니다.
	virtual void InitializeComponent() override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	//~ End of UActorComponent Interface

	// Called when the game starts
	virtual void BeginPlay() override;
#pragma endregion

#pragma region Inventory
protected:
	TMap<FGuid, FItemInformation> Inventory;
#pragma endregion
};
