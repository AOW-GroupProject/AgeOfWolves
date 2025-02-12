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
struct FBaseAbilitySet_GrantedHandles;
class UBaseAbilitySystemComponent;
class UAbilityManagerSubsystem;
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
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByAI)
//@AI의 Attribute Set 초기화 완료 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAIAttributeSetInitialized);

//@타겟 발견
DECLARE_MULTICAST_DELEGATE_OneParam(FAILockOnStateChanged, bool)
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
	//@외부 바인딩
	void ExternalBindToAnimInstance(APawn* InPawn);

protected:
	//@내부 바인딩
	void InternalBindToPerceptionComp();
	void InternalBindingToASC();

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
	//@BT
	UPROPERTY(Transient)
		UBehaviorTreeComponent* AIBehaviorTree;
	//@BB
	UPROPERTY(Transient)
		UBlackboardComponent* BBComponent;
	//@AI Perception
	UPROPERTY(VisibleAnywhere)
		UAIPerceptionComponent* AIPerceptionComponent;
	//@ASC
	UPROPERTY()
		UBaseAbilitySystemComponent* AbilitySystemComponent;
	//@Attribute Set
	UPROPERTY()
		TSoftObjectPtr<UBaseAttributeSet> AttributeSet;

protected:
	//@Character Tag
	UPROPERTY(EditDefaultsOnly, Category = "AI | 캐릭터 태그")
	FGameplayTag CharacterTag;

	//@AI 유형
	UPROPERTY(EditDefaultsOnly)
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
		void OnCharacterStateEventOnGameplay(const FGameplayTag& CharacterStateTag);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
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
	FORCEINLINE float GetMinAttackRange() { return MinAttackRange; }
	FORCEINLINE float GetMaxAttackRange() { return MaxAttackRange; }

public:
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
#pragma endregion

};
