// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Component/UIComponent.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/PlayerController.h"

#include "14_Subsystem/UIManagerSubsystem.h"
#include "08_UI/01_HUD/PlayerHUD.h"
#include "08_UI/02_System/SystemUI.h"

#include "03_Player/BasePlayerController.h"
#include "04_Component/BaseInputComponent.h"
#include "04_Component/InventoryComponent.h"

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

}

void UUIComponent::InternalBindToHUDUI()
{
	UPlayerHUD* PlayerHUD = Cast<UPlayerHUD>(HUDUI);
	if (!PlayerHUD)
	{
		UE_LOGFMT(LogUI, Error, "HUDUI를 UPlayerHUD로 캐스팅할 수 없습니다.");
		return;
	}

	PlayerHUD->NotifyQuickSlotsInitFinished.BindUFunction(this, "QuickSlotsInitFinishedNotified");
	PlayerHUD->NotifyStateBarsInitFinished.BindUFunction(this, "StateBarsInitFinishedNotified");
}

void UUIComponent::InternalBindToSystemUI()
{
	USystemUI* SystemUIRef = Cast<USystemUI>(SystemUI);
	if (!SystemUIRef)
	{
		UE_LOGFMT(LogUI, Error, "HUDUI를 UPlayerHUD로 캐스팅할 수 없습니다.");
		return;
	}

	SystemUIRef->NotifyInventoryUIInitFinished.BindUFunction(this, "InventoryUIInitFinishedNotified");
}

void UUIComponent::InitializeUIComponent()
{
	//@PlayerController 가져오기
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOGFMT(LogUI, Error, "Owner가 PlayerController가 아닙니다.");
		return;
	}
	//@GameInstance 가져오기
	UGameInstance* GameInstance = PC->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOGFMT(LogUI, Error, "GameInstance가 null입니다.");
		return;
	}
	//@UIManagerSubsystem 가져오기
	UUIManagerSubsystem* UIManagerSubsystem = GameInstance->GetSubsystem<UUIManagerSubsystem>();
	if (!UIManagerSubsystem)
	{
		UE_LOGFMT(LogUI, Error, "UI Manager Subsystem이 유효하지 않습니다!");
		return;
	}
	//@EUICategory 열거형 정보 가져오기
	UEnum* EnumPtr = StaticEnum<EUICategory>();
	int32 EnumCount = EnumPtr->GetMaxEnumValue();
	//@각 UI 카테고리에 대한 처리
	for (int32 i = 0; i < EnumCount; ++i)
	{
		EUICategory UICategory = static_cast<EUICategory>(i);
		const TArray<FUIInformation>* UIInfos = UIManagerSubsystem->GetUICategoryInformations(UICategory);

		if (!UIInfos || UIInfos->Num() == 0)
		{
			UE_LOGFMT(LogUI, Warning, "UI Information 정보 중 {0}이 비어있습니다.",
				EnumPtr->GetNameStringByValue(static_cast<int64>(UICategory)));
			continue;
		}

		//@각 UI 정보에 대한 위젯 생성 및 설정
		for (const auto& UIInfo : *UIInfos)
		{
			CreateAndSetupWidget(PC, UICategory, UIInfo, EnumPtr);
		}
	}

	//@초기화 요청 이벤트
	RequestInitializationByUIComp.Broadcast();
}
#pragma endregion

