#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "BasePlayerController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBasePC, Log, All)

//@전방 선언
#pragma region Forward Declaration
class APawn;
class UUIComponent;
class UBaseInputComponent;
class UObjectiveDetectionComponent;
class UInteractionComponent;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
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
//@친추 클래스
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
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
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
    //@초기화
    void InitializePlayerController();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    void SetupInputModeOnBeginPlay();
    void SetupViewportClientOnBeginPlay();

private:
    //@UI 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UUIComponent* UIComponent;
    
    //@입력 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UBaseInputComponent* BaseInputComponent;

    //@주변 목표물 인식 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UObjectiveDetectionComponent* ODComponent;

    //@상호 작용 컴포넌트
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        UInteractionComponent* InteractComponent;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 요청 이벤트(비동기 초기화 작업)
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
    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
        UObjectiveDetectionComponent* GetODComponent() const;
    UFUNCTION(BlueprintCallable, Category = "Interaction")
        UInteractionComponent* GetInteractionComponent() const;
#pragma endregion

};