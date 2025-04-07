#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "TimeManipulationSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTimeManipulation, Log, All);

//@열거형
#pragma region Enums
/**
 * @UENUM 타임 딜레이션 모드
 * 재생 속도를 조절하는 방식을 결정합니다.
 */
UENUM(BlueprintType)
enum class ETimeDilationMode : uint8
{
    SlowMotion UMETA(DisplayName = "슬로우 모션"),
    FastMotion UMETA(DisplayName = "빠른 모션"),
    Stop       UMETA(DisplayName = "완전 정지"),
    HitStop    UMETA(DisplayName = "히트 스톱")
};
/**
 * @UENUM 타임 딜레이션 강도
 * 재생 속도 변화의 정도를 결정합니다.
 */
UENUM(BlueprintType)
enum class ETimeDilationIntensity : uint8
{
    Low UMETA(DisplayName = "약"),
    Medium UMETA(DisplayName = "중"),
    High UMETA(DisplayName = "강")
};
#pragma endregion

//@구조체
#pragma region Structs
/**
 * @USTRUCT 타임 딜레이션 설정
 * 타임 딜레이션 관련 설정을 모아놓은 구조체입니다.
 */
USTRUCT(BlueprintType)
struct FTimeDilationSettings
{
    GENERATED_BODY()

    //@타임 딜레이션 모드
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "타임 딜레이션")
        ETimeDilationMode DilationMode = ETimeDilationMode::SlowMotion;

    //@타임 딜레이션 강도
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "타임 딜레이션")
        ETimeDilationIntensity DilationIntensity = ETimeDilationIntensity::Medium;

    //@커스텀 타임 딜레이션 값 (직접 설정 시 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "타임 딜레이션", meta = (ClampMin = "0.01", ClampMax = "10.0"))
        float CustomDilationValue = 1.0f;

    //@부드러운 전환 사용 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "타임 딜레이션",
        meta = (EditCondition = "DilationMode != ETimeDilationMode::HitStop"))
        bool bSmoothTransition = true;

    //@전환 시간 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "타임 딜레이션",
        meta = (EditCondition = "bSmoothTransition && DilationMode != ETimeDilationMode::HitStop", ClampMin = "0.01", ClampMax = "1.0"))
        float TransitionDuration = 0.2f;

    //@Stop 모드일 때 지속 시간 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "타임 딜레이션",
        meta = (EditCondition = "DilationMode == ETimeDilationMode::Stop", ClampMin = "0.01", ClampMax = "1.0"))
        float StopDuration = 0.1f;
};

/**
*
*   @FTimeDilationInfo
*
*   타임 딜레이션 정보를 저장하는 구조체
*/
USTRUCT()
struct FTimeDilationInfo
{
    GENERATED_BODY()

     //@원래 타임 딜레이션 값
    float OriginalDilation = 1.0f;

    //@목표 타임 딜레이션 값
    float TargetDilation = 1.0f;

    //@글로벌 적용 여부
    bool bGlobal = false;

    //@적용 시작 시간
    float StartTime = 0.0f;

    //@전환 시간 (초)
    float TransitionDuration = 0.0f;

    //@부드러운 전환 사용 여부
    bool bSmoothTransition = false;
};
#pragma endregion

/**
 *  @UTimeManipulationSubsystem
 *
 *  게임 플레이의 시간 조작과 관련된 작업들을 처리하는 서브시스템
 */
UCLASS()
class AGEOFWOLVES_API UTimeManipulationSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
#pragma region Friend Class
    friend class UAttackGameplayAbility;
    friend class UANS_ApplyDilation;
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool IsTickable() const override { return true; }
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UTimeManipulationSubsystem, STATGROUP_Tickables); }
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
     //@타임 딜레이션 값을 계산합니다.
    UFUNCTION(BlueprintCallable, Category = "Time Manipulation")
        float CalculateTimeDilationValue(const FTimeDilationSettings& Settings) const;

