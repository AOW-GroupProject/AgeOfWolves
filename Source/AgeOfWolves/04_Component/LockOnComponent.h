#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

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
class UBillboardComponent;
class AController;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_MULTICAST_DELEGATE_OneParam(FLockOnStateChanged, bool)
#pragma endregion

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGEOFWOLVES_API ULockOnComponent : public UActorComponent
{
 //@친추 클래스
#pragma region Friend Class
    friend class UBaseInputComponent;
    friend class APlayerCharacter;
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    ULockOnComponent();
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    //@내부 바인딩

protected:
    //@외부 바인딩
    void ExternalBindToInputComp(const AController* Controller);
    void ExternalBindToASCComp();

protected:
    //@초기화
    UFUNCTION()
        void InitializeLockOnComp(const AController* Controller);
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    void StartLockOn();
    void CancelLockOn();

protected:
    bool FindTargetEnemy();

protected:
    //@현재 타겟의 상태 변화 이벤트 바인딩/언바인딩
    void BindCurrentTargetStateEvents();
    void UnbindCurrentTargetStateEvents();

private:
    void UpdateSpringArmSettings(bool bIsLockingOn);

protected:
    void UpdateControllerRotation(float DeltaTime);
    void UpdateSpringArmTransform(float DeltaTime, const FVector& Target, const FRotator& TargetRotation);

private:
    void UpdateBillboardComponent(bool bVisible, bool bChangeTransformOnly = false);

protected:
    UPROPERTY(BlueprintReadWrite, Category = "Lock On")
        FRotator FinalRotation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lock On")
        float MaxDetectRadius = 1000.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Lock On")
        float MaxLockOnDistance = 2000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Lock On")
        float InterpolationSpeed = 10.f;

protected:
    bool bLockOn;

protected:
    //@주위에 있는 적들 목록
    UPROPERTY()
        TArray<AActor*> NearByEnemies;

    //@거리, 적 정보 저장한 TMap
    UPROPERTY()
        TMap<float, AActor*> EnemyMap;

    //@Target으로 설정한 적의 약한 참조
    UPROPERTY()
        TWeakObjectPtr<AActor> TargetEnemyRef;

protected:
    UPROPERTY()
        UBillboardComponent* LockOnBillboardComponent;

    UPROPERTY(EditDefaultsOnly, Category = "락온 | 효과")
        TSoftObjectPtr<UTexture2D> LockOnTexture;

    UPROPERTY(EditDefaultsOnly, Category = "락온 | 효과")
        float BillboardForwardOffset = 100.0f;

    UPROPERTY(EditDefaultsOnly, Category = "락온 | 효과")
        float TextureScale = 0.08f;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@Lock On 상태 이벤트
    FLockOnStateChanged LockOnStateChanged;
#pragma endregion

//@Callbacks
#pragma region Callbacks
public:
    UFUNCTION()
        void OnLockOnTargetChanged(const FGameplayTag& InputTag, const float Value);

protected:
    //@Owner 캐릭터의 상태 변화 콜백
    UFUNCTION()
        void OnOwnerStateChanged(const FGameplayTag& StateTag);

protected:
    //@주변 적들 목록에 추가된 적들의 상태 변화 이벤트를 구독하는 콜백
    UFUNCTION()
        void OnTargetStateChanged(const FGameplayTag& StateTag);
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
    UFUNCTION(BlueprintCallable)
        FORCEINLINE bool GetbLockOn() const { return bLockOn; }
    UFUNCTION(BlueprintCallable)
        FORCEINLINE AActor* GetTargetEnemy() const { return TargetEnemyRef.Get(); }
    FORCEINLINE FRotator GetFinalRotation() const
    {
        return FinalRotation;
    }
#pragma endregion
};