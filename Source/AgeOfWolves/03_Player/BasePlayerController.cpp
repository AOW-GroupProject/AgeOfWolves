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

	if (auto PS = GetPlayerState<APlayerStateBase>())
	{
		PS->InitializePlayerSystem();
	}
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

void ABasePlayerController::PreProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PreProcessInput(DeltaTime, bGamePaused);
}

void ABasePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	// @���� : Player State -> ASC ������ �����ͼ�, ASC�� ProcessAbilityInput() �Լ��� ȣ���մϴ�.
	// @���� : ASC�� ProcessAbilityInput�� �� �����Ӹ��� ȣ��Ǹ�, ASC�� ��ϵǾ� Ȱ��ȭ Ȥ�� ��Ȱ��ȭ�� ��ٸ��� Gameplay Ability�鿡 ���� �۾��� �̷���ϴ�.
	// @���� : BaseAbilitySystemComponent.cpp
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
