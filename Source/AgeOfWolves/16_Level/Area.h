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
class ACharacterBase;
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

/*
 *  @EStructureType
 *
 *  게임 내 구조물의 기본 타입을 정의하는 열거형
 */
UENUM(BlueprintType)
enum class EStructureType : uint8
{
    SavePoint       UMETA(DisplayName = "세이브 포인트"),
    Store           UMETA(DisplayName = "상점"),
    StoryTeller     UMETA(DisplayName = "스토리텔러"),
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
    TWeakObjectPtr<ACharacterBase> AIActor;

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

    FAreaAIInfo(ACharacterBase* InActor, const FGameplayTag& InState, UBaseAbilitySystemComponent* InASC,
        EAIHierarchyType InType = EAIHierarchyType::Regular, int32 InPriority = 0,
        const FGuid& InGroupID = FGuid())
        : AIActor(InActor)
        , HierarchyType(InType)
        , Priority(InPriority)
        , CurrentState(InState.IsValid() ? InState : FGameplayTag::RequestGameplayTag("State.Alive"))
        , AIASC(InASC)
        , GroupID(InGroupID)
        , AlertLevel(0)
    {
    }

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

    //@그룹의 순찰 경로
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
    TObjectPtr<AActor> PatrolPath;

    FAIGroupInfo() {}

    FAIGroupInfo(FGuid InGroupID, const FString& InName)
        : GroupID(InGroupID)
        , GroupName(InName)
    {    }
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
    {
    }

    //@지연 해제 필요 여부 확인
    bool ShouldDelayUnbind(float CurrentTime) const
    {
        return (CurrentTime - LastExitTime) < UnbindDelay;
    }

};

/*
*   @FStructureData
*
*   게임 내 구조물의 메타데이터와 상태 정보를 관리하는 구조체
*/
USTRUCT(BlueprintType)
struct FStructureData
{
    GENERATED_BODY()

    //@구조물의 고유 식별자
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "식별자")
    FGuid StructureID;

    //@개발자 친화적인 구조물 이름
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "식별자")
    FName StructureName;

    //@구조물의 기본 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "메타데이터")
    EStructureType StructureType = EStructureType::SavePoint;

    //@구조물의 용도나 특징에 대한 설명
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "메타데이터", meta = (MultiLine = true))
    FString Description;

    //@전환 요청할 다음 레벨의 태그
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "메타데이터", meta = (Categories = "Level"))
    FGameplayTag NextLevelTag;

    //@플레이어 시작 위치의 태그 (리스폰 및 레벨 전환 시 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "메타데이터", meta = (Categories = "PlayerStart"))
    FGameplayTag PlayerStartTag;

    //@구조물이 파괴 가능한지 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "설정")
    bool bIsDestructible = false;

    //@구조물이 이동 가능한지 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "설정")
    bool bIsMovable = false;

    //@구조물의 현재 활성화 상태
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "상태")
    bool bIsActive = false;

    //@실제 레벨에 배치된 구조물 액터에 대한 약한 참조
    //@약한 참조 사용으로 메모리 누수 방지 및 안전한 액터 생명주기 관리
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "액터 참조")
    TWeakObjectPtr<AActor> StructureActor;

    //@기본 생성자 - 모든 필드를 안전한 기본값으로 초기화
    FStructureData()
    {
        StructureID = FGuid::NewGuid();
        StructureName = FName("DefaultStructure");
        StructureType = EStructureType::SavePoint;
        Description = FString("기본 구조물");
        NextLevelTag = FGameplayTag(); // 빈 태그로 초기화
        PlayerStartTag = FGameplayTag(); // 빈 태그로 초기화
        bIsDestructible = false;
        bIsMovable = false;
        bIsActive = false;
        StructureActor.Reset(); // 약한 참조를 명시적으로 초기화
    }

    //@매개변수 생성자 - 필수 정보를 받아서 구조체를 초기화
    FStructureData(const FName& InName, EStructureType InType, const FString& InDescription)
        : FStructureData() // 위임 생성자로 기본 생성자를 먼저 호출하여 중복 코드 방지
    {
        StructureName = InName;
        StructureType = InType;
        Description = InDescription;
    }

    //@Getter 메서드들 - 각 필드에 대한 읽기 전용 접근을 제공
    FGuid GetStructureID() const { return StructureID; }
    FName GetStructureName() const { return StructureName; }
    EStructureType GetStructureType() const { return StructureType; }
    FString GetDescription() const { return Description; }
    FGameplayTag GetNextLevelTag() const { return NextLevelTag; }
    FGameplayTag GetPlayerStartTag() const { return PlayerStartTag; }
    bool IsDestructible() const { return bIsDestructible; }
    bool IsMovable() const { return bIsMovable; }
    bool IsActive() const { return bIsActive; }

    //@약한 참조의 특성상 액터가 파괴되었을 수 있으므로 유효성 검사가 중요
    AActor* GetStructureActor() const { return StructureActor.Get(); }
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

