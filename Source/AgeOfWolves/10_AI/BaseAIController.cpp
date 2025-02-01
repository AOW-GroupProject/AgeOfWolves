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

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "14_Subsystem/AbilityManagerSubsystem.h"

#include "00_GameInstance/AOWGameInstance.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogBaseAIC)

//@Defualt Setting
#pragma region Default Setting
ABaseAIController::ABaseAIController(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    PrimaryActorTick.bCanEverTick = true;

    //@BT
    AIBehaviorTree = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("Behavior Tree"));
    //@BB
    BBComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));
    //@AI Perception
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));
    //@AI Sense Config - Sight
    Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI Sight Config"));
    //@AI Sense Config - Hearing
    Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("AI Hearing Config"));
    //@AI Manager Subsystem
    AIManagerRef.Reset();
    //@Ability Manger Subsystem
    AbilityManagerRef.Reset();
    //@ASC
    AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("Ability System Component"));

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

void ABaseAIController::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    //@TODO: 컴포넌트 간 바인딩

    //@Ability Manager Subsystem
    const auto& GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GameInstance)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "Game Instance가 유효하지 않음");
        return;
    }

    //@캐싱 - Ability Manager
    AbilityManagerRef = GameInstance->GetSubsystem<UAbilityManagerSubsystem>();
    if (!AbilityManagerRef.IsValid())
    {
        UE_LOGFMT(LogBaseAIC, Warning, "Ability Manager Subsystem 캐싱 실패");
        return;
    }

    //@캐싱 - AI Manager
    AIManagerRef = GameInstance->GetSubsystem<UAIManagerSubsystem>();
    if (!AIManagerRef.IsValid())
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI Manager Subsystem 캐싱 실패");
        return;
    }

}

void ABaseAIController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    //@초기화 호출
    InitializeAIController(InPawn);

    //@비동기 초기화 요청
    RequestStartInitByAI.Broadcast();
}

void ABaseAIController::UpdateControlRotation(float DeltaTime, bool bUpdatePawn)
{
    Super::UpdateControlRotation(DeltaTime, false);

    APawn* const AgentPawn = GetPawn();

    //@Pawn
    if (!AgentPawn)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 폰을 찾을 수 없습니다. AI 회전 업데이트가 실패했습니다.");
        return;
    }

    //@현재 회전
    const FRotator CurrentPawnRotation = AgentPawn->GetActorRotation();

    //@타겟 회전으로 선형 보간
    FRotator Rotation = UKismetMathLibrary::RInterpTo_Constant(AgentPawn->GetActorRotation(), ControlRotation, DeltaTime, SmootRotationSpeed);
    if (CurrentPawnRotation.Equals(Rotation, 1e-3f) == false)
    {
        AgentPawn->FaceRotation(Rotation, DeltaTime);
    }
}

void ABaseAIController::InternalBindToPerceptionComp()
{
    //@AI Percception
    if (!AIPerceptionComponent)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 초기화 실패: 퍼셉션 컴포넌트가 유효하지 않습니다.");
        return;
    }

    //@내부 바인딩
    AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ABaseAIController::OnPerception);
}

void ABaseAIController::InitializeAIController(APawn* InPawn)
{
    //@AI System
    InitializeAISystem(InPawn);

    //@ASC
    InitializeAbilitySystem(InPawn);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void ABaseAIController::InitializeAISystem(APawn* InPawn)
{
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
    if (!AIManagerRef.IsValid())
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 컨트롤러 초기화 실패: AI 매니저 서브시스템을 찾을 수 없습니다.");
        return;
    }

    //@AI 데이터 셋
    FAIDataSet AIDataSet;
    if (!AIManagerRef->GetAIDataSet(AIType, AIDataSet))
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

    //@내부 바인딩
    InternalBindToPerceptionComp();

}

