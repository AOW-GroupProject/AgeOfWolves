#pragma once

#include "CoreMinimal.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"
#include "04_Component/InteractionComponent.h"

#include "InteractionGameplayAbility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteractionGA, Log, All);

//@전방 선언
#pragma region Forward Declaration
class ACharacterBase;
class UAnimMontage;
#pragma endregion

//@열거형
#pragma region Enums
/*
*	@상호작용 트레이스 유형
*
*	상호작용 감지를 위한 충돌 유형을 정의합니다.
*/
UENUM(BlueprintType)
enum class EInteractionTraceType : uint8
{
	Line    UMETA(DisplayName = "Line Trace"),
	Sphere  UMETA(DisplayName = "Sphere Trace"),
	Box     UMETA(DisplayName = "Box Trace")
};
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

UCLASS()
class AGEOFWOLVES_API UInteractionGameplayAbility : public UBaseGameplayAbility
{
//@친추 클래스
#pragma region Friend Class
	friend class UANS_InteractionTrace;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UInteractionGameplayAbility(const FObjectInitializer& ObjectInitializer);
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@상호작용 이벤트 전송
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 상호작용")
		void SendInteractionEvent(const FHitResult& HitResult);

protected:
	//@트레이스 시작
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌")
		virtual void StartInteractionTrace();

	//@트레이스 처리
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌")
		virtual void ProcessInteractionTrace();

	//@트레이스 종료
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 충돌")
		virtual void EndInteractionTrace(bool bSuccess = false, const FHitResult& HitResult = FHitResult());

	//@Line Trace
	void PerformLineTrace(const FVector& Start, const FVector& End,
		FCollisionQueryParams& QueryParams, TArray<FHitResult>& OutHitResults);

	//@통합된 Sweep 함수를 위한 템플릿 함수
	template<typename TShape>
	void PerformSweepTrace(const FVector& Start, const FVector& End,
		const TShape& Shape, FCollisionQueryParams& QueryParams,
		TArray<FHitResult>& OutHitResults)
	{
		GetWorld()->SweepMultiByChannel(
			OutHitResults,
			Start,
			End,
			FQuat::Identity,
			ECC_Visibility,
			Shape,
			QueryParams
		);
	}

private:
	// @트레이스 상태 관리를 위한 동기화 객체
	FCriticalSection TraceStateLock;

protected:
	//@트레이스 관련 상태 변수들
	UPROPERTY(Transient)
		uint8 bIsTracing : 1;

	//@약참조로 변경하여 메모리 안전성 향상
	UPROPERTY(Transient)
		TArray<TWeakObjectPtr<AActor>> ActorsToIgnore;

	//@Trace 타입 설정
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌")
		EInteractionTraceType TraceType = EInteractionTraceType::Line;

protected:
	//@트레이스에 사용될 소켓 이름들
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌")
		FName WeaponTraceStartSocket;

	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌")
		FName WeaponSocket;

	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌")
		FName WeaponTraceEndSocket;

protected:
	//@Sphere Trace용 반경
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌",
		meta = (EditCondition = "TraceType == EWeaponTraceType::Sphere"))
		float SphereTraceRadius = 20.0f;

protected:
	//@Box Trace용 크기
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 충돌",
		meta = (EditCondition = "TraceType == EWeaponTraceType::Box"))
		FVector BoxTraceHalfSize = FVector(20.0f, 20.0f, 20.0f);

protected:
	//@상호작용 데이터 객체
	UPROPERTY(Transient)
		UInteractionData* InteractionData;

	//@캐시된 타겟 액터, 최초 어빌리티 활성화 시점에 꼭 Event Data로부터 Target 설정 필수!
	UPROPERTY(Transient)
		TWeakObjectPtr<AActor> TargetActor;

	//@상호작용 처리 완료 여부
	UPROPERTY(Transient)
		bool bInteractionCompleted;
#pragma endregion

	//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@오버라이드
	virtual void OnChainActionActivated_Implementation(FGameplayTag ChainActionEventTag) override;

	//@오버라이드
	virtual void OnChainActionFinished_Implementation(FGameplayTag ChainActionEventTag) override;
#pragma endregion

	//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Getter")
		ACharacterBase* GetCharacterFromActorInfo() const;

protected:
	UFUNCTION(BlueprintCallable)
		void SetInteractionData(UInteractionData* Data) { InteractionData = Data; }

	UFUNCTION(BlueprintCallable)
		UInteractionData* GetInteractionData() const { return InteractionData; }

protected:
	UFUNCTION(BlueprintCallable)
		void SetTargetActor(AActor* Target) { TargetActor = Target; }

	UFUNCTION(BlueprintCallable)
		AActor* GetTargetActor() const { return TargetActor.Get(); }

protected:
	//@상호작용 가능 여부 검사
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 상호작용")
		bool CanInteractWith(AActor* Target) const;
#pragma endregion
};