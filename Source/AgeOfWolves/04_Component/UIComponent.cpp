// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Component/UIComponent.h"
#include "Logging/StructuredLog.h"

#include "14_Subsystem/UIManagerSubsystem.h"
#include "Blueprint/UserWidget.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogUI)
// UE_LOGFMT(LogUI, Log, "");

#pragma region Default Setting
UUIComponent::UUIComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{ 
	PrimaryComponentTick.bCanEverTick = false; 
	bWantsInitializeComponent = true;//Initialize Component 활용에 필요 
}

void UUIComponent::OnRegister()
{
	Super::OnRegister();

}

void UUIComponent::OnUnregister()
{
	Super::OnUnregister();

}

void UUIComponent::InitializeComponent()
{
	Super::InitializeComponent();

	//@GameInstance
	if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
	{
		//UIs: HUD, System, Interaction
		if (UUIManagerSubsystem* UIManagerSubsystem = GameInstance->GetSubsystem<UUIManagerSubsystem>())
		{
			//@HUD
			CreateUI(*UIManagerSubsystem, EUICategory::HUD);
			//@System
			CreateUI(*UIManagerSubsystem, EUICategory::System);
			//@Interaction
			CreateUI(*UIManagerSubsystem, EUICategory::Interaction);
		}
		else
		{
			UE_LOGFMT(LogUI, Error, "UI Manager Subsystem이 유효하지 않습니다!");
		}
	}
	else
	{
		UE_LOGFMT(LogUI, Error, "Game Instance가 유효하지 않습니다!");
	}
}

void UUIComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

}

void UUIComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}
#pragma endregion

#pragma region UI
void UUIComponent::CreateUI(const UUIManagerSubsystem& UIManagerSubsystem, EUICategory Category)
{
	//@PC
	APlayerController* PC = GetOwner()->GetWorld()->GetFirstPlayerController();
	if (!PC)
	{
		UE_LOGFMT(LogUI, Error, "Player Controller가 유효하지 않습니다.");
		return;
	}
	//@UI Information
	const TArray<FUIInformation>* UIInfos = UIManagerSubsystem.GetUICategoryInformations(Category);
	if (!UIInfos || UIInfos->IsEmpty())
	{
		UE_LOGFMT(LogUI, Warning, "{0} UI Information이 비어있습니다.", static_cast<int32>(Category));
		return;
	}

	for (const auto& UIInfo : *UIInfos)
	{
		UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC, UIInfo.UIClass);
		if (NewWidget)
		{
			// 카테고리에 따라 적절한 맵에 추가
			switch (Category)
			{
			case EUICategory::HUD:
				MHUDUIs.Add(UIInfo.UITag, NewWidget);
				break;
			case EUICategory::System:
				MSystemUIs.Add(UIInfo.UITag, NewWidget);
				break;
			case EUICategory::Interaction:
				MInteractionUIs.Add(UIInfo.UITag, NewWidget);
				break;
			}
			//@bShownOnBeginPlay
			UIInfo.bShownOnBeginPlay ? ShowUI(Category, UIInfo.UITag) : HideUI(Category, UIInfo.UITag);
			//@Add To Viewport
			NewWidget->AddToViewport();

			UE_LOGFMT(LogUI, Log, "{0} 카테고리의 {1} UI가 생성되었습니다.",
				static_cast<int32>(Category), *UIInfo.UITag.ToString());
		}
		else
		{
			UE_LOGFMT(LogUI, Error, "{0} 카테고리의 {1} UI 생성에 실패했습니다.",
				static_cast<int32>(Category), *UIInfo.UITag.ToString());
		}
	}
}

void UUIComponent::LoadUI()
{
	//TODO: UI 관련 데이터들을 로드하는 작업

}

