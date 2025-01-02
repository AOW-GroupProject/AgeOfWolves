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

    // Team Setting Info �ε�
    const FString AssetPath = TEXT("/Game/Blueprints/01_Character/DA_BaseTeamSettings");
    TeamSettingInfos = LoadObject<UTeamSettingInfos>(nullptr, *AssetPath);

    if (!TeamSettingInfos)
    {
        UE_LOGFMT(LogTeamManager, Warning, "�� ���� ������ ã�� �� �����ϴ�. ���: {0}", AssetPath);
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

    //@�� ���� �����κ��� ĳ�� ����
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
    // ĳ�ÿ��� �µ� ��ȸ
    const TMap<uint8, TEnumAsByte<ETeamAttitude::Type>>* AttitudeMap = TeamAttitudeCache.Find(SourceTeamId);
    if (!AttitudeMap)
    {
        return ETeamAttitude::Neutral;
    }

    const TEnumAsByte<ETeamAttitude::Type>* Attitude = AttitudeMap->Find(TargetTeamId);
    return Attitude ? *Attitude : ETeamAttitude::Neutral;
}