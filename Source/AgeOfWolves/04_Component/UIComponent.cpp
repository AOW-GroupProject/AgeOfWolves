#include "04_Component/UIComponent.h"
#include "Logging/StructuredLog.h"

#include "14_Subsystem/UIManagerSubsystem.h"
#include "08_UI/01_HUD/PlayerHUD.h"
#include "08_UI/02_Menu/MenuUI.h"

#include "03_Player/BasePlayerController.h"
#include "04_Component/BaseInputComponent.h"
#include "04_Component/ObjectiveDetectionComponent.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogUI)
// UE_LOGFMT(LogUI, Log, "");

//@Defualt Setting
#pragma region Default Setting
UUIComponent::UUIComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{ 
	PrimaryComponentTick.bCanEverTick = true; 
	bWantsInitializeComponent = true;//Initialize Component 활용에 필요 

	HUDUI = nullptr;
	MenuUI = nullptr;

	MIndicatorUIs.Empty();
	IndicatorTargets.Empty();
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

void UUIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//@Indicator가 비활성화되어 있으면 업데이트 안 함
	if (!bEnableIndicators)
	{
		return;
	}

	//@Indicator 위치 업데이트
	UpdateIndicatorPositions(DeltaTime);
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

void UUIComponent::ExternalBindingToODComponent(const APlayerController* PlayerController)
{
	UObjectiveDetectionComponent* ODComp =
		PlayerController->FindComponentByClass<UObjectiveDetectionComponent>();

	if (!ODComp)
	{
		UE_LOGFMT(LogUI, Error, "ObjectiveDetectionComponent를 찾을 수 없습니다.");
		return;
	}

	//@이벤트 바인딩
	ODComp->CurrentTargetChanged.AddUObject(this, &UUIComponent::OnCurrentTargetChanged);
	ODComp->ExecutionTargetChanged.AddUObject(this, &UUIComponent::OnExecutionTargetChanged);
	ODComp->AmbushTargetChanged.AddUObject(this, &UUIComponent::OnAmbushTargetChanged);
	ODComp->DetectedStructureChanged.AddUObject(this, &UUIComponent::OnStructureDetected);

	UE_LOGFMT(LogUI, Log, "ObjectiveDetectionComponent 이벤트 바인딩 완료");
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
	ExternalBindingToODComponent(PC);

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


	UE_LOGFMT(LogUI, Log, "UI Component 초기화 완료 (Indicator: {0}개)", MIndicatorUIs.Num());
}

void UUIComponent::CheckAllUIsForDefaultVisibilitySetting()
{
	if (bHUDInitFinished && bMenuUIInitFinished)
	{
		bHUDInitFinished = false;
		bMenuUIInitFinished = false;

		UIsForInventoryReady.ExecuteIfBound();
		UIsForAttributeSetReady.ExecuteIfBound();

		//@모든 UI들에 대한 리셋 작업
		UE_LOGFMT(LogUI, Log, "초기화");
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

	// ✅ 수정: Interaction -> Indicator로 변경 (또는 둘 다 처리)
	//@Indicator UIs 리셋
	ResetCategoryUI(EUICategory::Indicator, UIManagerSubsystem);
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
		// ✅ 개선: Indicator는 GetIndicatorByTag 사용
		UUserWidget* Widget = nullptr;

		if (UICategory == EUICategory::Indicator)
		{
			Widget = GetIndicatorByTag(UIInfo.UITag);
		}
		else
		{
			Widget = GetUI(UICategory, UIInfo.UITag);
		}

		if (Widget)
		{
			if (UIInfo.bShownOnBeginPlay)
			{
				//@Indicator는 bShownOnBeginPlay가 false여야 하지만, 안전하게 처리
				if (UICategory != EUICategory::Indicator)
				{
					Widget->AddToViewport();
					ShowUI(UICategory, UIInfo.UITag);
				}
				else
				{
					UE_LOGFMT(LogUI, Warning, "Indicator UI는 BeginPlay에 표시될 수 없습니다: {0}",
						*UIInfo.UITag.ToString());
				}
			}
			else
			{
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
	case EUICategory::Indicator:
		SetupIndicatorUI(UIInfo.UITag, NewWidget);
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

void UUIComponent::SetupIndicatorUI(const FGameplayTag& UITag, UUserWidget* NewWidget)
{
	if (!NewWidget)
	{
		UE_LOGFMT(LogUI, Error, "Invalid Widget for Indicator: {0}", *UITag.ToString());
		return;
	}

	//@Map에 추가
	MIndicatorUIs.Add(UITag, NewWidget);

	//@타겟 맵 초기화
	IndicatorTargets.Add(UITag, nullptr);

	//@Viewport에 추가 (HUD 위에, 초기 숨김 상태)
	NewWidget->AddToViewport(100); // Z-Order 100
	NewWidget->SetVisibility(ESlateVisibility::Collapsed);

	UE_LOGFMT(LogUI, Log, "Indicator UI 설정 완료: {0}", *UITag.ToString());
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
				//@UI Tag 찾기
				FGameplayTag UITag;

				// ✅ 수정: Indicator 처리
				if (UICategory == EUICategory::Indicator)
				{
					for (const auto& Pair : MIndicatorUIs)
					{
						if (Pair.Value == Widget)
						{
							UITag = Pair.Key;
							break;
						}
					}
				}
				// HUD, Menu는 Tag가 필요 없음 (단일 Widget)

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
				//@UI Tag 찾기
				FGameplayTag UITag;

				// ✅ 수정: Indicator 처리
				if (UICategory == EUICategory::Indicator)
				{
					for (const auto& Pair : MIndicatorUIs)
					{
						if (Pair.Value == Widget)
						{
							UITag = Pair.Key;
							break;
						}
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

void UUIComponent::UpdateActiveIndicator()
{
	//@Indicator 전체가 비활성화된 경우
	if (!bEnableIndicators)
	{
		HideAllIndicators();
		return;
	}

	//@각 Indicator를 개별적으로 처리 (독립적)

	// 1. LockOn Indicator
	{
		FGameplayTag LockOnTag = FGameplayTag::RequestGameplayTag(FName("UI.Indicator.LockOn"));
		UUserWidget* LockOnIndicator = GetIndicatorByTag(LockOnTag);
		AActor* Target = GetIndicatorTarget(LockOnTag);

		bool bShouldShow = (bShowLockOnIndicator || bDebugMode) && Target != nullptr;

		if (LockOnIndicator)
		{
			if (bShouldShow)
			{
				ShowIndicatorWidget(LockOnIndicator);
			}
			else
			{
				HideIndicatorWidget(LockOnIndicator);
			}
		}
	}

	// 2. Execution Indicator
	{
		FGameplayTag ExecutionTag = FGameplayTag::RequestGameplayTag(FName("UI.Indicator.Execution"));
		UUserWidget* ExecutionIndicator = GetIndicatorByTag(ExecutionTag);
		AActor* Target = GetIndicatorTarget(ExecutionTag);

		bool bShouldShow = (bShowExecutionIndicator || bDebugMode) && Target != nullptr;

		if (ExecutionIndicator)
		{
			if (bShouldShow)
			{
				ShowIndicatorWidget(ExecutionIndicator);
			}
			else
			{
				HideIndicatorWidget(ExecutionIndicator);
			}
		}
	}

	// 3. Ambush Indicator
	{
		FGameplayTag AmbushTag = FGameplayTag::RequestGameplayTag(FName("UI.Indicator.Ambush"));
		UUserWidget* AmbushIndicator = GetIndicatorByTag(AmbushTag);
		AActor* Target = GetIndicatorTarget(AmbushTag);

		bool bShouldShow = (bShowAmbushIndicator || bDebugMode) && Target != nullptr;

		if (AmbushIndicator)
		{
			if (bShouldShow)
			{
				ShowIndicatorWidget(AmbushIndicator);
			}
			else
			{
				HideIndicatorWidget(AmbushIndicator);
			}
		}
	}

	// 4. Structure Indicator
	{
		FGameplayTag StructureTag = FGameplayTag::RequestGameplayTag(FName("UI.Indicator.StatueInteractionUI"));
		UUserWidget* StructureIndicator = GetIndicatorByTag(StructureTag);
		AActor* Target = GetIndicatorTarget(StructureTag);

		bool bShouldShow = (bShowStructureIndicator || bDebugMode) && Target != nullptr;

		if (StructureIndicator)
		{
			if (bShouldShow)
			{
				ShowIndicatorWidget(StructureIndicator);
			}
			else
			{
				HideIndicatorWidget(StructureIndicator);
			}
		}
	}
}

void UUIComponent::ShowIndicatorWidget(UUserWidget* Widget)
{
	if (!Widget) return;

	//@이미 표시 중이면 중복 처리 방지
	if (Widget->GetVisibility() == ESlateVisibility::HitTestInvisible)
	{
		return;
	}

	Widget->SetVisibility(ESlateVisibility::HitTestInvisible);

	UE_LOGFMT(LogUI, Verbose, "Indicator 표시됨");
}

void UUIComponent::HideIndicatorWidget(UUserWidget* Widget)
{
	if (!Widget) return;

	//@이미 숨겨져 있으면 중복 처리 방지
	if (Widget->GetVisibility() == ESlateVisibility::Collapsed)
	{
		return;
	}

	Widget->SetVisibility(ESlateVisibility::Collapsed);

	UE_LOGFMT(LogUI, Verbose, "Indicator 숨김");
}

void UUIComponent::HideAllIndicators()
{
	for (const auto& Pair : MIndicatorUIs)
	{
		if (Pair.Value)
		{
			HideIndicatorWidget(Pair.Value);
		}
	}
}

void UUIComponent::UpdateIndicatorPositions(float DeltaTime)
{
	//@표시 중인 모든 Indicator의 위치 업데이트
	for (const auto& Pair : MIndicatorUIs)
	{
		UUserWidget* Indicator = Pair.Value;
		const FGameplayTag& Tag = Pair.Key;
		AActor* Target = GetIndicatorTarget(Tag);

		//@Indicator가 표시 중이고 타겟이 있으면 위치 업데이트
		if (Indicator && Target && Indicator->GetVisibility() == ESlateVisibility::HitTestInvisible)
		{
			UpdateSingleIndicatorPosition(Indicator, Target, DeltaTime);
		}
	}
}

bool UUIComponent::UpdateSingleIndicatorPosition(UUserWidget* Indicator, AActor* Target, float DeltaTime)
{
	if (!Indicator || !Target)
	{
		return false;
	}

	//@PlayerController 가져오기
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		return false;
	}

	//@Indicator Tag 찾기
	FGameplayTag IndicatorTag;
	for (const auto& Pair : MIndicatorUIs)
	{
		if (Pair.Value == Indicator)
		{
			IndicatorTag = Pair.Key;
			break;
		}
	}

	if (!IndicatorTag.IsValid())
	{
		return false;
	}

	FString TagString = IndicatorTag.ToString();

	//@타겟의 3D 월드 위치 계산
	FVector TargetWorldLocation;
	if (!GetIndicatorWorldPosition(IndicatorTag, Target, TargetWorldLocation))
	{
		return false;
	}

	//@3D 월드 좌표 → 2D 스크린 좌표 변환
	FVector2D ScreenPosition;
	bool bIsOnScreen = PC->ProjectWorldLocationToScreen(TargetWorldLocation, ScreenPosition, true);

	//@화면 밖이면 숨김 처리
	if (!bIsOnScreen && bHideIndicatorWhenOffScreen)
	{
		if (Indicator->GetVisibility() != ESlateVisibility::Collapsed)
		{
			Indicator->SetVisibility(ESlateVisibility::Collapsed);
		}
		return false;
	}

	//@✅ Structure는 2D 화면 좌표에서 오프셋 적용 (화면 기준 오른쪽)
	if (TagString.Contains(TEXT("StatueInteractionUI")))
	{
		ScreenPosition += StructureScreenOffset;
		UE_LOGFMT(LogUI, Verbose, "Structure 2D 오프셋 적용 후: ({0}, {1})",
			ScreenPosition.X, ScreenPosition.Y);
	}

	//@화면 안이면 표시
	if (Indicator->GetVisibility() == ESlateVisibility::Collapsed)
	{
		Indicator->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

	//@현재 위치 가져오기
	FVector2D CurrentPosition = Indicator->GetCachedGeometry().GetAbsolutePosition();

	//@✅ DeadZone & SoftZone 적용
	FVector2D PositionDelta = ScreenPosition - CurrentPosition;
	float Distance = PositionDelta.Size();

	FVector2D NewPosition;

	// DeadZone: 거리가 너무 작으면 이동 안 함
	if (Distance < IndicatorDeadZone)
	{
		NewPosition = CurrentPosition;  // 현재 위치 유지
		// 로그 생략 (너무 많이 찍힘)
	}
	// SoftZone: 거리가 작으면 느리게 이동
	else if (Distance < IndicatorSoftZone)
	{
		float SlowSpeed = IndicatorInterpolationSpeed * SoftZoneSpeedMultiplier;
		NewPosition.X = FMath::FInterpTo(CurrentPosition.X, ScreenPosition.X, DeltaTime, SlowSpeed);
		NewPosition.Y = FMath::FInterpTo(CurrentPosition.Y, ScreenPosition.Y, DeltaTime, SlowSpeed);

		UE_LOGFMT(LogUI, VeryVerbose, "SoftZone 보간 (거리: {0}): {1} → {2}",
			Distance, *CurrentPosition.ToString(), *NewPosition.ToString());
	}
	// 일반: 거리가 크면 정상 속도로 이동
	else
	{
		NewPosition.X = FMath::FInterpTo(CurrentPosition.X, ScreenPosition.X, DeltaTime, IndicatorInterpolationSpeed);
		NewPosition.Y = FMath::FInterpTo(CurrentPosition.Y, ScreenPosition.Y, DeltaTime, IndicatorInterpolationSpeed);

		UE_LOGFMT(LogUI, VeryVerbose, "일반 보간 (거리: {0}): {1} → {2}",
			Distance, *CurrentPosition.ToString(), *NewPosition.ToString());
	}

	//@위치 설정
	Indicator->SetPositionInViewport(NewPosition, false);

	return true;
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

void UUIComponent::OnCurrentTargetChanged(const AActor* NewTarget)
{
	FGameplayTag LockOnTag = FGameplayTag::RequestGameplayTag(FName("UI.Indicator.LockOn"));

	//@타겟 맵 업데이트
	if (IndicatorTargets.Contains(LockOnTag))
	{
		IndicatorTargets[LockOnTag] = const_cast<AActor*>(NewTarget);
	}

	if (NewTarget)
	{
		UE_LOGFMT(LogUI, Log, "LockOn 타겟 설정: {0}", *NewTarget->GetName());
	}
	else
	{
		UE_LOGFMT(LogUI, Log, "LockOn 타겟 해제");
	}

	UpdateActiveIndicator();
}

void UUIComponent::OnExecutionTargetChanged(const AActor* NewTarget)  // ✅ const 추가
{
	FGameplayTag ExecutionTag = FGameplayTag::RequestGameplayTag(FName("UI.Indicator.Execution"));

	//@타겟 맵 업데이트 - const_cast 필요
	if (IndicatorTargets.Contains(ExecutionTag))
	{
		IndicatorTargets[ExecutionTag] = const_cast<AActor*>(NewTarget);  // ✅ const_cast
	}

	if (NewTarget)
	{
		UE_LOGFMT(LogUI, Log, "Execution 타겟 설정: {0}", *NewTarget->GetName());
	}
	else
	{
		UE_LOGFMT(LogUI, Log, "Execution 타겟 해제");
	}

	UpdateActiveIndicator();
}

void UUIComponent::OnAmbushTargetChanged(const AActor* NewTarget)  // ✅ const 추가
{
	FGameplayTag AmbushTag = FGameplayTag::RequestGameplayTag(FName("UI.Indicator.Ambush"));

	//@타겟 맵 업데이트
	if (IndicatorTargets.Contains(AmbushTag))
	{
		IndicatorTargets[AmbushTag] = const_cast<AActor*>(NewTarget);  // ✅ const_cast
	}

	if (NewTarget)
	{
		UE_LOGFMT(LogUI, Log, "Ambush 타겟 설정: {0}", *NewTarget->GetName());
	}
	else
	{
		UE_LOGFMT(LogUI, Log, "Ambush 타겟 해제");
	}

	UpdateActiveIndicator();
}

void UUIComponent::OnStructureDetected(const AActor* Structure, bool bDetected)  // ✅ const 추가
{
	FGameplayTag StructureTag = FGameplayTag::RequestGameplayTag(FName("UI.Indicator.StatueInteractionUI"));

	//@타겟 맵 업데이트
	if (IndicatorTargets.Contains(StructureTag))
	{
		IndicatorTargets[StructureTag] = bDetected ? const_cast<AActor*>(Structure) : nullptr;  // ✅ const_cast
	}

	if (bDetected && Structure)
	{
		UE_LOGFMT(LogUI, Log, "Structure 감지: {0}", *Structure->GetName());
	}
	else
	{
		UE_LOGFMT(LogUI, Log, "Structure 감지 해제");
	}

	UpdateActiveIndicator();
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
	case EUICategory::Indicator:
		if (auto* Widget = MIndicatorUIs.Find(UITag))
		{
			return *Widget;
		}
		UE_LOGFMT(LogUI, Warning, "Indicator UI를 찾을 수 없습니다. Tag: {0}", UITag.ToString());
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
	case EUICategory::Indicator:
		MIndicatorUIs.GenerateValueArray(Result);
		break;
	default:
		UE_LOGFMT(LogUI, Warning, "유효하지 않은 UI Category입니다.");
	}
	return Result;
}

UUserWidget* UUIComponent::GetIndicatorByTag(const FGameplayTag& IndicatorTag) const
{
	if (auto* Indicator = MIndicatorUIs.Find(IndicatorTag))
	{
		return *Indicator;
	}

	UE_LOGFMT(LogUI, Warning, "Indicator를 찾을 수 없습니다. Tag: {0}", *IndicatorTag.ToString());
	return nullptr;
}

UUserWidget* UUIComponent::GetIndicatorByType(const FString& TypeName) const
{
	//@UI.Indicator.{TypeName} 형식으로 태그 생성
	FString TagString = FString::Printf(TEXT("UI.Indicator.%s"), *TypeName);
	FGameplayTag IndicatorTag = FGameplayTag::RequestGameplayTag(FName(*TagString));

	return GetIndicatorByTag(IndicatorTag);
}

AActor* UUIComponent::GetIndicatorTarget(const FGameplayTag& IndicatorTag) const
{
	if (auto* Target = IndicatorTargets.Find(IndicatorTag))
	{
		return Target->Get();
	}

	return nullptr;
}

bool UUIComponent::GetIndicatorWorldPosition(const FGameplayTag& IndicatorTag, AActor* Target, FVector& OutWorldPosition)
{
	if (!Target)
	{
		return false;
	}

	FString TagString = IndicatorTag.ToString();

	// ============================================
	// 1. LockOn Indicator - Spine 소켓 위치
	// ============================================
	if (TagString.Contains(TEXT("LockOn")))
	{
		USkeletalMeshComponent* SkeletalMesh = Target->FindComponentByClass<USkeletalMeshComponent>();
		if (!SkeletalMesh)
		{
			UE_LOGFMT(LogUI, Warning, "LockOn 타겟에 SkeletalMeshComponent가 없음: {0}", *Target->GetName());
			OutWorldPosition = Target->GetActorLocation();
			OutWorldPosition.Z += GeneralIndicatorHeightOffset;
			return true;
		}

		if (SkeletalMesh->DoesSocketExist(LockOnTargetSocketName))
		{
			OutWorldPosition = SkeletalMesh->GetSocketLocation(LockOnTargetSocketName);
			OutWorldPosition += LockOnSocketOffset;
			return true;
		}
		else if (SkeletalMesh->DoesSocketExist(FName("spine_02")))
		{
			OutWorldPosition = SkeletalMesh->GetSocketLocation(FName("spine_02"));
			OutWorldPosition += LockOnSocketOffset;
			return true;
		}
		else
		{
			UE_LOGFMT(LogUI, Warning, "Spine 소켓을 찾을 수 없음: {0}", *Target->GetName());
			OutWorldPosition = Target->GetActorLocation();
			OutWorldPosition.Z += GeneralIndicatorHeightOffset;
			return true;
		}
	}

	// ============================================
	// 2. Structure Indicator - ✅ 수정: 중심점만 반환 (2D 오프셋은 나중에)
	// ============================================
	else if (TagString.Contains(TEXT("StatueInteractionUI")))
	{
		OutWorldPosition = Target->GetActorLocation();
		OutWorldPosition.Z += StructureHeightOffset;  // 높이만 3D에서 적용

		UE_LOGFMT(LogUI, Verbose, "Structure 3D 중심: {0}", *OutWorldPosition.ToString());
		return true;
	}

	// ============================================
	// 3. 기타 Indicator
	// ============================================
	else
	{
		OutWorldPosition = Target->GetActorLocation();
		OutWorldPosition.Z += GeneralIndicatorHeightOffset;
		return true;
	}
}
#pragma endregion