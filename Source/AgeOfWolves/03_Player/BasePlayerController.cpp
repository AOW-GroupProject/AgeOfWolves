// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "Logging/StructuredLog.h"

#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "04_Component/InventoryComponent.h"
#include "04_Component/UIComponent.h"

#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogBasePC)
// UE_LOGFMT(LogBasePC, Log, "");

#pragma region Default Setting
ABasePlayerController::ABasePlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	UIComponent = CreateDefaultSubobject<UUIComponent>(TEXT("UI Component"));
}

void ABasePlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void ABasePlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
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

    //1. UI
    InitializeUIComponent();
    //2. Player State(ASC)
    InitializePlayerState();
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
			if (const auto& BaseASC = CastChecked<UBaseAbilitySystemComponent>(ASC))
			{
				BaseASC->ProcessAbilityInput(DeltaTime, bGamePaused);
			}
		}
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
#pragma endregion

#pragma region Init
void ABasePlayerController::InitializeUIComponent()
{
    check(AcknowledgedPawn)

    //@UI Component
    if (UIComponent && UIComponent->LoadUI() && UIComponent->LoadUIInputComponent())
    {
        UIComponent->BindUIToUIComponent();
        UIComponent->BindToInventoryComponent(AcknowledgedPawn);
        UIComponent->BindInputComponentToInputActions();
    }
}

void ABasePlayerController::InitializePlayerState()
{
    //@Pawn
    if (!AcknowledgedPawn)
    {
        UE_LOGFMT(LogPlayerController, Error, "AcknowledgePossession: Possessed Pawn�� ��ȿ���� �ʽ��ϴ�.");
        return;
    }
    //@Character Base
    ACharacterBase* CharacterBase = Cast<ACharacterBase>(AcknowledgedPawn);
    if (!CharacterBase)
    {
        UE_LOGFMT(LogPlayerController, Warning, "AcknowledgePossession: Possessed Pawn�� ACharacterBase Ÿ���� �ƴմϴ�.");
        return;
    }
    //@Player State
    APlayerStateBase* PS = GetPlayerState<APlayerStateBase>();
    if (!PS)
    {
        UE_LOGFMT(LogPlayerController, Error, "AcknowledgePossession: PlayerState�� ��ȿ���� �ʰų� APlayerStateBase Ÿ���� �ƴմϴ�.");
        return;
    }
    //@Player State Load
    PS->LoadAbilitySystem();
    //@ASC
    UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
    if (ASC)
    {
        CharacterBase->SetAbilitySystemComponent(ASC);
        UE_LOGFMT(LogPlayerController, Log, "AbilitySystemComponent�� Character�� ���������� �����Ǿ����ϴ�.");
    }
    else
    {
        UE_LOGFMT(LogPlayerController, Error, "AcknowledgePossession: PlayerState�� AbilitySystemComponent�� ��ȿ���� �ʰų� UBaseAbilitySystemComponent Ÿ���� �ƴմϴ�.");
    }

    UE_LOGFMT(LogPlayerController, Log, "AcknowledgePossession: ���μ����� �Ϸ�Ǿ����ϴ�.");
}
#pragma endregion
