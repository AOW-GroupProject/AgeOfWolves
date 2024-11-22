#include "InventoryComponent.h"
#include "Logging/StructuredLog.h"
#include "Kismet/GameplayStatics.h"

#include "00_GameInstance/AOWGameInstance.h"
#include "14_Subsystem/ItemManagerSubsystem.h"
#include "09_Item/Item.h"

#include "04_Component/UIComponent.h" // 제거 예정

#include "08_UI/01_HUD/PlayerHUD.h"
#include "08_UI/01_HUD/QuickSlots.h"

#include "08_UI/02_Menu/01_InventoryUI/InventoryUI.h"
#include "08_UI/02_Menu/01_InventoryUI/ItemSlots.h"

DEFINE_LOG_CATEGORY(LogInventory)
// UE_LOGFMT(LogInventory, Log, "");

//@Sturcts
#pragma region Structs
FInventoryItem::FInventoryItem(AActor* Actor, TSubclassOf<AItem> ItemBlueprintClass, int32 Num, FGuid ItemID, bool bRemovable)
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
    //@bRemovable
    this->bRemovable = bRemovable;
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

EItemType FInventoryItem::GetItemType() const
{
    if (!ItemCDO.Get())
    {
        UE_LOGFMT(LogInventory, Error, "Item Instance가 유효하지 않습니다. FInventoryItem::GetItemClass에서 오류 발생.");
        return EItemType::MAX;
    }

    return ItemCDO->GetItemType();
}

AItem* FInventoryItem::GetItem()
{
    return ItemCDO.Get() ? ItemCDO : nullptr;
}
#pragma endregion

//@Defualt Setting
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

    // ItemManager 캐싱
    if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
    {
        CachedItemManager = GameInstance->GetSubsystem<UItemManagerSubsystem>();
        if (!CachedItemManager.IsValid())
        {
            UE_LOGFMT(LogInventory, Error, "ItemManagerSubsystem 캐싱 실패");
        }
        else
        {
            UE_LOGFMT(LogInventory, Log, "ItemManagerSubsystem 캐싱 성공");
        }
    }
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

    //@TODO: Save Game에 현재 Inventory 정보 저장
}

void UInventoryComponent::ExternalBindToUIComponent(const AController* Controller)
{
    check(Controller);

    if (auto UIComponent = Controller->FindComponentByClass<UUIComponent>())
    {
        //@UI Comp의 HUD 초기화 완료 이벤트(아이템 로드 작업 과 HUD의 초기화 작업의 동기화)
        UIComponent->UIsForInventoryReady.BindUFunction(this, "LoadInventory");

        UE_LOGFMT(LogInventory, Log, "Inventory Component가 UI Component에 성공적으로 바인딩 되었습니다.");
    }
}

void UInventoryComponent::ExternalBindToQuickSlots()
{
    UE_LOGFMT(LogInventory, Log, "{0}: QuickSlots 바인딩 작업을 시작합니다.", __FUNCTION__);

    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogInventory, Error, "{0}: World is null", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogInventory, Error, "{0}: PlayerController is null", __FUNCTION__);
        return;
    }

    //@UI Comp
    UUIComponent* UIComponent = PC->FindComponentByClass<UUIComponent>();
    if (!UIComponent)
    {
        UE_LOGFMT(LogInventory, Warning, "{0}: UIComponent를 가져오는 데 실패했습니다.", __FUNCTION__);
        return;
    }

    //@Player HUD
    UPlayerHUD* PlayerHUD = Cast<UPlayerHUD>(UIComponent->GetUI(EUICategory::HUD));
    if (!PlayerHUD)
    {
        UE_LOGFMT(LogInventory, Warning, "{0}: PlayerHUD를 가져오는 데 실패했습니다.", __FUNCTION__);
        return;
    }

    //@Quick Slots
    //UQuickSlots* QuickSlotsUI = PlayerHUD->GetQuickSlotsUI();
    //if (!QuickSlotsUI)
    //{
    //    UE_LOGFMT(LogInventory, Warning, "{0}: QuickSlots를 가져오는 데 실패했습니다.", __FUNCTION__);
    //    return;
    //}

    ////@External Binding
    //QuickSlotsUI->QuickSlotItemActivated.BindUFunction(this, "StartUseItem");

    UE_LOGFMT(LogInventory, Log, "{0}: Inventory Component가 QuickSlots에 성공적으로 바인딩 되었습니다.", __FUNCTION__);
}

