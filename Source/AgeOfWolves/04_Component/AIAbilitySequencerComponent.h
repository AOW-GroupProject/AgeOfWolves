#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "10_AI/AIDataSetInfos.h"

#include "AIAbilitySequencerComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAICombatPattern, Log, All);

//@전방 선언
#pragma region Forward Declaration
class UBaseAbilitySystemComponent;
class UGameplayAbility;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@어빌리티 활성화 요청 이벤트
DECLARE_DELEGATE_RetVal_OneParam(bool, FRequestActivateAbilityBlockUnit, const FGameplayTag&);
#pragma endregion

/**
 * @UAIAbilitySequencerComponent
 *
 * AI의 전투 패턴과 어빌리티 블록을 관리하는 컴포넌트입니다.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGEOFWOLVES_API UAIAbilitySequencerComponent : public UActorComponent
{
    GENERATED_BODY()

        //@친추 클래스
#pragma region Friend Class
#pragma endregion

//@Default Setting
#pragma region Default Setting
public:
    UAIAbilitySequencerComponent(const FObjectInitializer& ObjectInitializer);

protected:
    //~UActorComponent Interface
    virtual void OnRegister() override;
    virtual void OnUnregister() override;
    virtual void InitializeComponent() override;
    virtual void DestroyComponent(bool bPromoteChildren = false) override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of UActorComponent Interface

private:
    //@외부 바인딩
    void ExternalBindToASC(UBaseAbilitySystemComponent* ASC);

public:
    //@초기화
    UFUNCTION()
        void InitializeCombatPatternComponent();
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    //@블록 초기화
    void InitializeAbilityBlocks(const TArray<FAIAbilityBlock>& InBlocks);



protected:
    //@캐싱된 어빌리티 블록들
    UPROPERTY()
        TArray<FAIAbilityBlock> CachedAbilityBlocks;

    //@현재 실행 중인 블록/유닛 인덱스
    UPROPERTY()
        int32 CurrentBlockIndex;

    UPROPERTY()
        int32 CurrentUnitIndex;

    //@전투 준비 상태
    UPROPERTY()
        bool bCombatReady;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@어빌리티 활성화 요청 이벤트
    FRequestActivateAbilityBlockUnit RequestActivateAbilityBlockUnit;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@다음 어빌리티 유닛 활성화 시도
    UFUNCTION()
        bool OnRequestActivateAICombatLoop();

protected:
    //@ASC 이벤트 콜백
    void OnAbilityActivated(UGameplayAbility* Ability);
    void OnAbilityEnded(UGameplayAbility* Ability);
    void OnAbilityCancelled(UGameplayAbility* Ability);
#pragma endregion

    //@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
    //@인덱스 관리
    void AdvanceToNextUnit();
    void AdvanceToNextBlock();

public:
    //@상태 조회/설정
    FORCEINLINE bool IsCombatReady() const { return bCombatReady; }
    FORCEINLINE void SetCombatReady(bool bReady) { bCombatReady = bReady; }

    //@인덱스 getter
    FORCEINLINE int32 GetCurrentBlockIndex() const { return CurrentBlockIndex; }
    FORCEINLINE int32 GetCurrentUnitIndex() const { return CurrentUnitIndex; }

    //@블록 정보 getter
    FORCEINLINE const TArray<FAIAbilityBlock>& GetCachedAbilityBlocks() const { return CachedAbilityBlocks; }
#pragma endregion
};