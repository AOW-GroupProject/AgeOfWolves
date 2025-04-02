#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "10_AI/AIDataSetInfos.h"
#include "GameplayTagContainer.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "AbilitySystemInterface.h"

#include "BaseAIController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBaseAIC, Log, All)

//@전방 선언
#pragma region Forward Declaration
class AAIManager;
class UBehaviorTreeComponent;
class UBlackboardComponent;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
class UAIManagerSubsystem;
class UAOWSaveGame;
class UBaseAbilitySystemComponent;
class UAbilityManagerSubsystem;
class UAIAbilitySequencerComponent;
class UObjectiveDetectionComponent;

struct FBaseAbilitySet_GrantedHandles;
#pragma endregion

//@열거형
#pragma region Enums
/*
*	@EAIState
* 
*	AI의 상태를 열거형으로 정의합니다.
*/
UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle = 0			UMETA(DisplayName = "Idle"),
	Investigate 		UMETA(DisplayName = "Investigate"),
	CoverFire			UMETA(DisplayName = "ComverFire"),
	Attack	 			UMETA(DisplayName = "Attack"),
};

/*
*	@EAICombatPhase
* 
*	AI의 전투 페이즈를 열거형으로 정읳바니다.
*/
UENUM(BlueprintType)
enum class EAICombatPhase : uint8
{
	Phase1			UMETA(DisplayName = "Phase 1"),
	Phase2			UMETA(DisplayName = "Phase 2"),
	Phase3			UMETA(DisplayName = "Phase 3"),
	Max
};

/*
*   @EAISharingInfoType
*
*   그룹 내 AI들 간 정보 공유 방식을 정의합니다.
*/
UENUM(BlueprintType)
enum class EAISharingInfoType : uint8
{
	All             UMETA(DisplayName = "모든 그룹원에게"),
	SameRank        UMETA(DisplayName = "동일 계층에게만"),
	HigherRank      UMETA(DisplayName = "상위 계층에게만"),
	LowerRank       UMETA(DisplayName = "하위 계층에게만"),
	Leader          UMETA(DisplayName = "리더에게만"),
	Exclude         UMETA(DisplayName = "제외 대상 제외"),
	Custom          UMETA(DisplayName = "커스텀 대상")
};
#pragma endregion

//@구조체
#pragma region Structs
/*
*   @FSharingInfoWithGroup
*
*   그룹 내 AI들 간 공유하는 정보를 정의하는 구조체입니다.
*/
USTRUCT(BlueprintType)
struct FSharingInfoWithGroup
{
	GENERATED_BODY()

	//@정보 식별자 (중복 정보 필터링용)
	UPROPERTY()
		FGuid InfoID;

	//@정보 공유 유형
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EAISharingInfoType SharingType = EAISharingInfoType::All;

	//@보낸 AI의 상태 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGameplayTag StateTag;

	//@정보의 우선순위 (높을수록 중요)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Priority = 0;

	//@정보가 유효한 시간 (초, 0이면 무제한)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ValidTime = 5.0f;

	//@제외할 대상 (SharingType이 Exclude일 때 사용)
	UPROPERTY()
		TArray<TWeakObjectPtr<AActor>> ExcludedTargets;

	//@커스텀 대상 (SharingType이 Custom일 때 사용)
	UPROPERTY()
		TArray<TWeakObjectPtr<AActor>> CustomTargets;

	// 생성 시간
	UPROPERTY()
		float CreationTime = 0.0f;

	// 기본 생성자
	FSharingInfoWithGroup()
		: InfoID(FGuid::NewGuid())
	{
		CreationTime = GWorld ? GWorld->GetTimeSeconds() : 0.0f;
	}

	// 매개변수 생성자
	FSharingInfoWithGroup(
		EAISharingInfoType InSharingType,
		const FGameplayTag& InStateTag,
		int32 InPriority = 0,
		float InValidTime = 5.0f,
		const FVector& InLastKnownLocation = FVector::ZeroVector,
		AActor* InDetectedTarget = nullptr)
		: InfoID(FGuid::NewGuid())
		, SharingType(InSharingType)
		, StateTag(InStateTag)
		, Priority(InPriority)
		, ValidTime(InValidTime)
	{
		CreationTime = GWorld ? GWorld->GetTimeSeconds() : 0.0f;
	}

