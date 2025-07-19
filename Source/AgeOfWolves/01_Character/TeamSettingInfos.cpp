#include "TeamSettingInfos.h"
#include "Logging/StructuredLog.h"

#if WITH_EDITOR
#include "Editor.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Misc/MessageDialog.h"
#endif

DEFINE_LOG_CATEGORY(LogTeamSettingInfos)

//@Default Setting
#pragma region Default Setting
UTeamSettingInfos::UTeamSettingInfos(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

#if WITH_EDITOR
void UTeamSettingInfos::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FTeamValidationResult ValidationResult = ValidateTeamSettings();

    if (!ValidationResult.bIsValid)
    {
        //@에러 출력
        ShowValidationError(ValidationResult);

        //@자동 수정 시도
        if (!TryAutoFixValidationError(ValidationResult))
        {
            UE_LOGFMT(LogTeamSettingInfos, Warning,
                "팀 설정 무결성 검사 실패 - 수동 수정 필요: {0}",
                *ValidationResult.ErrorMessage);
        }
    }
}

void UTeamSettingInfos::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    FTeamValidationResult ValidationResult = ValidateTeamSettings();

    if (!ValidationResult.bIsValid)
    {
        ShowValidationError(ValidationResult);
        TryAutoFixValidationError(ValidationResult);
    }
}

FTeamValidationResult UTeamSettingInfos::ValidateTeamSettings() const
{

    // 1. 팀 ID 중복 검사 - 가장 기본적인 검사부터 시작
    FTeamValidationResult Result = ValidateUniqueTeamIds();
    if (!Result.bIsValid) return Result;

    // 2. 유효하지 않은 팀 ID 검사 - NoTeam 등의 무효한 값 확인
    Result = ValidateValidTeamIds();
    if (!Result.bIsValid) return Result;

    // 3. 타겟 팀 ID 중복 검사 - 같은 팀 내에서 중복 태도 설정 방지
    Result = ValidateUniqueTargetTeamIds();
    if (!Result.bIsValid) return Result;

    // 4. 자기 자신 대상 설정 방지 - 논리적으로 의미없는 설정 차단
    Result = ValidateNoSelfTargeting();
    if (!Result.bIsValid) return Result;

    // 5. 존재하지 않는 팀에 대한 태도 설정 방지 - 참조 무결성 검사
    Result = ValidateTargetTeamExistence();
    if (!Result.bIsValid) return Result;

    // 6. 상호 태도 일관성 검사 - 논리적 모순 방지 (선택적)
    Result = ValidateAttitudeConsistency();
    if (!Result.bIsValid) return Result;

    // 모든 검사를 통과한 경우
    return FTeamValidationResult(); // 성공 결과 반환
}

FTeamValidationResult UTeamSettingInfos::ValidateUniqueTeamIds() const
{
    //@TSet
    TSet<uint8> UsedTeamIds;

    for (int32 i = 0; i < TeamSettings.Num(); ++i)
    {
        const FTeamSettings& TeamSetting = TeamSettings[i];
        uint8 TeamIdValue = TeamSetting.TeamId.GetId();

        //@이미 사용된 팀 ID인지 확인
        if (UsedTeamIds.Contains(TeamIdValue))
        {
            return FTeamValidationResult(
                ETeamValidationError::DuplicateTeamId,
                FString::Printf(TEXT("중복된 팀 ID 발견: %d (인덱스 %d)"), TeamIdValue, i),
                i
            );
        }

        UsedTeamIds.Add(TeamIdValue);
    }

    return FTeamValidationResult(); // 성공
}

FTeamValidationResult UTeamSettingInfos::ValidateUniqueTargetTeamIds() const
{
    for (int32 TeamIndex = 0; TeamIndex < TeamSettings.Num(); ++TeamIndex)
    {
        const FTeamSettings& TeamSetting = TeamSettings[TeamIndex];
        TSet<uint8> UsedTargetTeamIds;

        for (int32 AttitudeIndex = 0; AttitudeIndex < TeamSetting.TeamAttitudes.Num(); ++AttitudeIndex)
        {
            const FTeamAttitudeSettings& AttitudeSetting = TeamSetting.TeamAttitudes[AttitudeIndex];
            uint8 TargetTeamIdValue = AttitudeSetting.TargetTeamId.GetId();

            if (UsedTargetTeamIds.Contains(TargetTeamIdValue))
            {
                return FTeamValidationResult(
                    ETeamValidationError::DuplicateTargetTeamId,
                    FString::Printf(TEXT("팀 %d에서 중복된 대상 팀 ID %d 발견 (태도 인덱스 %d)"),
                        TeamSetting.TeamId.GetId(), TargetTeamIdValue, AttitudeIndex),
                    TeamIndex,
                    AttitudeIndex
                );
            }

            UsedTargetTeamIds.Add(TargetTeamIdValue);
        }
    }

    return FTeamValidationResult();
}

