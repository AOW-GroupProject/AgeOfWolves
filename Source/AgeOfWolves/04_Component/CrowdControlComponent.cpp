#include "CrowdControlComponent.h"
#include "Logging/StructuredLog.h"

#include "16_Level/Area.h"

DEFINE_LOG_CATEGORY(LogCrowdCtrl)

//@Defualt Setting
#pragma region Default Setting
UCrowdControlComponent::UCrowdControlComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCrowdControlComponent::OnRegister()
{
	Super::OnRegister();
}

void UCrowdControlComponent::OnUnregister()
{
	Super::OnUnregister();
}

void UCrowdControlComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UCrowdControlComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
}

void UCrowdControlComponent::BeginPlay()
{
	Super::BeginPlay();

    //@정보 처리 타이머 설정
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().SetTimer(
            InfoProcessingTimerHandle,
            this,
            &UCrowdControlComponent::ProcessInfoQueue,
            InfoProcessingInterval,
            true);
    }
}

void UCrowdControlComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    //@Clear Timer
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(InfoProcessingTimerHandle);
    }

    //@Queue 정리
    FSharingInfoQuery DummyQuery;
    while (!GroupInfoQueue.IsEmpty())
    {
        GroupInfoQueue.Dequeue(DummyQuery);
    }

    Super::EndPlay(EndPlayReason);
}

void UCrowdControlComponent::ExternalBindingToArea(AArea* Area)
{
    //@Area
    if (!IsValid(Area))
    {
        return;
    }

    //@외부 바인딩...
    Area->NotifyGroupToShareInfo.AddUFunction(this, "GroupToShareInfoNotified");

    UE_LOGFMT(LogCrowdCtrl, Log, "Area {0}에 대한 그룹 정보 이벤트 바인딩 완료", *Area->GetName());
}