void UInventoryComponent::ExternalBindToInventoryUI()
{

    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogInventory, Error, "{0}: World is null", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogInventory, Error, "{0}: PlayerController is null", __FUNCTION__);
        return;
    }

    //@UI Comp
    UUIComponent* UIComponent = PC->FindComponentByClass<UUIComponent>();
    if (!UIComponent)
    {
        UE_LOGFMT(LogInventory, Warning, "{0}: UIComponent를 가져오는 데 실패했습니다.", __FUNCTION__);
        return;
    }

    //@Menu UI
    UMenuUI* Menu = Cast<UMenuUI>(UIComponent->GetUI(EUICategory::Menu));
    if (!Menu)
    {
        UE_LOGFMT(LogInventory, Warning, "{0}: Menu를 가져오는 데 실패했습니다.", __FUNCTION__);
        return;
    }

    //@Inventory UI
    UInventoryUI* InventoryUI = Cast<UInventoryUI>(Menu->GetCategoryUI(EMenuCategory::Inventory));
    if (!InventoryUI)
    {
        UE_LOGFMT(LogInventory, Warning, "{0}: Inventory UI를 가져오는 데 실패했습니다.", __FUNCTION__);
        return;
    }

    //@외부 바인딩
    for (uint8 i = 0; i < static_cast<uint8>(EItemType::MAX); ++i)
    {
        EItemType ItemType = static_cast<EItemType>(i);
        UItemSlots* ItemSlots = InventoryUI->GetItemSlotsByType(ItemType);

        if (ItemSlots)
        {
            ItemSlots->ItemUsed.AddUObject(this, &UInventoryComponent::StartUseItem);
            ItemSlots->ItemLeft.AddUObject(this, &UInventoryComponent::LeaveItem);
            ItemSlots->ItemDiscarded.AddUObject(this, &UInventoryComponent::DiscardItem);

            UE_LOGFMT(LogInventory, Log, "{0}: {1} 타입의 ItemSlots에 이벤트가 성공적으로 바인딩되었습니다.", __FUNCTION__, *UEnum::GetValueAsString(ItemType));
        }
        else
        {
            UE_LOGFMT(LogInventory, Warning, "{0}: {1} 타입의 ItemSlots를 찾을 수 없습니다.", __FUNCTION__, *UEnum::GetValueAsString(ItemType));
        }
    }

    UE_LOGFMT(LogInventory, Log, "{0}: InventoryUI에 성공적으로 바인딩되었습니다.", __FUNCTION__);
}

void UInventoryComponent::InternalBindToItem(AItem* Item, FGuid UniqueItemID)
{
    if (!Item)
    {
        UE_LOGFMT(LogInventory, Error, "{0}: Item이 유효하지 않습니다", __FUNCTION__);
        return;
    }

    Item->ItemActivationEnded.AddUFunction(this, "OnItemActivationEnded", UniqueItemID);
}

void UInventoryComponent::InitializeInventory(const AController* Controller)
{
    //@외부 바인딩
    ExternalBindToUIComponent(Controller);
    //@Inventory
    Inventory.Empty();
    //@Quick Slots
    QuickSlots.SetNum(MaxQuicKSlotSize);
}
#pragma endregion