FTeamValidationResult UTeamSettingInfos::ValidateNoSelfTargeting() const
{
    for (int32 TeamIndex = 0; TeamIndex < TeamSettings.Num(); ++TeamIndex)
    {
        const FTeamSettings& TeamSetting = TeamSettings[TeamIndex];

        for (int32 AttitudeIndex = 0; AttitudeIndex < TeamSetting.TeamAttitudes.Num(); ++AttitudeIndex)
        {
            const FTeamAttitudeSettings& AttitudeSetting = TeamSetting.TeamAttitudes[AttitudeIndex];

            if (TeamSetting.TeamId == AttitudeSetting.TargetTeamId)
            {
                return FTeamValidationResult(
                    ETeamValidationError::SelfTargetingTeam,
                    FString::Printf(TEXT("팀 %d가 자기 자신에 대한 태도를 설정하고 있습니다 (태도 인덱스 %d)"),
                        TeamSetting.TeamId.GetId(), AttitudeIndex),
                    TeamIndex,
                    AttitudeIndex
                );
            }
        }
    }

    return FTeamValidationResult(); 
}

FTeamValidationResult UTeamSettingInfos::ValidateValidTeamIds() const
{
    for (int32 TeamIndex = 0; TeamIndex < TeamSettings.Num(); ++TeamIndex)
    {
        const FTeamSettings& TeamSetting = TeamSettings[TeamIndex];

        if (TeamSetting.TeamId == FGenericTeamId::NoTeam)
        {
            return FTeamValidationResult(
                ETeamValidationError::InvalidTeamId,
                FString::Printf(TEXT("유효하지 않은 팀 ID (NoTeam) 발견 (팀 인덱스 %d)"), TeamIndex),
                TeamIndex
            );
        }

        //@Target Team IDs
        for (int32 AttitudeIndex = 0; AttitudeIndex < TeamSetting.TeamAttitudes.Num(); ++AttitudeIndex)
        {
            const FTeamAttitudeSettings& AttitudeSetting = TeamSetting.TeamAttitudes[AttitudeIndex];

            if (AttitudeSetting.TargetTeamId == FGenericTeamId::NoTeam)
            {
                return FTeamValidationResult(
                    ETeamValidationError::InvalidTeamId,
                    FString::Printf(TEXT("유효하지 않은 대상 팀 ID (NoTeam) 발견 (팀 %d, 태도 인덱스 %d)"),
                        TeamSetting.TeamId.GetId(), AttitudeIndex),
                    TeamIndex,
                    AttitudeIndex
                );
            }
        }
    }

    return FTeamValidationResult(); // 성공
}

FTeamValidationResult UTeamSettingInfos::ValidateTargetTeamExistence() const
{
    //@Tset
    TSet<uint8> RegisteredTeamIds;
    for (const FTeamSettings& TeamSetting : TeamSettings)
    {
        RegisteredTeamIds.Add(TeamSetting.TeamId.GetId());
    }

    for (int32 TeamIndex = 0; TeamIndex < TeamSettings.Num(); ++TeamIndex)
    {
        const FTeamSettings& TeamSetting = TeamSettings[TeamIndex];

        for (int32 AttitudeIndex = 0; AttitudeIndex < TeamSetting.TeamAttitudes.Num(); ++AttitudeIndex)
        {
            const FTeamAttitudeSettings& AttitudeSetting = TeamSetting.TeamAttitudes[AttitudeIndex];
            uint8 TargetTeamIdValue = AttitudeSetting.TargetTeamId.GetId();

            if (!RegisteredTeamIds.Contains(TargetTeamIdValue))
            {
                return FTeamValidationResult(
                    ETeamValidationError::OrphanedTargetTeam,
                    FString::Printf(TEXT("존재하지 않는 대상 팀 %d에 대한 태도 설정 발견 (팀 %d, 태도 인덱스 %d)"),
                        TargetTeamIdValue, TeamSetting.TeamId.GetId(), AttitudeIndex),
                    TeamIndex,
                    AttitudeIndex
                );
            }
        }
    }

    return FTeamValidationResult();
}

