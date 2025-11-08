#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "16_Level/Area.h"

#include "ObjectiveDetectionComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogObjectiveDetection, Log, All)

#pragma region Forward Declaration
class ABaseAIController;
class APlayerController;
class UCapsuleComponent;
#pragma endregion

#pragma region Enums
UENUM(BlueprintType)
enum class ETargetState : uint8
{
    None                    UMETA(DisplayName = "None"),
    Normal                  UMETA(DisplayName = "Normal"),
    Fragile                 UMETA(DisplayName = "Fragile"),
    BackExposed             UMETA(DisplayName = "BackExposed"),
    FragileAndBackExposed   UMETA(DisplayName = "FragileAndBackExposed")
};
#pragma endregion

#pragma region Structs
USTRUCT(BlueprintType)
struct FAreaBindingInfo
{
    GENERATED_BODY()

public:
    UPROPERTY()
    TWeakObjectPtr<AArea> AreaRef;

    UPROPERTY(BlueprintReadOnly)
    FGuid AreaID;

    UPROPERTY()
    float BindTime;

    UPROPERTY()
    float LastUpdateTime;

public:
    FAreaBindingInfo()
        : BindTime(0.0f)
        , LastUpdateTime(0.0f)
    {
    }

    FAreaBindingInfo(AArea* InArea, const FGuid& InAreaID, float InTime)
        : AreaRef(InArea)
        , AreaID(InAreaID)
        , BindTime(InTime)
        , LastUpdateTime(InTime)
    {
    }

    bool operator==(const FAreaBindingInfo& Other) const
    {
        return AreaID == Other.AreaID;
    }

    bool operator!=(const FAreaBindingInfo& Other) const
    {
        return !(*this == Other);
    }

    bool IsValid() const
    {
        return AreaRef.IsValid() && AreaID.IsValid();
    }

    void UpdateTime(float NewTime)
    {
        LastUpdateTime = NewTime;
    }
};
#pragma endregion