//@Property/Info...etc
#pragma region Inventory
void UInventoryComponent::LoadInventory()
{
    UE_LOGFMT(LogInventory, Warning, "Inventory의 Load 작업을 시작합니다 : {0}", __FUNCTION__);

    //@외부 바인딩
    ExternalBindToInventoryUI();
    ExternalBindToQuickSlots();

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
            LoadDefaultItemsFromItemManager(ItemManager);
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
    //@TODO: Quick Slot Item들 추가
}

void UInventoryComponent::LoadDefaultItemsFromItemManager(UItemManagerSubsystem* ItemManager)
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
        RequestInitializationByInvenComp.Broadcast(NewID);
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
        //@Delegate
        RequestInitializationByInvenComp.Broadcast(NewID);
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
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(OwnerActor);
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
    if (Num <= 0 || Num > ItemInfo->MaxStack)
    {
        UE_LOGFMT(LogInventory, Warning, "아이템 최대 스택 수치를 초과할 수 없습니다. 요청: {0}, 최대 스택: {1}",
            Num, ItemInfo->MaxStack);
        return FGuid();
    }

    //@Binding
    //@TODO: Inventory의 internal Binding
    if (AlreadySpawnedItem)
    {
        RequestInitializationByInvenComp.AddUFunction(AlreadySpawnedItem, "InitializeItem");
    }

    //@FInventoryItem
    FGuid NewID = FGuid::NewGuid();
    FInventoryItem NewInvenItem(OwnerActor, BlueprintItemClass, Num, NewID, ItemInfo->bRemovable);
    Inventory.Add(NewID, NewInvenItem);

    //@내부 바인딩
    InternalBindToItem(ItemToUse, NewID);

    //@Disable Item
    if (AlreadySpawnedItem)
    {
        DisableItem(AlreadySpawnedItem);
    }

    //@Quick Slots
    if (ItemInfo->bObssessedToQuickSlots
        && ItemInfo->bConsumable
        && ItemInfo->QuickSlotNum > 0 && ItemInfo->QuickSlotNum < 4)
    {
        //@Quick Slots
        QuickSlots[ItemInfo->QuickSlotNum] = NewID;
        //@Delegate
        QuickSlotItemsLoaded.Broadcast(ItemInfo->QuickSlotNum, NewID, ItemInfo->ItemType, ItemInfo->ItemTag, Num);
    }

    //@Delegate
    ItemAssignedToInventory.Broadcast(NewID, ItemInfo->ItemType, ItemInfo->ItemTag, Num);

    UE_LOGFMT(LogInventory, Log, "{0}: 인벤토리에 새로운 아이템이 추가되었습니다. 개수: {1}",
        ItemToUse->GetItemTag().ToString(), Num);

    return NewID;
}

void UInventoryComponent::AddExistingItem(const FGuid& ItemId, const FItemInformation& ItemInfo, int32 Num)
{
    //@Existing FInventory Item
    if (!Inventory.Contains(ItemId))
    {
        UE_LOGFMT(LogInventory, Error, "인벤토리에 ItemId {0}를 가진 아이템이 없습니다.", ItemId.ToString());
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
        UE_LOGFMT(LogInventory, Warning, "아이템 {0}: 최대 저장 갯수({1})를 초과하여 일부만 추가되었습니다. 추가된 갯수: {2}, 초과된 갯수: {3}",
            ExistingItem.GetItemTag().ToString(), MaxStack, ActuallyAdded, Num - ActuallyAdded);
    }

    UE_LOGFMT(LogInventory, Log, "기존 아이템 {0}: {1}개가 추가되었습니다. 현재 갯수: {2}/{3}",
        ExistingItem.GetItemTag().ToString(), ActuallyAdded, ExistingItem.ItemCount, MaxStack);

    //@Quick Slots에 할당되어 있는 아이템인지 확인
    int32 QuickSlotIndex = IsItemAssignedToQuickSlots(ItemId);
    if (QuickSlotIndex != -1 && QuickSlotIndex >= 0 && QuickSlotIndex < MaxQuicKSlotSize)
    {
        QuickSlotItemUpdated.Broadcast(QuickSlotIndex + 1, ItemId, Num);
    }

    //@Delegate for existing item update
    InventoryItemUpdated.Broadcast(ItemId, ExistingItem.GetItemType(), ExistingItem.GetItemTag(), ExistingItem.ItemCount);
}

