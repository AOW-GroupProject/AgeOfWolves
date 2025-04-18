#pragma once
#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "MotionWarpingComponent.h"
#include "04_Component/InteractionComponent.h"
#include "AT_UpdateMotionWarpTarget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LOGAT_UpdateMotionWarpTarget, Log, All)

//@전방 선언
#pragma region Forward Declaration
class APlayerCharacter;
class ULockOnComponent;
class ABaseAIController;
#pragma endregion

//@열거형
#pragma region Enums
/*
*	@EMotionWarpDirection
*
*	Target 기준 Motion Warp 방향
*/
UENUM(BlueprintType)
enum class EMotionWarpDirection : uint8
{
	Front    UMETA(DisplayName = "타겟 기준 앞에서부터 시작"),
	Left     UMETA(DisplayName = "타겟 기준 왼쪽에서부터 시작"),
	Right    UMETA(DisplayName = "타겟 기준 오른쪽에서부터 시작"),
	Back     UMETA(DisplayName = "타겟 기준 뒤에서부터 시작"),
	Normal   UMETA(DisplayName = "타겟 기준 위치 무관")
};

/*
*	@EMotionWarpProximity
*
*	Target 기준 Motion Warp의 근접도(가까운 정도)
*/
UENUM(BlueprintType)
enum class EMotionWarpProximity : uint8
{
	Normal		UMETA(DisplayName = "타겟 위치"),
	Close		UMETA(DisplayName = "약간 가깝게"),
	Closer		UMETA(DisplayName = "가깝게"),
	Closest		UMETA(DisplayName = "매우 가깝게"),
	Fit			UMETA(DisplayName = "지정 위치"),
};
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/*
*	@UAT_UpdateMotionWarpTarget
*
*	Motion Warp의 Target 정보를 업데이트하는 Ability Task입니다.
*  'Fit' 유형 근접도 사용 시 회전 충돌을 방지하기 위한 로직이 포함됩니다.
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
	UAT_UpdateMotionWarpTarget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;
	virtual void OnDestroy(bool bInOwnerFinished) override;
#pragma endregion

	//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAT_UpdateMotionWarpTarget* CreateMotionWarpTargetTracker(
			UGameplayAbility* OwningAbility,
			FName TaskInstanceName,
			AActor* TargetActor,
			FName WarpTargetName,
			EMotionWarpDirection WarpDirection = EMotionWarpDirection::Normal,
			EMotionWarpProximity WarpProximity = EMotionWarpProximity::Normal,
			EInteractionType InteractionType = EInteractionType::None);

protected:
	//@Warp Target 업데이트
	void UpdateWarpTarget();

protected:
	//@충돌 설정 변경 처리
	void SetupCollisionResponse();

	//@충돌 설정 복원 처리
	void RestoreCollisionResponse();

protected:
	//@회전 제어 중지 처리
	void SuspendRotationControl();

	//@회전 제어 복원 처리
	void RestoreRotationControl();

private:
	//@추적할 타겟
	UPROPERTY()
		TWeakObjectPtr<AActor> TargetToTrack;

	//@Motion Warp 타겟 이름
	UPROPERTY()
		FName MotionWarpTargetName;

	//@타겟 기준 워프 방향
	UPROPERTY()
		EMotionWarpDirection WarpDirection;

	//@타겟 기준 근접도
	UPROPERTY()
		EMotionWarpProximity WarpProximity;

	//@회전 보정값
	UPROPERTY(EditDefaultsOnly, Category = "Motion Warping")
		float RotationOffset = 80.f;

private:
	//@상호작용 유형
	UPROPERTY()
		EInteractionType InteractionType;

	//@플레이어 캐릭터 참조
	UPROPERTY()
		TWeakObjectPtr<APlayerCharacter> PlayerCharacterRef;

	//@락온 컴포넌트 참조
	UPROPERTY()
		TWeakObjectPtr<ULockOnComponent> LockOnComponentRef;

	//@AI 컨트롤러 참조
	UPROPERTY()
		TWeakObjectPtr<ABaseAIController> AIControllerRef;

	//@회전 제어 중지 여부
	bool bRotationControlSuspended = false;

protected:
	//@충돌 응답 저장
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
private:
	//@방향 및 근접도에 따른 Offset 계산
	FVector CalculatePositionOffset(const AActor* Target) const;

public:
	//@근접도에 따른 오프셋 거리 스칼라 값 반환
	float GetProximityScalar() const;

public:
	//@상호작용 소켓 위치 가져오기
	FVector GetFitLocationFromTargetToTrack() const;

public:
	//@상호작용 소켓 회전 가져오기
	FRotator GetFitRotationFromTargetToTrack() const;

public:
	//@Motion Warp 컴포넌트 가져오기
	UMotionWarpingComponent* GetMotionWarpingComponent() const;
#pragma endregion
};