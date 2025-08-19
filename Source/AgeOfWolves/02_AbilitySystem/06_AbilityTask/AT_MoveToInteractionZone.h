#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"

#include "AT_MoveToInteractionZone.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAT_InteractionZone, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractionZoneReached);

UCLASS()
class AGEOFWOLVES_API UAT_MoveToInteractionZone : public UAbilityTask
{
	GENERATED_BODY()

public:
	UAT_MoveToInteractionZone(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAT_MoveToInteractionZone* MoveToInteractionZone(
		UGameplayAbility* OwningAbility,
		AActor* TargetActor,
		FName SocketName = FName("ExecuterTarget"),
		float Duration = 1.0f
	);

	virtual void Activate() override;
	virtual void TickTask(float DeltaTime) override;

	UPROPERTY(BlueprintAssignable)
	FOnInteractionZoneReached OnInteractionZoneReached;

protected:
	// 충돌 설정 변경/복원
	void SetupCollisionResponse();
	void RestoreCollisionResponse();

	// 이전 충돌 응답 저장
	UPROPERTY()
	TEnumAsByte<ECollisionResponse> PreviousPawnResponse;

protected:
	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;

	UPROPERTY()
	FName SocketName;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	FRotator TargetRotation;

	UPROPERTY()
	FVector StartLocation;

	UPROPERTY()
	FRotator StartRotation;

	UPROPERTY()
	float Duration;

	UPROPERTY()
	float ElapsedTime;

private:
	bool CalculateSocketTransform();
};