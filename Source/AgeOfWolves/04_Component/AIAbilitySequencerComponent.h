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
struct FAIDataSet;
struct FAICombatSequence;
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

//@전투 패턴 Exit Block 완료 통지
DECLARE_DELEGATE_RetVal(bool, FNotifyCombatPatternExitComplete);
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
    void ExternalBindToAIController();

public:
    //@초기화
    UFUNCTION()
        void InitializeCombatPatternComponent();
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    //@Exit Block 실행 시작
    bool StartExitBlock();
    //@Exit Block 유닛들 순차 실행
    UFUNCTION()
        void ExecuteExitBlockUnits();

protected:
    //@캐싱된 전투 시퀀스
    UPROPERTY()
        FAICombatSequence CachedCombatSequence;

    //@현재 활성화 된 블록 유닛의 어빌리티 태그
    UPROPERTY()
        FGameplayTag CurrentActivatingUnitTag;

    //@현재 실행 중인 블록/유닛 인덱스
    //@brief -1은 Start Block, 0~N 은 Ability Blocks 내부의 블록 인덱스 의미
    UPROPERTY()
        int32 CurrentBlockIndex;

    UPROPERTY()
        int32 CurrentUnitIndex;

    //@전투 준비 상태
    UPROPERTY()
        bool bCombatReady;

    //@최초 실행 여부
    UPROPERTY()
        bool bIsFirstRun;

    //@현재 유닛 완료 후 Exit Block 시작 대기 상태
    UPROPERTY()
        bool bWaitingForExitBlock;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@어빌리티 활성화 요청 이벤트
    FRequestActivateAbilityBlockUnit RequestActivateAbilityBlockUnit;

public:
    //@전투 패턴 Exit Block 완료 통지 이벤트
    FNotifyCombatPatternExitComplete NotifyCombatPatternExitComplete;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@다음 어빌리티 유닛 활성화 시도
    UFUNCTION()
        bool OnRequestActivateAICombatLoop();

    //@전투 패턴 활성화 종료 이벤트 구독
    UFUNCTION()
        bool OnRequestEndCombatPattern();

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

private:
    //@Start Block 실행 중인지 확인
    FORCEINLINE bool IsExecutingStartBlock() const
    {
        return bIsFirstRun && CurrentBlockIndex == -1;
    }

    //@AbilityBlocks 실행 중인지 확인
    FORCEINLINE bool IsExecutingAbilityBlocks() const
    {
        return CurrentBlockIndex >= 0;
    }

    //@Exit Block 실행 중인지 확인
    FORCEINLINE bool IsExecutingExitBlock() const
    {
        return CurrentBlockIndex == -2;
    }

    //@Exit Block 실행 상태로 설정
    FORCEINLINE void SetExecutingExitBlock()
    {
        CurrentBlockIndex = -2;
    }

private:
    //@AI Combat Pattern에서 관리하는 블록 유닛의 어빌리티 태그인지 체크
    bool ValidateAbilityTag(const UGameplayAbility* Ability) const;

public:
    //@상태 조회/설정
    FORCEINLINE bool IsCombatReady() const { return bCombatReady; }
    FORCEINLINE void SetCombatReady(bool bReady) { bCombatReady = bReady; }

    //@인덱스 getter
    FORCEINLINE int32 GetCurrentBlockIndex() const { return CurrentBlockIndex; }
    FORCEINLINE int32 GetCurrentUnitIndex() const { return CurrentUnitIndex; }

    //@블록 정보 getter
    FORCEINLINE const FAICombatSequence& GetCachedCachedCombatSequence() const { return CachedCombatSequence; }
#pragma endregion
};