void UInventoryComponent::RemoveExistingItem(const FGuid& ItemId, const FItemInformation& ItemInfo, int32 Num)
{
    //@아이템 존재 확인
    if (!Inventory.Contains(ItemId))
    {
        UE_LOGFMT(LogInventory, Error, "아이템 제거 실패: 인벤토리에 ID {0}인 아이템이 없습니다.", ItemId.ToString());
        return;
    }

    //@아이템 정보 가져오기
    FInventoryItem& ExistingItem = Inventory[ItemId];
    int32 CurrentCount = ExistingItem.ItemCount;
    const EItemType ItemType = ExistingItem.GetItemType();  // ItemType 저장

    //@제거할 수량 계산
    int32 ActuallyRemoved = FMath::Min(CurrentCount, Num);
    ExistingItem.ItemCount -= ActuallyRemoved;

    //@아이템 완전 제거 여부 확인
    if (ExistingItem.ItemCount <= 0 && ExistingItem.bRemovable)
    {
        //@인벤토리에서 아이템 제거
        Inventory.Remove(ItemId);

        UE_LOGFMT(LogInventory, Log, "아이템 {0}이(가) 인벤토리에서 완전히 제거되었습니다.", ItemInfo.ItemTag.ToString());

        //@아이템 제거 이벤트 발생 (ItemType 추가)
        ItemRemovedFromInventory.Broadcast(ItemId, ItemType);
    }
    else
    {
        if (ExistingItem.ItemCount <= 0)
        {
            //@제거 불가능한 아이템은 수량을 0으로 유지
            ExistingItem.ItemCount = 0;

            UE_LOGFMT(LogInventory, Log, "제거 불가능한 아이템 {0}의 수량이 0이 되었습니다.", ItemInfo.ItemTag.ToString());
        }
        else
        {
            UE_LOGFMT(LogInventory, Log, "아이템 {0}의 수량이 {1}개 감소했습니다. 현재 수량: {2}",
                ItemInfo.ItemTag.ToString(), ActuallyRemoved, ExistingItem.ItemCount);
        }

        //@아이템 업데이트 이벤트 발생
        InventoryItemUpdated.Broadcast(ItemId, ItemType, ItemInfo.ItemTag, ExistingItem.ItemCount);
    }

    //@퀵슬롯 업데이트
    int32 QuickSlotIndex = IsItemAssignedToQuickSlots(ItemId);
    if (QuickSlotIndex != -1 && QuickSlotIndex >= 0 && QuickSlotIndex < MaxQuicKSlotSize)
    {
        if (ExistingItem.ItemCount <= 0 && ExistingItem.bRemovable)
        {
            //@퀵슬롯에서 제거
            QuickSlots[QuickSlotIndex] = FGuid();

            UE_LOGFMT(LogInventory, Log, "아이템 {0}이(가) 퀵슬롯 {1}에서 제거되었습니다.",
                ItemInfo.ItemTag.ToString(), QuickSlotIndex + 1);
        }
        else
        {
            //@퀵슬롯 수량 업데이트
            QuickSlotItemUpdated.Broadcast(QuickSlotIndex + 1, ItemId, ExistingItem.ItemCount);

            UE_LOGFMT(LogInventory, Log, "퀵슬롯 {0}의 아이템 {1} 수량이 업데이트되었습니다. 현재 수량: {2}",
                QuickSlotIndex + 1, ItemInfo.ItemTag.ToString(), ExistingItem.ItemCount);
        }
    }
}

