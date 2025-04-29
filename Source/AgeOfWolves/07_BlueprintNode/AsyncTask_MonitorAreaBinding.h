#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "GameplayEffectTypes.h"

#include "AsyncTask_MonitorAreaBinding.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAreaBindingChanged, bool, bIsInDanger);

//@전방 선언
#pragma region Forward Declaration
class UObjectiveDetectionComponent;
class UAbilitySystemComponent;

struct FAreaBindingInfo;
struct FActiveGameplayEffectHandle;
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
* @UAsyncTask_MonitorAreaBinding
*
* 플레이어가 Area에 진입/이탈할 때 AlertState를 변경하는 비동기 태스크
*/
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class AGEOFWOLVES_API UAsyncTask_MonitorAreaBinding : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	// 위험 영역 상태 변경 델리게이트
	UPROPERTY(BlueprintAssignable)
		FAreaBindingChanged OnAreaBindingChanged;

	// 목적: ObjectiveDetectionComponent의 Area 바인딩 이벤트를 감시하는 AsyncTask 생성
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UAsyncTask_MonitorAreaBinding* ListenForAreaBindingChanges(APlayerController* PlayerController);

	// AsyncTask 종료 및 정리
	UFUNCTION(BlueprintCallable)
		void EndTask();

protected:
	UFUNCTION(BlueprintNativeEvent)
		void OnPlayerBoundToArea(FAreaBindingInfo AreaInfo, bool bIsBound);
	virtual void OnPlayerBoundToArea_Implementation(FAreaBindingInfo AreaInfo, bool bIsBound);

	//@GE 적용 함수
	void ApplyAlertStateEffect(bool bIsInDanger);

private:
	// 참조 저장
	TWeakObjectPtr<UObjectiveDetectionComponent> ODComponent;
	TWeakObjectPtr<UAbilitySystemComponent> ASC;

	// 현재 적용 중인 GE 핸들
	FActiveGameplayEffectHandle AlertStateEffectHandle;
};