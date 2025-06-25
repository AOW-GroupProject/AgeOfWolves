#include "BasePlayerController.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"

#include "17_GameMode/AgeOfWolvesGameMode.h"
#include "17_GameMode/AOWGameState.h"
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
        PS->NotifyPlayerRevivalEvent.AddUFunction(this, "OnPlayerRevival");
    }
    
    UE_LOGFMT(LogBasePC, Log, "Player State Death 이벤트 바인딩 완료");
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
    UE_LOGFMT(LogBasePC, Warning, "캐릭터 사망 - 리스폰 시퀀스 시작");

    DisableInput(this);
    bRespawnCompleted = false; // 플래그 초기화

    //@죽음 화면 표시 및 시퀀스 시작
    if (UIComponent)
    {
        // UIComponent->ShowDeathScreen();
        UE_LOGFMT(LogBasePC, Log, "죽음 화면 UI 표시");
    }

    //@시퀀스 시작
    CurrentRespawnState = ERespawnState::DeathScreen;
    GetWorldTimerManager().SetTimer(RespawnSequenceTimer, this, &ABasePlayerController::ProcessRespawnSequence, 3.0f, false);
}


void ABasePlayerController::ProcessRespawnSequence()
{
    switch (CurrentRespawnState)
    {
        case ERespawnState::DeathScreen:
        {
            //@로딩 화면으로 전환
            if (UIComponent)
            {
                // UIComponent->HideDeathScreen();
                // UIComponent->ShowLoadingScreen();
                UE_LOGFMT(LogBasePC, Log, "로딩 화면 표시");
            }

            CurrentRespawnState = ERespawnState::LoadingScreen;
            GetWorldTimerManager().SetTimer(RespawnSequenceTimer, this, &ABasePlayerController::ProcessRespawnSequence, 0.5f, false);
            break;
        }

        case ERespawnState::LoadingScreen:
        {
            //@리스폰 실행
            UE_LOGFMT(LogBasePC, Log, "리스폰 작업 실행");

            if (auto GameMode = Cast<AAgeOfWolvesGameMode>(GetWorld()->GetAuthGameMode()))
            {
                if (IsValid(GameMode))
                {
                    GameMode->HandlePlayerDeath(this);
                }
            }

            CurrentRespawnState = ERespawnState::Respawning;

            //@만약 이미 리스폰이 완료되었다면 바로 게임 재개
            if (bRespawnCompleted)
            {
                GetWorldTimerManager().SetTimer(RespawnSequenceTimer, this, &ABasePlayerController::ProcessRespawnSequence, 1.0f, false);
            }
            //@아니면 GameState 이벤트 대기
            break;
        }

        case ERespawnState::Respawning:
        {
            //@게임 재개
            if (UIComponent)
            {
                // UIComponent->HideLoadingScreen();
                UE_LOGFMT(LogBasePC, Log, "로딩 화면 제거");
            }

            EnableInput(this);
            CurrentRespawnState = ERespawnState::Complete;

            UE_LOGFMT(LogBasePC, Log, "리스폰 시퀀스 완료");
            break;
        }
    }
}

void ABasePlayerController::HandleCharacterRevive()
{
    //@이 함수는 더 이상 GameState에서 직접 호출되지 않음
    //@ASC의 부활 어빌리티 완료 후 호출됨
    UE_LOGFMT(LogBasePC, Warning, "ASC 부활 어빌리티 완료 - UI 정리 시작");

    //@로딩 화면 제거
    if (UIComponent)
    {
        // UIComponent->HideLoadingScreen();
        UE_LOGFMT(LogBasePC, Log, "로딩 화면 제거");
    }

    //@몇 초 후 입력 활성화
    FTimerHandle InputEnableTimer;
    GetWorldTimerManager().SetTimer(
        InputEnableTimer,
        [this]()
        {
            EnableInput(this);
            CurrentRespawnState = ERespawnState::Complete;
            UE_LOGFMT(LogBasePC, Log, "입력 활성화 - 리스폰 시퀀스 완료");
        },
        2.0f, // 2초 후 입력 활성화
        false
    );
}

#pragma endregion

//@Callbacks
#pragma region Callbacks
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

void ABasePlayerController::OnPlayerRevival(APlayerStateBase* RespawnPlayerState)
{
    //@기본 유효성 검증
    if (!IsValid(RespawnPlayerState))
    {
        UE_LOGFMT(LogBasePC, Warning, "리스폰 콜백 실패: 유효하지 않은 Player State");
        return;
    }

    //@자신의 리스폰인지 확인
    if (RespawnPlayerState->GetOwner() != this)
    {
        UE_LOGFMT(LogBasePC, Log, "다른 플레이어 리스폰 완료 확인: {0}", GetNameSafe(RespawnPlayerState->GetOwner()));
        return;
    }

    UE_LOGFMT(LogBasePC, Log, "자신의 리스폰 완료 확인: {0}", GetNameSafe(this));

    //@부활 작업
    HandleCharacterRevive();

}
#pragma endregion

//@Utility(Setter, Getter,...etc)
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