void UCrowdControlComponent::InitializeCrowdControlComp(TArray<FAIGroupInfo> AIGroups)
{
    //@그룹이 없는 경우 얼리 리턴
    if (AIGroups.IsEmpty())
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "군중 제어 컴포넌트 초기화: 등록된 AI 그룹이 없습니다.");
        return;
    }

    //@그룹 맵 초기화
    MAIToGroups.Empty();

    UE_LOGFMT(LogCrowdCtrl, Log, "군중 제어 컴포넌트 초기화 시작. 그룹 수: {0}", AIGroups.Num());

    // 전체 AI 멤버 수 및 유효 바인딩 수 추적
    int32 TotalAICount = 0;
    int32 ValidBindingCount = 0;

    // 각 그룹 정보 로깅 (그룹별 하나의 로그만 출력) 및 AI 바인딩
    for (int32 i = 0; i < AIGroups.Num(); ++i)
    {
        const FAIGroupInfo& GroupInfo = AIGroups[i];

        //@그룹 정보 캐싱
        if (GroupInfo.GroupID.IsValid())
        {
            MAIToGroups.Add(GroupInfo.GroupID, GroupInfo);
        }
        else
        {
            UE_LOGFMT(LogCrowdCtrl, Warning, "유효하지 않은 그룹 ID를 가진 그룹 무시: 인덱스={0}, 이름='{1}'",
                i, *GroupInfo.GroupName);
            continue;
        }

        // 유효 멤버 수 계산
        int32 ValidMemberCount = 0;

        // 각 멤버에 대해 바인딩 수행
        for (const FAreaAIInfo& Member : GroupInfo.GroupMembers)
        {
            TotalAICount++;

            if (Member.AIActor.IsValid())
            {
                ValidMemberCount++;
            }
        }

        //@Leader
        FString LeaderName = GroupInfo.GroupLeader.IsValid() ? GroupInfo.GroupLeader->GetName() : TEXT("없음");

        UE_LOGFMT(LogCrowdCtrl, Log, "그룹 #{0}: ID={1}, 이름='{2}', 멤버={3}/{4}, 리더={5}",
            i + 1,
            *GroupInfo.GroupID.ToString(),
            *GroupInfo.GroupName,
            ValidMemberCount,
            GroupInfo.GroupMembers.Num(),
            *LeaderName);
    }

    UE_LOGFMT(LogCrowdCtrl, Log, "군중 제어 컴포넌트 초기화 완료: 총 {0}개", TotalAICount);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UCrowdControlComponent::ProcessInfoQueue()
{
    //@Empty?
    if (GroupInfoQueue.IsEmpty())
    {
        return;
    }

    //@Current Time
    float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    CleanupExpiredProcessedInfoIDs(CurrentTime);

    int32 ProcessedCount = 0;
    FSharingInfoQuery Query;

    while (!GroupInfoQueue.IsEmpty() && ProcessedCount < MaxInfosToProcess)
    {
        //@Dequeue
        GroupInfoQueue.Dequeue(Query);
        ProcessedCount++;

        //@유효 시간 체크
        if (!ShouldProcessInfo(Query.Info))
        {
            continue;
        }

        //@이미 처리한 정보에 추가
        ProcessedInfoIDs.Add(Query.Info.InfoID, CurrentTime);

        //@Recipients
        int32 SuccessCount = 0;
        for (const TWeakObjectPtr<AActor>& RecipientPtr : Query.Recipients)
        {
            if (!RecipientPtr.IsValid())
            {
                continue;
            }

            AActor* Recipient = RecipientPtr.Get();
            APawn* RecipientPawn = Cast<APawn>(Recipient);
            if (!RecipientPawn)
            {
                continue;
            }

            //@Base AI Controller
            ABaseAIController* AIController = Cast<ABaseAIController>(RecipientPawn->GetController());
            if (AIController)
            {
                //@정보 수신
                AIController->ReceiveInfoFromGroup(Query.SenderAI, Query.Info);
                SuccessCount++;
            }
        }

        UE_LOGFMT(LogCrowdCtrl, Log, "그룹 정보 처리 완료: 발신자={0}, 성공한 수신자={1}/{2}, 상태={3}",
            *Query.SenderAI->GetName(), SuccessCount, Query.Recipients.Num(), *Query.Info.StateTag.ToString());
    }
}
void UCrowdControlComponent::ProcessDeadStateInfo(AActor* SenderAI, AActor* RecipientAI, const FSharingInfoWithGroup& InfoData)
{
    UE_LOGFMT(LogCrowdCtrl, Log, "사망 정보 처리: 발신자={0}, 수신자={1}, 상태={2}",
        SenderAI ? *SenderAI->GetName() : TEXT("Unknown"),
        RecipientAI ? *RecipientAI->GetName() : TEXT("Unknown"),
        *InfoData.StateTag.ToString());

    //@TODO: 사망 정보에 대한 실제 처리 로직 구현...
}

void UCrowdControlComponent::ProcessFragileStateInfo(AActor* SenderAI, AActor* RecipientAI, const FSharingInfoWithGroup& InfoData)
{
    UE_LOGFMT(LogCrowdCtrl, Log, "취약 상태 정보 처리: 발신자={0}, 수신자={1}, 상태={2}",
        SenderAI ? *SenderAI->GetName() : TEXT("Unknown"),
        RecipientAI ? *RecipientAI->GetName() : TEXT("Unknown"),
        *InfoData.StateTag.ToString());

    //@TODO: 취약 상태 정보에 대한 실제 처리 로직 구현...
}

void UCrowdControlComponent::ProcessGenericStateInfo(AActor* SenderAI, AActor* RecipientAI, const FSharingInfoWithGroup& InfoData)
{
    UE_LOGFMT(LogCrowdCtrl, Log, "일반 정보 처리: 발신자={0}, 수신자={1}, 상태={2}",
        SenderAI ? *SenderAI->GetName() : TEXT("Unknown"),
        RecipientAI ? *RecipientAI->GetName() : TEXT("Unknown"),
        *InfoData.StateTag.ToString());

    //@TODO: 일반 정보에 대한 실제 처리 로직 구현...
}