void UUIComponent::ShowUI(EUICategory Category, const FGameplayTag& UITag)
{
	if (UUserWidget* UI = GetUI(Category, UITag))
	{
		UI->SetVisibility(ESlateVisibility::Visible);
	}
}
void UUIComponent::HideUI(EUICategory Category, const FGameplayTag& UITag)
{
	if (UUserWidget* UI = GetUI(Category, UITag))
	{
		UI->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UUserWidget* UUIComponent::GetUI(EUICategory UICategory, const FGameplayTag& UITag) const
{
	const TMap<FGameplayTag, UUserWidget*>* CategoryMap = nullptr;

	switch (UICategory)
	{
	case EUICategory::HUD:
		CategoryMap = &MHUDUIs;
		break;
	case EUICategory::System:
		CategoryMap = &MSystemUIs;
		break;
	case EUICategory::Interaction:
		CategoryMap = &MInteractionUIs;
		break;
	default:
		UE_LOGFMT(LogUI, Warning, "유효하지 않은 UI Category: {0}", static_cast<int32>(UICategory));
		return nullptr;
	}

	if (UUserWidget* const* FoundWidget = CategoryMap->Find(UITag))
	{
		return *FoundWidget;
	}

	UE_LOGFMT(LogUI, Warning, "{0} UI Category의 {1} UI를 찾지 못했습니다.",
		static_cast<int32>(UICategory), *UITag.ToString());
	return nullptr;
}

TArray<UUserWidget*> UUIComponent::GetCategoryUIs(EUICategory UICategory) const
{
	TArray<UUserWidget*> Result;

	const TMap<FGameplayTag, UUserWidget*>* CategoryMap = nullptr;

	switch (UICategory)
	{
	case EUICategory::HUD:
		CategoryMap = &MHUDUIs;
		break;
	case EUICategory::System:
		CategoryMap = &MSystemUIs;
		break;
	case EUICategory::Interaction:
		CategoryMap = &MInteractionUIs;
		break;
	default:
		UE_LOGFMT(LogUI, Warning, "유효하지 않은 UI Category: {0}", static_cast<int32>(UICategory));
		return Result;
	}

	CategoryMap->GenerateValueArray(Result);
	return Result;
}
#pragma endregion

#pragma region Inventory UI

#pragma region Inventory UI - Delegates
void UUIComponent::RequestItemRemoval(const FGuid& UniqueItemID)
{
	UE_LOGFMT(LogUI, Log, "UI에서 아이템 {0} 제거를 요청했습니다.", UniqueItemID.ToString());
	OnItemRemovalRequested.Broadcast(UniqueItemID);
}
void UUIComponent::RequestItemActivation(const FGuid& UniqueItemID)
{
	UE_LOGFMT(LogUI, Log, "UI에서 아이템 {0} 활성화를 요청했습니다.", UniqueItemID.ToString());

	OnItemActivated.Broadcast(UniqueItemID);
}

void UUIComponent::RequestQuickSlotAssignment(int32 SlotIndex, const FGuid& UniqueItemID)
{
	UE_LOGFMT(LogUI, Log, "UI에서 아이템 {0}의 퀵슬롯 추가를 요청했습니다.", UniqueItemID.ToString());

	OnQuickSlotAssigned.Broadcast(SlotIndex, UniqueItemID);
}
#pragma endregion

#pragma region Inventory UI - Callbacks
void UUIComponent::OnInventoryItemAdded(const FGuid& UniqueItemID)
{
	// 인벤토리 UI 업데이트 로직
	UE_LOGFMT(LogUI, Log, "인벤토리에 아이템 {0}이(가) 추가되었습니다. UI를 업데이트합니다.", UniqueItemID.ToString());
	//@Inventory UI
	if (UUserWidget* InventoryWidget = GetUI(EUICategory::HUD, FGameplayTag::RequestGameplayTag(FName("UI.HUD.Inventory"))))
	{
		//@TODO: Inventory UI 업데이트
	}
}

void UUIComponent::OnInventoryItemRemoved(const FGuid& UniqueItemID, const FGameplayTag& ItemTag)
{
	UE_LOGFMT(LogUI, Log, "인벤토리에서 아이템 {0} ({1})이(가) 제거되었습니다. UI를 업데이트합니다.",
		UniqueItemID.ToString(), ItemTag.ToString());
	
	//@Inventory UI
	if (UUserWidget* InventoryWidget = GetUI(EUICategory::HUD, FGameplayTag::RequestGameplayTag(FName("UI.HUD.Inventory"))))
	{
		//@TODO: Inventory UI 업데이튼
	}
}
#pragma endregion

#pragma endregion