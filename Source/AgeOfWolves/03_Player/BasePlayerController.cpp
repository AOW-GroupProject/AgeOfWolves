#include "BasePlayerController.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"

#include "03_Player/PlayerStateBase.h"

#include "04_Component/PlayerAbilitySystemComponent.h"
#include "04_Component/UIComponent.h"
#include "04_Component/BaseInputComponent.h"



DEFINE_LOG_CATEGORY(LogBasePC)
// UE_LOGFMT(LogBasePC, Log, "");

#pragma region Default Setting
ABasePlayerController::ABasePlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UIComponent = CreateDefaultSubobject<UUIComponent>(TEXT("UI Component"));
    InputComponent = CreateDefaultSubobject<UBaseInputComponent>(TEXT("Input Component"));
}

void ABasePlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();

}

void ABasePlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
    
    //@Input Mode
    //FInputModeGameAndUI DefaultInputMode;
    //bShowMouseCursor = true;
    //DefaultInputMode.SetHideCursorDuringCapture(false);
    //DefaultInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
    //SetInputMode(DefaultInputMode);
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();

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

    //@�ʱ�ȭ �Լ�
    InitializePlayerController();

    //@Delegate : �� ������Ʈ�� �ʱ�ȭ �۾��� �����ϴ� Delegate�̹Ƿ�, Binding ���Ŀ� ����
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
			if (const auto& PlayerASC = CastChecked<UPlayerAbilitySystemComponent>(ASC))
			{
                PlayerASC->ProcessAbilityInput(DeltaTime, bGamePaused);
			}
		}
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}

void ABasePlayerController::InitializePlayerController()
{
    //@TODO: �ʱ� �Է� ��� ������ �׽�Ʈ�Դϴ�. ���� ���� �۾� ���� �Ʒ�����...

    //@Input Mode ����
    SetupInputModeOnBeginPlay();

    //@ViewportClient ����
    SetupViewportClientOnBeginPlay();

    //@Input Comp
    if (UBaseInputComponent* BaseInputComp = Cast<UBaseInputComponent>(InputComponent))
    {
        //@�ʱ�ȭ �۾� ����ȭ
        RequestStartInitByPC.AddUFunction(BaseInputComp, "InitializeInputComponent");
    }

    //@UI Comp
    if (UIComponent)
    {
        //@�ʱ�ȭ �۾� ����ȭ
        RequestStartInitByPC.AddUFunction(UIComponent, "InitializeUIComponent");
    }

    //@PS
    if (APlayerStateBase* PS = GetPlayerState<APlayerStateBase>())
    {
        RequestStartInitByPC.AddUFunction(PS, "InitializePlayerState");
    }

}
#pragma endregion

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
#pragma endregion

#pragma region Utility
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
#pragma endregion