void UInventoryComponent::StartUseItem(const FGuid& UniqueItemID, int32 ItemCount)
{
    //@FInventoryItem
    FInventoryItem* InventoryItem = Inventory.Find(UniqueItemID);
    if (!InventoryItem)
    {
        UE_LOGFMT(LogInventory, Warning, "아이템 사용 실패: 아이템 ID {0}를 인벤토리에서 찾을 수 없습니다.", UniqueItemID.ToString());
        return;
    }

    //@Item Manager Subsystem
    UItemManagerSubsystem* ItemManager = GetWorld()->GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogInventory, Error, "ItemManagerSubsystem을 가져올 수 없습니다.");
        return;
    }

    //@FItemInformation
    const FItemInformation* ItemInfo = ItemManager->GetItemInformation<FItemInformation>(InventoryItem->GetItemType(), InventoryItem->GetItemTag());
    if (!ItemInfo)
    {
        UE_LOGFMT(LogInventory, Error, "ItemInformation을 가져올 수 없습니다.");
        return;
    }

    //@Item Instance
    AItem* Item = InventoryItem->GetItem();
    if (!Item)
    {
        UE_LOGFMT(LogInventory, Error, "아이템 {0}의 CDO가 유효하지 않습니다.", UniqueItemID.ToString());
        return;
    }

    //@Try Activate
    if (!Item->TryActivateItem())
    {
        UE_LOGFMT(LogInventory, Warning, "아이템 {0} 사용 실패.", InventoryItem->GetItemTag().ToString());
        return;
    }

    //@아이템 제거
    RemoveExistingItem(UniqueItemID, *ItemInfo, ItemCount);

    UE_LOGFMT(LogInventory, Log, "아이템 {0} 사용 완료", InventoryItem->GetItemTag().ToString());
}

void UInventoryComponent::LeaveItem(const FGuid& UniqueItemID, int32 ItemCount)
{
    //@FInventoryItem
    FInventoryItem* InventoryItem = Inventory.Find(UniqueItemID);
    if (!InventoryItem)
    {
        UE_LOGFMT(LogInventory, Warning, "아이템 드롭 실패: 아이템 ID {0}를 인벤토리에서 찾을 수 없습니다.", UniqueItemID.ToString());
        return;
    }

    //@Item Manager Subsystem
    UItemManagerSubsystem* ItemManager = GetWorld()->GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogInventory, Error, "ItemManagerSubsystem을 가져올 수 없습니다.");
        return;
    }

    //@FItemInformation
    const FItemInformation* ItemInfo = ItemManager->GetItemInformation<FItemInformation>(InventoryItem->GetItemType(), InventoryItem->GetItemTag());
    if (!ItemInfo)
    {
        UE_LOGFMT(LogInventory, Error, "ItemInformation을 가져올 수 없습니다.");
        return;
    }

    //@Spawn Location
    AActor* OwnerActor = GetOwner();
    FVector SpawnLocation = OwnerActor->GetActorLocation() + OwnerActor->GetActorForwardVector() * 100.0f;
    FRotator SpawnRotation = FRotator::ZeroRotator;

    //@Spawn Item
    AItem* SpawnedItem = SpawnItem(UniqueItemID, SpawnLocation, SpawnRotation);
    if (!SpawnedItem)
    {
        return;
    }

    //@TODO: 드롭한 아이템의 개수를 설정하는 작업
    //SpawnedItem->SetItemCount(ItemCount);

    //@아이템 제거
    RemoveExistingItem(UniqueItemID, *ItemInfo, ItemCount);

    UE_LOGFMT(LogInventory, Log, "아이템 {0}을(를) 드롭했습니다.", InventoryItem->GetItemTag().ToString());
}

