// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"
#include "Logging/StructuredLog.h"

#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "04_Component/InventoryComponent.h"
#include "04_Component/UIComponent.h"
#include "04_Component/BaseInputComponent.h"

#include "01_Character/CharacterBase.h"

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
    
    //@Input Comp
    if (const auto BaseInputComp = Cast<UBaseInputComponent>(InputComponent))
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
    if (auto PS = GetPlayerState<APlayerStateBase>())
    {
        RequestStartInitByPC.AddUFunction(PS, "InitializePlayerState");
    }
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
			if (const auto& BaseASC = CastChecked<UBaseAbilitySystemComponent>(ASC))
			{
				BaseASC->ProcessAbilityInput(DeltaTime, bGamePaused);
			}
		}
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
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