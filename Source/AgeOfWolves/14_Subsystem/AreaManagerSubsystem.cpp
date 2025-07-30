#include "AreaManagerSubsystem.h"
#include "Logging/StructuredLog.h"

#include "16_Level/Area.h"
#include "16_Level/AreaQuestDataInfos.h"
#include "Components/SplineComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogAreaManager, Log, All);

//@Defualt Setting
#pragma region Default Setting
UAreaManagerSubsystem::UAreaManagerSubsystem()
{
    AreaQuestDataInfos = nullptr;
}

void UAreaManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    UE_LOGFMT(LogAreaManager, Log, "AreaManagerSubsystem 초기화");

    //@Load
    const FString AssetPath = TEXT("/Game/Blueprints/Level/00_Area/DA_AreaQuestDataInformations");
    AreaQuestDataInfos = LoadObject<UAreaQuestDataInfos>(nullptr, *AssetPath);

    //@AreaQuest Data Infos Infos
    if (!AreaQuestDataInfos)
    {
        UE_LOGFMT(LogAreaManager, Warning, "AreaQuest Data Infos 로드 실패. 경로: {0}", AssetPath);
        return;
    }

    UE_LOGFMT(LogAreaManager, Log, "AreaQuest Data Infos 로드 완료");
}

void UAreaManagerSubsystem::Deinitialize()
{
    UE_LOGFMT(LogAreaManager, Log, "AreaManagerSubsystem 종료");
    RegisteredAreas.Empty();
    Super::Deinitialize();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UAreaManagerSubsystem::RegisterArea(AArea* Area)
{
    if (!Area)
    {
        return;
    }

    const FGuid& AreaID = Area->GetAreaID();
    if (!AreaID.IsValid())
    {
        UE_LOGFMT(LogAreaManager, Warning, "유효하지 않은 ID를 가진 Area 등록 시도");
        return;
    }

    RegisteredAreas.Add(AreaID, Area);

    UE_LOGFMT(LogAreaManager, Log, "Area {0} 등록됨", AreaID.ToString());

}

void UAreaManagerSubsystem::UnregisterArea(AArea* Area)
{
    if (!Area)
    {
        return;
    }

    const FGuid& AreaID = Area->GetAreaID();
    if (RegisteredAreas.Remove(AreaID) > 0)
    {
        UE_LOGFMT(LogAreaManager, Log, "Area {0} 등록 해제됨", AreaID.ToString());
    }
}
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
AArea* UAreaManagerSubsystem::GetAreaByID(const FGuid& AreaID) const
{
    if (!AreaID.IsValid())
    {
        return nullptr;
    }

    if (AArea* const* FoundArea = RegisteredAreas.Find(AreaID))
    {
        return *FoundArea;
    }

    return nullptr;
}

FGuid UAreaManagerSubsystem::GetAIGroupID(AActor* AIActor) const
{
    if (!AIActor)
    {
        return FGuid();
    }

    // 모든 등록된 Area 검색
    for (const auto& AreaPair : RegisteredAreas)
    {
        AArea* Area = AreaPair.Value;
        if (Area)
        {
            FGuid GroupID = Area->GetAIGroupID(AIActor);
            if (GroupID.IsValid())
            {
                return GroupID;
            }
        }
    }

    return FGuid();
}

TArray<FAIGroupInfo> UAreaManagerSubsystem::GetAllGroups() const
{
    TArray<FAIGroupInfo> AllGroups;

    for (const auto& AreaPair : RegisteredAreas)
    {
        AArea* Area = AreaPair.Value;
        if (Area)
        {
            TArray<FAIGroupInfo> AreaGroups = Area->GetAllAIGroupsAsArray();
            AllGroups.Append(AreaGroups);
        }
    }

    return AllGroups;
}

bool UAreaManagerSubsystem::GetGroupInfo(const FGuid& GroupID, FAIGroupInfo& OutGroupInfo) const
{
    if (!GroupID.IsValid())
    {
        return false;
    }

    for (const auto& AreaPair : RegisteredAreas)
    {
        AArea* Area = AreaPair.Value;
        if (Area)
        {
            TArray<FAIGroupInfo> AreaGroups = Area->GetAllAIGroupsAsArray();
            for (const FAIGroupInfo& GroupInfo : AreaGroups)
            {
                if (GroupInfo.GroupID == GroupID)
                {
                    OutGroupInfo = GroupInfo;
                    return true;
                }
            }
        }
    }

    return false;
}

AActor* UAreaManagerSubsystem::GetPatrolPathForGroup(const FGuid& GroupID) const
{
    FAIGroupInfo GroupInfo;
    if (GetGroupInfo(GroupID, GroupInfo))
    {
        return GroupInfo.PatrolPath;
    }

    return nullptr;
}

TArray<FAreaAIInfo> UAreaManagerSubsystem::GetAreaAliveAIInfos(const FGuid& AreaID) const
{
    TArray<FAreaAIInfo> AliveAIInfos;
    
    AArea* Area = GetAreaByID(AreaID);
    if (!Area)
    {
        UE_LOGFMT(LogAreaManager, Warning, "AreaID({0})에 해당하는 Area를 찾을 수 없습니다.", *AreaID.ToString());
        return AliveAIInfos;
    }
    
    TArray<FAreaAIInfo> AllAIInfos = Area->GetAreaAIInfos();
    
    // 살아있는 AI만 필터링
    for (const FAreaAIInfo& AIInfo : AllAIInfos)
    {
        if (AIInfo.AIActor.IsValid() && 
            !AIInfo.CurrentState.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead")))
        {
            AliveAIInfos.Add(AIInfo);
        }
    }
    
    return AliveAIInfos;
}

FAreaQuestDataSet UAreaManagerSubsystem::GetAreaQuestDataSet(FGameplayTag AreaTag) const
{
    if (!AreaQuestDataInfos)
    {
        UE_LOGFMT(LogAreaManager, Warning, "AreaQuestDataInfos가 유효하지 않습니다.");
        return FAreaQuestDataSet();
    }

    for (const FAreaQuestDataSet& DataSet : AreaQuestDataInfos->AreaQuestSets)
    {
        if (DataSet.AreaTag == AreaTag)
        {
            return DataSet;
        }
    }

    UE_LOGFMT(LogAreaManager, Warning, "AreaTag({0})에 해당하는 AreaQuestDataSet을 찾을 수 없습니다.", *AreaTag.ToString());
    return FAreaQuestDataSet();
}

const UAreaQuestDataInfos* UAreaManagerSubsystem::GetAreaQuestDataInfos() const
{
    return Cast<UAreaQuestDataInfos>(AreaQuestDataInfos);
}
#pragma endregion