FTeamValidationResult UTeamSettingInfos::ValidateAttitudeConsistency() const
{
    for (int32 TeamIndex1 = 0; TeamIndex1 < TeamSettings.Num(); ++TeamIndex1)
    {
        const FTeamSettings& TeamSetting1 = TeamSettings[TeamIndex1];

        for (const FTeamAttitudeSettings& AttitudeSetting1 : TeamSetting1.TeamAttitudes)
        {
            //@TeamSetting1이 다른 팀에 대해 가지는 태도
            FGenericTeamId Team1Id = TeamSetting1.TeamId;
            FGenericTeamId Team2Id = AttitudeSetting1.TargetTeamId;
            ETeamAttitude::Type Team1ToTeam2Attitude = AttitudeSetting1.Attitude;

            for (int32 TeamIndex2 = 0; TeamIndex2 < TeamSettings.Num(); ++TeamIndex2)
            {
                const FTeamSettings& TeamSetting2 = TeamSettings[TeamIndex2];

                if (TeamSetting2.TeamId == Team2Id)
                {
                    for (const FTeamAttitudeSettings& AttitudeSetting2 : TeamSetting2.TeamAttitudes)
                    {
                        if (AttitudeSetting2.TargetTeamId == Team1Id)
                        {
                            ETeamAttitude::Type Team2ToTeam1Attitude = AttitudeSetting2.Attitude;

                            //@심각한 불일치 검사 (적대 vs 우호)
                            if ((Team1ToTeam2Attitude == ETeamAttitude::Hostile && Team2ToTeam1Attitude == ETeamAttitude::Friendly) ||
                                (Team1ToTeam2Attitude == ETeamAttitude::Friendly && Team2ToTeam1Attitude == ETeamAttitude::Hostile))
                            {
                                return FTeamValidationResult(
                                    ETeamValidationError::InconsistentAttitude,
                                    FString::Printf(TEXT("팀 %d와 팀 %d 간의 태도가 심각하게 불일치합니다 (%s vs %s)"),
                                        Team1Id.GetId(), Team2Id.GetId(),
                                        *UEnum::GetValueAsString(Team1ToTeam2Attitude),
                                        *UEnum::GetValueAsString(Team2ToTeam1Attitude)),
                                    TeamIndex1
                                );
                            }
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }

    return FTeamValidationResult(); // 성공
}

void UTeamSettingInfos::ShowValidationError(const FTeamValidationResult& ValidationResult) const
{
    UE_LOGFMT(LogTeamSettingInfos, Error, "팀 설정 검증 실패: {0}", *ValidationResult.ErrorMessage);
}

bool UTeamSettingInfos::TryAutoFixValidationError(const FTeamValidationResult& ValidationResult)
{

    switch (ValidationResult.ErrorType)
    {
    case ETeamValidationError::SelfTargetingTeam:
    {
        // 자기 자신을 대상으로 하는 태도 설정은 명확히 잘못된 것이므로 안전하게 제거 가능
        if (ValidationResult.ProblemTeamIndex >= 0 && ValidationResult.ProblemAttitudeIndex >= 0)
        {
            TeamSettings[ValidationResult.ProblemTeamIndex].TeamAttitudes.RemoveAt(ValidationResult.ProblemAttitudeIndex);
            UE_LOGFMT(LogTeamSettingInfos, Log, "자기 자신을 대상으로 하는 태도 설정을 자동 제거했습니다");
            return true;
        }
        break;
    }

    case ETeamValidationError::InvalidTeamId:
    {
        // NoTeam ID는 유효한 값으로 대체 가능 (사용되지 않는 가장 작은 ID 찾기)
        if (ValidationResult.ProblemTeamIndex >= 0)
        {
            uint8 NewTeamId = FindAvailableTeamId();
            TeamSettings[ValidationResult.ProblemTeamIndex].TeamId = FGenericTeamId(NewTeamId);
            UE_LOGFMT(LogTeamSettingInfos, Log, "무효한 팀 ID를 {0}으로 자동 변경했습니다", NewTeamId);
            return true;
        }
        break;
    }

    default:
    {
        return false;
    }
    }

    return false;
}

uint8 UTeamSettingInfos::FindAvailableTeamId() const
{
    TSet<uint8> UsedIds;
    for (const FTeamSettings& TeamSetting : TeamSettings)
    {
        UsedIds.Add(TeamSetting.TeamId.GetId());
    }

    for (uint8 Id = 0; Id < 255; ++Id)
    {
        if (!UsedIds.Contains(Id))
        {
            return Id;
        }
    }

    return 0;
}

void UTeamSettingInfos::ValidateTeamSettingsManually()
{
    FTeamValidationResult ValidationResult = ValidateTeamSettings();

    if (ValidationResult.bIsValid)
    {
        UE_LOGFMT(LogTeamSettingInfos, Log, "팀 설정 무결성 검사 성공: 모든 설정이 유효합니다");
    }
    else
    {
        ShowValidationError(ValidationResult);
    }
}
#endif

#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc

ETeamAttitude::Type UTeamSettingInfos::GetTeamAttitude(FGenericTeamId FromTeam, FGenericTeamId ToTeam) const
{
    if (FromTeam == ToTeam)
    {
        return ETeamAttitude::Friendly;
    }

    for (const FTeamSettings& TeamSetting : TeamSettings)
    {
        if (TeamSetting.TeamId == FromTeam)
        {
            for (const FTeamAttitudeSettings& AttitudeSetting : TeamSetting.TeamAttitudes)
            {
                if (AttitudeSetting.TargetTeamId == ToTeam)
                {
                    return AttitudeSetting.Attitude;
                }
            }
            break;
        }
    }

    return ETeamAttitude::Neutral;
}

TArray<FGenericTeamId> UTeamSettingInfos::GetAllTeamIds() const
{
    TArray<FGenericTeamId> TeamIds;

    for (const FTeamSettings& TeamSetting : TeamSettings)
    {
        TeamIds.Add(TeamSetting.TeamId);
    }

    return TeamIds;
}

bool UTeamSettingInfos::IsTeamRegistered(FGenericTeamId TeamId) const
{
    for (const FTeamSettings& TeamSetting : TeamSettings)
    {
        if (TeamSetting.TeamId == TeamId)
        {
            return true;
        }
    }

    return false;
}

#pragma endregion