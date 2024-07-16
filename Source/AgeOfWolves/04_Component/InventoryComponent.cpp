#include "InventoryComponent.h"
#include "Logging/StructuredLog.h"
#include "Kismet/GameplayStatics.h"

#include "00_GameInstance/AOWGameInstance.h"
#include "14_Subsystem/ItemManagerSubsystem.h"
#include "09_Item/Item.h"

// UE_LOGFMT(LogInventory, Log, "");

DEFINE_LOG_CATEGORY(LogInventory)

#pragma region Inventory Item

#pragma region Default Setting
FInventoryItem::FInventoryItem(AActor* Actor, TSubclassOf<AItem> ItemBlueprintClass, int32 Num, FGuid ItemID)
{
	check(Actor);
    check(ItemBlueprintClass);

	//@Owner Actor
	OwnerActorPtr = Actor;
	//@Item Class
	ItemClass = ItemBlueprintClass;
    //@Item Instance
    ItemCDO = ItemBlueprintClass.GetDefaultObject();
    if (!ItemCDO)
    {
        UE_LOGFMT(LogInventory, Error, "Item을 Inventory에 추가하는 과정에서, Item Instance 생성에 실패했습니다.");
    }
	//@Item Count
	ItemCount = Num;
}

const FGameplayTag FInventoryItem::GetItemTag() const
{
    //@Item Instance
    if (!ItemCDO.Get())
    {
        UE_LOGFMT(LogInventory, Error, "Item Instance가 유효하지 않습니다. FInventoryItem::GetItemTag에서 오류 발생.");
        static const FGameplayTag EmptyTag;
        return EmptyTag;
    }

    return ItemCDO->GetItemTag();
}

TSubclassOf<AItem> FInventoryItem::GetItemClass() const
{
    if (!ItemCDO.Get())
    {
        UE_LOGFMT(LogInventory, Error, "Item Instance가 유효하지 않습니다. FInventoryItem::GetItemClass에서 오류 발생.");
        return nullptr;
    }

    return ItemCDO->GetClass();
}
#pragma endregion

//@TODO: 수정, 사용하지 않음
//AItem* FInventoryItem::SpawnAndDisableItem(AActor* OwnerActor, TSubclassOf<AItem> ItemBlueprintClass)
//{
//    // Owner Actor 확인
//    if (!IsValid(OwnerActor))
//    {
//        UE_LOG(LogInventory, Error, TEXT("Invalid Owner Actor"));
//        return nullptr;
//    }
//
//    // World 가져오기
//    UWorld* World = OwnerActor->GetWorld();
//    if (!World)
//    {
//        UE_LOG(LogInventory, Error, TEXT("Invalid World reference"));
//        return nullptr;
//    }
//
//    // Transform 설정
//    FTransform SpawnTransform = FTransform::Identity;
//
//    // Begin Deferred Spawn
//    AItem* NewItem = Cast<AItem>(UGameplayStatics::BeginDeferredActorSpawnFromClass(World, ItemBlueprintClass, SpawnTransform));
//    if (NewItem)
//    {
//        // 충돌 Geometry 계산 생략
//        NewItem->SetActorEnableCollision(false);
//
//        // 가시성 설정
//        NewItem->SetActorHiddenInGame(true);
//
//        // Tick 함수 비활성화
//        NewItem->SetActorTickEnabled(false);
//
//        // 컴포넌트 비활성화
//        TArray<UPrimitiveComponent*> PrimitiveComponents;
//        NewItem->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
//        for (auto* Comp : PrimitiveComponents)
//        {
//            Comp->SetVisibility(false);
//        }
//
//        // Finish Deferred Spawn
//        UGameplayStatics::FinishSpawningActor(NewItem, SpawnTransform);
//        return NewItem;
//    }
//    else
//    {
//        UE_LOGFMT(LogInventory, Error, "{0} 유형의 객체의 Deferred Spawn에 실패했습니다.", ItemBlueprintClass->GetName());
//        return nullptr;
//    }
//}

#pragma endregion

