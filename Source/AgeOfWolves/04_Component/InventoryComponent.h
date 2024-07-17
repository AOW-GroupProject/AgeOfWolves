// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "09_Item/Item.h"

#include "InventoryComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInventory, Log, All);

//@인벤토리 아이템 추가 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnItemAddedToInventory, const FGuid&, UniqueId);
//@인벤토리 아이템 제거
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemRemovedFromInventory, const FGuid&, UniqueItemID, const FGameplayTag&, ItemTag);

class UAOWSaveGame;
class UItemManagerSubsystem;

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
	FInventoryItem(AActor* Actor, TSubclassOf<AItem> ItemBlueprintClass, int32 Num, FGuid ItemID);
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
protected:
	//@수정, 이제 사용하지 않음
	//AItem* SpawnAndDisableItem(AActor* OwnerActor, TSubclassOf<AItem> ItemBlueprintClass);
	//@TODO: Item StartUse/EndUse

public:
	//@Item 갯수
	UPROPERTY(VisibleAnywhere)
		int32 ItemCount = -1;
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
	// @설명: Tick 함수는 임시 주석 처리, 필요할 때 정의하여 사용합니다.
	//virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End of UActorComponent Interface
public:
	/*
	* @목적: Inventory의 현재 Item 정보를 초기화합니다.
	* @설명: Save File이 있을 경우 LoadItemsFromSaveGame를 호출하며, 없을 경우 LoadDefaultItems를 호출합니다.
	* @참조: LoadItemsFromSaveGame(), LoadDefaultItems()
	*/
	void LoadInventory();
private:
	//@TODO: Save Game 기능 구현 시 추가 구현
	//@Inventory의 Item 정보를 Save File로부터 Load합니다.
	void LoadItemsFromSaveGame(UAOWSaveGame* SaveGame);
	//@Inventory의 Item 정보를 Item Manager로부터 Load합니다.
	void LoadDefaultItemsFromItemManager(UItemManagerSubsystem* ItemManager);
#pragma endregion

#pragma region Inventory
private:
	//@저장할 수 있는 최대 아이템 갯수, 수정 절대 하지 마세요
	const int MaxInventorySize = 30;
public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE int GetMaxInventorySize() { return MaxInventorySize; }
protected:
	//@인벤토리에 저장된 Item Unique Id와 Item 정보 매핑
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TMap<FGuid, FInventoryItem> Inventory;
public:
	/*
	* @목적: Item Actor를 Inventory에 추가합니다.
	* @설명: 두 가지 AddItem 함수를 활용해, Inventory에 아이템 정보를 추가합니다.
	* @참고: AddItem(AItem* Item, int32 Num = 1), AddItem(TSubclassOf<AItem> BlueprintItemClass, int32 Num = 1)
	*/
	void AddItem(AItem* AlreadySpawnedItem, int32 Num = 1);
	void AddItem(UItemManagerSubsystem* ItemManager, TSubclassOf<AItem> BlueprintItemClass, int32 Num = 1);
	/*
	* @목적: Item Actor가 현재 Inventory에 저장되어 있는지 확인합니다.
	* @설명: Item Tag와 Item Class를 통해 확인할 수 있습니다.
	*/
	bool FindExistingItem(const FGameplayTag& ItemTag, /*OUT*/FGuid& OutItemID);
	bool FindExistingItemByClass(TSubclassOf<AItem> ItemClass, /*OUT*/FGuid& OutItemID);
protected:
	//@Inventory에 새로운 아이템 추가
	FGuid AddNewItem(TSubclassOf<AItem> BlueprintItemClass, int32 Num, AItem* AlreadySpawnedItem = nullptr);
	//@Inventory에 기존 아이템 추가
	void AddExistingItem(const FGuid& ItemId, const FItemInformation& ItemInfo, int32 Num);
	//@아이템 제거
	void RemoveExistingItem();
	//@아이템 사용
	void StartUseItem(const FGuid& Item);
	//@아이템 스폰
	void SpawnItem();
	//@아이템 교체
	void SwapItem();
	//@아이템 사용 종료
	void EndUseItem();
#pragma endregion

#pragma region Quick Slots
private:
	//@설정 가능한 최대 퀵 슬롯 아이템 갯수, 절대 수정하지 마세요.
	const int MaxQuicKSlotSize = 3;
public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE int GetMaxQuicKSlotSize() { return MaxQuicKSlotSize; }
protected:
	//@퀵 슬롯에 추가된 고유의 아이템 아이디 목록, 크기는 3 이하로 고정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TArray<FGuid> QuickSlots;
#pragma endregion

#pragma region Item
protected:
	//@추가된 아이템의 재활성화
	void EnableItem(AItem* Item);
	//@추가된 아이템의 비활성화
	void DisableItem(AItem* Item);
#pragma endregion

#pragma region Inventory UI

#pragma region Inventory UI - Delegates
public:
	//@Delegate, Inventory에 아이템 추가 시 실행
	UPROPERTY(BlueprintAssignable)
		FOnItemAddedToInventory OnItemAddedToInventory;
	//@Delegate: Inventory에서 기존 아이템 제거 시 실행
	UPROPERTY(BlueprintAssignable)
		FOnItemRemovedFromInventory OnItemRemovedFromInventory;
#pragma endregion

#pragma region Inventory UI - Callbacks
public:
	//@Callbacks: 기존 아이템 제거 시 호출되는 콜백
	UFUNCTION()
		void OnUIItemRemovalRequested(const FGuid& UniqueItemID);
	//@Callbacks: QuickSlot에 새로운 아이템 추가 시 호출되는 콜백
	UFUNCTION()
		void OnUIQuickSlotAssigned(int32 SlotIndex, const FGuid& UniqueItemID);
	//@Callbaks: QuickSlot의 아이템 활성화 시 호출되는 콜백
	UFUNCTION()
		void OnUIItemActivated(const FGuid& UniqueItemID);
#pragma endregion

#pragma endregion

};
