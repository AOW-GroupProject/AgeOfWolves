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
/**
 * 타겟 기준 접근 방향
 * - 각 방향은 "내가 Target의 어느 방향에서 접근하는지"를 의미
 * - Front: Target 앞에서 접근 (Proximity 증가할수록 Target에서 멀어짐)
 * - Back: Target 뒤에서 접근 (Proximity 증가할수록 Target으로 가까워짐)
 * - Left: Target 좌측에서 접근 (Proximity 증가할수록 Target으로 가까워짐)
 * - Right: Target 우측에서 접근 (Proximity 증가할수록 Target으로 가까워짐)
 * - Normal: Target 정확한 위치로 이동 (Proximity 무시)
 */
	UENUM(BlueprintType)
	enum class EMotionWarpDirection : uint8
{
	Front    UMETA(DisplayName = "From Front"),
	Left     UMETA(DisplayName = "From Left"),
	Right    UMETA(DisplayName = "From Right"),
	Back     UMETA(DisplayName = "From Back"),
	Normal   UMETA(DisplayName = "Normal")
};

/** 타겟 기준 거리 레벨 */
UENUM(BlueprintType)
enum class EMotionWarpProximity : uint8
{
	Normal   UMETA(DisplayName = "Normal"),
	Close    UMETA(DisplayName = "Close"),
	Closer   UMETA(DisplayName = "Closer"),
	Closest  UMETA(DisplayName = "Closest")
};
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
			bool bFollowRotation = true,
			EMotionWarpDirection WarpDirection = EMotionWarpDirection::Normal,
			EMotionWarpProximity WarpProximity = EMotionWarpProximity::Normal);

protected:
	void UpdateWarpTarget();

	/** 방향 및 근접도에 따른 위치 오프셋 계산 */
	FVector CalculatePositionOffset(const AActor* Target) const;

	/** 근접도에 따른 오프셋 거리 스칼라 값 반환 */
	float GetProximityScalar() const;

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

	/** 타겟 기준 워프 방향 */
	UPROPERTY()
		EMotionWarpDirection WarpDirection;

	/** 타겟 기준 워프 근접도 */
	UPROPERTY()
		EMotionWarpProximity WarpProximity;

protected:
	UPROPERTY()
		TEnumAsByte<ECollisionResponse> PreviousPawnResponse;

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