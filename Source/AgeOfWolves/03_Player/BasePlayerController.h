// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "BasePlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBasePC, Log, All)

//@���� ����
#pragma region Forward Declaration
class APawn;
class UUIComponent;
class UBaseInputComponent;
#pragma endregion

//@������
#pragma region Enums
#pragma endregion

//@����ü
#pragma region Structs
#pragma endregion

//@�̺�Ʈ/��������Ʈ
#pragma region Delegates
//@�ʱ�ȭ ��û �̺�Ʈ
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByPC)
#pragma endregion

/**
 * @ABasePlayerController
 * 
 * ����� ��Ʈ�ѷ��� �����մϴ�.
 */
UCLASS()
class AGEOFWOLVES_API ABasePlayerController : public APlayerController
{
    //@ģ�� Ŭ����
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
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

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    void SetupInputModeOnBeginPlay();
    void SetupViewportClientOnBeginPlay();

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UUIComponent* UIComponent;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UBaseInputComponent* BaseInputComponent;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@�ʱ�ȭ ��û �̺�Ʈ(�񵿱� �ʱ�ȭ �۾�)
    FRequestStartInitByPC RequestStartInitByPC;
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    UFUNCTION(BlueprintCallable, Category = "UI")
        UUIComponent* GetUIComponent() const;
    UFUNCTION(BlueprintCallable, Category = "Input")
        UBaseInputComponent* GetBaseInputComponent() const;
#pragma endregion

};