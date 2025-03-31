#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "16_Level/Area.h"

#include "ObjectiveDetectionComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogObjectiveDetection, Log, All)

//@전방 선언
#pragma region Forward Declaration
class ABaseAIController;
class APlayerController;
class UCapsuleComponent;
class UBillboardComponent;
class UTexture2D; 
#pragma endregion

//@열거형
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

//@구조체
#pragma region Structs
/*
*   @FAreaBindingInfo
*
*   바인딩된 Area 정보를 담는 구조체
*/
USTRUCT(BlueprintType)
struct FAreaBindingInfo
{
    GENERATED_BODY()

public:
    //@Area 약한 참조
    UPROPERTY()
        TWeakObjectPtr<AArea> AreaRef;

    //@Area ID
    UPROPERTY(BlueprintReadOnly)
        FGuid AreaID;

    //@바인딩 시간
    UPROPERTY()
        float BindTime;

    //@마지막 업데이트 시간
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

    // == 연산자 오버로딩 (FGuid 기준)
    bool operator==(const FAreaBindingInfo& Other) const
    {
        return AreaID == Other.AreaID;
    }

    // != 연산자 오버로딩
    bool operator!=(const FAreaBindingInfo& Other) const
    {
        return !(*this == Other);
    }

    //@Area 유효성 확인
    bool IsValid() const
    {
        return AreaRef.IsValid() && AreaID.IsValid();
    }

    //@업데이트 시간 갱신
    void UpdateTime(float NewTime)
    {
        LastUpdateTime = NewTime;
    }
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@시야 안에 있는 AI의 상태 변화 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FDetectedAIStateChanged, const FGameplayTag&, const AActor*)

//@시야 안에 있는 AI 중 매복 암살 가능 타겟 변경 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FAmbushTargetChanged, const AActor*)
#pragma endregion

/*
*   @UObjectiveDetectionComponent
*
*   주변 목표물의 상태를 감지하고 처리하는 컴포넌트
*   PlayerController와 AIController에서 사용하며,
*   Pawn의 CapsuleComponent 오버랩 이벤트를 통해 Area를 감지합니다.
*/
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGEOFWOLVES_API UObjectiveDetectionComponent : public UActorComponent
{
//@친추 클래스
#pragma region Friend Class
    friend class AArea;
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UObjectiveDetectionComponent();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
    //@내부 바인딩

protected:
    //@외부 바인딩
    void ExternalBindToPawnCapsuleComponent();
    void UnbindFromPawnCapsuleEvents();

    void ExternalBindToArea(AArea* Area);
    void UnbindFromAreaEvents(AArea* Area);

    void ExternalBindToLockOnComponent();

protected:
    //@초기화
    UFUNCTION()
        void InitializeODComponent();
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
    //@컴포넌트 고유 ID
    FGuid ComponentID;

protected:
    //@컴포넌트 업데이트 - Pawn이 변경될 때 호출
    void UpdateControlledPawn(APawn* NewPawn);

protected:
    //@유효하지 않은 Area들 정리
    void CleanupInvalidReferences();

protected:
    //@Billboard 컴포넌트 업데이트 (위치, 회전, 가시성)
    void UpdateBillboardComponent(bool bVisible, bool bChangeTransformOnly = false);

    bool UpdateBillboardPosition(AActor* TargetActor);

    void UpdateBillboardTexture();

protected:
    //@등을 노출하고 있는 AI 정보 업데이트
    void UpdateAIBackExposureState();

protected:
    UPROPERTY()
        UBillboardComponent* IndicatorBillboardComponent;

protected:
    // LockOn 인디케이터 텍스처
    UPROPERTY(EditAnywhere, Category = "Objective Detection|Visuals")
        TSoftObjectPtr<UTexture2D> LockOnIndicator;

    // Executable(처형 가능) 인디케이터 텍스처
    UPROPERTY(EditAnywhere, Category = "Objective Detection|Visuals")
        TSoftObjectPtr<UTexture2D> ExecutableIndicator;


    //@텍스처 크기 스케일, 기본 0.05 스케일
    UPROPERTY(EditAnywhere, Category = "Objective Detection|Visuals")
        float TextureScale = 0.05f;

    //@빌보드 전방 오프셋 (타겟으로부터 얼마나 앞에 표시할지)
    UPROPERTY(EditAnywhere, Category = "Objective Detection|Visuals")
        float BillboardForwardOffset = 100.0f;

    //@빌보드 보간 속도, 기본 30
    UPROPERTY(EditAnywhere, Category = "Objective Detection|Visuals")
        float BillboardInterpolationSpeed = 30.0f;

protected:
    //@바인딩된 Area 배열
    UPROPERTY()
        TArray<FAreaBindingInfo> BoundAreas;

protected:
    //@현재 타겟 액터 참조
    UPROPERTY()
        TWeakObjectPtr<AActor> CurrentTargetAI;

    //@감지할 상태 태그 설정
    UPROPERTY(EditAnywhere, Category = "Objective Detection")
        TArray<FGameplayTag> StateTagsToDetect;

    //@카메라 시야 내 액터만 감지할지 여부 (PlayerController에서만 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Objective Detection")
        bool bOnlyDetectInCameraView = false;

    //@FOV 확장 (시야 체크 시 사용)
    UPROPERTY(EditAnywhere, Category = "Objective Detection", meta = (EditCondition = "bOnlyDetectInCameraView"))
        float FOVExpansion = 10.0f;

    //@정리 타이머
    FTimerHandle CleanupTimerHandle;

    //@정리 주기 (초)
    float CleanupInterval = 10.0f;

    //@취약 여부
    bool bIsCurrentTargetFragile = false;

protected:
    //@잠재적 매복 타겟 (후면 노출된 AI)
    UPROPERTY()
            TWeakObjectPtr<AActor> AmbushTarget;

    //@후면 노출 상태 체크 간격 (seconds)
    UPROPERTY(EditAnywhere, Category = "Objective Detection|Advanced")
        float BackExposureCheckInterval = 0.1f;

    //@마지막 체크 시간
    float LastBackExposureCheckTime = 0.0f;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@목표 AI의 상태 변화 이벤트
    FDetectedAIStateChanged DetectedAIStateChanged;

public:
    //@매복 암살 가능한 AI 타겟 변경 이벤트
    FAmbushTargetChanged AmbushTargetChanged;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@Pawn의 CapsuleComponent 오버랩 이벤트 콜백
    UFUNCTION()
        void OnPawnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult& SweepResult);

    //@Pawn의 CapsuleComponent 오버랩 종료 이벤트를 구독하는 콜백
    UFUNCTION()
        void OnPawnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
    UFUNCTION()
        void OnLockOnStateChanged(bool bIsLockOn, AActor* TargetActor = nullptr);