void ABaseAIController::InitializeAbilitySystem(APawn* InPawn)
{
    // AbilityManagerSubsystem으로부터 AbilitySet 가져오기
    UBaseAbilitySet* SetToGrant = AbilityManagerRef->GetAbilitySet(CharacterTag);
    if (!IsValid(SetToGrant))
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AbilitySet이 유효하지 않음 - Tag: {0}", CharacterTag.ToString());
        return;
    }

    //@ActorInfo 초기화
    AbilitySystemComponent->InitAbilityActorInfo(InPawn, InPawn);

    //@기본 AttributeSet 등록
    SetToGrant->GiveStartupAttributeSetToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
    UE_LOGFMT(LogBaseAIC, Log, "기본 AttributeSet 등록 완료");

    //@Attribute 변경 콜백 함수 등록
    for (auto& AS : AbilitySystemComponent->GetSpawnedAttributes())
    {
        if (IsValid(AS))
        {
            AttributeSet = AS;
            TArray<FGameplayAttribute> Attributes = AttributeSet->GetAllAttributes();
            for (const FGameplayAttribute& Attribute : Attributes)
            {
                AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ABaseAIController::OnAttributeValueChanged);
            }
            UE_LOGFMT(LogBaseAIC, Log, "Attribute 변경 콜백 등록 완료");
            break;
        }
    }

    //@ATMR
    auto* ATMR = SetToGrant->GetATMR();
    if (!ATMR)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "ATMR이 유효하지 않음");
        return;
    }

    //@ATMR 초기화
    ATMR->InitializeCacheMaps();

    //@SetAbilityTagRelationshipMapping
    AbilitySystemComponent->SetAbilityTagRelationshipMapping(ATMR);

    UE_LOGFMT(LogBaseAIC, Log, "태그 관계 매핑 완료");

    //@TODO: 임시 바인딩
    RequestStartInitByAI.AddUFunction(this, "LoadGameAbilitySystem");

}

void ABaseAIController::LoadGameAbilitySystem()
{
    UE_LOGFMT(LogBaseAIC, Warning, "어빌리티 시스템의 Load 작업을 시작합니다 : {0}", __FUNCTION__);

    //@GameInstance
    if (const auto& GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        //@SaveFile
        if (GameInstance->DoesSaveGameExist())
        {
            auto* SaveGameInstance = GameInstance->GetSaveGameInstance();
            LoadAbilitySystemFromSaveGame(SaveGameInstance);
        }
        //@Ability Manager
        else
        {
            LoadDefaultAbilitySystemFromAbilityManager();
        }
    }
}

void ABaseAIController::LoadDefaultAbilitySystemFromAbilityManager()
{
    // @ASC와 AttributeSet 초기화 검증
    if (!AbilitySystemComponent || !AttributeSet.Get())
    {
        UE_LOGFMT(LogBaseAIC, Warning, "LoadGameAbilitySystem 실패: ASC 또는 AttributeSet이 초기화되지 않음");
        return;
    }

    if (!AbilityManagerRef.IsValid())
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AbilityManagerSubsystem이 유효하지 않음");
        return;
    }

    UBaseAbilitySet* SetToGrant = AbilityManagerRef->GetAbilitySet(CharacterTag);
    if (!IsValid(SetToGrant))
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AbilitySet이 유효하지 않음 - Tag: {0}", CharacterTag.ToString());
        return;
    }

    // 캐릭터의 기본 Gameplay Effect를 ASC에 최초 등록/적용합니다.
    SetToGrant->GiveStartupGameplayEffectToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

    // 캐릭터의 기본 Gameplay Ability를 ASC에 최초 등록/적용합니다.
    SetToGrant->GiveStartupGameplayAbilityToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

    // ASC에 Startup GA, GE, AttributeSet의 등록 완료 이벤트 호출
    AIAttributeSetInitialized.Broadcast();
}

void ABaseAIController::LoadAbilitySystemFromSaveGame(UAOWSaveGame* SaveGame)
{
}

void ABaseAIController::ChangeAgentAIState(EAIState InStateType)
{
}
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

void ABaseAIController::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
UAbilitySystemComponent* ABaseAIController::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

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