	//@정보가 여전히 유효한지 확인
	bool IsValid(float CurrentTime) const
	{
		//@ValidTime이 0이면 시간 제한 없음
		if (ValidTime <= 0.0f)
			return true;

		return (CurrentTime - CreationTime) < ValidTime;
	}

	//@다른 정보보다 높은 우선순위를 가지는지 확인
	bool HasHigherPriorityThan(const FSharingInfoWithGroup& OtherInfo) const
	{
		return Priority > OtherInfo.Priority;
	}
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByAI)
//@AI의 Attribute Set 초기화 완료 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAIAttributeSetInitialized);

//@락온 타겟 설정/해제 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FAILockOnStateChanged, bool, AActor*)

//@타겟 인지 이벤트
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAIDetectsTarget, bool,  AActor*, AActor*)

//@전투 패턴 활성화 요청
DECLARE_DELEGATE_RetVal(bool, FRequestStartCombatPattern)
//@전투 패턴 종료 요청
DECLARE_DELEGATE_RetVal(bool, FRequestEndCombatPattern)
//@전투 패턴 Exit Block 완료 통지
DECLARE_DELEGATE_RetVal(bool, FNotifyCombatPatternExitComplete)

//@인지 정보 전달 이벤트
DECLARE_MULTICAST_DELEGATE_TwoParams(FSendInfoToBelongingGroup, AActor*, FSharingInfoWithGroup)
#pragma endregion

