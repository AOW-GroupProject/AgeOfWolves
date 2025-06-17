#include "BasePlayerController.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"

#include "03_Player/PlayerStateBase.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "04_Component/UIComponent.h"
#include "04_Component/BaseInputComponent.h"
#include "04_Component/ObjectiveDetectionComponent.h"
#include "04_Component/InteractionComponent.h"

DEFINE_LOG_CATEGORY(LogBasePC)

//@Defualt Setting
#pragma region Default Setting
ABasePlayerController::ABasePlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

    //@Components...
	UIComponent = CreateDefaultSubobject<UUIComponent>(TEXT("UI Component"));
    InputComponent = CreateDefaultSubobject<UBaseInputComponent>(TEXT("Input Component"));
    ODComponent = CreateDefaultSubobject< UObjectiveDetectionComponent>(TEXT("Objective Detection Component"));
    InteractComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("Interaction Component"));
}

void ABasePlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();

}

void ABasePlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
    
    //@바인딩
    if (APlayerStateBase* PS = GetPlayerState<APlayerStateBase>())
    {
        UIComponent->UIsForAttributeSetReady.BindUFunction(PS, "LoadGameAbilitySystem"); 
    }

}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

}

void ABasePlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

void ABasePlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void ABasePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}

void ABasePlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ABasePlayerController::AcknowledgePossession(APawn* P)
{
    Super::AcknowledgePossession(P);

    //@초기화 함수
    InitializePlayerController();

    //@Delegate : 각 컴포넌트의 초기화 작업을 유도하는 Delegate이므로, Binding 이후에 실행
    RequestStartInitByPC.Broadcast();
}

void ABasePlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ABasePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
    //@Player State
	if (PlayerState->IsA<APlayerStateBase>())
	{
        //@ASC
		if (UAbilitySystemComponent* ASC = Cast<APlayerStateBase>(PlayerState)->GetAbilitySystemComponent())
		{
            //@Actove GA
			if (const auto& PlayerASC = CastChecked<UBaseAbilitySystemComponent>(ASC))
			{
                PlayerASC->ProcessAbilityInput(DeltaTime, bGamePaused);
			}
		}
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void ABasePlayerController::InternalBindToPlayerState()
{
    if (APlayerStateBase* PS = GetPlayerState<APlayerStateBase>())
    {
        PS->NotifyPlayerDeathEvent.AddUFunction(this, "OnPlayerDeath");
        UE_LOGFMT(LogBasePC, Log, "Player State Death 이벤트 바인딩 완료");
    }
}

void ABasePlayerController::InitializePlayerController()
{
    //@내부 바인딩...
    InternalBindToPlayerState();

    //@Input Mode 설정
    SetupInputModeOnBeginPlay();

    //@ViewportClient 설정
    SetupViewportClientOnBeginPlay();

    //@Input Comp
    if (UBaseInputComponent* BaseInputComp = Cast<UBaseInputComponent>(InputComponent))
    {
        //@초기화 작업 동기화
        RequestStartInitByPC.AddUFunction(BaseInputComp, "InitializeInputComponent");
    }
    //@UI Comp
    if (UIComponent)
    {
        //@초기화 작업 동기화
        RequestStartInitByPC.AddUFunction(UIComponent, "InitializeUIComponent");
    }
    //@PS
    if (APlayerStateBase* PS = GetPlayerState<APlayerStateBase>())
    {
        RequestStartInitByPC.AddUFunction(PS, "InitializePlayerState");
    }
    //@OD Component
    if (ODComponent)
    {
        RequestStartInitByPC.AddUFunction(ODComponent, "InitializeODComponent");
    }
    //@Interaction Component
    if (InteractComponent)
    {
        RequestStartInitByPC.AddUFunction(InteractComponent, "InitializeInteractionComp");
    }
}
#pragma endregion

//@Property/Info...etc
#pragma region Properties
void ABasePlayerController::SetupInputModeOnBeginPlay()
{
    //@FInputModeGameAndUI
    FInputModeGameAndUI InputMode;
    bShowMouseCursor = false;
    InputMode.SetHideCursorDuringCapture(false);
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);

    SetInputMode(InputMode);
}

void ABasePlayerController::SetupViewportClientOnBeginPlay()
{
    //@ViewportClient
    UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
    if(!ViewportClient)
    {
        return;
    }

    ViewportClient->SetMouseLockMode(EMouseLockMode::LockAlways);
    ViewportClient->SetMouseCaptureMode(EMouseCaptureMode::CapturePermanently);
}

void ABasePlayerController::HandleCharacterDeath()
{
    UE_LOGFMT(LogBasePC, Warning, "캐릭터 사망 - 입력 시스템 비활성화 및 Loading UI 표시");

    DisableInput(this);

    if (UIComponent)
    {
        //UIComponent->ShowUI(EUICategory::InGameLoading, FGameplayTag::RequestGameplayTag("UI.InGameLoading"));
        //UIComponent->HideAllUI(EUICategory::HUD);
        //UIComponent->HideAllUI(EUICategory::Interaction);
        UE_LOGFMT(LogBasePC, Log, "InGameLoading UI 표시 및 다른 UI 숨김 완료");
    }
}

void ABasePlayerController::HandleCharacterRevive()
{
    UE_LOGFMT(LogBasePC, Warning, "캐릭터 부활 - 입력 시스템 재활성화 및 UI 복원");

    EnableInput(this);

    if (UIComponent)
    {
        //UIComponent->HideUI(EUICategory::InGameLoading, FGameplayTag::RequestGameplayTag("UI.InGameLoading"));
        //UIComponent->ShowAllUI(EUICategory::HUD);
        //UIComponent->ShowAllUI(EUICategory::Interaction);
        UE_LOGFMT(LogBasePC, Log, "InGameLoading UI 숨김 및 다른 UI 복원 완료");
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
void ABasePlayerController::OnPlayerDeath(APlayerStateBase* DeadPlayerState)
{
    if (APlayerStateBase* PS = GetPlayerState<APlayerStateBase>())
    {
        if (PS == DeadPlayerState)
        {
            HandleCharacterDeath();
            return;
        }
    }
}

UUIComponent* ABasePlayerController::GetUIComponent() const
{
    if (!UIComponent)
    {
        return nullptr;
    }

    return UIComponent;
}

UBaseInputComponent* ABasePlayerController::GetBaseInputComponent() const
{
    if (auto BaseInputComp = Cast<UBaseInputComponent>(InputComponent))
    {
        return BaseInputComp;
    }

    return nullptr;
}

UObjectiveDetectionComponent* ABasePlayerController::GetODComponent() const
{
    if (!ODComponent)
    {
        return nullptr;
    }

    return ODComponent;
}

UInteractionComponent* ABasePlayerController::GetInteractionComponent() const
{
    if (!InteractComponent)
    {
        return nullptr;
    }

    return InteractComponent;
}
#pragma endregion