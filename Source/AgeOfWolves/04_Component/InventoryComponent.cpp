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
FInventoryItem::FInventoryItem(AActor* Actor, AItem* Item, int32 Num, FGuid ItemID)
{
	check(Actor);
	check(Item);

	//@Owner Actor
	OwnerActorPtr = Actor;
	//@Item Class
	ItemClass = Item->GetClass();
	//@Item Instance
	ItemInstance = Item;
	//@Item Count
	ItemCount = Num;
}

FInventoryItem::FInventoryItem(AActor* Actor, TSubclassOf<AItem> ItemBlueprintClass, int32 Num, FGuid ItemID)
{
	check(Actor);

	//@Owner Actor
	OwnerActorPtr = Actor;
	//@Item Class
	ItemClass = ItemBlueprintClass;
    //@Item Instance
    ItemInstance = SpawnAndDisableItem(Actor, ItemBlueprintClass);
    if (!ItemInstance)
    {
        UE_LOGFMT(LogInventory, Error, "Item을 Inventory에 추가하는 과정에서, Item Instance 생성에 실패했습니다.");
    }
	//@Item Count
	ItemCount = Num;
}

const FGameplayTag FInventoryItem::GetItemTag() const
{
    //@Item Instance
    if (!ItemInstance.Get())
    {
        UE_LOGFMT(LogInventory, Error, "Item Instance가 유효하지 않습니다. FInventoryItem::GetItemTag에서 오류 발생.");
        static const FGameplayTag EmptyTag;
        return EmptyTag;
    }

    return ItemInstance->GetItemTag();
}

TSubclassOf<AItem> FInventoryItem::GetItemClass() const
{
    if (!ItemInstance.Get())
    {
        UE_LOGFMT(LogInventory, Error, "Item Instance가 유효하지 않습니다. FInventoryItem::GetItemClass에서 오류 발생.");
        return nullptr;
    }

    return ItemInstance->GetClass();
}

#pragma endregion

#pragma region Item
AItem* FInventoryItem::SpawnAndDisableItem(AActor* OwnerActor, TSubclassOf<AItem> ItemBlueprintClass)
{
    //@Owner Actor
    if (!IsValid(OwnerActor))
    {
        UE_LOG(LogInventory, Error, TEXT("Invalid Owner Actor"));
        return nullptr;
    }
    //@World
    UWorld* World = OwnerActor->GetWorld();
    if (!World)
    {
        UE_LOG(LogInventory, Error, TEXT("Invalid World reference"));
        return nullptr;
    }
    //@Transform
    FTransform SpawnTransform = FTransform::Identity;
    //@Begin Deferred Spawn
    AItem* NewItem = Cast<AItem>(UGameplayStatics::BeginDeferredActorSpawnFromClass(World, ItemBlueprintClass, SpawnTransform));
    if (NewItem)
    {
        //@충돌 Geometry 계산 생략
        NewItem->SetActorEnableCollision(false);
        //@가시성
        NewItem->SetActorHiddenInGame(true);
        //@Tick함수 비활성화
        NewItem->SetActorTickEnabled(false);
        //@컴포넌트 비활성화
        TArray<UPrimitiveComponent*> PrimitiveComponents;
        NewItem->GetComponents<UPrimitiveComponent>(PrimitiveComponents);
        for (auto* Comp : PrimitiveComponents)
        {
            Comp->SetVisibility(false);
        }
        //@Finish Deferred Spawn
        UGameplayStatics::FinishSpawningActor(NewItem, SpawnTransform);
        return NewItem;
    }
    else
    {
        UE_LOGFMT(LogInventory, Error, "{0} 유형의 객체의 Deferred Spawn에 실패했습니다.", ItemBlueprintClass->GetName());
        return nullptr;
    }
}
#pragma endregion

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

