// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "BasePlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBasePC, Log, All)

#pragma region Forward Declaration
class APawn;
class UUIComponent;
class UBaseInputComponent;
#pragma endregion

#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByPC)
#pragma endregion

/**
 * @ABasePlayerController
 * 
 * 사용자 컨트롤러를 설정합니다.
 */
UCLASS()
class AGEOFWOLVES_API ABasePlayerController : public APlayerController
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    ABasePlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
    //~AActor Interface
    virtual void PreInitializeComponents() override;
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override; // Load
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of AActor

protected:
    //~ACommonPlayerController Interface
    virtual void OnPossess(APawn* InPawn) override;
    virtual void OnUnPossess() override;
    //~End of ACommonPlayerController

protected:
    //~APlayerController interface
    virtual void AcknowledgePossession(class APawn* P) override;
    virtual void PreProcessInput(const float DeltaTime, const bool bGamePaused) override;
    virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
    //~End of APlayerController

protected:
    void InitializePlayerController();


#pragma endregion

#pragma region Properties
protected:
    void SetupInputModeOnBeginPlay();
    void SetupViewportClientOnBeginPlay();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UUIComponent* UIComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UBaseInputComponent* BaseInputComponent;
#pragma endregion

#pragma region Utility
public:
    UFUNCTION(BlueprintCallable, Category = "UI")
        UUIComponent* GetUIComponent() const;
    UFUNCTION(BlueprintCallable, Category = "Input")
        UBaseInputComponent* GetBaseInputComponent() const;
#pragma endregion

#pragma region Delegates
public:
    //@초기화 요청 이벤트(비동기 초기화 작업)
    FRequestStartInitByPC RequestStartInitByPC;
#pragma endregion

};