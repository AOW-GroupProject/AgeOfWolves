#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameFramework/WorldSettings.h"

#include "ANS_ApplyDilation.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_TimeDilation, Log, All);

//@전방 선언
#pragma region Forward Declaration
class USkeletalMeshComponent;
class UAnimSequenceBase;
#pragma endregion

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
    FastMotion UMETA(DisplayName = "빠른 모션")
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
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "타임 딜레이션")
        bool bSmoothTransition = true;

    //@전환 시간 (초)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "타임 딜레이션", meta = (EditCondition = "bSmoothTransition", ClampMin = "0.01", ClampMax = "1.0"))
        float TransitionDuration = 0.2f;
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UANS_ApplyDilation
 *
 *	애니메이션 특정 재생 구간 동안 글로벌 타임 딜레이션을 구현합니다.
 */
UCLASS(meta = (DisplayName = "타임 딜레이션 적용"))
class AGEOFWOLVES_API UANS_ApplyDilation : public UAnimNotifyState
{
    //@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    //@생성자
    UANS_ApplyDilation();

public:
    //@애니메이션 노티파이 인터페이스 구현
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

 //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
    //@모드와 강도에 따른 타임 딜레이션 값 계산
    float CalculateTimeDilationValue() const;

    //@타임 딜레이션 적용 (부드러운 전환 지원)
    void ApplyTimeDilation(USkeletalMeshComponent* MeshComp, float TargetValue);

    //@타임 딜레이션 복원 (부드러운 전환 지원)
    void RestoreTimeDilation(USkeletalMeshComponent* MeshComp);

private:
    //@글로벌 타임 딜레이션 적용 여부 (true: 전체 월드, false: 캐릭터만)
    UPROPERTY(EditAnywhere, Category = "타임 딜레이션", meta = (DisplayName = "글로벌 적용"))
        bool bApplyGlobalDilation = true;

    //@타임 딜레이션 설정
    UPROPERTY(EditAnywhere, Category = "타임 딜레이션", meta = (DisplayName = "딜레이션 설정"))
        FTimeDilationSettings DilationSettings;

    //@원래 타임 딜레이션 값 (복원용)
    float OriginalTimeDilation = 1.0f;

    //@목표 타임 딜레이션 값
    float TargetTimeDilation = 1.0f;

    //@전환 타이머 핸들
    FTimerHandle TransitionTimerHandle;
#pragma endregion

    //@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
private:
    //@타이머 콜백: 전환 진행
    void OnTransitionTick(USkeletalMeshComponent* MeshComp, float StartValue, float EndValue, float Duration);

private:
    UFUNCTION()
        void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    virtual FString GetNotifyName_Implementation() const override;
#pragma endregion


};