void UCrowdControlComponent::CleanupExpiredProcessedInfoIDs(float CurrentTime)
{

    TArray<FGuid> ExpiredIDs;

    for (auto& Pair : ProcessedInfoIDs)
    {
        if ((CurrentTime - Pair.Value) >= ProcessedInfoExpiryTime)
        {
            ExpiredIDs.Add(Pair.Key);
        }
    }

    //@만료된 ID 제거
    for (const FGuid& ID : ExpiredIDs)
    {
        ProcessedInfoIDs.Remove(ID);
    }

    //@많은 ID가 제거되었으면 로그 출력
    if (ExpiredIDs.Num() > 10)
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "{0}개의 만료된 정보 ID 정리 완료", ExpiredIDs.Num());
    }

}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UCrowdControlComponent::GroupToShareInfoNotified(AActor* AI, FSharingInfoWithGroup SharingInfo)
{
    //@AI Actor
    if (!IsValid(AI))
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "그룹 정보 수신 실패: 유효하지 않은 AI 액터");
        return;
    }

    //@State Tag
    if (!SharingInfo.StateTag.IsValid())
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "그룹 정보 수신 실패: AI {0}에서 유효하지 않은 상태 태그", *AI->GetName());
        return;
    }

    //@Current Time
    float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

    //@Sharing Info 유효 시간
    if (!SharingInfo.IsValid(CurrentTime))
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "그룹 정보 수신 실패: AI {0}에서 만료된 정보 (유효 시간: {1}초)",
            *AI->GetName(), SharingInfo.ValidTime);
        return;
    }

    //@이미 처리된 정보인지 확인
    if (IsInfoAlreadyProcessed(SharingInfo.InfoID, CurrentTime))
    {
        UE_LOGFMT(LogCrowdCtrl, Log, "중복 정보 무시: AI={0}, ID={1}",
            *AI->GetName(), *SharingInfo.InfoID.ToString());
        return;
    }

    //@Group 정보가 캐싱되어 있는지 확인
    if (!MAIToGroups.Contains(SharingInfo.GroupID))
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "그룹 정보 수신 실패: 그룹 ID {0}에 대한 캐싱된 정보 없음",
            *SharingInfo.GroupID.ToString());
        return;
    }

    //@큐 항목 생성
    FSharingInfoQuery Query(AI, SharingInfo);

    //@수신자 목록 생성
    FindRecipients(AI, SharingInfo, Query.Recipients);

    //@우선순위 높은 정보는 특별 로그
    if (SharingInfo.Priority >= HighPriorityThreshold)
    {
        UE_LOGFMT(LogCrowdCtrl, Log, "==== 중요 그룹 정보 수신 (우선순위: {0}) ====", SharingInfo.Priority);
    }

    UE_LOGFMT(LogCrowdCtrl, Log, "그룹 정보 수신: AI={0}, 상태={1}, 공유유형={2}, 우선순위={3}, 대상 수={4}, ID={5}",
        *AI->GetName(),
        *SharingInfo.StateTag.ToString(),
        *UEnum::GetValueAsString(SharingInfo.SharingType),
        SharingInfo.Priority,
        Query.Recipients.Num(),
        *SharingInfo.InfoID.ToString());

    //@델리게이트 브로드캐스트
    GroupInfoReceived.Broadcast(AI, SharingInfo);

    //@큐에 저장
    GroupInfoQueue.Enqueue(Query);

    //@우선순위 높은 정보는 즉시 처리
    if (SharingInfo.Priority >= HighPriorityThreshold)
    {
        ProcessInfoQueue();
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
void UCrowdControlComponent::FindRecipients(
    AActor* SenderAI,
    const FSharingInfoWithGroup& SharingInfo,
    TArray<TWeakObjectPtr<AActor>>& OutRecipients)
{
    //@그룹 정보 확인
    if (!MAIToGroups.Contains(SharingInfo.GroupID))
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "수신자 목록 생성 실패: 그룹 ID {0}에 대한 캐싱된 정보 없음",
            *SharingInfo.GroupID.ToString());
        return;
    }

    //@그룹 정보와 전달자의 계층 구조
    const FAIGroupInfo& GroupInfo = MAIToGroups[SharingInfo.GroupID];
    const EAIHierarchyType SenderHierarchy = GetAIHierarchyType(SenderAI, GroupInfo);

    //@수신자 목록 초기화
    OutRecipients.Empty();

    //@전달 유형에 따라 수신자 필터링
    switch (SharingInfo.SharingType)
    {
    case EAISharingInfoType::All:
        //@모든 그룹원에게 전달
        for (const FAreaAIInfo& Member : GroupInfo.GroupMembers)
        {
            if (Member.AIActor.IsValid() && Member.AIActor.Get() != SenderAI)
            {
                OutRecipients.Add(Member.AIActor);
            }
        }
        break;

    case EAISharingInfoType::SameRank:
        //@동일 계층에게만 전달
        for (const FAreaAIInfo& Member : GroupInfo.GroupMembers)
        {
            if (Member.AIActor.IsValid() && Member.AIActor.Get() != SenderAI &&
                Member.HierarchyType == SenderHierarchy)
            {
                OutRecipients.Add(Member.AIActor);
            }
        }
        break;

    case EAISharingInfoType::HigherRank:
        //@상위 계층에게만 전달
        for (const FAreaAIInfo& Member : GroupInfo.GroupMembers)
        {
            if (Member.AIActor.IsValid() && Member.AIActor.Get() != SenderAI &&
                static_cast<int32>(Member.HierarchyType) < static_cast<int32>(SenderHierarchy))
            {
                OutRecipients.Add(Member.AIActor);
            }
        }
        break;

    case EAISharingInfoType::LowerRank:
        //@하위 계층에게만 전달
        for (const FAreaAIInfo& Member : GroupInfo.GroupMembers)
        {
            if (Member.AIActor.IsValid() && Member.AIActor.Get() != SenderAI &&
                static_cast<int32>(Member.HierarchyType) > static_cast<int32>(SenderHierarchy))
            {
                OutRecipients.Add(Member.AIActor);
            }
        }
        break;

    case EAISharingInfoType::Leader:
        //@리더에게만 전달
        if (GroupInfo.GroupLeader.IsValid() && GroupInfo.GroupLeader.Get() != SenderAI)
        {
            OutRecipients.Add(GroupInfo.GroupLeader);
        }
        break;

    case EAISharingInfoType::Exclude:
        //@제외 대상 제외하고 전달
        for (const FAreaAIInfo& Member : GroupInfo.GroupMembers)
        {
            if (Member.AIActor.IsValid() && Member.AIActor.Get() != SenderAI &&
                !SharingInfo.ExcludedTargets.Contains(Member.AIActor))
            {
                OutRecipients.Add(Member.AIActor);
            }
        }
        break;

    case EAISharingInfoType::Custom:
        //@커스텀 대상에게만 전달
        for (const TWeakObjectPtr<AActor>& CustomTarget : SharingInfo.CustomTargets)
        {
            if (CustomTarget.IsValid())
            {
                OutRecipients.Add(CustomTarget);
            }
        }
        break;
    }
}

