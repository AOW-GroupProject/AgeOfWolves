// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/*
	@목적 : Controller가 Character를 소유하는 시점을 알리는 이벤트
	@설명 : Controller -> PS 순서의 초기화 순서를 유지하기 위해, Controller는 Character 소유 시점을 PS에게 알립니다.
*/
DECLARE_DYNAMIC_DELEGATE(FControllerPossessCharacter);

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
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor 인터페이스

	//~ACommonPlayerController 인터페이스
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	//~End of ACommonPlayerController interface

protected:
	//~APlayerController interface
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	// @설명 : PostProcessInput 함수는 사용자 입력 세팅 이후 매 프레임마다 호출되는 함수로, 사용자의 Ability Input Action 처리 함수가 호출되는 곳입니다.
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

public:
	FControllerPossessCharacter OnControllerPossessCharacter;

};
