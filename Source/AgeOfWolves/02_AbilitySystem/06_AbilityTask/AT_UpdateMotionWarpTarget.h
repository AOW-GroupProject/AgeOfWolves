#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "MotionWarpingComponent.h"

#include "AT_UpdateMotionWarpTarget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LOGAT_UpdateMotionWarpTarget, Log, All)

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 * 모션 워프 타겟을 실시간으로 추적하는 AbilityTask
 */
UCLASS()
class AGEOFWOLVES_API UAT_UpdateMotionWarpTarget : public UAbilityTask
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	// 생성자
	UAT_UpdateMotionWarpTarget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	//@Ability Task 생성
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAT_UpdateMotionWarpTarget* CreateMotionWarpTargetTracker(
			UGameplayAbility* OwningAbility,
			FName TaskInstanceName,
			AActor* TargetActor,
			FName WarpTargetName,
			bool bFollowRotation = true);

protected:
	void UpdateWarpTarget();

private:
	/** 추적할 타겟 액터 */
	UPROPERTY()
		TWeakObjectPtr<AActor> TargetToTrack;

	/** 모션 워프 타겟 이름 */
	UPROPERTY()
		FName MotionWarpTargetName;

	/** 회전 추적 여부 */
	UPROPERTY()
		bool bTrackRotation;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	/** 모션 워프 컴포넌트 가져오기 */
	UMotionWarpingComponent* GetMotionWarpingComponent() const;
#pragma endregion

};