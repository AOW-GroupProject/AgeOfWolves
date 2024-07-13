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
#pragma endregion

#pragma region Callbacks
void AItem::OnItemAddedToInventory(const FGuid& UniqueId)
{
	 UE_LOGFMT(LogItem, Log, "{0}이 Iventory에 추가되었습니다.", GetName());

}

void AItem::OnItemRemovedFromInventory()
{
	UE_LOGFMT(LogItem, Log, "{0}이 Iventory에서 제거되었습니다.", GetName());

}

void AItem::OnItemStartUse()
{
	UE_LOGFMT(LogItem, Log, "{0}이 활성화되었습니다.", GetName());

}

void AItem::OnItemEndUse()
{
	UE_LOGFMT(LogItem, Log, "{0}의 활성화가 종료되었습니다.", GetName());

}
#pragma endregion

#pragma region Activation
bool AItem::ActivateItem_Implementation()
{
	return false;
}
#pragma endregion