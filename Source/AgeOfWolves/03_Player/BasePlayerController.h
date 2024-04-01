// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	

public:
	ABasePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor 인터페이스
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor 인터페이스

	//~ACommonPlayerController 인터페이스
	virtual void OnPossess(APawn* InPawn) override;
	//~End of ACommonPlayerController interface

protected:
	//~APlayerController interface
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	// @설명 : PostProcessInput 함수는 사용자 입력 세팅 이후 매 프레임마다 호출되는 함수로, 사용자의 Ability Input Action 처리 함수가 호출되는 곳입니다.
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

};
