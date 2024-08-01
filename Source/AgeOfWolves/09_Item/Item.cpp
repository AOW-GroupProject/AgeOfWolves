#include "Item.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogItem)
// UE_LOGFMT(LogItem, Warning, "");

#pragma region Default Setting
AItem::AItem(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

}

void AItem::PreRegisterAllComponents()
{
	Super::PreRegisterAllComponents();

}

void AItem::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

}

void AItem::PreInitializeComponents()
{
	//@External Binding

	Super::PreInitializeComponents();

}

void AItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItem::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void AItem::InitializeItem(const FGuid& UniqueId)
{
	UE_LOGFMT(LogItem, Log, "{0}이 Iventory에 추가되었습니다.", GetName());

	//@External Binding(Inventory Comp)
	
}
#pragma endregion

#pragma region Item
bool AItem::TryActivateItem_Implementation()
{
	return true;
}
#pragma endregion

#pragma region Callbacks

void AItem::OnItemRemovedFromInventory()
{
	UE_LOGFMT(LogItem, Log, "{0}이 Iventory에서 제거되었습니다.", GetName());

}
#pragma endregion
