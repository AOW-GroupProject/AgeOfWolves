// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Component/UIComponent.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/PlayerController.h"

#include "14_Subsystem/UIManagerSubsystem.h"
#include "08_UI/PlayerHUD.h"
#include "08_UI/QuickSlots.h"

#include "04_Component/BaseInputComponent.h"

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

	if (UIInputComponent) DestroyUIInputComponent();
}
#pragma endregion

#pragma region UI
void UUIComponent::CreateUI(const UUIManagerSubsystem& UIManagerSubsystem, EUICategory UICategory)
{
	UEnum* EnumPtr = StaticEnum<EUICategory>();
	//@Player Controller
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOGFMT(LogUI, Error, "PlayerController가 유효하지 않습니다.");
		return;
	}
	//@UI Info
	const TArray<FUIInformation>* UIInfos = UIManagerSubsystem.GetUICategoryInformations(UICategory);
	if (!UIInfos || UIInfos->IsEmpty())
	{
		UE_LOGFMT(LogUI, Warning, "UI Information 정보 중 {0}이 비어있습니다.", EnumPtr->GetNameStringByValue(static_cast<int64>(UICategory)));
		return;
	}
	//@Widget
	for (const auto& UIInfo : *UIInfos)
	{
		UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC, UIInfo.UIClass);
		if (NewWidget)
		{
			switch (UICategory)
			{
			case EUICategory::HUD:
				HUDUI = NewWidget;
				//@Binding?

				break;
			case EUICategory::System:
				SystemUI = NewWidget;
				//@Binding?

				break;
			case EUICategory::Interaction:
				MInteractionUIs.Add(UIInfo.UITag, NewWidget);
				//@BInding?

				break;
			}

			UIInfo.bShownOnBeginPlay ? ShowUI(UICategory, UIInfo.UITag) : HideUI(UICategory, UIInfo.UITag);
			NewWidget->AddToViewport();

			UE_LOGFMT(LogUI, Log, "{0} 카테고리의 {1} UI가 생성되었습니다.",
				EnumPtr->GetNameStringByValue(static_cast<int64>(UICategory)), *UIInfo.UITag.ToString());
		}
		else
		{
			UE_LOGFMT(LogUI, Error, "{0} 카테고리의 {1} UI 생성에 실패했습니다.",
				EnumPtr->GetNameStringByValue(static_cast<int64>(UICategory)), *UIInfo.UITag.ToString());
		}
	}
}

bool UUIComponent::LoadUI()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOGFMT(LogUI, Error, "Owner is not a PlayerController in UIComponent::InitializeComponent");
		return false;
	}

	UGameInstance* GameInstance = PC->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOGFMT(LogUI, Error, "GameInstance is null in UIComponent::InitializeComponent");
		return false;
	}

	UUIManagerSubsystem* UIManagerSubsystem = GameInstance->GetSubsystem<UUIManagerSubsystem>();
	if (UIManagerSubsystem)
	{
		CreateUI(*UIManagerSubsystem, EUICategory::HUD);
		CreateUI(*UIManagerSubsystem, EUICategory::System);
		CreateUI(*UIManagerSubsystem, EUICategory::Interaction);
		return true;
	}
	else
	{
		UE_LOGFMT(LogUI, Error, "UI Manager Subsystem이 유효하지 않습니다!");
		return false;
	}

	return false;
}