#pragma region UI
void UUIComponent::CreateAndSetupWidget(APlayerController* PC, EUICategory UICategory, const FUIInformation& UIInfo, UEnum* EnumPtr)
{
	//@위젯 생성
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC, UIInfo.UIClass);
	if (!NewWidget)
	{
		UE_LOGFMT(LogUI, Error, "{0} 카테고리의 {1} UI 생성에 실패했습니다.",
			EnumPtr->GetNameStringByValue(static_cast<int64>(UICategory)), *UIInfo.UITag.ToString());
		return;
	}
	//@카테고리별 위젯 설정
	switch (UICategory)
	{
	case EUICategory::HUD:
		SetupHUDUI(NewWidget);
		break;
	case EUICategory::System:
		SetupSystemUI(NewWidget);
		break;
	case EUICategory::Interaction:
		SetupInteractionUI(UIInfo.UITag, NewWidget);
		break;
	}
	//@UI 표시 여부 설정
	UIInfo.bShownOnBeginPlay ? ShowUI(UICategory, UIInfo.UITag) : HideUI(UICategory, UIInfo.UITag);
	NewWidget->AddToViewport();
	//@UI 생성 로그
	UE_LOGFMT(LogUI, Log, "{0} 카테고리의 {1} UI가 생성되었습니다.",
		EnumPtr->GetNameStringByValue(static_cast<int64>(UICategory)), *UIInfo.UITag.ToString());
}

void UUIComponent::SetupHUDUI(UUserWidget* NewWidget)
{
	//@HUD UI 설정
	HUDUI = NewWidget;
	UPlayerHUD* PlayerHUD = Cast<UPlayerHUD>(HUDUI);
	if (!PlayerHUD)
	{
		UE_LOGFMT(LogUI, Error, "HUDUI를 UPlayerHUD로 캐스팅할 수 없습니다.");
		return;
	}
	//@HUD 지연 초기화 함수 바인딩
	RequestInitializationByUIComp.AddUFunction(PlayerHUD, "InitializePlayerHUD");
	//@HUD 내부 바인딩
	InternalBindToHUDUI();

	UE_LOGFMT(LogUI, Log, "HUD가 성공적으로 설정되었습니다.");
}

void UUIComponent::SetupSystemUI(UUserWidget* NewWidget)
{
	// SystemUI 설정
	SystemUI = NewWidget;
	USystemUI* System = Cast<USystemUI>(SystemUI);
	if (!System)
	{
		UE_LOGFMT(LogUI, Error, "SystemUI를 USystemUI로 캐스팅할 수 없습니다.");
		return;
	}
	//@SystemUI 지연 초기화 함수 바인딩
	RequestInitializationByUIComp.AddUFunction(System, "InitializeSystemUI");
	//@SystemUI 내부 바인딩
	InternalBindToSystemUI();

	UE_LOGFMT(LogUI, Log, "SystemUI가 성공적으로 설정되었습니다.");
}

void UUIComponent::SetupInteractionUI(const FGameplayTag& UITag, UUserWidget* NewWidget)
{
	//@MInteractionUIs
	MInteractionUIs.Add(UITag, NewWidget);
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

#pragma region Callbacks
void UUIComponent::StateBarsInitFinishedNotified()
{
	bStateBarsReadyForLoading = true;

	CheckAllUIsForAttributeSetReady();
}
void UUIComponent::QuickSlotsInitFinishedNotified()
{
	bQuickSlotsReadyForLoading = true;

	//@Inventory Loading 시작을 위한 준비가 완료되었는지 체크합니다.
	CheckUIsForInventoryReady();
}

void UUIComponent::InventoryUIInitFinishedNotified()
{
	bInventoryUIReadyForLoading = true;

	//@Inventory Loading 시작을 위한 준비가 완료되었는지 체크합니다.
	CheckUIsForInventoryReady();
}

void UUIComponent::CheckUIsForInventoryReady()
{
	//@Delgate, Iventory 로딩 이전에 초기화 완료 확인할 UI들 추가
	if (bQuickSlotsReadyForLoading && bInventoryUIReadyForLoading)
	{
		UIsForInventoryReady.ExecuteIfBound();

		bQuickSlotsReadyForLoading = false;
		bInventoryUIReadyForLoading = false;
	}
}
void UUIComponent::CheckAllUIsForAttributeSetReady()
{
	//@Delegate, Attribute Set 로딩 이전에 초기화 완료 확인할 UI들 추가
	if (bStateBarsReadyForLoading)
	{
		UIsForAttributeSetReady.ExecuteIfBound();

		bStateBarsReadyForLoading = false;
	}
}
#pragma endregion