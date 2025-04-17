#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "MotionWarpingComponent.h"
#include "04_Component/InteractionComponent.h"

#include "AT_UpdateMotionWarpTarget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LOGAT_UpdateMotionWarpTarget, Log, All)

//@전방 선언
#pragma region Forward Declaration
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
*	Motion Warp의 Target 정보를 업데이트하는 Abilit Task입니다.
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
			bool bFollowRotation = true,
			EMotionWarpDirection WarpDirection = EMotionWarpDirection::Normal,
			EMotionWarpProximity WarpProximity = EMotionWarpProximity::Normal,
			EInteractionType InteractionType = EInteractionType::None);

protected:
	//@Warp Target 업데이트
	void UpdateWarpTarget();

	//@방향 및 근접도에 따른 Offset 계산
	FVector CalculatePositionOffset(const AActor* Target) const;

private:
	//@추적할 타겟
	UPROPERTY()
		TWeakObjectPtr<AActor> TargetToTrack;

	//@Motion Warp 타겟 이름
	UPROPERTY()
		FName MotionWarpTargetName;

	//@회전 추적 여부
	UPROPERTY()
		bool bTrackRotation;

	const float RotationOffset = 120.f;

	//@타겟 기준 워프 방햫
	UPROPERTY()
		EMotionWarpDirection WarpDirection;

	//@타겟 기준 근접도
	UPROPERTY()
		EMotionWarpProximity WarpProximity;

private:
	//@상호작용 유형
	UPROPERTY()
		EInteractionType InteractionType;

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
	//@근접도에 따른 오프센 거리 스칼라 값 반환
	float GetProximityScalar() const;

public:
	FVector GetFitLocationFromTargetToTrack() const;

public:
	FRotator GetFitRotationFromTargetToTrack() const;

public:
	//@Motion Warp 컴포넌트 가져오기
	UMotionWarpingComponent* GetMotionWarpingComponent() const;
#pragma endregion

};