void UUIComponent::BindUIToUIComponent()
{
	//@HUD
	if (HUDUI)
	{
		UPlayerHUD* PlayerHUD = Cast<UPlayerHUD>(HUDUI);
		if (PlayerHUD)
		{
			UQuickSlots* QuickSlotsWidget = PlayerHUD->GetQuickSlotsWidget();
			if (QuickSlotsWidget)
			{
				RequestItemAssignmentToQuickSlots.AddDynamic(QuickSlotsWidget, &UQuickSlots::OnRequestItemAssignment);
				RequestQuickSlotItemRemoval.AddDynamic(QuickSlotsWidget, &UQuickSlots::OnRequestItemRemoval);
				RequestQuickSlotItemUpdate.AddDynamic(QuickSlotsWidget, &UQuickSlots::OnRequestItemUpdate);
				UE_LOGFMT(LogUI, Log, "QuickSlots 위젯이 성공적으로 바인딩되었습니다.");
			}
			else
			{
				UE_LOGFMT(LogUI, Warning, "QuickSlots 위젯을 찾을 수 없습니다.");
			}
		}
		else
		{
			UE_LOGFMT(LogUI, Warning, "PlayerHUD로 캐스팅할 수 없습니다.");
		}
	}
	else
	{
		UE_LOGFMT(LogUI, Warning, "HUDUI가 유효하지 않습니다.");
	}
	//@System
	//@Interaction

}

void UUIComponent::BindToInventoryComponent(const APawn* OwningPawn)
{


}

void UUIComponent::ShowUI(EUICategory UICategory, const FGameplayTag& UITag)
{
	if (UUserWidget* UI = GetUI(UICategory, UITag))
	{
		UI->SetVisibility(ESlateVisibility::Visible);
	}
}

void UUIComponent::HideUI(EUICategory UICategory, const FGameplayTag& UITag)
{
	if (UUserWidget* UI = GetUI(UICategory, UITag))
	{
		UI->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UUserWidget* UUIComponent::GetUI(EUICategory UICategory, const FGameplayTag& UITag) const
{
	switch (UICategory)
	{
	case EUICategory::HUD:
		return HUDUI;
	case EUICategory::System:
		return SystemUI;
	case EUICategory::Interaction:
		if (auto* Widget = MInteractionUIs.Find(UITag))
		{
			return *Widget;
		}
		UE_LOGFMT(LogUI, Warning, "Interaction UI를 찾을 수 없습니다. Tag: {0}", UITag.ToString());
		return nullptr;
	default:
		UE_LOGFMT(LogUI, Warning, "유효하지 않은 UI Category입니다.");
		return nullptr;
	}
}

TArray<UUserWidget*> UUIComponent::GetCategoryUIs(EUICategory UICategory) const
{
	TArray<UUserWidget*> Result;
	switch (UICategory)
	{
	case EUICategory::HUD:
		if (HUDUI) Result.Add(HUDUI);
		break;
	case EUICategory::System:
		if (SystemUI) Result.Add(SystemUI);
		break;
	case EUICategory::Interaction:
		MInteractionUIs.GenerateValueArray(Result);
		break;
	default:
		UE_LOGFMT(LogUI, Warning, "유효하지 않은 UI Category입니다.");
	}
	return Result;
}
#pragma endregion

#pragma region Input Component
bool UUIComponent::LoadUIInputComponent()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC) return false;

	BaseInputComponent = PC->FindComponentByClass<UBaseInputComponent>();
	if (!BaseInputComponent) return false;

	BaseInputComponent->OnUIInputTriggered.AddDynamic(this, &UUIComponent::OnUIInputTagTriggered);
	BaseInputComponent->OnUIInputReleased.AddDynamic(this, &UUIComponent::OffUIInputTagReleased);

	return true;
}

void UUIComponent::BindInputComponentToInputActions()
{
}

void UUIComponent::DestroyUIInputComponent()
{

}

void UUIComponent::EnterSystemUIMode()
{
	SetUIInputPriority(10);
	SetUIInputBlocking(true);
}

void UUIComponent::ExitSystemUIMode()
{
	SetUIInputPriority(0);
	SetUIInputBlocking(false);
}

void UUIComponent::SetUIInputPriority(int32 NewPriority)
{
	//if (BaseInputComponent)
	//{
	//	BaseInputComponent->SetPriority(NewPriority);
	//}
}

void UUIComponent::SetUIInputBlocking(bool bShouldBlock)
{
	//if (BaseInputComponent)
	//{
	//	BaseInputComponent->SetBlockingInput(bShouldBlock);
	//}
}