protected:
    //@타임 딜레이션 시작
    void StartTimeDilation(AActor* Owner, const FTimeDilationSettings& Settings, bool bGlobal);
    //@타임 딜레이션 종료
    void StopTimeDilation(AActor* Owner, bool bSmoothTransition, float TransitionDuration);

public:
    //@글로벌 타임 딜레이션 시작
    UFUNCTION(BlueprintCallable, Category = "Time Manipulation|Global")
        void StartGlobalTimeDilation(const FTimeDilationSettings& Settings);

    //@글로벌 타임 딜레이션 종료
    UFUNCTION(BlueprintCallable, Category = "Time Manipulation|Global")
        void StopGlobalTimeDilation(bool bSmoothTransition = true, float TransitionDuration = 0.2f);

public:
    //@로컬 타임 딜레이션 시작
    UFUNCTION(BlueprintCallable, Category = "Time Manipulation|Local")
        void StartLocalTimeDilation(AActor* Owner, const FTimeDilationSettings& Settings);

    //@로컬 타임 딜레이션 종료
    UFUNCTION(BlueprintCallable, Category = "Time Manipulation|Local")
        void StopLocalTimeDilation(AActor* Owner, bool bSmoothTransition = true, float TransitionDuration = 0.2f);

protected:
    //@히트 스톱 효과를 적용합니다.
    UFUNCTION(BlueprintCallable, Category = "Time Manipulation")
        void ApplyHitStop(AActor* Owner, const FTimeDilationSettings& Settings, bool bGlobal);

     //@히트 스톱 효과를 적용합니다.
    void ApplyHitStop(AActor* Owner, int32 FrameCount = 2, bool bGlobal = false);

protected:
    //@타임 딜레이션 업데이트
    void UpdateTimeDilation(AActor* Owner, FTimeDilationInfo& DilationInfo, float DeltaTime);

    //@타임 딜레이션 설정
    void SetTimeDilation(AActor* Owner, const FTimeDilationInfo& DilationInfo, float Value);

protected:
    //@애니메이션 인스턴스 등록
    void RegisterAnimInstance(AActor* Owner);

    //@애니메이션 인스턴스 등록 해제
    void UnregisterAnimInstance(AActor* Owner);

private:
    //@현재 적용 중인 타임 딜레이션 정보
    TMap<AActor*, FTimeDilationInfo> ActiveDilations;

    //@전환 중인 타임 딜레이션 정보
    TMap<AActor*, FTimeDilationInfo> TransitioningDilations;

    //@애니메이션 인스턴스와 소유자 매핑
    TMap<UAnimInstance*, AActor*> AnimInstanceOwners;

    //@타임 딜레이션 타이머 핸들 맵
    TMap<AActor*, FTimerHandle> TimeDilationTimerHandles;

private:
    //@현재 활성화된 시간 조작 소유자
    TWeakObjectPtr<AActor> ActiveDilationOwner;

    //@이전 요청을 무시할지 여부
    UPROPERTY(EditAnywhere, Category = "Time Manipulation")
        bool bIgnorePreviousRequests = true;
#pragma endregion

    //@Callbacks
#pragma region Callbacks
private:
    //@몽타주 종료 이벤트 콜백
    UFUNCTION()
        void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

    //@히트 스톱 타이머 만료 콜백
    UFUNCTION()
        void OnHitStopTimerExpired(AActor* Owner);

    //@스톱 모드 타이머 만료 콜백
    UFUNCTION()
        void OnStopModeTimerExpired(AActor* Owner, bool bSmoothTransition, float TransitionDuration);
#pragma endregion

    //@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    //@해당 액터가 현재 타입 딜레이션 적용중인지 체크
    UFUNCTION(BlueprintCallable, Category = "Time Manipulation")
        bool IsActorTimeDilated(AActor* Owner) const;
#pragma endregion
};