const TArray<FAIGroupInfo>& UCrowdControlComponent::GetCachedAIGroups() const
{
    static TArray<FAIGroupInfo> GroupsArray;
    GroupsArray.Empty(MAIToGroups.Num());

    MAIToGroups.GenerateValueArray(GroupsArray);
    return GroupsArray;
}

EAIHierarchyType UCrowdControlComponent::GetAIHierarchyType(AActor* AI, const FAIGroupInfo& GroupInfo) const
{
    for (const FAreaAIInfo& Member : GroupInfo.GroupMembers)
    {
        if (Member.AIActor.Get() == AI)
        {
            return Member.HierarchyType;
        }
    }

    return EAIHierarchyType::Regular;
}

bool UCrowdControlComponent::IsInfoAlreadyProcessed(const FGuid& InfoID, float CurrentTime)
{
    //@Find
    float* ProcessedTime = ProcessedInfoIDs.Find(InfoID);
    if (ProcessedTime)
    {
        return (CurrentTime - *ProcessedTime) < ProcessedInfoExpiryTime;
    }

    return false;
}

bool UCrowdControlComponent::ShouldProcessInfo(const FSharingInfoWithGroup& InfoData)
{
    float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
    if (!InfoData.IsValid(CurrentTime))
    {
        return false;
    }

    return true;
}

int32 UCrowdControlComponent::GetPendingInfoCount() const
{
    return GroupInfoQueue.IsEmpty() ? 0 : 1;
}
#pragma endregion