protected:
    //@목표물 상태 변경 통지 (Area에서 호출)
    UFUNCTION()
        void OnAreaObjectiveStateChanged(AActor* ObjectiveActor, const FGameplayTag& StateTag, AArea* SourceArea, const FGuid& AreaID);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
    //@소유 컨트롤러의 폰에 대한 약한 참조
    UPROPERTY()
        TWeakObjectPtr<APawn> ControlledPawn;

protected:
    //@AIController 소유 여부 확인
    bool IsOwnerAIController() const;
    
    //@PlayerController 소유 여부 확인
    bool IsOwnerPlayerController() const;

protected:
    //@컨트롤러로부터 Pawn 가져오기
    APawn* GetControlledPawn() const;
    
protected:
    //@Pawn의 위치 가져오기
    FVector GetPawnLocation() const;

    //@Pawn의 CapsuleComponent 가져오기
    UCapsuleComponent* GetPawnCapsuleComponent() const;

    //@카메라 컴포넌트 가져오기 (플레이어용)
    UCameraComponent* GetPlayerCameraComponent() const;

protected:
    //@Camera View 안에 목표물이 존재하는지 체크합니다.
    bool IsActorInCameraView(AActor* Actor) const;

protected:
    //@Area 찾기 (GUID 기준)
    AArea* FindAreaByGuid(const FGuid& AreaGuid) const;
    
    //@바인딩된 모든 Area 가져오기
    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
        TArray<FAreaBindingInfo> GetBoundAreas() const;

    //@Area 바인딩 여부 확인
    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
        bool IsAreaBound(const FGuid& AreaID) const;

protected:
    //@컴포넌트 고유 ID 가져오기
    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
        FGuid GetComponentID() const;

protected:
    //@액터의 후면 노출 상태 확인
    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
        bool IsActorBackExposed(AActor* Actor) const;

    //@현재 시야 내 후면 노출된 AI 목록 가져오기
    AActor* UObjectiveDetectionComponent::GetAmbushTarget() const
    {
        return AmbushTarget.IsValid() ? AmbushTarget.Get() : nullptr;
    }

protected:
    AActor* DetermineTargetActor();

    //@현재 타겟 액터 설정
    void SetCurrentTargetAI(AActor* NewTargetActor);

    //@현재 타겟 액터 가져오기
    AActor* GetCurrentTargetAI() const;

protected:
    // 인디케이터 텍스처 변경
    void SetIndicatorTexture(UTexture2D* NewTexture);
#pragma endregion

};