#pragma region Default Setting
UInventoryComponent::UInventoryComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
    bWantsInitializeComponent = true;//Initialize Component 활용에 필요 

}

void UInventoryComponent::OnRegister()
{
	Super::OnRegister();

}

void UInventoryComponent::OnUnregister()
{
	Super::OnUnregister();

}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

    Inventory.Empty();
    QuickSlots.SetNum(MaxQuicKSlotSize);
}

void UInventoryComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UInventoryComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

#pragma endregion

#pragma region Inventory
void UInventoryComponent::LoadInventory()
{
    //@GameInstance
    if (const auto& GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        //@SaveFile
        if (GameInstance->DoesSaveGameExist())
        {
            auto* SaveGameInstance = GameInstance->GetSaveGameInstance();
            LoadItemsFromSaveGame(SaveGameInstance);
        }
        //@Item Manager
        else
        {
            UItemManagerSubsystem* ItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
            LoadDefaultItems(ItemManager);
        }
    }
}

void UInventoryComponent::LoadItemsFromSaveGame(UAOWSaveGame* SaveGame)
{
    if (!SaveGame)
    {
        UE_LOGFMT(LogInventory, Error, "Save Game이 유효하지 않습니다!");
        return;
    }

    //@TODO: SaveGame으로부터 Save된 Inventory 정보 가져와서 초기화
}

void UInventoryComponent::LoadDefaultItems(UItemManagerSubsystem* ItemManager)
{
    //@Item Manager
    if (!ItemManager)
    {
        UE_LOGFMT(LogInventory, Error, "Item Manager가 유효하지 않습니다!");
        return;
    }
    //@Default Items from Item Manager
    TArray<TPair<int32, TSubclassOf<AItem>>> DefaultItems = ItemManager->GetAllDefaultItems();
    if (DefaultItems.IsEmpty())
    {
        UE_LOGFMT(LogInventory, Error, "Default Item 목록이 비어있습니다!");
        return;
    }
    //@Inventory
    for (auto& it : DefaultItems)
    {
        AddItem(ItemManager, it.Value, it.Key);
    }

}

void UInventoryComponent::AddItem(AItem* AlreadySpawnedItem, int32 Num)
{
    //@Item
    if (!IsValid(AlreadySpawnedItem))
    {
        UE_LOGFMT(LogInventory, Error, "추가하고자 하는 AlreadySpawnedItem이 유효하지 않습니다.");
        return;
    }
    //@Item Tag
    const FGameplayTag& ItemTag = AlreadySpawnedItem->GetItemTag();
    //@Item Manager Subsystem
    UItemManagerSubsystem* ItemManager = GetWorld()->GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogInventory, Error, "ItemManagerSubsystem을 가져올 수 없습니다.");
        return;
    }
    //@FItemInformation
    const FItemInformation* ItemInfo = ItemManager->GetItemInformation<FItemInformation>(AlreadySpawnedItem->GetItemType(), ItemTag);
    if (!ItemInfo)
    {
        UE_LOGFMT(LogInventory, Error, "ItemInformation을 가져올 수 없습니다.");
        return;
    }
    //@Add Existing Item
    FGuid ExistingItemID;
    if (FindExistingItem(ItemTag, ExistingItemID))
    {
        AddExistingItem(ExistingItemID, *ItemInfo, Num);
        OnItemAddedToInventory.Broadcast(ExistingItemID);
    }
    //@Add New Item
    else
    {
        FGuid NewID = AddNewItem(AlreadySpawnedItem->GetClass(), Num, AlreadySpawnedItem);
        if (!NewID.IsValid())
        {
            UE_LOGFMT(LogInventory, Error, "{0}: 인벤토리에 해당 아이템을 추가하는데 실패했습니다.", ItemTag.ToString());
            return;
        }
        OnItemAddedToInventory.Broadcast(NewID);
    }
}

