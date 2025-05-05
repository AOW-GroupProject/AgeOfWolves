#pragma once

#include "CoreMinimal.h"
#include "10_AI/BaseAIController.h"

#include "AnimalAIController.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAnimalAIC, Log, All)

/**
 *	@AAnimalAIController
 *
 *	동물형 AI 컨트롤러를 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API AAnimalAIController : public ABaseAIController
{
	GENERATED_BODY()

	//@Defualt Setting
#pragma region Default Setting
public:
	AAnimalAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//~AAIController Interface
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void UpdateControlRotation(float DeltaTime, bool bUpdatePawn = true) override;
	//~End Of AAIController Interface

protected:
	//@초기화
	virtual void InitializeAIController(class APawn* InPawn) override;
#pragma endregion

	//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@타겟 액터에 포커스 설정
	void SetFocusToTargetActor(bool bEnable);

	//@현재 상태에 따라 회전 방식 업데이트
	void UpdateRotationSettings();
#pragma endregion

	//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@Target에 대한 인지 이벤트를 구독하는 콜백
	virtual void OnPerception(AActor* Actor, FAIStimulus Stimulus) override;
#pragma endregion

	//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	//@타겟에 대한 포커스 여부 반환
	UFUNCTION(BlueprintCallable, Category = "Animal AI")
	bool IsFocusingOnTarget() const { return bIsFocusingOnTarget; }

	//@타겟에 대한 포커스 설정
	UFUNCTION(BlueprintCallable, Category = "Animal AI")
	void SetFocusToTarget(bool bEnable);

protected:
	//@타겟에 포커스 중인지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animal AI")
	bool bIsFocusingOnTarget;

	//@포커스 모드 활성화 시 회전 속도 (기본 회전 속도보다 느림)
	UPROPERTY(EditDefaultsOnly, Category = "Animal AI")
	float FocusRotationSpeed;

	//@포커스 타겟과의 최소 시야각 (이 각도를 초과할 경우에만 회전)
	UPROPERTY(EditDefaultsOnly, Category = "Animal AI")
	float MinFocusAngle;
#pragma endregion
};