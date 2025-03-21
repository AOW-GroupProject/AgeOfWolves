#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "InteractionComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteraction, Log, All);

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
/*
*   @EInteractionType
* 
*   상호작용 타입을 정의하는 열거형
*/
UENUM(BlueprintType)
enum class EInteractionType : uint8
{
    None,
    Execution,    
    ItemPickup,   
    Dialogue,     
    UseObject,    
    MAX,        
};
#pragma endregion

//@구조체
#pragma region Structs
/*
*   @FPotentialInteraction
* 
*   상호작용 가능한 잠재 이벤트를 구조체로 정의합니다.
*/
USTRUCT(BlueprintType)
struct FPotentialInteraction
{
    GENERATED_BODY()

    //@상호작용 대상
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
        FGameplayTag ObjectTag;

    //@상호작용 유형
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
        EInteractionType InteractionType;

    //@상호작용에 요구되는 입력
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
        FGameplayTag InputTag;

    //@상호작용 활성화 시 발생할 이벤트 태그
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
        FGameplayTag EventTag;

    //@상호작용 거리 요구사항
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
        float RequiredDistance;

    //@상호작용 추가 조건 만족 여부
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
        bool bAdditionalConditionsMet;

    //@상호작용 우선순위 (높을수록 우선)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
        int32 Priority;

    FPotentialInteraction()
        : InteractionType(EInteractionType::None)
        , RequiredDistance(200.0f)
        , bAdditionalConditionsMet(false)
        , Priority(0)
    {}

    FPotentialInteraction(FGameplayTag InObjectTag, EInteractionType InType, FGameplayTag InEventTag, float InDistance = 200.0f, int32 InPriority = 0)
        : ObjectTag(ObjectTag)
        , InteractionType(InType)
        , EventTag(InEventTag)
        , RequiredDistance(InDistance)
        , bAdditionalConditionsMet(false)
        , Priority(InPriority)
    {}

    //@오버로딩
    bool operator==(const FPotentialInteraction& Other) const
    {
        return ObjectTag.IsValid() && Other.ObjectTag.IsValid() &&
            ObjectTag == Other.ObjectTag;
    }

    //@연산자 오버로딩
    bool operator!=(const FPotentialInteraction& Other) const
    {
        return !(*this == Other);
    }
    
    //@우선순위 비교 연산자
    bool operator<(const FPotentialInteraction& Other) const
    {
        return Priority < Other.Priority;
    }

    //@상호작용 완전 가능 여부 (거리 + 추가 조건)
    bool IsFullyAvailable() const
    {
        return ObjectTag.IsValid() && bAdditionalConditionsMet;
    }

};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
// 상호작용 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPotentialInteractionChanged, AActor*, TargetActor, const FPotentialInteraction&, Interaction);
#pragma endregion

/**
 *  @UInteractionComponent
 *
 *  플레이어 컨트롤러에 부착되어 모든 잠재적 상호작용을 관리하는 컴포넌트
 *  ObjectiveDetectionComponent로부터 감지 이벤트를 수신하고
 *  가능한 상호작용 목록을 필터링하여 AbilitySystemComponent에 이벤트를 전달
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API UInteractionComponent : public UActorComponent
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UInteractionComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    //@외부 바인딩
    void ExeternalBindToASComp();
    void UnbindExeternalBindToASCComp();

    void ExternalBindToODComp();
    void UnbindExternalBindToODComp();

protected:
    //@내부 바인딩

protected:
    //@초기화
    UFUNCTION()
        void InitializeInteractionComp();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    //@잠재적 상호작용 등록
    UFUNCTION(BlueprintCallable, Category = "Interaction")
        void RegisterPotentialInteraction(AActor* TargetActor);
    //@단일 유형의 잠재적 상호작용 유형 등록
    void RegisterPotentialInteraction(AActor* TargetActor, EInteractionType InteractionType);

    //@잠재적 상호작용 제거
    UFUNCTION(BlueprintCallable, Category = "Interaction")
        void RemovePotentialInteraction(AActor* TargetActor);

    //@단일 유형의 잠재적 상호작용 제거
    void RemovePotentialInteraction(AActor* TargetActor, EInteractionType InteractionType);

private:
    //@거리 기반으로 상호작용 가능 여부 업데이트
    void UpdateInteractionDistances();

protected:
    //@상호작용 활성화 이벤트 발생
    void TryActivateInteraction(AActor* TargetActor, const FPotentialInteraction& Interaction);

    //@상호작용 취소 이벤트 발생
    void CancelInteractionActivated(AActor* TargetActor, const FPotentialInteraction& Interaction);

protected:
    //@등록된 모든 잠재적 상호작용
    UPROPERTY(EditDefaultsOnly, Category = "Interaction")
        TArray<FPotentialInteraction> PotentialInteractions;

    //@액터별 상호작용 정보 캐싱용 맵
    TMap<TWeakObjectPtr<AActor>, TMap<EInteractionType, FPotentialInteraction>> MPotentialInteractions;

    //@현재 활성화된 우선순위 상호작용
    FPotentialInteraction CurrentPriorityInteraction;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@상호작용 변경 이벤트
    UPROPERTY(BlueprintAssignable, Category = "Interaction")
        FPotentialInteractionChanged PotentialInteractionChanged;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@Owner의 상태 변화 이벤트 관찰
    UFUNCTION()
        void OnOwnerStateEventOnGameplay(AActor* OwnerActor, const FGameplayTag& StateTag);

protected:
    //@상태 변경 이벤트 수신 콜백
    UFUNCTION()
        void OnDetectedAIStateChanged(const FGameplayTag& StateTag, AActor* ObjectiveActor);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
    //@Pawn 캐싱
    UPROPERTY()
        TWeakObjectPtr<APawn> OwnerPawn;

public:
    //@특정 유형의 상호작용이 가능한지 확인
    UFUNCTION(BlueprintCallable, Category = "Interaction")
        bool IsInteractionTypeAvailable(EInteractionType InteractionType) const;

public:
    //@특정 액터에 해당되는 특정 잠재적 상호작용 찾기
    FPotentialInteraction* FindInteraction(AActor* TargetActor, EInteractionType Type);

    //@특정 액터에 해당되는 모든 잠재적 상호작용 찾기
    TArray<FPotentialInteraction> FindInteraction(AActor* TargetActor);

public:
    //@현재 최우선 상호작용 반환
    UFUNCTION(BlueprintCallable, Category = "Interaction")
        FPotentialInteraction GetHighestPriorityInteraction() const;

    UFUNCTION(BlueprintCallable, Category = "Interaction")
        FPotentialInteraction GetHighestPriorityInteractionForActor(AActor* TargetActor);
#pragma endregion
};