//@AI의 그룹 등록 완료 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FAIRegisteredToAIGroup, AActor*, const FGuid&);

//@그룹 상태 변경 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FGroupStatusChanged, const FGuid&, const FString&);

//@그룹 공유 정보 전달 받음 알림 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FNotifyGroupToShareInfo, AActor*, FSharingInfoWithGroup)
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

#if WITH_EDITOR
protected:
    // 에디터에서 프로퍼티 변경 시 호출되는 함수
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    // 에디터의 맵 체크 기능에서 호출되는 함수  
    virtual void CheckForErrors() override;

    // 에디터에서 액터 로드 완료 후 호출되는 함수
    virtual void PostLoad() override;

    // 에디터에서 액터가 생성된 직후 호출되는 함수
    virtual void PostActorCreated() override;

private:
    // Area Tag 유효성 검사 헬퍼 함수
    bool ValidateAreaTag(bool bShowDetailedFeedback = true) const;

    // 에디터 알림 표시 함수 - SystemMessageConfig의 ShowEditorNotification을 참고
    void ShowAreaTagNotification(const FString& Message, bool bIsError = true) const;

    // 맵 체크 오류 추가 함수
    void AddMapCheckError(const FString& ErrorMessage) const;
#endif
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
    //@영역 태그
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area", meta = (Categories = "Area"))
    FGameplayTag AreaTag;

    //@영역 식별자
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Area")
    FGuid AreaID;

    //@영역 우선순위 (중첩 처리용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area")
    int32 AreaPriority = 0;

    //@자동으로 영역 내 AI 감지할지 여부
    UPROPERTY(EditAnywhere, Category = "Area | AI")
    bool bAutoDetectAI = true;

    //@영역 내 AI 감지 반경 (자동 감지 시)
    UPROPERTY(EditAnywhere, Category = "Area | AI", meta = (EditCondition = "bAutoDetectAI"))
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

    //@등록된 AI 그룹 Map
    UPROPERTY()
    TMap<FGuid, FAIGroupInfo> MAIGroups;

protected:
    UPROPERTY(EditAnywhere, Category = "Area | 구조물")
    FStructureData StructureData;

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
    UPROPERTY(EditAnywhere, Category = "Area")
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
    //@AI의 그룹 등록 완료 이벤트
    FAIRegisteredToAIGroup AIRegisteredToAIGroup;

    //@그룹 상태 변화 이벤트
    FGroupStatusChanged GroupStatusChanged;

public:
    //@AI 그룹이 공유할 정보 알림 이벤트
    FNotifyGroupToShareInfo NotifyGroupToShareInfo;
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

protected:
    //@그룹 정보 수신 콜백
    UFUNCTION()
    void OnSendInfoToBelongingGroup(AActor* AI, FSharingInfoWithGroup SharingInfo);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    //@영역 태그
    UFUNCTION(BlueprintCallable, Category = "Area")
    FGameplayTag GetAreaTag() const { return AreaTag; }

    //@영역 ID 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
    FGuid GetAreaID() const { return AreaID; }

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