void UInventoryComponent::AddItem(UItemManagerSubsystem* ItemManager, TSubclassOf<AItem> BlueprintItemClass, int32 Num)
{
    //@Item Class
    if (!BlueprintItemClass)
    {
        UE_LOGFMT(LogInventory, Error, "유효하지 않은 Item Class입니다.");
        return;
    }
    //@Item CDO
    AItem* DefaultItem = BlueprintItemClass.GetDefaultObject();
    if (!DefaultItem)
    {
        UE_LOGFMT(LogInventory, Error, "아이템 클래스로부터 유효한 Default Object을 가져오는데 실패했습니다.");
        return;
    }
    //@Item Tag
    const FGameplayTag& ItemTag = DefaultItem->GetItemTag();
    if (!ItemTag.IsValid())
    {
        UE_LOGFMT(LogInventory, Error, "아이템 태그가 유효하지 않습니다.");
        return;
    }
    //@Item Type
    EItemType ItemType = DefaultItem->GetItemType();
    //@Item Manager
    if (!ItemManager)
    {
        UE_LOGFMT(LogInventory, Error, "ItemManagerSubsystem을 가져올 수 없습니다.");
        return;
    }
    //@Item Info
    const FItemInformation* ItemInfo = ItemManager->GetItemInformation<FItemInformation>(ItemType, ItemTag);
    if (!ItemInfo)
    {
        UE_LOGFMT(LogInventory, Error, "ItemInformation을 가져올 수 없습니다.");
        return;
    }
    //@Add Existing Item
    FGuid ExistingItemID;
    if (FindExistingItemByClass(BlueprintItemClass, ExistingItemID))
    {
        AddExistingItem(ExistingItemID, *ItemInfo, Num);
        OnItemAddedToInventory.Broadcast(ExistingItemID);
    }
    //@Add New Item
    else
    {
        FGuid NewID = AddNewItem(BlueprintItemClass, Num);
        if (!NewID.IsValid())
        {
            UE_LOGFMT(LogInventory, Error, "{0}: 인벤토리에 해당 아이템을 추가하는데 실패했습니다.", ItemTag.ToString());
            return;
        }
        OnItemAddedToInventory.Broadcast(NewID);
    }
}

FGuid UInventoryComponent::AddNewItem(TSubclassOf<AItem> BlueprintItemClass, int32 Num, AItem* AlreadySpawnedItem)
{
    //@Owner Actor
    AActor* OwnerActor = GetOwner();
    if (!IsValid(OwnerActor))
    {
        UE_LOGFMT(LogInventory, Error, "Inventory 소유자가 유효하지 않습니다!");
        return FGuid();
    }
    //@Game Insatnce
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
    if (!GameInstance)
    {
        UE_LOGFMT(LogInventory, Error, "GameInstance를 가져올 수 없습니다.");
        return FGuid();
    }
    //@Inventory Max Size
    if (Inventory.Num() >= MaxInventorySize)
    {
        UE_LOGFMT(LogInventory, Warning, "Inventory가 꽉차서 더 이상 아이템을 추가할 수 없습니다.");
        return FGuid();
    }
    //Item Manager Subsystem
    UItemManagerSubsystem* ItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogInventory, Error, "ItemManagerSubsystem을 가져올 수 없습니다.");
        return FGuid();
    }
    //@Item-1
    AItem* ItemToUse = AlreadySpawnedItem;
    if (!ItemToUse)
    {
        ItemToUse = BlueprintItemClass.GetDefaultObject();
    }
    //@Item-2
    if (!ItemToUse)
    {
        UE_LOGFMT(LogInventory, Error, "유효한 아이템 객체를 얻는데 실패했습니다.");
        return FGuid();
    }
    //@FItemInformation
    const FItemInformation* ItemInfo = ItemManager->GetItemInformation<FItemInformation>(ItemToUse->GetItemType(), ItemToUse->GetItemTag());
    if (!ItemInfo)
    {
        UE_LOGFMT(LogInventory, Error, "ItemInformation을 가져올 수 없습니다.");
        return FGuid();
    }
    //@Max Stack
    if (Num > ItemInfo->MaxStack)
    {
        UE_LOGFMT(LogInventory, Warning, "아이템 최대 스택 수치를 초과할 수 없습니다. 요청: {0}, 최대 스택: {1}",
            Num, ItemInfo->MaxStack);
        return FGuid();
    }
    //@Callbacks
    if (AlreadySpawnedItem)
    {
        OnItemAddedToInventory.AddDynamic(AlreadySpawnedItem, &AItem::OnItemAddedToInventory);
    }
    //@FInventoryItem
    FGuid NewID = FGuid::NewGuid();
    FInventoryItem NewInvenItem(OwnerActor, BlueprintItemClass, Num, NewID);
    Inventory.Add(NewID, NewInvenItem);
    //@Disable Item
    if (AlreadySpawnedItem)
    {
        DisableItem(AlreadySpawnedItem);
    }

    UE_LOGFMT(LogInventory, Log, "{0}: 인벤토리에 새로운 아이템이 추가되었습니다. 개수: {1}",
        ItemToUse->GetItemTag().ToString(), Num);

    return NewID;
}

