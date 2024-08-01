// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBasePC, Log, All)

/*
    @���� : Controller�� Character�� �����ϴ� ������ �˸��� �̺�Ʈ
    @���� : Controller -> PS ������ �ʱ�ȭ ������ �����ϱ� ����, Controller�� Character ���� ������ PS���� �˸��ϴ�.
*/
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByPC);

class APawn;
class UUIComponent;
class UBaseInputComponent;

/**
 *
 */
UCLASS()
class AGEOFWOLVES_API ABasePlayerController : public APlayerController
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    ABasePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    //~AActor Interface
    virtual void PreInitializeComponents() override;
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override; // Load
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of AActor

    //~ACommonPlayerController Interface
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    //~End of ACommonPlayerController
protected:
    //~APlayerController interface
    virtual void AcknowledgePossession(class APawn* P) override; // Binding
    virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
    virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
    //~End of APlayerController
private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UUIComponent* UIComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UBaseInputComponent* BaseInputComponent;
public:
    UFUNCTION(BlueprintCallable, Category = "UI")
        UUIComponent* GetUIComponent() const { return UIComponent; }
    UFUNCTION(BlueprintCallable, Category = "Input")
        UBaseInputComponent* GetBaseInputComponent() const;
#pragma endregion

#pragma region Delegates
public:
    FRequestStartInitByPC RequestStartInitByPC;
#pragma endregion

};