#include "AIManagerSubsystem.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogAIManager)

UAIManagerSubsystem::UAIManagerSubsystem()
{
    AIDataSetInfos = nullptr;
}

void UAIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    //@Load
    const FString AssetPath = TEXT("/Game/Blueprints/04_AI/DA_BaseAIDataInformations");
    AIDataSetInfos = LoadObject<UAIDataSetInfos>(nullptr, *AssetPath);

    //@AI Data Set Infos
    if (!AIDataSetInfos)
    {
        UE_LOGFMT(LogAIManager, Warning, "AI Data Set Infos 로드 실패. 경로: {0}", AssetPath);
        return;
    }

    UE_LOGFMT(LogAIManager, Log, "AI Dat Set Infos 로드 완료");

}

const UAIDataSetInfos* UAIManagerSubsystem::GetAIDataSetInfos() const
{
    return Cast<UAIDataSetInfos>(AIDataSetInfos);
}

bool UAIManagerSubsystem::GetAIDataSet(EAIType AIType, FAIDataSet& OutDataSet) const
{
    //@AI Data Set Infos
    const UAIDataSetInfos* DataSetInfos = GetAIDataSetInfos();
    if (!DataSetInfos)
    {
        UE_LOGFMT(LogAIManager, Warning, "AI 데이터셋 정보를 찾을 수 없습니다.");
        return false;
    }

    //@AI Data Set
    const FAIDataSet* FoundDataSet = DataSetInfos->AIDataSets.FindByPredicate(
        [AIType](const FAIDataSet& DataSet)
        {
            return DataSet.AIType == AIType;
        });

    if (!FoundDataSet)
    {
        UE_LOGFMT(LogAIManager, Warning, "해당 AI 타입({0})의 데이터셋을 찾을 수 없습니다.",
            StaticEnum<EAIType>()->GetNameStringByValue(static_cast<int64>(AIType)));
        return false;
    }

    OutDataSet = *FoundDataSet;
    return true;
}

TArray<FAIDataSet> UAIManagerSubsystem::GetAIDataSets(const TArray<EAIType>& AITypes) const
{
    TArray<FAIDataSet> ResultDataSets;
    const UAIDataSetInfos* DataSetInfos = GetAIDataSetInfos();

    if (!DataSetInfos)
    {
        UE_LOGFMT(LogAIManager, Warning, "AI 데이터셋 정보를 찾을 수 없습니다.");
        return ResultDataSets;
    }

    for (const EAIType& AIType : AITypes)
    {
        FAIDataSet DataSet;
        if (GetAIDataSet(AIType, DataSet))
        {
            ResultDataSets.Add(DataSet);
        }
    }

    return ResultDataSets;
}