void UInventoryComponent::AddExistingItem(const FGuid& ItemId, const FItemInformation& ItemInfo, int32 Num)
{
    //@Existing FInventory Item
    if (!Inventory.Contains(ItemId))
    {
        UE_LOGFMT(LogInventory, Error, "인벤토리에 해당 ItemId를 가진 아이템이 없습니다.");
        return;
    }
    //@Existing Item
    FInventoryItem& ExistingItem = Inventory[ItemId];
    //@Count
    int32 CurrentCount = ExistingItem.ItemCount;
    int32 MaxStack = ItemInfo.MaxStack;
    //@Available Space
    int32 AvailableSpace = FMath::Max(0, MaxStack - CurrentCount);
    int32 ActuallyAdded = FMath::Min(Num, AvailableSpace);
    //@Add Existing Item
    ExistingItem.ItemCount += ActuallyAdded;

    if (ActuallyAdded < Num)
    {
        UE_LOGFMT(LogInventory, Warning, "{0}: 최대 저장 갯수({1})를 초과하여 일부만 추가되었습니다. 추가된 갯수: {2}, 초과된 갯수: {3}",
            ExistingItem.GetItemTag().ToString(), MaxStack, ActuallyAdded, Num - ActuallyAdded);
    }
    else
    {
        UE_LOGFMT(LogInventory, Log, "{0}: 아이템 {1}개가 성공적으로 추가되었습니다. 현재 갯수: {2}/{3}",
            ExistingItem.GetItemTag().ToString(), ActuallyAdded, ExistingItem.ItemCount, MaxStack);
    }
}

void UInventoryComponent::RemoveExistingItem()
{
    //@갯수 확인, bRemovable 확인
    //@콜백 함수 제거 
    //@제거
}

bool UInventoryComponent::FindExistingItemByClass(TSubclassOf<AItem> ItemClass, FGuid& OutItemID)
{
    for (const auto& Pair : Inventory)
    {
        if (Pair.Value.GetItemClass() == ItemClass)
        {
            OutItemID = Pair.Key;
            return true;
        }
    }
    return false;
}

bool UInventoryComponent::FindExistingItem(const FGameplayTag& ItemTag, FGuid& OutItemID)
{
    for (const auto& Pair : Inventory)
    {
        if (Pair.Value.GetItemTag() == ItemTag)
        {
            OutItemID = Pair.Key;
            return true;
        }
    }
    return false;
}

void UInventoryComponent::StartUseItem(const FGuid& UniqueItemId)
{
	//@Item 활성화
}

void UInventoryComponent::SpawnItem()
{
	//@Deferred Spawn
}

void UInventoryComponent::SwapItem()
{
	//@Equipped Item
}

void UInventoryComponent::EndUseItem()
{

}
#pragma endregion