void UInventoryComponent::AddItem(AItem* Item, int32 Num)
{
    //@Item
    if (!IsValid(Item))
    {
        UE_LOGFMT(LogInventory, Error, "추가하고자 하는 Item이 유효하지 않습니다.");
        return;
    }
    //@Item TAG
    const FGameplayTag& ItemTag = Item->GetItemTag();
    //@Item 비활성화
    DisableItem(Item);
    //@기존 아이템 검색 및 개수 증가
    FGuid ExistingItemID;
    if (FindExistingItem(ItemTag, ExistingItemID))
    {
        //TODO: Max Stack에 Clamping
        Inventory[ExistingItemID].ItemCount += Num;
        //@Add Item Event
        OnItemAddedToInventory.Broadcast(ExistingItemID);
        UE_LOGFMT(LogInventory, Log, "{0}: 인벤토리의 기존 아이템 개수가 증가했습니다.", ItemTag.ToString());
        UE_LOGFMT(LogInventory, Warning, "ItemTag : {0}, Item Count : {1}",
            Inventory[ExistingItemID].GetItemTag().ToString(),
            FString::SanitizeFloat(Inventory[ExistingItemID].ItemCount));
    }
    //@새 아이템 추가
    else
    {
        //@Add New Item
        FGuid NewID = AddNewItem(Item, Num);
        if (!NewID.IsValid())
        {
            UE_LOGFMT(LogInventory, Error, "{0}: 인벤토리에 해당 아이템을 추가하는데 실패했습니다.", ItemTag.ToString());
            return;
        }
        //@Add Item Delegate
        OnItemAddedToInventory.Broadcast(NewID);
        UE_LOGFMT(LogInventory, Log, "{0}: 인벤토리에 새로운 아이템이 추가되었습니다.", ItemTag.ToString());
        UE_LOGFMT(LogInventory, Warning, "ItemTag : {0}, Item Count : {1}",
            Inventory[NewID].GetItemTag().ToString(),
            FString::SanitizeFloat(Inventory[NewID].ItemCount));
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
    //@기존 아이템 검색 및 개수 증가
    FGuid ExistingItemID;
    if (FindExistingItemByClass(BlueprintItemClass, ExistingItemID))
    {
        //@Add Existing Item
        //@TODO: Max Stack 에 Clamp
        Inventory[ExistingItemID].ItemCount += Num;
        OnItemAddedToInventory.Broadcast(ExistingItemID);
        UE_LOGFMT(LogInventory, Log, "{0}: 인벤토리의 기존 아이템 개수가 증가했습니다.", ItemTag.ToString());
        UE_LOGFMT(LogInventory, Warning, "ItemTag : {0}, Item Count : {1}",
            Inventory[ExistingItemID].GetItemTag().ToString(),
            FString::SanitizeFloat(Inventory[ExistingItemID].ItemCount));
    }
    //@새 아이템 추가
    else
    {
        //@Add New Item
        FGuid NewID = AddNewItem(DefaultItem, Num);
        if (!NewID.IsValid())
        {
            UE_LOGFMT(LogInventory, Error, "{0}: 인벤토리에 해당 아이템을 추가하는데 실패했습니다.", ItemTag.ToString());
            return;
        }
        //@Add Item Event
        OnItemAddedToInventory.Broadcast(NewID);
        UE_LOGFMT(LogInventory, Log, "{0}: 인벤토리에 새로운 아이템이 추가되었습니다.", ItemTag.ToString());
        UE_LOGFMT(LogInventory, Warning, "ItemTag : {0}, Item Count : {1}",
            Inventory[NewID].GetItemTag().ToString(),
            FString::SanitizeFloat(Inventory[NewID].ItemCount));
    }
}

FGuid UInventoryComponent::AddNewItem(AItem* Item, int32 Num)
{
    //@Owner Actor
    AActor* OwnerActor = GetOwner();
    if (!IsValid(OwnerActor))
    {
        UE_LOGFMT(LogInventory, Error, "Inventory 소유자가 유효하지 않습니다!");
        return FGuid();
    }
    // @Game Instance
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
    if (!GameInstance)
    {
        UE_LOGFMT(LogInventory, Error, "GameInstance를 가져올 수 없습니다.");
        return FGuid();
    }
    //@Inventory Max Size
    if (Inventory.Num() >= InventoryMaxSize)
    {
        UE_LOGFMT(LogInventory, Warning,"Inventory가 꽉차서 더 이상 아이템을 추가할 수 없습니다.");
        return FGuid();
    }
    //@Item Manager Subsystem
    UItemManagerSubsystem* ItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogInventory, Error, "ItemManagerSubsystem을 가져올 수 없습니다.");
        return FGuid();
    }
    //@Item Max Stack
    if (const auto ItemInfo = ItemManager->GetItemInformation<FItemInformation>(Item->GetItemType(), Item->GetItemTag()))
    {
        if (Num > ItemInfo->MaxStack)
        {
            UE_LOGFMT(LogInventory, Warning, "아이템 최대 스택 수치를 초과할 수 없습니다. 요청: {0}, 최대 스택: {1}",
                Num, ItemInfo->MaxStack);
            return FGuid();
        }
    }
    //@CallBack 바인딩, Item
    OnItemAddedToInventory.AddDynamic(Item, &AItem::OnItemAddedToInventory);
    //@FInventoryItem 생성 및 추가
    FGuid NewID = FGuid::NewGuid();
    FInventoryItem NewInvenItem(OwnerActor, Item, Num, NewID);
    Inventory.Add(NewID, NewInvenItem);

    return NewID;

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

void UInventoryComponent::StartUseItem()
{
	//@Item Instancing
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

//void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//}