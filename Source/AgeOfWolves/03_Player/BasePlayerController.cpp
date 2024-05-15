// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerController.h"

#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"

ABasePlayerController::ABasePlayerController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

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

	// OnPosess 이벤트 호출
	//OnControllerPossessCharacter.ExecuteIfBound();

	if (auto PS = GetPlayerState<APlayerStateBase>())
	{
		PS->InitializeGameplayAbilitySystem();
	}
}

void ABasePlayerController::OnUnPossess()
{
	Super::OnUnPossess();
}

void ABasePlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ABasePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	// @목적 : Player State -> ASC 순서로 가져와서, ASC의 ProcessAbilityInput() 함수를 호출합니다.
	// @설명 : ASC의 ProcessAbilityInput은 매 프레임마다 호출되며, ASC에 등록되어 활성화 혹은 비활성화를 기다리는 Gameplay Ability들에 대한 작업이 이루업니다.
	// @참조 : BaseAbilitySystemComponent.cpp
	if (PlayerState->IsA<APlayerStateBase>())
	{
		if (UAbilitySystemComponent* ASC = Cast<APlayerStateBase>(PlayerState)->GetAbilitySystemComponent())
		{
			if (const auto& BaseASC = CastChecked<UBaseAbilitySystemComponent>(ASC))
			{
				BaseASC->ProcessAbilityInput(DeltaTime, bGamePaused);
			}
		}
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
