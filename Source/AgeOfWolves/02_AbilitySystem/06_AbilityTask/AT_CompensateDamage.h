#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "GameplayTagContainer.h"
#include "Engine/Engine.h"

#include "AT_CompensateDamage.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCompensateDamage, Log, All);

//@전방 선언
#pragma region Forward Declaration
class UBaseAbilitySystemComponent;
struct FGameplayEventData;
#pragma endregion

//@델리게이트
#pragma region Delegates
//@강공격 파훼 성공 이벤트
DECLARE_MULTICAST_DELEGATE_ThreeParams(FStrongAttackCountered,
    const AActor*,
    const AActor*,
    const FGameplayEventData&);

//@일반 공격 처리 이벤트 
DECLARE_MULTICAST_DELEGATE_ThreeParams(FNormalDamageProcessed,
    const AActor*, 
    const AActor*, 
    const FGameplayEventData&);
#pragma endregion

/**
 * @UAT_CompensateDamage
 *
 * 강공격 파훼 매커니즘을 처리하는 어빌리티 태스크
 * - EventTag.OnDamaged 이벤트를 모니터링
 * - OptionalObject1의 AssetTags에서 AttackMagnitude.Strong 확인
 * - 강공격인 경우 데미지 처리를 차단하고 공격자에게 EventTag.OnAttackFailed 전달
 */
UCLASS()
class AGEOFWOLVES_API UAT_CompensateDamage : public UAbilityTask
{
    GENERATED_BODY()

    //@Default Setting
#pragma region Default Setting
public:
    UAT_CompensateDamage(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void Activate() override;
    virtual void OnDestroy(bool AbilityEnded) override;

protected:
    //@내부 바인딩
    void BindToGameplayEvent();
    void UnbindFromGameplayEvent();

protected:
    //@초기화
    void InitializeCompensateDamage();
#pragma endregion

    //@Property/Info...etc  
#pragma region Property or Subwidgets or Infos...etc
public:
    /**
     * 강공격 파훼 어빌리티 태스크 생성
     * @param OwningAbility - 소유 어빌리티
     * @param TaskInstanceName - 태스크 인스턴스 이름
     * @param bOnlyTriggerOnce - 한 번만 트리거할지 여부 (기본: false)
     * @return 생성된 어빌리티 태스크 인스턴스
     */
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
        meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility",
            BlueprintInternalUseOnly = "TRUE",
            DisplayName = "Wait For Strong Attack Compensation"))
    static UAT_CompensateDamage* WaitForStrongAttackCompensation(
        UGameplayAbility* OwningAbility,
        FName TaskInstanceName = NAME_None,
        bool bOnlyTriggerOnce = false
    );

private:
    //@공격 실패 이벤트 전달
    bool SendAttackFailedEvent(const FGameplayEventData& OriginalEventData) const;

protected:
    //@모니터링할 이벤트 태그
    FGameplayTag DamageEventTag;

    //@강공격 매그니튜드 태그
    FGameplayTag StrongAttackTag;

    //@공격 실패 이벤트 태그
    FGameplayTag AttackFailedEventTag;

    //@한 번만 트리거할지 여부
    UPROPERTY()
    bool bOnlyTriggerOnce;

    //@이미 트리거되었는지 여부
    UPROPERTY()
    bool bHasTriggered;

    //@바인딩된 ASC 참조
    UPROPERTY()
    TWeakObjectPtr<UBaseAbilitySystemComponent> BoundASC;

    //@이벤트 핸들
    FDelegateHandle GameplayEventHandle;
#pragma endregion

    //@Delegates
#pragma region Delegates
public:
    //@강공격 파훼 성공 델리게이트
    FStrongAttackCountered OnStrongAttackCountered;

    //@일반 데미지 처리 델리게이트
    FNormalDamageProcessed OnNormalDamageProcessed;
#pragma endregion

    //@Callbacks
#pragma region Callbacks
protected:
    //@데미지 이벤트 전처리 핸들러 (BaseASC 델리게이트용)
    UFUNCTION()
    void OnDamageEventPreProcess(const FGameplayTag& EventTag, const FGameplayEventData& EventData, bool& bShouldContinueProcessing);

    //@데미지 이벤트 핸들러
    UFUNCTION()
    void OnDamageEventReceived(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
#pragma endregion

    //@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
    //@강공격 여부 확인
    bool IsStrongAttack(const FGameplayEventData& EventData) const;

    //@AssetTags 추출 및 검증
    FGameplayTagContainer ExtractAssetTagsFromEventData(const FGameplayEventData& EventData) const;

    //@로그 출력용 이벤트 데이터 정보
    FString GetEventDataInfoString(const FGameplayEventData& EventData) const;
#pragma endregion
};