/**
 *	@ABaseAIController
 * 
 *	AI 컨트롤러의 기본 유형을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API ABaseAIController : public AAIController, public IAbilitySystemInterface
{
//@친추 클래스
#pragma region Friend Class
	friend class UBTService_SetFocus;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	ABaseAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//~AAIController Interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(class APawn* InPawn) override;
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true);
	//~End Of AAIController Interface

protected:
	//@외부 바인딩...
	void ExternalBindToAnimInstance(APawn* InPawn);

protected:
	//@내부 바인딩...
	void InternalBindToPerceptionComp();
	void InternalBindingToASC();
	void InternalBindingToAISequencerComp();
	void InternalBindingToODComp();

public:
	//@초기화
	UFUNCTION()
		void InitializeAIController(class APawn* InPawn);
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@AI 시스템 초기화
	void InitializeAISystem(class APawn* InPawn);
	//@Ability System 초기화
	void InitializeAbilitySystem(class APawn* InPawn);

protected:
	//@로딩 작업
	UFUNCTION()
		void LoadGameAbilitySystem();
	//@캐릭터의 Ability System 정보를 Ability Manager Subsystem으로부터 Load합니다.
	void LoadDefaultAbilitySystemFromAbilityManager();
	//@캐릭터의 Ability System 정보를 Save File로부터 Load합니다.
	void LoadAbilitySystemFromSaveGame(UAOWSaveGame* SaveGame);

protected:
	//@AI State의 변경
	virtual void ChangeAgentAIState(EAIState InStateType);

protected:
	//@AIC에서 수행할 Character의 상태 이벤트 처리
	void HandleCharacterStateEvent(const FGameplayTag& CharacterStateTag);

protected:
	//@캐릭터 죽음 상태 처리
	void ProcessCharacterDeathEvent();

protected:
	//@Target Actor의 상태 변화 이벤트 바인딩/언바인딩
	void BindTargetActorStateEvents(AActor* NewTarget);
	void UnbindTargetActorStateEvents(AActor* OldTarget);

protected:
	//@AI Group에게 공유 정보 전달
	bool ShareInfoToGroup(
		const FGameplayTag& StateTag,
		EAISharingInfoType SharingType = EAISharingInfoType::All,
		int32 Priority = 1,
		float ValidTime = 5.0f);

protected:
	//@AI Perception
	UPROPERTY(VisibleAnywhere)
		UAIPerceptionComponent* AIPerceptionComponent;

	//@ASC
	UPROPERTY()
		UBaseAbilitySystemComponent* AbilitySystemComponent;

	//@Attribute Set
	UPROPERTY()
		TSoftObjectPtr<UBaseAttributeSet> AttributeSet;

	//@AI Combat Pattern 컴포넌트
	UPROPERTY(Transient)
		UAIAbilitySequencerComponent* AIAbilitySequencerComponent;

	//@Objective Detection Comp
	UPROPERTY(Transient)
		UObjectiveDetectionComponent* ODComponent;

protected:
	//@BT
	UPROPERTY(Transient)
		UBehaviorTreeComponent* AIBehaviorTree;
	//@BB
	UPROPERTY(Transient)
		UBlackboardComponent* BBComponent;

protected:
	//@Character Tag
	UPROPERTY(EditDefaultsOnly, Category = "AI | 캐릭터 태그")
		FGameplayTag CharacterTag;

	//@AI 유형
	UPROPERTY(EditDefaultsOnly, Category = "AI | AI 유형")
		EAIType AIType;

private:
	FBaseAbilitySet_GrantedHandles* SetGrantedHandles;

private:
	UPROPERTY(VisibleAnywhere)
		FRotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly)
		float SmootRotationSpeed = 540.f;

protected:
	UAISenseConfig_Sight* Sight;
	UAISenseConfig_Hearing* Hearing;

protected:
	UPROPERTY(EditDefaultsOnly)
		float MinAttackRange;
	UPROPERTY(EditDefaultsOnly)
		float MaxAttackRange;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@초기화 요청 이벤트
	FRequestStartInitByAI RequestStartInitByAI;

public:
	//@AI의 Attribute Set 초기화 완료 이벤트
	FAIAttributeSetInitialized AIAttributeSetInitialized;

public:
	//@AI의 Lock On 상태 변화 이벤트
	FAILockOnStateChanged AILockOnStateChanged;

public:
	//@AI의 타겟 인지 이벤트
	FAIDetectsTarget AIDetectsTarget;

public:
	//@전투 패턴 활성화 요청 이벤트
	FRequestStartCombatPattern RequestStartCombatPattern;
	//@전투 패턴 활성화 종료 요청 이벤트
	FRequestEndCombatPattern RequestEndCombatPattern;
	//@전투 패턴 활성화 종료 완료 이벤트
	FNotifyCombatPatternExitComplete NotifyCombatPatternExitComplete;

public:
	//@그룹 공유 정보 전달 이벤트
	FSendInfoToBelongingGroup SendInfoToBelongingGroup;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@Target에 대한 인지 이벤트를 구독하는 콜백
	UFUNCTION()
		virtual void OnPerception(AActor* Actor, FAIStimulus Stimulus);

	//@Target에 대한 인지 소실 이벤트를 구독하는 콜백
	UFUNCTION()
		void OnTargetPerceptionLost(AActor* Actor);

protected:
	//@AI Attribute Set의 속성 수치 값 변화 이벤트를 구독하는 콜백
	void OnAttributeValueChanged(const FOnAttributeChangeData& Data);

protected:
	//@캐릭터 상태 관련 이벤트 발생 시 호출되는 콜백
	UFUNCTION()
		void OnCharacterStateEventOnGameplay(AActor* Actor, const FGameplayTag& CharacterStateTag);

protected:
	//@전투 패턴 Exit Block 완료 콜백
	UFUNCTION()
		bool OnCombatPatternExitComplete();

protected:
	// Target Actor 상태 변화 콜백
	UFUNCTION()
		void OnTargetActorStateChanged(AActor* Actor, const FGameplayTag& StateTag);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	UPROPERTY()
		TWeakObjectPtr<APawn> AgentPawnRef;

protected:
	//@AI Manager Subsystem 캐싱
	UPROPERTY()
		TWeakObjectPtr<UAIManagerSubsystem> AIManagerRef;
	
protected:
	//@Ability Manger Subsystem 캐싱
	UPROPERTY()
		TWeakObjectPtr<UAbilityManagerSubsystem> AbilityManagerRef;

public:
	//~IAbilitySystemInterface Interface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End Of IAbilitySystemInterface Interface

public:
	FORCEINLINE EAIType GetAIType() const { return AIType; }

public:
	FORCEINLINE float GetMinAttackRange() { return MinAttackRange; }
	FORCEINLINE float GetMaxAttackRange() { return MaxAttackRange; }

public:
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
#pragma endregion

};