#pragma region Delegates
DECLARE_MULTICAST_DELEGATE_TwoParams(FDetectedAIStateChanged, const FGameplayTag&, const AActor*)
DECLARE_MULTICAST_DELEGATE_OneParam(FAmbushTargetChanged, const AActor*)
DECLARE_MULTICAST_DELEGATE_OneParam(FExecutionTargetChanged, const AActor*)
DECLARE_MULTICAST_DELEGATE_TwoParams(FDetectedStructureChanged, const AActor*, bool)
DECLARE_MULTICAST_DELEGATE_TwoParams(FPlyaerBoundToArea, FAreaBindingInfo, bool);
#pragma endregion

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGEOFWOLVES_API UObjectiveDetectionComponent : public UActorComponent
{
#pragma region Friend Class
    friend class AArea;
    friend class UAT_MonitorAreaBinding;
    friend class UAsyncTask_MonitorAreaBinding;
#pragma endregion

    GENERATED_BODY()

#pragma region Default Setting
public:
    UObjectiveDetectionComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITOR
public:
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
    //@외부 바인딩
    void ExternalBindToPawnCapsuleComponent();
    void UnbindFromPawnCapsuleEvents();

    void ExternalBindToArea(AArea* Area);
    void UnbindFromAreaEvents(AArea* Area);

    void ExternalBindToLockOnComponent();

protected:
    UFUNCTION()
    void InitializeODComponent();
#pragma endregion

#pragma region Property or Subwidgets or Infos...etc
private:
    FGuid ComponentID;

protected:
    void UpdateControlledPawn(APawn* NewPawn);
    void CleanupInvalidReferences();

protected:
    void UpdateAIBackExposureState();
    void UpdateExecutionTargetState();
    void UpdateDetectionStructure();

protected:
    UPROPERTY()
    TArray<FAreaBindingInfo> BoundAreas;

protected:
    UPROPERTY()
    TWeakObjectPtr<AActor> CurrentTargetAI;

    UPROPERTY(EditAnywhere, Category = "Objective Detection")
    TArray<FGameplayTag> StateTagsToDetect;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective Detection")
    bool bOnlyDetectInCameraView = false;

    UPROPERTY(EditAnywhere, Category = "Objective Detection", meta = (EditCondition = "bOnlyDetectInCameraView"))
    float FOVExpansion = 10.0f;

    FTimerHandle CleanupTimerHandle;
    float CleanupInterval = 10.0f;

    bool bIsCurrentTargetFragile = false;

protected:
    UPROPERTY()
    TWeakObjectPtr<AActor> AmbushTarget;

    UPROPERTY()
    TWeakObjectPtr<AActor> ExecutionTarget;

    UPROPERTY(EditAnywhere, Category = "Objective Detection|Advanced")
    float BackExposureCheckInterval = 0.1f;

    UPROPERTY(EditAnywhere, Category = "Objective Detection|Advanced")
    float ExecutionCheckInterval = 0.1f;

    UPROPERTY(EditAnywhere, Category = "Objective Detection|Advanced", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float ExecutionAngleThreshold = 0.9f;

    float LastBackExposureCheckTime = 0.0f;
    float LastExecutionCheckTime = 0.0f;

protected:
    UPROPERTY()
    TWeakObjectPtr<AActor> DetectedStructureActor;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective Detection|Structure")
    float DetectionStructureDistance = 500.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective Detection|Structure", meta = (ClampMin = "0.0", ClampMax = "180.0"))
    float DetectionStructureTotalAngleDegrees = 60.f;

    UPROPERTY(EditAnywhere, Category = "Objective Detection|Structure")
    float ExecutionStructureCheckInterval = 0.1f;

    float LastExecutionStructureCheckTime = 0.0f;

private:
    FVector CachedPawnLocation;
    bool bPawnLocationCached = false;

    TMap<TWeakObjectPtr<AActor>, bool> CachedViewResults;
    int32 LastViewCacheFrame = -1;

    TMap<FGuid, TArray<FAreaAIInfo>> CachedAreaAIs;
#pragma endregion

#pragma region Delegates
public:
    FDetectedAIStateChanged DetectedAIStateChanged;
    FAmbushTargetChanged AmbushTargetChanged;
    FExecutionTargetChanged ExecutionTargetChanged;
    FDetectedStructureChanged DetectedStructureChanged;
    FPlyaerBoundToArea PlyaerBoundToArea;

    DECLARE_MULTICAST_DELEGATE_OneParam(FCurrentTargetChanged, const AActor*);
    FCurrentTargetChanged CurrentTargetChanged;
#pragma endregion

#pragma region Callbacks
protected:
    UFUNCTION()
    void OnPawnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnPawnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
    UFUNCTION()
    void OnLockOnStateChanged(bool bIsLockOn, AActor* TargetActor = nullptr);

protected:
    UFUNCTION()
    void OnAreaObjectiveStateChanged(AActor* ObjectiveActor, const FGameplayTag& StateTag, AArea* SourceArea, const FGuid& AreaID);

protected:
    UFUNCTION()
    void OnDetectedByAI(bool bIsDetected, AActor* AI, APlayerCharacter* DetectedPlayer);
#pragma endregion

#pragma region Utility
protected:
    UPROPERTY()
    TWeakObjectPtr<APawn> ControlledPawn;

    UPROPERTY()
    TSet<TWeakObjectPtr<AActor>> AIsDetectingPawn;

protected:
    bool IsOwnerAIController() const;
    bool IsOwnerPlayerController() const;

protected:
    APawn* GetControlledPawn() const;
    FVector GetPawnLocation();
    UCapsuleComponent* GetPawnCapsuleComponent() const;
    UCameraComponent* GetPlayerCameraComponent() const;

protected:
    bool MeetsMinimumDetectionConditions(AActor* AIActor, const FGameplayTag& StateTag) const;
    bool IsInCameraView(AActor* Actor);
    bool IsActorFrontExposed(AActor* Actor) const;

protected:
    AArea* FindAreaByGuid(const FGuid& AreaGuid) const;

    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
    TArray<FAreaBindingInfo> GetBoundAreas() const;

    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
    bool IsAreaBound(const FGuid& AreaID) const;

protected:
    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
    FGuid GetComponentID() const;

protected:
    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
    bool IsActorBackExposed(AActor* Actor) const;

    AActor* GetAmbushTarget() const
    {
        return AmbushTarget.IsValid() ? AmbushTarget.Get() : nullptr;
    }

    AActor* GetExecutionTarget() const
    {
        return ExecutionTarget.IsValid() ? ExecutionTarget.Get() : nullptr;
    }

protected:
    void SetCurrentTargetAI(AActor* NewTargetActor);
    AActor* GetCurrentTargetAI() const;
#pragma endregion

};