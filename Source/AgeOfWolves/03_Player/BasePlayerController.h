// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

/*
	@���� : Controller�� Character�� �����ϴ� ������ �˸��� �̺�Ʈ
	@���� : Controller -> PS ������ �ʱ�ȭ ������ �����ϱ� ����, Controller�� Character ���� ������ PS���� �˸��ϴ�.
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

	//~AActor �������̽�
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor �������̽�

	//~ACommonPlayerController �������̽�
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	//~End of ACommonPlayerController interface

protected:
	//~APlayerController interface
	virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
	// @���� : PostProcessInput �Լ��� ����� �Է� ���� ���� �� �����Ӹ��� ȣ��Ǵ� �Լ���, ������� Ability Input Action ó�� �Լ��� ȣ��Ǵ� ���Դϴ�.
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

public:
	FControllerPossessCharacter OnControllerPossessCharacter;

};
