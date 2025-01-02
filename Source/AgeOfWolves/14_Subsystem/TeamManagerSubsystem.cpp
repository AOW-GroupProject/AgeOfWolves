#include "TeamManagerSubsystem.h"
#include "Logging/StructuredLog.h"

#include "01_Character/TeamSettingInfos.h"

DEFINE_LOG_CATEGORY(LogTeamManager)

UTeamManagerSubsystem::UTeamManagerSubsystem()
{
	TeamSettingInfos = nullptr;
}

void UTeamManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    // Team Setting Info 로드
    const FString AssetPath = TEXT("/Game/Blueprints/01_Character/DA_BaseTeamSettings");
    TeamSettingInfos = LoadObject<UTeamSettingInfos>(nullptr, *AssetPath);

    if (!TeamSettingInfos)
    {
        UE_LOGFMT(LogTeamManager, Warning, "팀 설정 정보를 찾을 수 없습니다. 경로: {0}", AssetPath);
        return;
    }

    InitializeTeamAttitudeCache();
}

void UTeamManagerSubsystem::InitializeTeamAttitudeCache()
{
    if (!TeamSettingInfos)
    {
        return;
    }

    TeamAttitudeCache.Empty();

    //@팀 설정 정보로부터 캐시 구축
    for (const FTeamSettings& TeamSetting : TeamSettingInfos->TeamSettings)
    {
        TMap<uint8, TEnumAsByte<ETeamAttitude::Type>>& AttitudeMap = TeamAttitudeCache.Add(TeamSetting.TeamId);

        for (const FTeamAttitudeSettings& AttitudeSetting : TeamSetting.TeamAttitudes)
        {
            AttitudeMap.Add(AttitudeSetting.TargetTeamId.GetId(), AttitudeSetting.Attitude);
        }
    }
}

ETeamAttitude::Type UTeamManagerSubsystem::GetAttitude(uint8 SourceTeamId, uint8 TargetTeamId) const
{
    // 캐시에서 태도 조회
    const TMap<uint8, TEnumAsByte<ETeamAttitude::Type>>* AttitudeMap = TeamAttitudeCache.Find(SourceTeamId);
    if (!AttitudeMap)
    {
        return ETeamAttitude::Neutral;
    }

    const TEnumAsByte<ETeamAttitude::Type>* Attitude = AttitudeMap->Find(TargetTeamId);
    return Attitude ? *Attitude : ETeamAttitude::Neutral;
}