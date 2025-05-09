#pragma once

#include "CoreMinimal.h"
#include "05_Animation/BaseAnimInstance.h"

#include "AnimalAnimInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAnimalAnim, Log, All)

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
/*
*	@EPivotDirection
*
*	피벗 방향 열거형
*/
UENUM(BlueprintType)
enum class EPivotDirection : uint8
{
	None = 0    UMETA(DisplayName = "None"),
	Left60      UMETA(DisplayName = "Left 60"),
	Left90      UMETA(DisplayName = "Left 90"),
	Left120     UMETA(DisplayName = "Left 120"),
	Right60     UMETA(DisplayName = "Right 60"),
	Right90     UMETA(DisplayName = "Right 90"),
	Right120    UMETA(DisplayName = "Right 120"),
	MAX         UMETA(DisplayName = "MAX"),
};

/*
*	@EPivotState
*
*	피벗 상태 열거형
*/
UENUM(BlueprintType)
enum class EPivotState : uint8
{
	None = 0    UMETA(DisplayName = "None"),
	Starting    UMETA(DisplayName = "Starting"),
	Pivoting    UMETA(DisplayName = "Pivoting"),
	Ending      UMETA(DisplayName = "Ending"),
	MAX         UMETA(DisplayName = "MAX"),
};
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UAnimalAnimInstance
 *
 *	동물형 캐릭터 애니메이션 인스턴스입니다.
 */
UCLASS()
class AGEOFWOLVES_API UAnimalAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UAnimalAnimInstance(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@오버라이딩
	virtual void FindMovementState() override;

	//@피벗 방향 업데이트
	void UpdatePivotDirection();

	//@피벗 상태 업데이트
	void UpdatePivotState(float DeltaSeconds);

protected:
	//@회전 차이 임계값들
	UPROPERTY(EditDefaultsOnly, Category = "Animal | Pivot")
	float PivotThreshold60;

	UPROPERTY(EditDefaultsOnly, Category = "Animal | Pivot")
	float PivotThreshold90;

	UPROPERTY(EditDefaultsOnly, Category = "Animal | Pivot")
	float PivotThreshold120;

	//@피벗 쿨다운 시간
	UPROPERTY(EditDefaultsOnly, Category = "Animal | Pivot")
	float PivotCooldownTime;

	//@피벗 지속 시간
	UPROPERTY(EditDefaultsOnly, Category = "Animal | Pivot")
	float PivotDuration;

	//@현재 피벗 타이머
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animal | Pivot")
	float CurrentPivotTime;

	//@현재 피벗 쿨다운 타이머
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animal | Pivot")
	float CurrentPivotCooldown;

	//@현재 회전 각도 차이
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animal | Pivot")
	float RotationDeltaAngle;

	//@피벗 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animal | Pivot")
	EPivotDirection PivotDirection;

	//@피벗 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animal | Pivot")
	EPivotState PivotState;

	//@피벗 중인지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animal | Pivot")
	bool bIsPivoting;

	//@피벗 쿨다운 중인지 여부
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animal | Pivot")
	bool bIsPivotCooldown;
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	//@피벗 회전 각도 계산
	void CalculatePivotAngle();

public:
	//@피벗 방향 반환
	UFUNCTION(BlueprintPure, Category = "Animation | Animal", meta = (BlueprintThreadSafe))
		EPivotDirection GetPivotDirection() const { return PivotDirection; }

	//@피벗 상태 반환
	UFUNCTION(BlueprintPure, Category = "Animation | Animal", meta = (BlueprintThreadSafe))
		EPivotState GetPivotState() const { return PivotState; }

	//@피벗 중인지 여부 반환
	UFUNCTION(BlueprintPure, Category = "Animation | Animal", meta = (BlueprintThreadSafe))
		bool IsPivoting() const { return bIsPivoting; }
#pragma endregion
};