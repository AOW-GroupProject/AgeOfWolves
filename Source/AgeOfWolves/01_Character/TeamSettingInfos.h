#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GenericTeamAgentInterface.h"
#include "TeamSettingInfos.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTeamSettingInfos, Log, All)

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
/**
 * @ETeamValidationError
 *
 * 팀 설정 검증 과정에서 발생할 수 있는 오류 유형을 정의합니다.
 * 이 열거형을 통해 어떤 종류의 오류가 발생했는지 명확하게 파악할 수 있습니다.
 */
UENUM(BlueprintType)
enum class ETeamValidationError : uint8
{
    None                    UMETA(DisplayName = "오류 없음"),
    DuplicateTeamId         UMETA(DisplayName = "중복된 팀 ID"),
    DuplicateTargetTeamId   UMETA(DisplayName = "중복된 대상 팀 ID"),
    SelfTargetingTeam       UMETA(DisplayName = "자기 자신을 대상으로 하는 팀"),
    InvalidTeamId           UMETA(DisplayName = "유효하지 않은 팀 ID"),
    OrphanedTargetTeam      UMETA(DisplayName = "존재하지 않는 대상 팀"),
    InconsistentAttitude    UMETA(DisplayName = "상호 태도 불일치")
};
#pragma endregion

//@구조체
#pragma region Structs
/**
 *	@FTeamAttitudeSettings
 *
 *	팀 간의 관계를 정의하는 구조체
 */
USTRUCT(BlueprintType)
struct FTeamAttitudeSettings
{
    GENERATED_BODY()

public:
    //@대상 팀 ID
    UPROPERTY(EditDefaultsOnly, Category = "Team Settings")
    FGenericTeamId TargetTeamId;

    //@대상 팀에 대한 태도
    UPROPERTY(EditDefaultsOnly, Category = "Team Settings")
    TEnumAsByte<ETeamAttitude::Type> Attitude;

public:
    FTeamAttitudeSettings()
        : TargetTeamId(FGenericTeamId::NoTeam)
        , Attitude(ETeamAttitude::Neutral)
    {
    }

    FTeamAttitudeSettings(FGenericTeamId InTargetTeamId, ETeamAttitude::Type InAttitude)
        : TargetTeamId(InTargetTeamId)
        , Attitude(InAttitude)
    {
    }
};

/**
 *	@FTeamSettings
 *
 *	팀 설정을 정의하는 구조체
 */
USTRUCT(BlueprintType)
struct FTeamSettings
{
    GENERATED_BODY()

public:
    //@팀 ID
    UPROPERTY(EditDefaultsOnly, Category = "Team Settings")
    FGenericTeamId TeamId;

    //@다른 팀들에 대한 태도 설정
    UPROPERTY(EditDefaultsOnly, Category = "Team Settings")
    TArray<FTeamAttitudeSettings> TeamAttitudes;

public:
    FTeamSettings()
        : TeamId(FGenericTeamId::NoTeam)
    {
    }

    FTeamSettings(FGenericTeamId InTeamId)
        : TeamId(InTeamId)
    {
    }
};

/**
 * @FTeamValidationResult
 *
 * 팀 설정 검증 결과를 담는 구조체입니다.
 * 어떤 오류가 어디서 발생했는지 상세한 정보를 제공합니다.
 */
USTRUCT(BlueprintType)
struct FTeamValidationResult
{
    GENERATED_BODY()

public:
    //@검증 결과 (성공/실패)
    UPROPERTY(BlueprintReadOnly)
    bool bIsValid = true;

    //@오류 유형
    UPROPERTY(BlueprintReadOnly)
    ETeamValidationError ErrorType = ETeamValidationError::None;

    //@오류 메시지
    UPROPERTY(BlueprintReadOnly)
    FString ErrorMessage;

    //@문제가 발생한 팀 인덱스 (TeamSettings 배열에서의 위치)
    UPROPERTY(BlueprintReadOnly)
    int32 ProblemTeamIndex = -1;

    //@문제가 발생한 태도 인덱스 (TeamAttitudes 배열에서의 위치)
    UPROPERTY(BlueprintReadOnly)
    int32 ProblemAttitudeIndex = -1;

public:
    FTeamValidationResult()
    {
    }
    FTeamValidationResult(ETeamValidationError InErrorType, const FString& InErrorMessage,
        int32 InTeamIndex = -1, int32 InAttitudeIndex = -1)
        : bIsValid(false)
        , ErrorType(InErrorType)
        , ErrorMessage(InErrorMessage)
        , ProblemTeamIndex(InTeamIndex)
        , ProblemAttitudeIndex(InAttitudeIndex)
    {
    }
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UTeamSettingInfos
 *
 *	캐릭터 간 팀 세팅 정보를 관리하는 Data Asset
 *  에디터에서 실시간 무결성 검사를 통해 설정 오류를 방지합니다.
 */
UCLASS()
class AGEOFWOLVES_API UTeamSettingInfos : public UPrimaryDataAsset
{
    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UTeamSettingInfos(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

protected:
#if WITH_EDITOR
    //@무결성 검사
    FTeamValidationResult ValidateTeamSettings() const;

    //@팀 ID 검증
    FTeamValidationResult ValidateUniqueTeamIds() const;
    //@타겟 팀 ID 고유성 검증
    FTeamValidationResult ValidateUniqueTargetTeamIds() const;
    //@자기 자신을 타겟으로 삼았는지 체크 
    FTeamValidationResult ValidateNoSelfTargeting() const;
    //@팀 ID가 유효한지 검증
    FTeamValidationResult ValidateValidTeamIds() const;
    //@타겟 팀 ID가 존재하는지 체크
    FTeamValidationResult ValidateTargetTeamExistence() const;
    //@일관성 검사
    FTeamValidationResult ValidateAttitudeConsistency() const;

    //@검증 실패 시 에러 출력
    void ShowValidationError(const FTeamValidationResult& ValidationResult) const;

    //@검증 실패 시 자동 수정 진행.
    bool TryAutoFixValidationError(const FTeamValidationResult& ValidationResult);
#endif

#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    //@팀 설정 정보 목록
    UPROPERTY(EditDefaultsOnly, Category = "팀 설정", meta = (TitleProperty = "TeamId"))
    TArray<FTeamSettings> TeamSettings;

public:
    //@팀 ID 찾기
    UFUNCTION(BlueprintCallable, Category = "Team Settings")
    uint8 FindAvailableTeamId() const;

    UFUNCTION(BlueprintCallable, Category = "Team Settings")
    ETeamAttitude::Type GetTeamAttitude(FGenericTeamId FromTeam, FGenericTeamId ToTeam) const;

    UFUNCTION(BlueprintCallable, Category = "Team Settings")
    TArray<FGenericTeamId> GetAllTeamIds() const;

    UFUNCTION(BlueprintCallable, Category = "Team Settings")
    bool IsTeamRegistered(FGenericTeamId TeamId) const;

#if WITH_EDITOR
    //@에디터에서 수동으로 무결성 검사를 실행할 수 있는 함수
    UFUNCTION(CallInEditor, Category = "Team Validation")
    void ValidateTeamSettingsManually();
#endif

#pragma endregion

};