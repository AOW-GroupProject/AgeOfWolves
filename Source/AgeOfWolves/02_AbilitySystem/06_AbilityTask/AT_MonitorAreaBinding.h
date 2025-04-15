#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"

#include "AT_MonitorAreaBinding.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAreaBindingMonitor, Log, All);

//@전방 선언
#pragma region Forward Declaration
struct FAreaBindingInfo;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnterDangerZone, bool, IsInDanger);
#pragma endregion

/**
 *  @UAT_MonitorAreaBinding
 * 
 *  Avatar의 Area 바인딩 이벤트를 감시하는 Ability Task
 *  주로, 아바타의 전시/비전시 상황을 감시하기 위한 용도
 */
UCLASS()
class AGEOFWOLVES_API UAT_MonitorAreaBinding : public UAbilityTask
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UAT_MonitorAreaBinding(const FObjectInitializer& ObjectInitializer);

    // Ability Task 생성 함수
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility"))
        static UAT_MonitorAreaBinding* MonitorAreaBinding(UGameplayAbility* OwningAbility);

protected:
    // 태스크 활성화
    virtual void Activate() override;

    // 태스크 종료
    virtual void OnDestroy(bool bInOwnerFinished) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    //@AlertState GE 적용
    void ApplyAlertStateEffect(bool bIsInDanger);

    //@GE 핸들
    FActiveGameplayEffectHandle AlertStateEffectHandle;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    FEnterDangerZone EnterDangerZone;
#pragma endregion

//@Callbacks
#pragma region Callbacks
// Area 바인딩 이벤트 콜백
    UFUNCTION(BlueprintNativeEvent)
        void OnPlayerBoundToArea(FAreaBindingInfo AreaInfo, bool bIsBound);
    virtual void OnPlayerBoundToArea_Implementation(FAreaBindingInfo AreaInfo, bool bIsBound);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
	
};
