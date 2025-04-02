#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "01_Character/PlayerCharacter.h"

#include "Area.generated.h"

//@전방 선언
#pragma region Forward Declaration
class UBoxComponent;
class UObjectiveDetectionComponent;
class UCrowdControlComponent;
#pragma endregion

//@열거형
#pragma region Enums
UENUM(BlueprintType)
enum class EAIHierarchyType : uint8
{
    Elite       UMETA(DisplayName = "지휘관"),
    Officer     UMETA(DisplayName = "간부"),
    Regular     UMETA(DisplayName = "병사 - 근거리"),
    Support     UMETA(DisplayName = "병사 - 원거리"),
};
#pragma endregion

//@구조체
#pragma region Structs
/*
 *  @FAreaAIInfo
 *
 *  Area에 등록된 AI들의 정보를 담은 구조체
 */
USTRUCT(BlueprintType)
struct FAreaAIInfo
{
    GENERATED_BODY()

    //@AI의 약한 참조
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TWeakObjectPtr<AActor> AIActor;

    //@계층 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        EAIHierarchyType HierarchyType = EAIHierarchyType::Regular;

    //@우선순위 (동일 계층 내에서의 순위)
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 Priority = 0;

    //@현재 상태
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        FGameplayTag CurrentState;

    //@ASC 약한 참조
    UPROPERTY()
        TWeakObjectPtr<UBaseAbilitySystemComponent> AIASC;

    //@소속 그룹 ID
    UPROPERTY()
        FGuid GroupID;

    //@경계 레벨 (0: 일반, 1: 주의, 2: 경계, 3: 경보)
    UPROPERTY()
        int32 AlertLevel = 0;

    FAreaAIInfo()
    {
        // State.Alive로 초기화
        CurrentState = FGameplayTag::RequestGameplayTag("State.Alive");
    }

    FAreaAIInfo(AActor* InActor, const FGameplayTag& InState, UBaseAbilitySystemComponent* InASC,
        EAIHierarchyType InType = EAIHierarchyType::Regular, int32 InPriority = 0,
        const FGuid& InGroupID = FGuid())
        : AIActor(InActor)
        , HierarchyType(InType)
        , Priority(InPriority)
        , CurrentState(InState.IsValid() ? InState : FGameplayTag::RequestGameplayTag("State.Alive"))
        , AIASC(InASC)
        , GroupID(InGroupID)
        , AlertLevel(0)
    {}

    //@경계 레벨 설정 메서드
    void SetAlertLevel(int32 NewAlertLevel)
    {
        AlertLevel = FMath::Clamp(NewAlertLevel, 0, 3);
    }
};

/*
*   @FAIGroupInfo
*
*   AI 그룹 정보를 담은 구조체
*/
USTRUCT(BlueprintType)
struct FAIGroupInfo
{
    GENERATED_BODY()

    //@그룹 ID
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FGuid GroupID = FGuid();

    //@그룹 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString GroupName;

    //@그룹 멤버 AI 목록
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<FAreaAIInfo> GroupMembers;

    //@그룹의 지휘자
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
        TWeakObjectPtr<AActor> GroupLeader;

    FAIGroupInfo() {}

    FAIGroupInfo(FGuid InGroupID, const FString& InName)
        : GroupID(InGroupID)
        , GroupName(InName)
    {}
};

/*
*   @FPlayerBindingInfo
*
*   Area 내 진입한 사용자와 Area 간 바인딩에 필요한 정보를 담은 구조체
*/
USTRUCT()
struct FPlayerBindingInfo
{
    GENERATED_BODY()

        //@Player Characetr 약한 참조
        UPROPERTY()
        TWeakObjectPtr<APlayerCharacter> PlayerCharacter;

    //@마지막 진입/이탈 시간
    float LastExitTime = 0.0f;

    //@바인딩 해제 지연 시간 (초)
    static constexpr float UnbindDelay = 5.0f;

    FPlayerBindingInfo() {}

    FPlayerBindingInfo(APlayerCharacter* InPlayer)
        : PlayerCharacter(InPlayer)
        , LastExitTime(0.0f)
    {}

    //@지연 해제 필요 여부 확인
    bool ShouldDelayUnbind(float CurrentTime) const
    {
        return (CurrentTime - LastExitTime) < UnbindDelay;
    }

};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FRequestStartInitByArea, TArray<FAIGroupInfo>);

//@AI 상태 변경 델리게이트
DECLARE_MULTICAST_DELEGATE_FourParams(FAreaAIStateChanged, AActor*, const FGameplayTag&, AActor*, const FGuid&);

//@AI의 Player 인지 이벤트
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAIDetectsPlayer, bool, AActor*, APlayerCharacter*);

//@그룹 상태 변경 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FGroupStatusChanged, const FGuid&, const FString&);
#pragma endregion

/*
*   @AArea
*
*   Level 공간 분할 단위로 특정 지역을 담당하며 해당 지역과 관련된 정보를 관리하는 객체
*/
UCLASS(Blueprintable)
class AGEOFWOLVES_API AArea : public AActor
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    AArea();

protected:
    //~AActor Interface
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void PreInitializeComponents() override;
    virtual void PostInitializeComponents() override;
    //~End Of AActor Interface

protected:
    //@내부 바인딩
    void InternalBindToAI(TWeakObjectPtr<AActor> AIActorPtr);
    void UnbindFromAI(TWeakObjectPtr<AActor> AIActorPtr);
    void UnbindFromAllAI();

    void InternalBindToPlayer(TWeakObjectPtr<APlayerCharacter> Player);
    void UnbindFromPlayer(TWeakObjectPtr<APlayerCharacter> Player);
    void UnbindFromAllPlayer();