void UInventoryComponent::DiscardItem(const FGuid& UniqueItemID, int32 ItemCount)
{
    //@FInventoryItem
    FInventoryItem* InventoryItem = Inventory.Find(UniqueItemID);
    if (!InventoryItem)
    {
        UE_LOGFMT(LogInventory, Warning, "아이템 삭제 실패: 아이템 ID {0}를 인벤토리에서 찾을 수 없습니다.", UniqueItemID.ToString());
        return;
    }

    //@Item Manager Subsystem
    UItemManagerSubsystem* ItemManager = GetWorld()->GetGameInstance()->GetSubsystem<UItemManagerSubsystem>();
    if (!ItemManager)
    {
        UE_LOGFMT(LogInventory, Error, "ItemManagerSubsystem을 가져올 수 없습니다.");
        return;
    }

    //@FItemInformation
    const FItemInformation* ItemInfo = ItemManager->GetItemInformation<FItemInformation>(InventoryItem->GetItemType(), InventoryItem->GetItemTag());
    if (!ItemInfo)
    {
        UE_LOGFMT(LogInventory, Error, "ItemInformation을 가져올 수 없습니다.");
        return;
    }

    //@아이템이 제거 가능한지 확인
    if (!InventoryItem->bRemovable)
    {
        UE_LOGFMT(LogInventory, Warning, "아이템 {0}은(는) 제거할 수 없는 아이템입니다.", InventoryItem->GetItemTag().ToString());
        return;
    }

    //@아이템 제거
    RemoveExistingItem(UniqueItemID, *ItemInfo, ItemCount);

    UE_LOGFMT(LogInventory, Log, "아이템 {0}을(를) 삭제했습니다.", InventoryItem->GetItemTag().ToString());
}

AItem* UInventoryComponent::SpawnItem(const FGuid& UniqueItemID, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
    //@FInventoryItem
    FInventoryItem* InventoryItem = Inventory.Find(UniqueItemID);
    if (!InventoryItem)
    {
        UE_LOGFMT(LogInventory, Warning, "아이템 스폰 실패: 아이템 ID {0}를 인벤토리에서 찾을 수 없습니다.", UniqueItemID.ToString());
        return nullptr;
    }

    //@Deferred Spawn
    FTransform SpawnTransform(SpawnRotation, SpawnLocation);
    AItem* SpawnedItem = Cast<AItem>(UGameplayStatics::BeginDeferredActorSpawnFromClass(this, InventoryItem->GetItemClass(), SpawnTransform));
    if (!SpawnedItem)
    {
        UE_LOGFMT(LogInventory, Error, "아이템 {0} 스폰 실패", InventoryItem->GetItemTag().ToString());
        return nullptr;
    }

    //@아이템 활성화
    EnableItem(SpawnedItem);

    //@Deferred Spawn 완료
    UGameplayStatics::FinishSpawningActor(SpawnedItem, SpawnTransform);

    UE_LOGFMT(LogInventory, Log, "아이템 {0}을(를) 성공적으로 스폰했습니다. 위치: {1}",
        InventoryItem->GetItemTag().ToString(), SpawnLocation.ToString());

    return SpawnedItem;

}

void UInventoryComponent::SwapItem()
{
	//@Equipped Item
}

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

//@Callbacks
#pragma region Callbacks
void UInventoryComponent::OnItemActivationEnded(FGuid UniqueItemID)
{
    UE_LOGFMT(LogInventory, Log, "{0} Item 활성화 작업이 종료되었습니다.", UniqueItemID.ToString());
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
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

int32 UInventoryComponent::IsItemAssignedToQuickSlots(const FGuid& UniqueItemID)
{
    for (int32 i = 0; i < QuickSlots.Num(); ++i)
    {
        if (QuickSlots[i] == UniqueItemID)
        {
            // 아이템이 퀵슬롯에 할당되어 있으면 해당 슬롯의 인덱스를 반환 (0-based)
            return i;
        }
    }

    // 아이템이 퀵슬롯에 할당되어 있지 않으면 -1 반환
    return -1;
}
#pragma endregion