FString UUIComponent::FindUICategoryFromInputTag(const FGameplayTag& InputTag)
{
	FString TagString = InputTag.ToString();

	if (TagString.StartsWith("Input.UI."))
	{
		TArray<FString> Parts;
		TagString.ParseIntoArray(Parts, TEXT("."), true);

		if (Parts.Num() >= 3)
		{
			return Parts[2]; // "HUD", "System", "Interaction" 등을 반환
		}
	}

	return FString(); // 빈 문자열 반환 (카테고리를 찾지 못한 경우)
}
#pragma endregion

#pragma region Callbacks
void UUIComponent::OnUIInputTagTriggered(const FGameplayTag& InputTag)
{
	//@Input Tag
	if (!InputTag.IsValid())
	{
		UE_LOGFMT(LogUI, Error, "{0}: 유효하지 않은 InputTag입니다.", __FUNCTION__);
		return;
	}

	UE_LOGFMT(LogUI, Log, "UI 입력이 트리거되었습니다. InputTag: {0}", InputTag.ToString());

	//@UI Category, Input Tag를 Parsing하여 관련 UI Category 찾기
	FString UICategoryString = FindUICategoryFromInputTag(InputTag);
	//@Delegate
	if (!UICategoryString.IsEmpty())
	{
		//@HUD
		if (UICategoryString == "HUD")
		{
			OnUIHUDInputTriggered.Broadcast(InputTag);
		}
		//@System
		else if (UICategoryString == "System")
		{
			OnUISystemInputTriggered.Broadcast(InputTag);
		}
		//@Interaction
		else if (UICategoryString == "Interaction")
		{
			OnUIInteractionInputTriggered.Broadcast(InputTag);
		}
		else
		{
			UE_LOGFMT(LogUI, Error, "{0}에 대응되는 UI Category가 존재하지 않습니다. {1}", 
				UICategoryString, __FUNCTION__);
			return;
		}
	}

}

void UUIComponent::OffUIInputTagReleased(const FGameplayTag& InputTag)
{
	//@Input Tag
	if (!InputTag.IsValid())
	{
		UE_LOGFMT(LogUI, Error, "{0}: 유효하지 않은 InputTag입니다.", __FUNCTION__);
		return;
	}

	UE_LOGFMT(LogUI, Log, "UI 입력이 해제되었습니다. InputTag: {0}", InputTag.ToString());

	//@UI Category, Input Tag를 Parsing하여 관련 UI Category 찾기
	FString UICategoryString = FindUICategoryFromInputTag(InputTag);
	//@Delegate
	if (!UICategoryString.IsEmpty())
	{
		//@HUD
		if (UICategoryString == "HUD")
		{
			OnUIHUDInputReleased.Broadcast(InputTag);
		}
		//@System
		else if (UICategoryString == "System")
		{
			OnUISystemInputReleased.Broadcast(InputTag);
		}
		//@Interaction
		else if (UICategoryString == "Interaction")
		{
			OnUIInteractionInputReleased.Broadcast(InputTag);
		}
		else
		{
			UE_LOGFMT(LogUI, Error, "{0}에 대응되는 UI Category가 존재하지 않습니다. {1}",
				UICategoryString, __FUNCTION__);
			return;
		}
	}
}

void UUIComponent::QuickSlotAssignmentRequested(int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount)
{
	//@Delegate
	RequestItemAssignmentToQuickSlots.Broadcast(SlotNum, UniqueItemID, ItemType, ItemTag, ItemCount);

}

void UUIComponent::QuickSlotItemRemovalRequested(int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount)
{
	RequestQuickSlotItemRemoval.Broadcast(SlotNum, UniqueItemID, ItemType, ItemTag, ItemCount);

}

void UUIComponent::QuickSlotItemUpdateRequested(int32 SlotNum, const FGuid& UniqueItemID, EItemType ItemType, const FGameplayTag& ItemTag, int32 ItemCount)
{
	//@Delegate
	RequestQuickSlotItemUpdate.Broadcast(SlotNum, UniqueItemID, ItemType, ItemTag, ItemCount);

}
#pragma endregion