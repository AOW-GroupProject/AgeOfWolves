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