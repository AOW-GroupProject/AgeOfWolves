#include "BTTask_MoveAlongPatrolPath.h"
#include "Logging/StructuredLog.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"

#include "00_GameInstance/AOWGameInstance.h"
#include "10_AI/BaseAIController.h"
#include "14_Subsystem/AreaManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogBTTask_Patrol, Log, All);

//@Defualt Setting
#pragma region Default Setting
UBTTask_MoveAlongPatrolPath::UBTTask_MoveAlongPatrolPath()
{
    NodeName = TEXT("Patrol Path 따라서 이동");

    //@BlackboardKey 자동 설정
    BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_MoveAlongPatrolPath, BlackboardKey));
}

EBTNodeResult::Type UBTTask_MoveAlongPatrolPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    //@AI 
    AAIController* AIController = OwnerComp.GetAIOwner();
    if (!AIController)
    {
        return EBTNodeResult::Failed;
    }

    //@BaseAIController로 캐스팅
    ABaseAIController* BaseController = Cast<ABaseAIController>(AIController);
    if (!BaseController)
    {
        UE_LOGFMT(LogBTTask_Patrol, Warning, "BTTask_PatrolPath: BaseAIController로 캐스팅 실패");
        return EBTNodeResult::Failed;
    }

    //@GroupID 가져오기
    FGuid GroupID = BaseController->GetAIGroupID();
    if (!GroupID.IsValid())
    {
        UE_LOGFMT(LogBTTask_Patrol, Warning, "BTTask_PatrolPath: 유효한 GroupID가 없음");
        return EBTNodeResult::Failed;
    }

    //@Area Manager Subsystem - GameInstance를 통해 가져오기
    const auto& GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(AIController));
    if (!GameInstance)
    {
        UE_LOGFMT(LogBTTask_Patrol, Error, "BTTask_PatrolPath: GameInstance를 찾을 수 없음");
        return EBTNodeResult::Failed;
    }

    //@Area Manage Subsystem
    UAreaManagerSubsystem* AreaManager = GameInstance->GetSubsystem<UAreaManagerSubsystem>();
    if (!AreaManager)
    {
        UE_LOGFMT(LogBTTask_Patrol, Error, "BTTask_PatrolPath: AreaManagerSubsystem을 찾을 수 없음");
        return EBTNodeResult::Failed;
    }

    //@Patrol Path
    auto* PatrolPathActor = AreaManager->GetPatrolPathForGroup(GroupID);
    if (!PatrolPathActor)
    {
        UE_LOGFMT(LogBTTask_Patrol, Warning, "BTTask_PatrolPath: 그룹 {0}의 유효한 순찰 경로가 없음", GroupID.ToString());
        return EBTNodeResult::Failed;
    }

    auto PatrolPath = PatrolPathActor->FindComponentByClass<USplineComponent>();
    if (!PatrolPath || PatrolPath->GetNumberOfSplinePoints() < 1)
    {
        return EBTNodeResult::Failed;
    }

    //@BB Comp
    UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
    if (!BlackboardComp)
    {
        return EBTNodeResult::Failed;
    }

    //@다음 위치
    FVector NextLocation;

    if (bRandomPatrol)
    {
        //@랜덤으로
        int32 RandomIndex = FMath::RandRange(0, PatrolPath->GetNumberOfSplinePoints() - 1);
        NextLocation = PatrolPath->GetLocationAtSplinePoint(RandomIndex, ESplineCoordinateSpace::World);
    }
    else
    {
        //@순차적으로
        NextLocation = PatrolPath->GetLocationAtSplinePoint(CurrentPointIndex, ESplineCoordinateSpace::World);

        //@Current Index
        CurrentPointIndex = (CurrentPointIndex + 1) % PatrolPath->GetNumberOfSplinePoints();
    }

    //@Move Location 업데이트
    BlackboardComp->SetValueAsVector("MoveToLocation", NextLocation);

    UE_LOGFMT(LogBTTask_Patrol, Log, "AI {0}이(가) 그룹 {1}의 패트롤 포인트 {2}번으로 이동 (위치: X={3}, Y={4}, Z={5})",
        *AIController->GetPawn()->GetName(),
        *GroupID.ToString(),
        bRandomPatrol ? TEXT("랜덤") : FString::FromInt(CurrentPointIndex),
        NextLocation.X, NextLocation.Y, NextLocation.Z);

    return EBTNodeResult::Succeeded;
}

FString UBTTask_MoveAlongPatrolPath::GetStaticDescription() const
{
    return FString::Printf(TEXT("%s: 그룹 순찰 경로 따라 이동\n%s에 다음 위치 설정"),
        *Super::GetStaticDescription(),
        *GetSelectedBlackboardKey().ToString());
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
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion