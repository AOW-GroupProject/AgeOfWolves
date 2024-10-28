#include "04_Component/UIComponent.h"
#include "Logging/StructuredLog.h"

#include "14_Subsystem/UIManagerSubsystem.h"
#include "08_UI/01_HUD/PlayerHUD.h"
#include "08_UI/02_Menu/MenuUI.h"

#include "03_Player/BasePlayerController.h"
#include "04_Component/BaseInputComponent.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogUI)
// UE_LOGFMT(LogUI, Log, "");

//@Defualt Setting
#pragma region Default Setting
UUIComponent::UUIComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{ 
	PrimaryComponentTick.bCanEverTick = false; 
	bWantsInitializeComponent = true;//Initialize Component 활용에 필요 

	HUDUI = nullptr;
	MenuUI = nullptr;
	MInteractionUIs.Empty();

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

void UUIComponent::ExternalBindingToInputComponent(const APlayerController* PlayerController)
{
	//@PC
	if (!PlayerController)
	{
		UE_LOGFMT(LogUI, Error, "PlayerController가 유효하지 않습니다: {0}", __FUNCTION__);
		return;
	}

	//@Input Comp
	UBaseInputComponent* BaseInputComp = Cast<UBaseInputComponent>(PlayerController->InputComponent);
	if (!BaseInputComp)
	{
		UE_LOGFMT(LogUI, Error, "BaseInputComponent를 찾을 수 없습니다: {0}", __FUNCTION__);
		return;
	}

	//@외부 바인딩
	BaseInputComp->UIInputTagTriggered.AddUFunction(this, "OnUIInputTriggered");
	BaseInputComp->UIInputTagReleased.AddUFunction(this, "OnUIInputReleased");

	UE_LOGFMT(LogUI, Log, "UI 입력 콜백이 성공적으로 바인딩되었습니다: {0}", __FUNCTION__);
}

void UUIComponent::InternalBindToHUDUI()
{
	//@HUD
	UPlayerHUD* PlayerHUD = Cast<UPlayerHUD>(HUDUI);
	if (!PlayerHUD)
	{
		UE_LOGFMT(LogUI, Error, "HUDUI를 UPlayerHUD로 캐스팅할 수 없습니다.");
		return;
	}

	//@내부 바인딩
	PlayerHUD->HUDInitFinished.BindUFunction(this, "OnHUDInitFinished");
	PlayerHUD->NotifyQuickSlotsInitFinished.BindUFunction(this, "QuickSlotsInitFinishedNotified");
	PlayerHUD->NotifyStateBarsInitFinished.BindUFunction(this, "StateBarsInitFinishedNotified");
}

void UUIComponent::InternalBindToMenuUI()
{
	//Menu UI
	UMenuUI* MenuUIRef = Cast<UMenuUI>(MenuUI);
	if (!MenuUIRef)
	{
		UE_LOGFMT(LogUI, Error, "HUDUI를 UPlayerHUD로 캐스팅할 수 없습니다.");
		return;
	}

	//@내부 바인딩
	MenuUIRef->MenuUIInitFinished.BindUFunction(this, "OnMenuUIInitFinished");
	MenuUIRef->NotifyInventoryUIInitFinished.BindUFunction(this, "InventoryUIInitFinishedNotified");
	MenuUIRef->RequestCloseMenuUI.BindUFunction(this, "OnRequestCloseMenuUI");
}

void UUIComponent::InitializeUIComponent()
{
	//@Player Controller
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOGFMT(LogUI, Error, "Owner가 PlayerController가 아닙니다.");
		return;
	}

	//@External Binding
	ExternalBindingToInputComponent(PC);

	//@GameInstance
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
	if (!GameInstance)
	{
		UE_LOGFMT(LogUI, Error, "GameInstance를 가져올 수 없습니다.");
		return;
	}

	//@UI Manager Subsystem
	UUIManagerSubsystem* UIManagerSubsystem = GameInstance->GetSubsystem<UUIManagerSubsystem>();
	if (!UIManagerSubsystem)
	{
		UE_LOGFMT(LogUI, Error, "UI Manager Subsystem이 유효하지 않습니다!");
		return;
	}

	//@EUICategory 열거형 정보 가져오기
	UEnum* EnumPtr = StaticEnum<EUICategory>();
	if (!EnumPtr)
	{
		UE_LOGFMT(LogUI, Error, "EUICategory 열거형 정보를 가져올 수 없습니다.");
		return;
	}

	int32 EnumCount = EnumPtr->GetMaxEnumValue();

	//@각 UI 카테고리에 대한 처리
	for (int32 i = 0; i < EnumCount; ++i)
	{
		EUICategory UICategory = static_cast<EUICategory>(i);
		const TArray<FUIInformation>* UIInfos = UIManagerSubsystem->GetUICategoryInformations(UICategory);

		if (!UIInfos || UIInfos->Num() == 0)
		{
			UE_LOGFMT(LogUI, Warning, "UI Information 정보 중 {0}이 비어있습니다.",
				*EnumPtr->GetNameStringByValue(static_cast<int64>(UICategory)));
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

void UUIComponent::CheckAllUIsForInventoryReady()
{
	//@Delgate, Iventory 로딩 이전에 초기화 완료 확인할 UI들 추가
	if (bQuickSlotsReadyForLoading && bInventoryUIReadyForLoading)
	{
		bQuickSlotsReadyForLoading = false;
		bInventoryUIReadyForLoading = false;

		UIsForInventoryReady.ExecuteIfBound();
	}
}

void UUIComponent::CheckAllUIsForAttributeSetReady()
{
	//@Delegate, Attribute Set 로딩 이전에 초기화 완료 확인할 UI들 추가
	if (bStateBarsReadyForLoading)
	{
		bStateBarsReadyForLoading = false;

		UIsForAttributeSetReady.ExecuteIfBound();
	}
}

void UUIComponent::CheckAllUIsForDefaultVisibilitySetting()
{
	if (bHUDInitFinished && bMenuUIInitFinished)
	{
		bHUDInitFinished = false;
		bMenuUIInitFinished = false;

		//@모든 UI들에 대한 리셋 작업
		UE_LOGFMT(LogUI, Error, "초기화");
		ResetUIs();
	}
}
#pragma endregion

//@Property/Info...etc
#pragma region UI
void UUIComponent::ResetUIs()
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
		UE_LOGFMT(LogUI, Error, "GameInstance를 가져올 수 없습니다.");
		return;
	}

	UUIManagerSubsystem* UIManagerSubsystem = GameInstance->GetSubsystem<UUIManagerSubsystem>();
	if (!UIManagerSubsystem)
	{
		UE_LOGFMT(LogUI, Error, "UI Manager Subsystem이 유효하지 않습니다!");
		return;
	}

	//@HUD UI 리셋
	ResetCategoryUI(EUICategory::HUD, UIManagerSubsystem);

	//@Menu UI 리셋
	ResetCategoryUI(EUICategory::Menu, UIManagerSubsystem);

	//@Interaction UIs 리셋
	ResetCategoryUI(EUICategory::Interaction, UIManagerSubsystem);
}

void UUIComponent::ResetCategoryUI(EUICategory UICategory, UUIManagerSubsystem* UIManagerSubsystem)
{
	const TArray<FUIInformation>* UIInfos = UIManagerSubsystem->GetUICategoryInformations(UICategory);
	if (!UIInfos)
	{
		UE_LOGFMT(LogUI, Warning, "{0} 카테고리의 UI 정보를 찾을 수 없습니다.", *UEnum::GetValueAsString(UICategory));
		return;
	}

	for (const auto& UIInfo : *UIInfos)
	{
		UUserWidget* Widget = GetUI(UICategory, UIInfo.UITag);
		if (Widget)
		{
			if (UIInfo.bShownOnBeginPlay)
			{
				//@Add To Viewport
				Widget->AddToViewport();
				//@Show UI
				ShowUI(UICategory, UIInfo.UITag);
			}
			else
			{
				//@Hide UI
				HideUI(UICategory, UIInfo.UITag);
			}
		}
		else
		{
			UE_LOGFMT(LogUI, Warning, "{0} 카테고리의 {1} UI를 찾을 수 없습니다.",
				*UEnum::GetValueAsString(UICategory), *UIInfo.UITag.ToString());
		}
	}
}

void UUIComponent::CreateAndSetupWidget(APlayerController* PC, EUICategory UICategory, const FUIInformation& UIInfo, UEnum* EnumPtr)
{
	//@PC, Enum
	if (!PC || !EnumPtr)
	{
		UE_LOGFMT(LogUI, Error, "Invalid PlayerController or EnumPtr in {0}", __FUNCTION__);
		return;
	}

	//@Create Widget
	UUserWidget* NewWidget = CreateWidget<UUserWidget>(PC, UIInfo.UIClass);
	if (!NewWidget)
	{
		UE_LOGFMT(LogUI, Error, "Failed to create {0} UI in category {1}",
			*UIInfo.UITag.ToString(), EnumPtr->GetNameStringByValue(static_cast<int64>(UICategory)));
		return;
	}

	//@EUICategory
	switch (UICategory)
	{
	case EUICategory::HUD:
		SetupHUDUI(NewWidget);
		break;
	case EUICategory::Menu:
		SetupMenuUI(NewWidget);
		break;
	case EUICategory::Interaction:
		SetupInteractionUI(UIInfo.UITag, NewWidget);
		break;
	default:
		UE_LOGFMT(LogUI, Warning, "Unknown UI category: {0}", EnumPtr->GetNameStringByValue(static_cast<int64>(UICategory)));
		return;
	}

	// UI 생성 로그
	UE_LOGFMT(LogUI, Log, "Created {0} UI in category {1}",
		*UIInfo.UITag.ToString(), EnumPtr->GetNameStringByValue(static_cast<int64>(UICategory)));
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

void UUIComponent::SetupMenuUI(UUserWidget* NewWidget)
{
	//@Menu UI
	MenuUI = NewWidget;
	UMenuUI* Menu = Cast<UMenuUI>(MenuUI);
	if (!Menu)
	{
		UE_LOGFMT(LogUI, Error, "MenuUI를 UMenuUI로 캐스팅할 수 없습니다.");
		return;
	}
	//@MenuUI 지연 초기화 함수 바인딩
	RequestInitializationByUIComp.AddUFunction(Menu, "InitializeMenuUI");
	//@MenuUI 내부 바인딩
	InternalBindToMenuUI();

	UE_LOGFMT(LogUI, Log, "MenuUI가 성공적으로 설정되었습니다.");
}

void UUIComponent::SetupInteractionUI(const FGameplayTag& UITag, UUserWidget* NewWidget)
{
	//@TODO: Interaction UI 설정 작업 아래에서 수행...

	//@MInteractionUIs
	MInteractionUIs.Add(UITag, NewWidget);
}

void UUIComponent::ShowUI(EUICategory UICategory, const FGameplayTag& UITag)
{
	//@UI
	UUserWidget* UI = GetUI(UICategory, UITag);
	if (!UI)
	{
		UE_LOGFMT(LogUI, Warning, "{0} 카테고리의 {1} UI를 찾을 수 없습니다.",
			*UEnum::GetValueAsString(UICategory), *UITag.ToString());
		return;
	}

	//@가시성 변화 이벤트
	WidgetVisibilityChanged.Broadcast(UI, true);

	UE_LOGFMT(LogUI, Log, "{0} 카테고리의 {1} UI가 뷰포트에 추가되었습니다.",
		*UEnum::GetValueAsString(UICategory), *UITag.ToString());
}

void UUIComponent::HideUI(EUICategory UICategory, const FGameplayTag& UITag)
{
	//@UI
	UUserWidget* UI = GetUI(UICategory, UITag);
	if (!UI)
	{
		UE_LOGFMT(LogUI, Warning, "{0} 카테고리의 {1} UI를 찾을 수 없습니다.",
			*UEnum::GetValueAsString(UICategory), *UITag.ToString());
		return;
	}

	//@가시성 변화 이벤트
	WidgetVisibilityChanged.Broadcast(UI, false);

	UE_LOGFMT(LogUI, Log, "{0} 카테고리의 {1} UI가 부모로부터 제거되었습니다.",
		*UEnum::GetValueAsString(UICategory), *UITag.ToString());
}

void UUIComponent::ShowAllUI(EUICategory UICategory)
{
	//@UIs
	TArray<UUserWidget*> Widgets = GetCategoryUIs(UICategory);
	if (Widgets.Num() > 0)
	{
		for (UUserWidget* Widget : Widgets)
		{
			if (Widget && Widget->IsValidLowLevel())
			{
				//@UI Tag
				FGameplayTag UITag;
				for (const auto& Pair : MInteractionUIs)
				{
					if (Pair.Value == Widget)
					{
						UITag = Pair.Key;
						break;
					}
				}
				//@Show UI
				ShowUI(UICategory, UITag);
			}
		}
	}
	else
	{
		UE_LOGFMT(LogUI, Warning, "{0} 카테고리에 표시할 UI가 없습니다.",
			*UEnum::GetValueAsString(UICategory));
	}
}

void UUIComponent::HideAllUI(EUICategory UICategory)
{
	//@UIs
	TArray<UUserWidget*> Widgets = GetCategoryUIs(UICategory);
	if (Widgets.Num() > 0)
	{
		for (UUserWidget* Widget : Widgets)
		{
			if (Widget && Widget->IsValidLowLevel())
			{
				//@UI Tag
				FGameplayTag UITag;
				for (const auto& Pair : MInteractionUIs)
				{
					if (Pair.Value == Widget)
					{
						UITag = Pair.Key;
						break;
					}
				}
				//@Hide UI
				HideUI(UICategory, UITag);
			}
		}
	}
	else
	{
		UE_LOGFMT(LogUI, Warning, "{0} 카테고리에 숨길 UI가 없습니다.",
			*UEnum::GetValueAsString(UICategory));
	}
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UUIComponent::OnHUDInitFinished()
{
	bHUDInitFinished = true;

	CheckAllUIsForDefaultVisibilitySetting();
}

void UUIComponent::OnMenuUIInitFinished()
{
	bMenuUIInitFinished = true;

	CheckAllUIsForDefaultVisibilitySetting();
}

void UUIComponent::StateBarsInitFinishedNotified()
{
	bStateBarsReadyForLoading = true;

	CheckAllUIsForAttributeSetReady();
}

void UUIComponent::QuickSlotsInitFinishedNotified()
{
	bQuickSlotsReadyForLoading = true;

	//@Inventory Loading 시작을 위한 준비가 완료되었는지 체크합니다.
	CheckAllUIsForInventoryReady();
}

void UUIComponent::InventoryUIInitFinishedNotified()
{
	bInventoryUIReadyForLoading = true;

	//@Inventory Loading 시작을 위한 준비가 완료되었는지 체크합니다.
	CheckAllUIsForInventoryReady();
}

void UUIComponent::OnUIInputTriggered(const FGameplayTag& InputTag)
{
	//@Base Input Comp
	UBaseInputComponent* BaseInputComp = Cast<UBaseInputComponent>(GetOwner()->InputComponent);
	if (!BaseInputComp)
	{
		UE_LOGFMT(LogUI, Error, "BaseInputComponent을 찾을 수 없습니다.");
		return;
	}

	FGameplayTag CurrentIMCTag = BaseInputComp->GetCurrentIMCTag();

	//@PlayerOnGround IMC
	if (CurrentIMCTag == FGameplayTag::RequestGameplayTag(FName("Input.IMC.PlayerOnGround")))
	{
		//@Menu UI 열기
		if (InputTag == FGameplayTag::RequestGameplayTag(FName("Input.UI.OpenMenuUI")))
		{
			//@Mouse Cursor Shown and Input Mode Set
			if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
			{
				//@Show Mouse Cursor
				PC->bShowMouseCursor = true;

				//@FInputModeUIOnly
				FInputModeUIOnly InputMode;
				InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PC->SetInputMode(InputMode);
			}

			//@Hide HUD
			HideAllUI(EUICategory::HUD);
			
			//@Hide Interaction
			HideAllUI(EUICategory::Interaction);
			
			//@Show Menu UI
			ShowAllUI(EUICategory::Menu);

		}
	}
}

void UUIComponent::OnUIInputReleased(const FGameplayTag& InputTag)
{
	//@Base Input Comp
	UBaseInputComponent* BaseInputComp = Cast<UBaseInputComponent>(GetOwner()->InputComponent);
	if (!BaseInputComp)
	{
		UE_LOGFMT(LogUI, Error, "BaseInputComponent을 찾을 수 없습니다.");
		return;
	}
	//@Current IMC
	FGameplayTag CurrentIMCTag = BaseInputComp->GetCurrentIMCTag();

	//@PlayerOnGround IMC
	if (CurrentIMCTag == FGameplayTag::RequestGameplayTag(FName("Input.IMC.PlayerOnGround")))
	{
		// 다른 UI 관련 Input Tag는 무시 (OpenMenuUI는 Release에서 특별한 처리가 필요 없음)
	}
	//@UI IMC
	else if (CurrentIMCTag == FGameplayTag::RequestGameplayTag(FName("Input.IMC.MenuUI")))
	{
		// MenuUI 모드에서의 UI 입력 해제 처리
		if (InputTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Input.UI.MenuUI"))))
		{
			NotifyMenuUIInputReleased.ExecuteIfBound(InputTag);
		}
	}
}

void UUIComponent::OnRequestCloseMenuUI()
{

	//@Hide Menu UI
	HideAllUI(EUICategory::Menu);

	//@Show HUD
	ShowAllUI(EUICategory::HUD);

	//@Interaction
	ShowAllUI(EUICategory::Interaction);

	//@Input Mode를 UIOnly -> GameAndUI로 변경 (이 부분을 먼저 처리)
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOGFMT(LogUI, Warning, "{0}: PlayerController를 찾을 수 없습니다.", __FUNCTION__);
		return;
	}

	//@Input Mode
	PC->bShowMouseCursor = false;
	FInputModeGameOnly GameOnly;
	GameOnly.SetConsumeCaptureMouseDown(false);
	PC->SetInputMode(GameOnly);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
FString UUIComponent::FindUICategoryFromInputTag(const FGameplayTag& InputTag)
{
	FString TagString = InputTag.ToString();

	if (TagString.StartsWith("Input.UI."))
	{
		TArray<FString> Parts;
		TagString.ParseIntoArray(Parts, TEXT("."), true);

		if (Parts.Num() >= 3)
		{
			return Parts[2];
		}
	}

	return FString();
}

UUserWidget* UUIComponent::GetUI(EUICategory UICategory, const FGameplayTag& UITag) const
{
	switch (UICategory)
	{
	case EUICategory::HUD:
		return HUDUI;
	case EUICategory::Menu:
		return MenuUI;
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
	case EUICategory::Menu:
		if (MenuUI) Result.Add(MenuUI);
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