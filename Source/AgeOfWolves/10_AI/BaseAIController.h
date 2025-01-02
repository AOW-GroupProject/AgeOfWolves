#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "10_AI/AIDataSetInfos.h"

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
	Attack	 		UMETA(DisplayName = "Attack"),

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
#pragma endregion

/**
 *	@ABaseAIController
 * 
 *	AI 컨트롤러의 기본 유형을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API ABaseAIController : public AAIController
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
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true);
	virtual void OnPossess(class APawn* InPawn) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	virtual void ChangeAgentAIState(EAIState InStateType);

protected:
	//@AI 유형
	UPROPERTY(EditDefaultsOnly)
		EAIType AIType;

private:
	UPROPERTY(VisibleAnywhere)
		FRotator TargetRotation = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly)
		float SmootRotationSpeed = 540.f;

protected:
	UPROPERTY(Transient)
		UBehaviorTreeComponent* AIBehaviorTree;

	UPROPERTY(Transient)
		UBlackboardComponent* BBComponent;

	UPROPERTY(VisibleAnywhere)
		UAIPerceptionComponent* AIPerceptionComponent;

	UAISenseConfig_Sight* Sight;
	UAISenseConfig_Hearing* Hearing;

protected:
	TTuple<float, float> AttackRange;
	TTuple<float, float> SkillRange;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	UFUNCTION()
		virtual void OnPerception(AActor* Actor, FAIStimulus Stimulus);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	TWeakObjectPtr<UAIManagerSubsystem> AIManagerRef;

public:
	FORCEINLINE TTuple<float, float> GetAttackRange() { return AttackRange; }

public:
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
#pragma endregion

};
