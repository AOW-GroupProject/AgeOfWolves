#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "16_Level/Area.h"

#include "ObjectiveDetectionComponent.generated.h"

//@UE_LOGFMT 활용을 위한 로그 매크로 선언
DECLARE_LOG_CATEGORY_EXTERN(LogObjectiveDetection, Log, All)

//@전방 선언
#pragma region Forward Declaration
class ABaseAIController;
class APlayerController;
class UCapsuleComponent;
#pragma endregion

//@열거형
#pragma region Enums
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

    // Area 유효성 확인
    bool IsValid() const
    {
        return AreaRef.IsValid() && AreaID.IsValid();
    }

    // 업데이트 시간 갱신
    void UpdateTime(float NewTime)
    {
        LastUpdateTime = NewTime;
    }
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
// 목표물 감지 관련 델리게이트 정의
// (구현부는 별도 작성)
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

protected:
    //@내부 바인딩

protected:
    //@외부 바인딩
    void ExternalBindToPawnCapsuleComponent();
    void UnbindFromPawnCapsuleEvents();

    void ExternalBindToArea(AArea* Area);
    void UnbindFromAreaEvents(AArea* Area);

protected:
    //@초기화
    UFUNCTION()
        void InitializeODComponent();
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    //@컴포넌트 업데이트 - Pawn이 변경될 때 호출
    void UpdateControlledPawn(APawn* NewPawn);

protected:
    //@유효하지 않은 Area들 정리
    void CleanupInvalidReferences();

protected:
    //@바인딩된 Area 배열
    UPROPERTY()
        TArray<FAreaBindingInfo> BoundAreas;

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
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@Pawn의 CapsuleComponent 오버랩 이벤트 콜백
    UFUNCTION()
        void OnPawnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
        void OnPawnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

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

    //@컨트롤러로부터 Pawn 가져오기
    APawn* GetControlledPawn() const;
    //@Pawn의 위치 가져오기
    FVector GetPawnLocation() const;
    //@Pawn의 CapsuleComponent 가져오기
    UCapsuleComponent* GetPawnCapsuleComponent() const;

    //@Camera View 안에 목표물이 존재하는지 체크합니다.
    bool IsActorInCameraView(AActor* Actor) const;

    //@Area 찾기 (GUID 기준)
    AArea* FindAreaByGuid(const FGuid& AreaGuid) const;

public:
    //@바인딩된 모든 Area 가져오기
    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
        TArray<FAreaBindingInfo> GetBoundAreas() const;

    //@Area 바인딩 여부 확인
    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
        bool IsAreaBound(const FGuid& AreaID) const;

    //@컴포넌트 고유 ID 가져오기
    UFUNCTION(BlueprintCallable, Category = "Objective Detection")
        FGuid GetComponentID() const;

private:
    // 컴포넌트 고유 ID
    FGuid ComponentID;
#pragma endregion
};