// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "09_Item/Item.h"

#include "InventoryComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

//@아이템 초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FRequestInitializationByInvenComp, const FGuid&);

//@인벤토리 아이템 추가 이벤트
DECLARE_MULTICAST_DELEGATE_ThreeParams(FNewItemAssignedToInventory, const FGuid&, EItemType, const FGameplayTag&);
//@인벤토리 아이템 제거 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FItemRemovedFromInventory, const FGuid&);
//@인벤토리 아이템 업데이트 이벤트(강화 정보, 개수 정보)
DECLARE_MULTICAST_DELEGATE_FourParams(FInventoryItemUpdated, const FGuid&, EItemType, const FGameplayTag&, int32);

//@퀵슬롯 정보 로드
DECLARE_DELEGATE_FiveParams(FQuickSlotItemsLoaded, int32, const FGuid&, EItemType, const FGameplayTag&, int32);
//@퀵슬롯 정보 업데이트 (강화 정보 등 추가 예정)
DECLARE_DELEGATE_ThreeParams(FQuickSlotItemUpdated, int32, const FGuid&, int32);

//@아이템 활성화 이벤트(한 번에 하나의 아이템 활성화 강요)
DECLARE_DELEGATE_RetVal(bool, FItemStartActivation);

class UAOWSaveGame;
class UItemManagerSubsystem;
class AController;

#pragma region Inventory Item
/*
* FInventoryItem
* 
* @목적: Inventory에서 관리하는 각 Item의 관련 정보들을 한데 묶어줍니다.
*/
USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

#pragma region Default Setting
public:
	FInventoryItem(){}
	/*FInventoryItem(AActor* Actor, AItem* Item, int32 Num, FGuid ItemID);*/
	FInventoryItem(AActor* Actor, TSubclassOf<AItem> ItemBlueprintClass, int32 Num, FGuid ItemID, bool bRemovable);
	//@사용자 정의 대입 연산자
	const bool operator=(const FInventoryItem& OtherInventoryItem) const
	{
		if (!ItemCDO || !OtherInventoryItem.ItemCDO)
		{
			UE_LOG(LogInventory, Error, TEXT("두 FIvnetoryItem 구조체 중 하나의 Instance가 유효하지 않습니다"));
			return false;
		}
		return ItemCDO->GetItemTag() == OtherInventoryItem.ItemCDO->GetItemTag();
	}
public:
	const FGameplayTag GetItemTag() const;
	TSubclassOf<AItem> GetItemClass() const;
	EItemType GetItemType() const;
	AItem* GetItem();
#pragma endregion

#pragma region Info
protected:
	UPROPERTY(VisibleAnywhere)
		TSubclassOf<AItem> ItemClass = nullptr;
	UPROPERTY(VisibleAnywhere)
		TObjectPtr<AItem> ItemCDO = nullptr;
	UPROPERTY(VisibleAnywhere)
		TWeakObjectPtr<AActor> OwnerActorPtr = nullptr;
#pragma endregion

#pragma region Item
public:
	//@Item 갯수
	UPROPERTY(VisibleAnywhere)
		int32 ItemCount = -1;
	bool bRemovable = false;
#pragma endregion

};
#pragma endregion
/*
* UInventoryComponent
* 
* Inventory Component는 Item객체 정보 FInventoryItem 형태로 저장하는 컴포넌트입니다.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

#pragma region Default Setting
public:	
	UInventoryComponent(const FObjectInitializer& ObjectInitializer);

protected:
	//~UActorComponent Interface
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void InitializeComponent() override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End of UActorComponent Interface
public:
	//@외부 바인딩
	void ExternalBindToUIComponent(const AController* Controller);
	void ExternalBindToQuickSlots();
private:
	//@초기화 함수
	UFUNCTION()
		void InitializeInventory(const AController* Controller);
#pragma endregion

#pragma region Inventory
public:
	/*
	* @목적: Inventory의 현재 Item 정보를 초기화합니다.
	* @설명: Save File이 있을 경우 LoadItemsFromSaveGame를 호출하며, 없을 경우 LoadDefaultItems를 호출합니다.
	* @참조: LoadItemsFromSaveGame(), LoadDefaultItems()
	*/
	UFUNCTION()
		void LoadInventory();
