#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"

#include "InteractionComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteraction, Log, All);

//@전방 선언
#pragma region Forward Declaration
class UAnimMontage;
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
    Execution   UMETA(DisplayName = "처형"),
    Ambush      UMETA(DisplayName = "암살"),
    ItemPickup  UMETA(DisplayName = "아이템 줍기"),
    Dialogue    UMETA(DisplayName = "대화"),
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

    //@상호작용 시 애니메이션 재생 여부
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Animation")
        bool bPlayMontage;

    //@몽타주가 페어를 이루는지 여부(타겟과 플레이어 동기화)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Animation", meta = (EditCondition = "bPlayMontage", InlineEditConditionToggle))
        bool bIsMontagesPaired;

    //@상호작용 시 플레이어가 재생할 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Animation", meta = (EditCondition = "bPlayMontage"))
        TSoftObjectPtr<UAnimMontage> PlayerMontage;

    //@상호작용 시 타겟이 재생할 애니메이션 몽타주
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction|Animation", meta = (EditCondition = "bIsMontagesPaired"))
        TSoftObjectPtr<UAnimMontage> TargetMontage;

    FPotentialInteraction()
        : InteractionType(EInteractionType::None)
        , RequiredDistance(200.0f)
        , bAdditionalConditionsMet(false)
        , Priority(0)
        , bPlayMontage(false)
        , bIsMontagesPaired(false)
    {}

    FPotentialInteraction(FGameplayTag InObjectTag, EInteractionType InType, FGameplayTag InEventTag, float InDistance = 200.0f, int32 InPriority = 0)
        : ObjectTag(InObjectTag)
        , InteractionType(InType)
        , EventTag(InEventTag)
        , RequiredDistance(InDistance)
        , bAdditionalConditionsMet(false)
        , Priority(InPriority)
        , bPlayMontage(false)
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

    //@애니메이션 몽타주가 유효한지 확인
    bool HasValidMontages() const
    {
        return bPlayMontage && PlayerMontage.IsValid() && TargetMontage.IsValid();
    }

};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
// 상호작용 이벤트 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPotentialInteractionChanged, AActor*, TargetActor, const FPotentialInteraction&, Interaction);
#pragma endregion

/**
 *  @UInteractionData
 *
 *  FPotentialInteraction을 감싸는 UObject 래퍼 클래스
 *  GameplayEvent를 통해 FPotentialInteraction 데이터를 전달하기 위해 사용
 */
UCLASS(BlueprintType)
class AGEOFWOLVES_API UInteractionData : public UObject
{
    GENERATED_BODY()

public:
    // 상호작용 데이터
    UPROPERTY(BlueprintReadOnly, Category = "Interaction")
        FPotentialInteraction InteractionData;

    // 초기화 함수
    void SetInteractionData(const FPotentialInteraction& InInteractionData)
    {
        InteractionData = InInteractionData;
    }

    // Blueprint 접근용 함수들
    UFUNCTION(BlueprintPure, Category = "Interaction")
        EInteractionType GetInteractionType() const
    {
        return InteractionData.InteractionType;
    }

    UFUNCTION(BlueprintPure, Category = "Interaction")
        FGameplayTag GetObjectTag() const
    {
        return InteractionData.ObjectTag;
    }

    UFUNCTION(BlueprintPure, Category = "Interaction")
        FGameplayTag GetEventTag() const
    {
        return InteractionData.EventTag;
    }

    UFUNCTION(BlueprintPure, Category = "Interaction")
        bool HasMontages() const
    {
        return InteractionData.bPlayMontage &&
            (InteractionData.bIsMontagesPaired ?
                InteractionData.PlayerMontage.IsValid() && InteractionData.TargetMontage.IsValid() :
                InteractionData.PlayerMontage.IsValid());
    }

    UFUNCTION(BlueprintPure, Category = "Interaction")
        TSoftObjectPtr<UAnimMontage> GetPlayerMontage() const
    {
        return InteractionData.PlayerMontage;
    }

    UFUNCTION(BlueprintPure, Category = "Interaction")
        TSoftObjectPtr<UAnimMontage> GetTargetMontage() const
    {
        return InteractionData.TargetMontage;
    }

    UFUNCTION(BlueprintPure, Category = "Interaction")
        bool IsMontagesPaired() const
    {
        return InteractionData.bIsMontagesPaired;
    }
};

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
    void CommitInteraction();

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

protected:
    //@잠재적 암살 타겟 변경 이벤트 구독
    UFUNCTION()
        void OnExecutionTargetChanged(AActor* PotentialExecutionTarget);

    //@잠재적 암살 타겟 변경 이벤트 구독
    UFUNCTION()
        void OnAmbushTargetChanged(AActor* PotentialAmbushTarget);
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