protected:
    //@외부 바인딩

protected:
    //@초기화
    void InitializeArea();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    void InitializeAreaAIInfos();

protected:
    //@AI 처리 함수
    void RegisterAI(AActor* AIActor);
    void UnregisterAI(AActor* AIActor);
    void UnregisterAllAI();

protected:
    //@플레이어 처리 함수
    void RegisterPlayer(APlayerCharacter* Player);
    void UnregisterPlayer(APlayerCharacter* Player);
    void UnregisterAllPlayer();

protected:
    void UpdateAIGroupLeader(const FGuid& GroupdID);

protected:
    //@영역 경계 박스
    UPROPERTY(VisibleAnywhere, Category = "Components")
        UBoxComponent* AreaBounds;

    UPROPERTY(VisibleAnywhere, Category = "Components")
        UCrowdControlComponent* CrowdControlComponent;

protected:
    //@영역 식별자
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area")
        FGuid AreaID;

    //@영역 태그 (전투 지역, 휴식 지역 등 특성)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area")
        TArray<FGameplayTag> AreaTags;

    //@영역 우선순위 (중첩 처리용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area")
        int32 AreaPriority = 0;

    //@자동으로 영역 내 AI 감지할지 여부
    UPROPERTY(EditAnywhere, Category = "Area|AI")
        bool bAutoDetectAI = true;

    //@영역 내 AI 감지 반경 (자동 감지 시)
    UPROPERTY(EditAnywhere, Category = "Area|AI", meta = (EditCondition = "bAutoDetectAI"))
        float AIDetectionRadius = 1000.0f;

protected:
    //@수동 등록할 AI 그룹
    UPROPERTY(EditAnywhere, Category = "Area | AI")
        TArray<FAIGroupInfo> RegisteredAIGroups;

    //@등록 시 그룹이 없는 AI는 자동으로 기본 그룹에 추가할지 여부
    UPROPERTY(EditAnywhere, Category = "Area | AI")
        bool bAddUnassignedAIToDefaultGroup = true;

    //@기본 그룹 ID (자동 할당용)
    UPROPERTY(EditAnywhere, Category = "Area | AI", meta = (EditCondition = "bAddUnassignedAIToDefaultGroup"))
        FGuid DefaultGroupID = FGuid();

    UPROPERTY(EditAnywhere, Category = "Area | AI", meta = (EditCondition = "bAddUnassignedAIToDefaultGroup"))
        FString DefaultGroupName = "Default";

protected:
    //@등록된 AI 그룹 Map
    UPROPERTY()
        TMap<FGuid, FAIGroupInfo> MAIGroups;

protected:
    //@영역 내 플레이어 정보
    UPROPERTY()
        TMap<TWeakObjectPtr<APlayerCharacter>, FPlayerBindingInfo> MPlayerBindings;

protected:
    //@정리 타이머
    FTimerHandle CleanupTimerHandle;

    //@마지막 정리 시간
    float LastCleanupTime;

    //@정리 주기 (초)
    UPROPERTY(EditAnywhere, Category = "Area|Advanced")
        float CleanupInterval = 60.0f;

protected:
    //@AI 상태 변경 처리
    UFUNCTION()
        void HandleAIStateChanged(AActor* AIActor, const FGameplayTag& StateTag);

protected:
    //@자원 정리
    void CleanupInvalidReferences();
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByArea RequestStartInitByArea;

public:
    //2AI 상태 변경 델리게이트
    FAreaAIStateChanged AreaAIStateChanged;

public:
    //@AI의 Player 인지 이벤트
    FAIDetectsPlayer AIDetectsPlayer;

public:
    //@그룹 상태 변화 이벤트
    FGroupStatusChanged GroupStatusChanged;
#pragma endregion

//@Callbacks
#pragma region Callbacks
public:
    //@충돌 시작 이벤트를 구독하는 콜백
    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult& SweepResult);
    //@충돌 종료 이벤트를 구독하는 콜백
    UFUNCTION()
        void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
    //@AI ASC 이벤트 콜백
    UFUNCTION()
        void OnAICharacterStateEvent(AActor* Actor, const FGameplayTag& StateTag);

    //@Player Character 상태 변화 이벤트 구독
    UFUNCTION()
        void OnPlayerStateEvent(AActor* Actor, const FGameplayTag& StateTag);

protected:
    //@AI의 락온 상태 변화 이벤트 구독
    UFUNCTION()
        void OnAIDetectsTarget(bool bLockOn, AActor* AI, AActor* DetectedTarget);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    //@영역 ID 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
        FGuid GetAreaID() const { return AreaID; }

    //@영역 태그 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
        const TArray<FGameplayTag>& GetAreaTags() const { return AreaTags; }

    //@영역 우선순위 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
        int32 GetAreaPriority() const { return AreaPriority; }

public:
    //@영역 내 모든 AI 정보 목록 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
        TArray<FAreaAIInfo> GetAreaAIInfos() const;

public:
    //@해당 AI가 속한 그룹 ID 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
        FGuid GetAIGroupID(AActor* AIActor) const;

    //@영역 내 모든 AI 그룹 정보를 배열로 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
        TArray<FAIGroupInfo> GetAllAIGroupsAsArray() const;

    //@해당 그룹에 속한 모든 AI 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
        TArray<AActor*> GetGroupMembers(const FGuid& GroupID) const;
#pragma endregion

};