private:
	//@Inventory의 Item 정보를 Save File로부터 Load합니다.
	void LoadItemsFromSaveGame(UAOWSaveGame* SaveGame);
	//@Inventory의 Item 정보를 Item Manager로부터 Load합니다.
	void LoadDefaultItemsFromItemManager(UItemManagerSubsystem* ItemManager);
public:
	void AddItem(AItem* AlreadySpawnedItem, int32 Num = 1);
	void AddItem(UItemManagerSubsystem* ItemManager, TSubclassOf<AItem> BlueprintItemClass, int32 Num = 1);
	bool FindExistingItem(const FGameplayTag& ItemTag, /*OUT*/FGuid& OutItemID);
	bool FindExistingItemByClass(TSubclassOf<AItem> ItemClass, /*OUT*/FGuid& OutItemID);
protected:
	//@Inventory에 새로운 아이템 추가
	FGuid AddNewItem(TSubclassOf<AItem> BlueprintItemClass, int32 Num, AItem* AlreadySpawnedItem = nullptr);
	//@Inventory에 기존 아이템 추가
	void AddExistingItem(const FGuid& ItemId, const FItemInformation& ItemInfo, int32 Num);
	//@아이템 제거
	void RemoveExistingItem(const FGuid& ItemId, const FItemInformation& ItemInfo, int32 Num=1);
	//@아이템 사용
	UFUNCTION()
		bool StartUseItem(const FGuid& UniqueItemID ,int32 QuickSlotNum = 0);
	//@아이템 스폰
	void SpawnItem();
	//@아이템 교체
	void SwapItem();
	//@아이템 사용 종료
	void EndUseItem();
private:
	//@저장할 수 있는 최대 아이템 갯수, 수정 절대 하지 마세요
	const int MaxInventorySize = 30;
public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE int GetMaxInventorySize() { return MaxInventorySize; }
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TMap<FGuid, FInventoryItem> Inventory;
#pragma endregion

#pragma region Item
protected:
	//@추가된 아이템의 재활성화
	void EnableItem(AItem* Item);
	//@추가된 아이템의 비활성화
	void DisableItem(AItem* Item);
#pragma endregion

#pragma region Quick Slots
private:
	const int MaxQuicKSlotSize = 3;
	//@퀵 슬롯에 추가된 고유의 아이템 아이디 목록, 크기는 3 이하로 고정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TArray<FGuid> QuickSlots;
public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE int GetMaxQuicKSlotSize() { return MaxQuicKSlotSize; }
	UFUNCTION(BlueprintCallable)
		int32 IsItemAssignedToQuickSlots(const FGuid& UniqueItemID);
#pragma endregion

#pragma region Delegates
public:
	//@초기화 요청 이벤트
	FRequestInitializationByInvenComp RequestInitializationByInvenComp;
	//@새 아이템 추가 이벤트: Item, Inven UI
	FNewItemAssignedToInventory NewItemAssignedToInventory;
	//@아이템 제거 이벤트: Item, Inven UI
	FItemRemovedFromInventory ItemRemovedFromInventory;
	//@아이템 업데이트 이벤트: Item, Inven UI
	FInventoryItemUpdated InventoryItemUpdated;
public:
	//@퀵슬롯 아이템 로딩 이벤트 
	FQuickSlotItemsLoaded QuickSlotItemsLoaded;
	//@퀵슬롯 아이템 업데이트 이벤트
	FQuickSlotItemUpdated QuickSlotItemUpdated;
public:
	//@TODO: Inventory UI 관련 이벤트


#pragma endregion

};
