#include "BaseAIController.h"
#include "Logging/StructuredLog.h"

#include "14_Subsystem/AIManagerSubsystem.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Runtime/AIModule/Classes/Perception/AISenseConfig_Sight.h"
#include "Runtime/AIModule/Classes/Perception/AISenseConfig_Hearing.h"

#include "Kismet/KismetMathLibrary.h"

#include "01_Character/CharacterBase.h"

DEFINE_LOG_CATEGORY(LogBaseAIC)

//@Defualt Setting
#pragma region Default Setting
ABaseAIController::ABaseAIController(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

    AIBehaviorTree = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree"));
    BBComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));
    Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI Sight Config"));
    Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("AI Hearing Config"));

    Sight->SightRadius = 2000.f;
    Sight->LoseSightRadius = Sight->SightRadius + 500.f;
    Sight->PeripheralVisionAngleDegrees = 90.f;
    Sight->DetectionByAffiliation.bDetectEnemies = true;
    Sight->DetectionByAffiliation.bDetectFriendlies = true;
    Sight->DetectionByAffiliation.bDetectNeutrals = true;

    Hearing->HearingRange = 2000.f;
    Hearing->DetectionByAffiliation.bDetectEnemies = true;
    Hearing->DetectionByAffiliation.bDetectNeutrals = true;
    Hearing->DetectionByAffiliation.bDetectFriendlies = true;

    AIPerceptionComponent->ConfigureSense(*Sight);
    AIPerceptionComponent->ConfigureSense(*Hearing);
    AIPerceptionComponent->SetDominantSense(Sight->GetSenseImplementation());
    AIPerceptionComponent->bAutoActivate = true;

    AIManagerRef.Reset();
}

void ABaseAIController::BeginPlay()
{
    Super::BeginPlay();
}

void ABaseAIController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateControlRotation(DeltaTime);
}

void ABaseAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
    Super::UpdateControlRotation(DeltaTime, false);

    APawn* const AgentPawn = GetPawn();

    if (!AgentPawn)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 폰을 찾을 수 없습니다. AI 회전 업데이트가 실패했습니다.");
        return;
    }

    const FRotator CurrentPawnRotation = AgentPawn->GetActorRotation();

    FRotator Rotation = UKismetMathLibrary::RInterpTo_Constant(AgentPawn->GetActorRotation(), ControlRotation, DeltaTime, SmootRotationSpeed);
    if (CurrentPawnRotation.Equals(Rotation, 1e-3f) == false)
    {
        AgentPawn->FaceRotation(Rotation, DeltaTime);
    }
}

void ABaseAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    //@소유 캐릭터
    ACharacterBase* OwningCharacter = Cast<ACharacterBase>(InPawn);
    if (!OwningCharacter)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 컨트롤러 초기화 실패: 캐릭터 베이스로 캐스팅할 수 없습니다.");
        return;
    }

    //@게임 인스턴스
    UGameInstance* GameInstance = GetWorld()->GetGameInstance();
    if (!GameInstance)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 컨트롤러 초기화 실패: 게임 인스턴스를 찾을 수 없습니다.");
        return;
    }

    //@AI 매니저
    UAIManagerSubsystem* AIManager = GameInstance->GetSubsystem<UAIManagerSubsystem>();
    if (!AIManager)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 컨트롤러 초기화 실패: AI 매니저 서브시스템을 찾을 수 없습니다.");
        return;
    }

    //@AI 데이터 셋
    FAIDataSet AIDataSet;
    if (!AIManager->GetAIDataSet(AIType, AIDataSet))
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 데이터 초기화 실패: AI 타입({0})에 대한 데이터 셋이 존재하지 않습니다.",
            StaticEnum<EAIType>()->GetNameStringByValue(static_cast<int64>(AIType)));
        return;
    }

    //@비헤이비어 트리
    if (!AIDataSet.BehaviorTree || !BBComponent || !AIBehaviorTree)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 초기화 실패: 비헤이비어 트리 컴포넌트가 유효하지 않습니다.");
        return;
    }

    //@블랙보드 에셋
    const UBlackboardData* BBAsset = AIDataSet.BehaviorTree->BlackboardAsset;
    if (!BBAsset)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 초기화 실패: AI 타입({0})의 블랙보드 데이터가 존재하지 않습니다.",
            StaticEnum<EAIType>()->GetNameStringByValue(static_cast<int64>(AIType)));
        return;
    }

    //@BB 초기화 및 BT 시작
    BBComponent->InitializeBlackboard(*AIDataSet.BehaviorTree->BlackboardAsset);
    AIBehaviorTree->StartTree(*AIDataSet.BehaviorTree);

    //@퍼셉션 델리게이트
    if (!AIPerceptionComponent)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 초기화 실패: 퍼셉션 컴포넌트가 유효하지 않습니다.");
        return;
    }

    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseAIController::OnPerception);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void ABaseAIController::ChangeAgentAIState(EAIState InStateType)
{
}

#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
void ABaseAIController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{

    ACharacterBase* OwningCharacter = Cast<ACharacterBase>(GetPawn());
    ACharacterBase* SensedCharacter = Cast<ACharacterBase>(Actor);


    //@Owning Character, Sensed Character
    if (!OwningCharacter || !SensedCharacter)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 퍼셉션 실패: 소유 캐릭터({0}), 감지된 캐릭터({1})",
            OwningCharacter ? OwningCharacter->GetName() : TEXT("Invalid"),
            SensedCharacter ? SensedCharacter->GetName() : TEXT("Invalid"));
        return;
    }

    //@팀 체크
    ETeamAttitude::Type Attitude = GetTeamAttitudeTowards(*SensedCharacter);
    if (Attitude != ETeamAttitude::Hostile)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 퍼셉션 무시: 캐릭터({0})가 적대적이지 않음. 태도: {1}",
            SensedCharacter->GetName(),
            Attitude == ETeamAttitude::Friendly ? TEXT("Friendly") : TEXT("Neutral"));
        return;
    }

    UE_LOGFMT(LogBaseAIC, Log, "{0} 캐릭터 감지!", SensedCharacter->GetName());

    //@FAIStimulus
    if (Stimulus.WasSuccessfullySensed())
    {
        BBComponent->SetValueAsBool("Contact", Stimulus.WasSuccessfullySensed());
        BBComponent->SetValueAsObject("TargetActor", Actor);
        BBComponent->SetValueAsVector("MoveToLocation", Stimulus.StimulusLocation);
    }
}

#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
FGenericTeamId ABaseAIController::GetGenericTeamId() const
{
    const IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(GetPawn());
    APawn* OwnerPawn = GetPawn();
    if (!TeamAgent)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "팀 ID 획득 실패: 폰({0})에서 팀 에이전트 인터페이스를 찾을 수 없습니다.",
            OwnerPawn ? OwnerPawn->GetName() : TEXT("Invalid"));
        return FGenericTeamId::NoTeam;
    }

    return TeamAgent->GetGenericTeamId();
}

ETeamAttitude::Type ABaseAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
    const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(&Other);
    if (!OtherTeamAgent)
    {
        return ETeamAttitude::Neutral;
    }

    return GetGenericTeamId() == OtherTeamAgent->GetGenericTeamId()
        ? ETeamAttitude::Friendly
        : ETeamAttitude::Hostile;
}
#pragma endregion