#pragma region Item
void UInventoryComponent::EnableItem(AItem* Item)
{
    //@Item
    if (!IsValid(Item))
    {
        UE_LOG(LogInventory, Warning, TEXT("EnableItem: 유효하지 않은 Item입니다!"));
        return;
    }
    //@Collision 비활성화(Geometry 계산 최소화)
    Item->SetActorEnableCollision(true);
    //@가시성 
    Item->SetActorHiddenInGame(false);
    //@Tick함수 끄기
    Item->SetActorTickEnabled(true);
    //@Component의 가시성
    TArray<UPrimitiveComponent*> PrimitiveComponents;
    Item->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
    for (auto* Comp : PrimitiveComponents)
    {
        if (IsValid(Comp))
        {
            Comp->SetVisibility(true);
        }
    }

    UE_LOG(LogInventory, Log, TEXT("아이템 '%s'이(가) 활성화되었습니다."), *Item->GetName());
}

void UInventoryComponent::DisableItem(AItem* Item)
{
    //@Item
    if (!IsValid(Item))
    {
        UE_LOG(LogInventory, Warning, TEXT("EnableItem: 유효하지 않은 Item입니다!"));
        return;
    }
    //@Collision 비활성화(Geometry 계산 최소화)
    Item->SetActorEnableCollision(false);
    //@가시성 
    Item->SetActorHiddenInGame(true);
    //@Tick함수 끄기
    Item->SetActorTickEnabled(false);
    //@Component의 가시성
    TArray<UPrimitiveComponent*> PrimitiveComponents;
    Item->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
    for (auto* Comp : PrimitiveComponents)
    {
        Comp->SetVisibility(false);
    }
}
#pragma endregion

#pragma region Inventory UI

#pragma region Inventory UI - Callbacks
void UInventoryComponent::OnUIItemRemovalRequested(const FGuid& UniqueItemID)
{
    if (Inventory.Contains(UniqueItemID))
    {
        FInventoryItem& ItemToRemove = Inventory[UniqueItemID];
        FGameplayTag ItemTag = ItemToRemove.GetItemTag();

        // 아이템 제거 로직
        Inventory.Remove(UniqueItemID);

        // 아이템 제거 이벤트 발생
        OnItemRemovedFromInventory.Broadcast(UniqueItemID, ItemTag);

        UE_LOGFMT(LogInventory, Log, "아이템 {0} ({1})이(가) 인벤토리에서 제거되었습니다.",
            UniqueItemID.ToString(), ItemTag.ToString());
    }
    else
    {
        UE_LOGFMT(LogInventory, Warning, "제거하려는 아이템 ID {0}을(를) 인벤토리에서 찾을 수 없습니다.", UniqueItemID.ToString());
    }

}

void UInventoryComponent::OnUIQuickSlotAssigned(int32 SlotIndex, const FGuid& UniqueItemID)
{
    //@MaxSize
    if (SlotIndex >= 0 && SlotIndex < MaxQuicKSlotSize)
    {
        //@Inventory
        if (Inventory.Contains(UniqueItemID))
        {
            //@QuickSlots
            QuickSlots[SlotIndex] = UniqueItemID;
            UE_LOGFMT(LogInventory, Log, "아이템 {0}이(가) 퀵슬롯 {1}에 할당되었습니다.",
                Inventory[UniqueItemID].GetItemTag().ToString(), SlotIndex);
        }
        else
        {
            UE_LOGFMT(LogInventory, Warning, "퀵슬롯에 할당하려는 아이템 ID {0}을(를) 찾을 수 없습니다.", UniqueItemID.ToString());
        }
    }
    else
    {
        UE_LOGFMT(LogInventory, Error, "유효하지 않은 퀵슬롯 인덱스: {0}", SlotIndex);
    }
}

void UInventoryComponent::OnUIItemActivated(const FGuid& UniqueItemID)
{
    //@Inventory
    if (Inventory.Contains(UniqueItemID))
    {
        //@FInventoryItem
        FInventoryItem& Item = Inventory[UniqueItemID];
        UE_LOGFMT(LogInventory, Log, "아이템 {0}이(가) 활성화를 시도합니다.", Item.GetItemTag().ToString());
        //Start Use Item
        StartUseItem(UniqueItemID);
    }
    else
    {
        UE_LOGFMT(LogInventory, Warning, "활성화하려는 아이템 ID {0}을(를) 찾을 수 없습니다.", UniqueItemID.ToString());
    }
}
#pragma endregion

#pragma endregion

//void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}