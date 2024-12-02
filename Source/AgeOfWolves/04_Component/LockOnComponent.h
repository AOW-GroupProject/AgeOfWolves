// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//@#include
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LockOnComponent.generated.h"

//@UE_LOGFMT 활용을 위한 로그 매크로 선언
DECLARE_LOG_CATEGORY_EXTERN(LogLockOn, Log, All)

//@전방 선언
#pragma region Forward Declaration
class APlayerCharacter;
class UBaseAnimInstance;
class USpringArmComponent;
class UCameraComponent;
class UBaseInputComponent;
struct FInputActionValue;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGEOFWOLVES_API ULockOnComponent : public UActorComponent
{
    //@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

        //@Defualt Setting
#pragma region Default Setting
public:
    ULockOnComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    void Input_LockOn();
    void StartLockOn();
    void CancelLockOn();
    void Input_ChangeLockOnTarget(const FInputActionValue& Value);
    void SetControllerRotationTowardTarget();
    void AdjustCameraTransform(float DeltaTime);
    bool FindTargetEnemy();

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lock On")
        float MaxDetectRadius = 1000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lock On")
        float MaxLockOnDistance = 2000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Lock On")
        float InterpolationSpeed = 40.f;

    UPROPERTY()
        TArray<AActor*> NearByEnemies;

    UPROPERTY()
        TMap<float, AActor*> EnemyMap;

    UPROPERTY()
        AActor* TargetEnemy;

    bool bLockOn = false;
    FVector2D InputVector;
#pragma endregion

    //@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
    UPROPERTY()
        TWeakObjectPtr<APlayerCharacter> PlayerCharacterRef;

    UPROPERTY()
        TWeakObjectPtr<UBaseAnimInstance> BaseAnimInstanceRef;

    UPROPERTY()
        TWeakObjectPtr<USpringArmComponent> SpringArmComponentRef;

    UPROPERTY()
        TWeakObjectPtr<UCameraComponent> FollowCameraComponentRef;

    UPROPERTY()
        TWeakObjectPtr<UBaseInputComponent> BaseInputComponentRef;

public:
    FORCEINLINE bool GetbLockOn() const { return bLockOn; }
#pragma endregion
};