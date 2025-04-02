#include "CrowdControlComponent.h"
#include "Logging/StructuredLog.h"

#include "16_Level/Area.h"
#include "10_AI/BaseAIController.h"

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
}

void UCrowdControlComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UCrowdControlComponent::ExternalBindingToAI(TWeakObjectPtr<AActor> AIActor)
{
    // AI 액터 유효성 검사
    if (!AIActor.IsValid())
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "AI 바인딩 실패: 유효하지 않은 액터");
        return;
    }

    // AI 액터를 Pawn으로 캐스팅
    APawn* AIPawn = Cast<APawn>(AIActor.Get());
    if (!AIPawn)
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "AI 바인딩 실패: 액터 {0}을(를) Pawn으로 캐스팅할 수 없음", *AIActor->GetName());
        return;
    }

    // AI 컨트롤러 가져오기
    AController* Controller = AIPawn->GetController();
    if (!IsValid(Controller))
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "AI 바인딩 실패: 액터 {0}의 유효한 컨트롤러를 찾을 수 없음", *AIActor->GetName());
        return;
    }

    // BaseAIController로 캐스팅
    ABaseAIController* AIController = Cast<ABaseAIController>(Controller);
    if (!AIController)
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "AI 바인딩 실패: 액터 {0}의 컨트롤러를 BaseAIController로 캐스팅할 수 없음", *AIActor->GetName());
        return;
    }

    //@외부 바인딩...
    AIController->SendInfoToBelongingGroup.AddUObject(this, &UCrowdControlComponent::OnSendInfoToBelongingGroup);

    UE_LOGFMT(LogCrowdCtrl, Log, "AI {0}에 대한 그룹 정보 이벤트 바인딩 완료", *AIActor->GetName());
}

void UCrowdControlComponent::InitializeCrowdControlComp(TArray<FAIGroupInfo> AIGroups)
{
    // 그룹이 없는 경우 얼리 리턴
    if (AIGroups.IsEmpty())
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "군중 제어 컴포넌트 초기화: 등록된 AI 그룹이 없습니다.");
        return;
    }

    UE_LOGFMT(LogCrowdCtrl, Log, "군중 제어 컴포넌트 초기화 시작. 그룹 수: {0}", AIGroups.Num());

    // 전체 AI 멤버 수 및 유효 바인딩 수 추적
    int32 TotalAICount = 0;
    int32 ValidBindingCount = 0;

    // 각 그룹 정보 로깅 (그룹별 하나의 로그만 출력) 및 AI 바인딩
    for (int32 i = 0; i < AIGroups.Num(); ++i)
    {
        const FAIGroupInfo& GroupInfo = AIGroups[i];

        // 유효 멤버 수 계산
        int32 ValidMemberCount = 0;

        // 각 멤버에 대해 바인딩 수행
        for (const FAreaAIInfo& Member : GroupInfo.GroupMembers)
        {
            TotalAICount++;

            if (Member.AIActor.IsValid())
            {
                ValidMemberCount++;

                //@외부 바인딩...
                ExternalBindingToAI(Member.AIActor);
                ValidBindingCount++;
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

    UE_LOGFMT(LogCrowdCtrl, Log, "군중 제어 컴포넌트 초기화 완료: 총 {0}개 AI 중 {1}개에 바인딩 성공", TotalAICount, ValidBindingCount);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UCrowdControlComponent::OnSendInfoToBelongingGroup(AActor* AI, FSharingInfoWithGroup SharingInfo)
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

    //@Sharing Info 유효성
    if (!SharingInfo.IsValid(CurrentTime))
    {
        UE_LOGFMT(LogCrowdCtrl, Warning, "그룹 정보 수신 실패: AI {0}에서 만료된 정보 (유효 시간: {1}초)",
            *AI->GetName(), SharingInfo.ValidTime);
        return;
    }

    //@우선순위 높은 정보는 특별 로그
    if (SharingInfo.Priority >= 5)
    {
        UE_LOGFMT(LogCrowdCtrl, Log, "==== 중요 그룹 정보 수신 (우선순위: {0}) ====", SharingInfo.Priority);
    }

    UE_LOGFMT(LogCrowdCtrl, Log, "그룹 정보 수신: AI={0}, 상태={1}, 공유유형={2}, 우선순위={3}, ID={4}",
        *AI->GetName(),
        *SharingInfo.StateTag.ToString(),
        *UEnum::GetValueAsString(SharingInfo.SharingType),
        SharingInfo.Priority,
        *SharingInfo.InfoID.ToString());

    //@State.Executed
    if (SharingInfo.StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead.Executed")))
    {
        UE_LOGFMT(LogCrowdCtrl, Log, "사망 정보 감지: AI {0}의 사망 관련 정보를 그룹에 전파합니다.", *AI->GetName());

        
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
