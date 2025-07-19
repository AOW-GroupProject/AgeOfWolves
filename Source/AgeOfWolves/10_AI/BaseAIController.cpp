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
#include "03_Player/PlayerStateBase.h"
#include "05_Animation/BaseAnimInstance.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "14_Subsystem/AbilityManagerSubsystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "04_Component/AIAbilitySequencerComponent.h"
#include "04_Component/ObjectiveDetectionComponent.h"

#include "15_SaveGame/AOWSaveGame.h"
#include "00_GameInstance/AOWGameInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
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
    //@ASC
    AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("Ability System Component"));
    //@AI Combat Component
    AIAbilitySequencerComponent = CreateDefaultSubobject<UAIAbilitySequencerComponent>(TEXT("AI Combat Pattern Component"));
    //@Objective Detection Component
    ODComponent = CreateDefaultSubobject<UObjectiveDetectionComponent>(TEXT("Objective Detection Comopnent"));

    //@AI Sense Config - Sight
    Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("AI Sight Config"));
    //@AI Sense Config - Hearing
    Hearing = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("AI Hearing Config"));

    //@AI Agent Pawn
    AgentPawnRef.Reset();
    //@AI Manager Subsystem
    AIManagerRef.Reset();
    //@Ability Manger Subsystem
    AbilityManagerRef.Reset();

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

    //@AI Combat Pattern Component
    if (!AIAbilitySequencerComponent)
    {
        UE_LOGFMT(LogBaseAIC, Error, "Combat Pattern Component가 유효하지 않습니다.");
        return;
    }

    //@비동기 초기화
    RequestStartInitByAI.AddUFunction(AIAbilitySequencerComponent, "InitializeCombatPatternComponent");
    RequestStartInitByAI.AddUFunction(ODComponent, "InitializeODComponent");

    //@내부 바인딩...
    InternalBindToPerceptionComp();
    InternalBindingToASC();
    InternalBindingToAISequencerComp();
    InternalBindingToODComp();

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
    // 부모 클래스의 기본 업데이트를 먼저 호출 (bUpdatePawn은 false로 설정)
    Super::UpdateControlRotation(DeltaTime, false);

    // 소유한 폰이 유효한지 확인
    APawn* const AgentPawn = GetPawn();
    if (!AgentPawn)
    {
        return;
    }

    // 회전 업데이트가 필요하지 않은 경우 조기 반환
    if (CurrentUpdateControlRotationType == EAIUpdateControlRotationType::None)
    {
        return;
    }

    // === 공통 변수 선언 ===
    FVector Start = AgentPawn->GetActorLocation();
    FRotator CurrentRotation = GetControlRotation();
    FVector TargetLocation = FVector::ZeroVector;
    float RotationSpeed = SmootRotationSpeed; // 기본값은 멤버 변수 사용
    bool bShouldCheckCompletion = false; // 회전 완료 체크가 필요한지 여부

    // === 회전 타입에 따른 타겟 위치 및 설정 결정 ===
    switch (CurrentUpdateControlRotationType)
    {
    case EAIUpdateControlRotationType::TargetActor:
    {
        // 블랙보드에서 타겟 액터 가져오기
        AActor* TargetActor = nullptr;
        if (BBComponent)
        {
            TargetActor = Cast<AActor>(BBComponent->GetValueAsObject("TargetActor"));
        }

        // 타겟 액터가 유효하지 않으면 회전 중단
        if (!TargetActor)
        {
            CurrentUpdateControlRotationType = EAIUpdateControlRotationType::None;
            return;
        }

        TargetLocation = TargetActor->GetActorLocation();
        // TargetActor 회전은 기본 SmootRotationSpeed 사용
        // bShouldCheckCompletion은 false (연속적인 추적)
        break;
    }

    case EAIUpdateControlRotationType::TargetLocation:
    {
        TargetLocation = TargetLocationForUpdateRotation;
        RotationSpeed = 10.f; // 위치 기반 회전은 더 빠른 속도 사용
        bShouldCheckCompletion = true; // 완료 체크 필요
        break;
    }

    default:
    {
        // 예상치 못한 타입이면 None으로 설정하고 반환
        CurrentUpdateControlRotationType = EAIUpdateControlRotationType::None;
        return;
    }
    }

    // 타겟을 향한 회전 계산 (LookAt 회전 계산)
    TargetRotation = UKismetMathLibrary::FindLookAtRotation(Start, TargetLocation);

    // Pitch와 Roll은 0으로 설정 (Yaw만 사용하여 수평 회전만)
    TargetRotation.Pitch = 0.0f;
    TargetRotation.Roll = 0.0f;

    // 현재 회전에서 목표 회전으로 부드럽게 보간
    FRotator FinalRotation = UKismetMathLibrary::RInterpTo(
        CurrentRotation,
        TargetRotation,
        DeltaTime,
        RotationSpeed
    );

    // 계산된 회전을 컨트롤러에 적용
    SetControlRotation(FinalRotation);

    // === 회전 완료 체크 (TargetLocation 타입에서만) ===
    if (bShouldCheckCompletion)
    {
        // Yaw 각도 차이를 계산 (절댓값으로)
        float YawDiff = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw));

        // 10도 이내로 가까워지면 회전 완료로 간주
        if (YawDiff < 10.f)
        {
            // 회전 완료, 타입을 None으로 변경
            CurrentUpdateControlRotationType = EAIUpdateControlRotationType::None;

            UE_LOGFMT(LogBaseAIC, Log,
                "위치 기반 회전 완료 | 각도 차이: {0}도, 목표 위치: {1}",
                YawDiff, *TargetLocationForUpdateRotation.ToString());
        }
        else
        {
            // 디버깅용 로그 (개발 중에만 활성화)
            UE_LOGFMT(LogBaseAIC, VeryVerbose,
                "위치 기반 회전 진행 중 | 각도 차이: {0}도, 목표: {1}, 현재: {2}, 최종: {3}",
                YawDiff,
                *TargetLocationForUpdateRotation.ToString(),
                *CurrentRotation.ToString(),
                *FinalRotation.ToString());
        }
    }
}

void ABaseAIController::ExternalBindToAnimInstance(APawn* InPawn)
{
    //@폰 유효성 체크
    if (!InPawn)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "바인딩 실패: 폰이 유효하지 않음");
        return;
    }

    //@Character로 캐스팅
    ACharacter* AgentCharacter = Cast<ACharacter>(InPawn);
    if (!AgentCharacter)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "바인딩 실패: Character로 캐스팅 실패");
        return;
    }

    //@Skeletal Mesh 컴포넌트 가져오기
    USkeletalMeshComponent* SkeletalMesh = AgentCharacter->GetMesh();
    if (!SkeletalMesh)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "바인딩 실패: 스켈레탈 메시 컴포넌트가 유효하지 않음");
        return;
    }

    //@Anim Instance 가져오기
    UAnimInstance* AnimInstance = SkeletalMesh->GetAnimInstance();
    if (!AnimInstance)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "바인딩 실패: 애님 인스턴스가 유효하지 않음");
        return;
    }

    //@BaseAnimInstance로 캐스팅
    UBaseAnimInstance* BaseAnimInstance = Cast<UBaseAnimInstance>(AnimInstance);
    if (!BaseAnimInstance)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "바인딩 실패: BaseAnimInstance로 캐스팅 실패");
        return;
    }

    //@Lock On 상태 변경 이벤트 바인딩
    AILockOnStateChanged.AddUObject(BaseAnimInstance, &UBaseAnimInstance::OnLockOnStateChanged);

    UE_LOGFMT(LogBaseAIC, Log, "AI {0}의 Lock On 상태 변경 이벤트 바인딩 완료", *InPawn->GetName());
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
    AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ABaseAIController::OnTargetPerceptionLost);
}

void ABaseAIController::InternalBindingToASC()
{
    if (!AbilitySystemComponent)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "InternalBindingToASC: ASC가 유효하지 않습니다");
        return;
    }

    //@내부 바인딩
    AbilitySystemComponent->CharacterStateEventOnGameplay.AddUFunction(this, "OnCharacterStateEventOnGameplay");

    AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(FGameplayTag::RequestGameplayTag("EventTag.OnDamaged"))
    .AddUObject(this, &ABaseAIController::OnDamagedEventOnGamePlay);
    

    UE_LOGFMT(LogBaseAIC, Log, "캐릭터 상태 이벤트 콜백이 성공적으로 바인딩되었습니다");
}

void ABaseAIController::InternalBindingToAISequencerComp()
{
    //@AIAbilitySequencer 유효성 체크
    if (!AIAbilitySequencerComponent)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "InternalBindingToAISequencerComp: AIAbilitySequencerComponent가 유효하지 않습니다");
        return;
    }

    //@전투 패턴 종료 요청
    RequestEndCombatPattern.BindUFunction(AIAbilitySequencerComponent, "OnRequestEndCombatPattern");

    //@Exit Block 완료 통지 바인딩
    AIAbilitySequencerComponent->NotifyCombatPatternExitComplete.BindUFunction(this, "OnCombatPatternExitComplete");

    UE_LOGFMT(LogBaseAIC, Log, "AI Sequencer Component 이벤트 바인딩 완료");
}

void ABaseAIController::InternalBindingToODComp()
{
    //@Objective Detection Component
    if (!ODComponent)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "InternalBindingToODComp: Objective Detection 컴포넌트가 유효하지 않습니다");
        return;
    }

    UE_LOGFMT(LogBaseAIC, Log, "Objective Detection 컴포넌트 내부 바인딩 완료");
}

void ABaseAIController::InitializeAIController(APawn* InPawn)
{
    //@외부 바인딩...
    ExternalBindToAnimInstance(InPawn);

    //@내부 바인딩...
    InitializeAISystem(InPawn);
    InitializeAbilitySystem(InPawn);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void ABaseAIController::InitializeAISystem(APawn* InPawn)
{
    if (!InPawn)
    {
        return;
    }

    //@캐싱
    AgentPawnRef = InPawn;

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
}

void ABaseAIController::InitializeAbilitySystem(APawn* InPawn)
{
    //@외부 바인딩...
    AbilitySystemComponent->ExternalBindToAIAbilitySequencer(this);
    AbilitySystemComponent->ExternalBindToAIController(this);

    //@Handle
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
    AIState = InStateType;
}

void ABaseAIController::HandleCharacterStateEvent(const FGameplayTag& CharacterStateTag)
{
    // 정적 태그들을 한 번만 생성 (성능 최적화)
    static FGameplayTag DeadTag = FGameplayTag::RequestGameplayTag(TEXT("State.Dead"));
    static FGameplayTag ExecutedTag = FGameplayTag::RequestGameplayTag(TEXT("State.Dead.Executed"));

    //@사망 상태인지 확인 (State.Dead 또는 하위 태그)
    if (CharacterStateTag.MatchesTag(DeadTag))
    {
        UE_LOGFMT(LogBaseAIC, Log, "캐릭터 사망 상태 감지. 사망 처리를 시작합니다.");

        //@특별히 처형된 경우인지 확인 (State.Dead.Executed)
        if (CharacterStateTag.MatchesTag(ExecutedTag))
        {
            UE_LOGFMT(LogBaseAIC, Log, "캐릭터가 처형되었습니다. 그룹에 정보 전송을 시작합니다.");

            //@처형 정보를 높은 우선순위로 그룹에 공유
            ShareInfoToGroup(CharacterStateTag, EAISharingInfoType::All, 10);
        }

        //@캐릭터 죽음 처리 (모든 사망 케이스에 공통)
        ProcessCharacterDeathEvent();
    }
}

void ABaseAIController::ProcessCharacterDeathEvent()
{
    //@BT 중지
    if (AIBehaviorTree)
    {
        AIBehaviorTree->StopTree();
        UE_LOGFMT(LogBaseAIC, Log, "비헤이비어 트리 실행을 중지했습니다.");
    }

    //@AI 퍼셉션 비활성화 
    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->Deactivate();
        UE_LOGFMT(LogBaseAIC, Log, "AI 퍼셉션 컴포넌트를 비활성화했습니다.");
    }

    //@BB 초기화
    if (BBComponent)
    {
        BBComponent->ClearValue("Contact");
        BBComponent->ClearValue("TargetActor");
        BBComponent->ClearValue("MoveToLocation");
        UE_LOGFMT(LogBaseAIC, Log, "블랙보드 값들을 초기화했습니다.");
    }

    //@현재 폰 확인
    APawn* CurrentPawn = GetPawn();
    if (!CurrentPawn)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "사망 처리 실패: 현재 폰이 유효하지 않음");
        return;
    }

    //@World 확인
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "사망 처리 실패: 월드가 유효하지 않음");
        return;
    }

    //@일정 시간 후 Unpossess
    FTimerHandle UnpossessTimerHandle;
    World->GetTimerManager().SetTimer(
        UnpossessTimerHandle,
        [this]()
        {
            if (APawn* PawnToUnpossess = GetPawn())
            {
                UnPossess();
                UE_LOGFMT(LogBaseAIC, Log, "AI 컨트롤러가 폰과의 연결을 해제했습니다.");
            }
        },
        5.0f,
            false
            );

    UE_LOGFMT(LogBaseAIC, Log, "5초 후 폰 연결 해제를 예약했습니다.");
}

void ABaseAIController::BindTargetActorStateEvents(AActor* NewTarget)
{
    if (!NewTarget) return;

    // CharacterBase로 캐스팅
    ACharacterBase* TargetCharacter = Cast<ACharacterBase>(NewTarget);
    if (!TargetCharacter) return;

    // ASC 가져오기
    if (UBaseAbilitySystemComponent* TargetASC =
        Cast<UBaseAbilitySystemComponent>(TargetCharacter->GetAbilitySystemComponent()))
    {
        // 상태 변화 이벤트 바인딩
        TargetASC->CharacterStateEventOnGameplay.AddUFunction(
            this, "OnTargetActorStateChanged");

        UE_LOGFMT(LogBaseAIC, Log, "타겟 {0}에 대한 상태 이벤트 바인딩 완료",
            *NewTarget->GetName());
    }
}

void ABaseAIController::UnbindTargetActorStateEvents(AActor* OldTarget)
{
    if (!OldTarget) return;

    ACharacterBase* TargetCharacter = Cast<ACharacterBase>(OldTarget);
    if (!TargetCharacter) return;

    if (UBaseAbilitySystemComponent* TargetASC =
        Cast<UBaseAbilitySystemComponent>(TargetCharacter->GetAbilitySystemComponent()))
    {
        TargetASC->CharacterStateEventOnGameplay.RemoveAll(this);
        UE_LOGFMT(LogBaseAIC, Log, "타겟 {0}에 대한 상태 이벤트 바인딩 해제",
            *OldTarget->GetName());
    }
}

bool ABaseAIController::ShareInfoToGroup(
    const FGameplayTag& StateTag,
    EAISharingInfoType SharingType,
    int32 Priority,
    float ValidTime,
    AActor* OptionalObject)
{
    // 소유한 Pawn이 있는지 확인
    AActor* ControlledActor = GetPawn();
    if (!IsValid(ControlledActor))
    {
        UE_LOGFMT(LogBaseAIC, Warning, "정보 공유 실패: 컨트롤러가 유효한 Pawn을 소유하고 있지 않습니다.");
        return false;
    }

    //@FSharingInfoWithGroup
    FSharingInfoWithGroup SharingInfo;
    SharingInfo.InfoID = FGuid::NewGuid();
    SharingInfo.SharingType = SharingType;
    SharingInfo.StateTag = StateTag;
    SharingInfo.Priority = Priority;
    SharingInfo.ValidTime = ValidTime;
    SharingInfo.OptionalObject = OptionalObject;

    //@그룹과 공유할 정보 전달 이벤트
    SendInfoToBelongingGroup.Broadcast(ControlledActor, SharingInfo);

    UE_LOGFMT(LogBaseAIC, Log, "그룹 정보 공유 완료. 정보 ID: {0}, 상태: {1}, 우선순위: {2}",
        *SharingInfo.InfoID.ToString(), *StateTag.ToString(), Priority);

    return true;
}

void ABaseAIController::ReceiveInfoFromGroup(AActor* SenderAI, const FSharingInfoWithGroup& SharingInfo)
{
    if (!IsValid(SenderAI) || !SharingInfo.StateTag.IsValid())
    {
        UE_LOGFMT(LogBaseAIC, Warning, "그룹 정보 수신 실패: 유효하지 않은 발신자 또는 상태 태그");
        return;
    }

    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "그룹 정보 수신 실패: 컨트롤러가 유효한 Pawn을 소유하지 않음");
        return;
    }

    //@전달 받은 공유 정보 처리
    ProcessReceivedGroupInfo(SenderAI, SharingInfo);

    UE_LOGFMT(LogBaseAIC, Log, "그룹원  정보 수신: 발신자={0}, 요청={1}",
        *SenderAI->GetName(), *SharingInfo.ResultTag.ToString());

}

void ABaseAIController::ProcessReceivedGroupInfo(AActor* SenderAI, const FSharingInfoWithGroup& SharingInfo)
{
    //@유효성 검사
    if (!IsValid(SenderAI) || !SharingInfo.StateTag.IsValid())
    {
        UE_LOGFMT(LogBaseAIC, Warning, "정보 처리 실패: 유효하지 않은 발신자 또는 상태 태그");
        return;
    }

    //@군중 제어 관련 태그 처리
    if (SharingInfo.ResultTag.MatchesTag(FGameplayTag::RequestGameplayTag("CrowdControl")))
    {
        ProcessCrowdControlInfo(SenderAI, SharingInfo);
    }

    UE_LOGFMT(LogBaseAIC, Log, "그룹 정보 처리 완료 | 발신자: {0}, 태그: {1}, 우선순위: {2}",
        *SenderAI->GetName(), *SharingInfo.ResultTag.ToString(), SharingInfo.Priority);
}

void ABaseAIController::ProcessCrowdControlInfo(AActor* SenderAI, const FSharingInfoWithGroup& SharingInfo)
{
    if (!SharingInfo.ResultTag.IsValid())
    {
        return;
    }

    const FGameplayTag ThreatenedTag     = FGameplayTag::RequestGameplayTag("CrowdControl.Threatened");    //@ 대상에 대한 '공포' 반응
    const FGameplayTag CoverFireReqTag   = FGameplayTag::RequestGameplayTag("CrowdControl.CoverFireRequested");  //@  CoverFire요청 반응

    const FGameplayTag& ResultTag = SharingInfo.ResultTag;

    //@ CrowdControl.Threatened : 전투 중 상태만 이벤트 발생
    if (ResultTag.MatchesTagExact(ThreatenedTag))
    {
        AActor* CurrentTarget = nullptr;
        if (BBComponent)
        {
            CurrentTarget = Cast<AActor>(BBComponent->GetValueAsObject("TargetActor"));
        }

        if (!IsValid(CurrentTarget))
        {
            return;
        }

        CrowdControlEventTriggered.ExecuteIfBound(ResultTag);

        UE_LOGFMT(LogBaseAIC, Log, "위협 상태에서 전투 중 - 군중 제어 이벤트 발생 | 타겟: {0}, 우선순위: {1}",
            *CurrentTarget->GetName(), SharingInfo.Priority);
    }
    //@ CrowdControl.CoverFireRequested : CoverFire요청 반응
    else if (ResultTag.MatchesTagExact(CoverFireReqTag))
    {
        // 블랙보드 값들을 업데이트합니다
        BBComponent->SetValueAsBool("Contact", true);
        BBComponent->SetValueAsObject("TargetActor", SharingInfo.OptionalObject);
        
        CrowdControlEventTriggered.ExecuteIfBound(ResultTag);

        UE_LOGFMT(LogBaseAIC, Log, "CoverFire요청 반응 - 군중 제어 이벤트 발생 | 타겟: {0}",
    *SharingInfo.OptionalObject->GetName());
    }


    UE_LOGFMT(LogBaseAIC, Log, "군중 제어 이벤트 발생 | 발신자: {0}, 군중 제어 요청: {1}, 우선순위: {2}",
        *SenderAI->GetName(), *SharingInfo.ResultTag.ToString(), SharingInfo.Priority);
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void ABaseAIController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
    // ========== 1단계: 기본 유효성 검사 ==========
    // AI 컨트롤러가 소유한 캐릭터를 가져옵니다
    ACharacterBase* OwningCharacter = Cast<ACharacterBase>(GetPawn());
    // 감지된 액터를 캐릭터로 캐스팅합니다
    ACharacterBase* SensedCharacter = Cast<ACharacterBase>(Actor);

    // 둘 중 하나라도 유효하지 않으면 처리하지 않습니다
    if (!OwningCharacter || !SensedCharacter)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 퍼셉션 실패: 소유 캐릭터({0}), 감지된 캐릭터({1})",
            OwningCharacter ? OwningCharacter->GetName() : TEXT("Invalid"),
            SensedCharacter ? SensedCharacter->GetName() : TEXT("Invalid"));
        return;
    }

    // ========== 2단계: 팀 관계 확인 ==========
    // 감지된 캐릭터가 적대적인지 확인합니다 (아군이나 중립은 무시)
    ETeamAttitude::Type Attitude = GetTeamAttitudeTowards(*SensedCharacter);
    if (Attitude != ETeamAttitude::Hostile)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "AI 퍼셉션 무시: 캐릭터({0})가 적대적이지 않음. 태도: {1}",
            SensedCharacter->GetName(),
            Attitude == ETeamAttitude::Friendly ? TEXT("Friendly") : TEXT("Neutral"));
        return;
    }

    // ========== 3단계: 현재 블랙보드 상태 확인 ==========
    // 블랙보드 컴포넌트 유효성 체크
    if (!BBComponent)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "블랙보드 컴포넌트가 유효하지 않습니다");
        return;
    }

    // 현재 설정된 타겟 액터를 가져옵니다
    AActor* CurrentTarget = Cast<AActor>(BBComponent->GetValueAsObject("TargetActor"));
    bool bCurrentlyHasContact = BBComponent->GetValueAsBool("Contact");

    // ========== 4단계: 감지 상태에 따른 분기 처리 ==========
    if (Stimulus.WasSuccessfullySensed())
    {
        // ===== 4-1: 감지 성공 케이스 처리 =====
        UE_LOGFMT(LogBaseAIC, Log, "{0} 캐릭터 감지 성공!", SensedCharacter->GetName());

        // 현재 타겟이 없거나, 감지된 액터가 현재 타겟과 동일한 경우에만 업데이트
        if (!CurrentTarget || CurrentTarget == Actor)
        {
            // 블랙보드 값들을 업데이트합니다
            BBComponent->SetValueAsBool("Contact", true);
            BBComponent->SetValueAsObject("TargetActor", Actor);
            BBComponent->SetValueAsVector("MoveToLocation", Stimulus.StimulusLocation);

            // 새로운 타겟인 경우에만 이벤트 바인딩을 수행합니다
            if (CurrentTarget != Actor)
            {
                // 기존 타겟이 있었다면 이벤트 바인딩을 해제합니다
                if (CurrentTarget)
                {
                    UnbindTargetActorStateEvents(CurrentTarget);
                    UE_LOGFMT(LogBaseAIC, Log, "기존 타겟 {0}의 이벤트 바인딩 해제", *CurrentTarget->GetName());
                }

                // 새로운 타겟에 대한 이벤트 바인딩을 설정합니다
                BindTargetActorStateEvents(Actor);
                UE_LOGFMT(LogBaseAIC, Log, "새로운 타겟 {0}에 이벤트 바인딩 완료", *Actor->GetName());
            }

            // 타겟 감지 성공 이벤트를 브로드캐스트합니다
            if (AgentPawnRef.IsValid())
            {
                AIDetectsTarget.Broadcast(true, AgentPawnRef.Get(), Actor);
            }

            // 상태 태그 추출
            FGameplayTag StateTag = GetCurrentCharacterStateTag();
            if (StateTag.IsValid())
            {
                //@타겟 감지를 다른 AI에게 공유하도록 정보 공유
                ShareInfoToGroup(StateTag, EAISharingInfoType::All, 1,5.f, Actor);
            }
            // Lock On 상태 변경 이벤트를 호출합니다
            AILockOnStateChanged.Broadcast(true, Actor);

            UE_LOGFMT(LogBaseAIC, Log, "타겟 설정 완료: Contact=true, TargetActor={0}", *Actor->GetName());
        }
        else
        {
            // 다른 타겟이 이미 설정되어 있는 경우
            UE_LOGFMT(LogBaseAIC, Log, "다른 타겟({0})이 이미 설정되어 있어 새로운 타겟({1}) 무시",
                *CurrentTarget->GetName(), *Actor->GetName());
        }
    }
    else
    {
        // ===== 4-2: 감지 실패 케이스 처리 (타겟 소실) =====
        UE_LOGFMT(LogBaseAIC, Log, "{0} 캐릭터 감지 소실!", SensedCharacter->GetName());

        // 감지를 잃은 액터가 현재 타겟과 동일한 경우에만 처리합니다
        // 이는 다른 액터의 감지 변화가 현재 타겟에 영향을 주지 않도록 보호합니다
        if (CurrentTarget == Actor)
        {
            UE_LOGFMT(LogBaseAIC, Log, "현재 타겟 {0}에 대한 감지 소실 - 타겟 해제 처리 시작", *Actor->GetName());

            // 블랙보드 상태를 초기화합니다
            BBComponent->SetValueAsBool("Contact", false);
            BBComponent->SetValueAsObject("TargetActor", nullptr);
            //BBComponent->ClearValue("MoveToLocation");

            // 타겟 액터의 상태 변화 이벤트 바인딩을 해제합니다
            UnbindTargetActorStateEvents(Actor);

            // 타겟 감지 소실 이벤트를 브로드캐스트합니다
            if (AgentPawnRef.IsValid())
            {
                AIDetectsTarget.Broadcast(false, AgentPawnRef.Get(), Actor);
            }

            // Lock On 상태를 해제합니다
            AILockOnStateChanged.Broadcast(false, nullptr);

            UE_LOGFMT(LogBaseAIC, Log, "타겟 소실 처리 완료: Contact=false, TargetActor=nullptr");
        }
        else if (CurrentTarget)
        {
            // 현재 타겟이 아닌 다른 액터의 감지 소실
            UE_LOGFMT(LogBaseAIC, Log, "감지 소실된 액터({0})가 현재 타겟({1})과 다름 - 블랙보드 상태 유지",
                *Actor->GetName(), *CurrentTarget->GetName());
        }
        else
        {
            // 타겟이 설정되지 않은 상태에서의 감지 소실
            UE_LOGFMT(LogBaseAIC, Log, "타겟이 설정되지 않은 상태에서 {0}의 감지 소실 - 처리 불필요", *Actor->GetName());
        }
    }

    // ========== 5단계: 최종 상태 로그 출력 ==========
    // 디버깅을 위해 현재 상태를 로그로 출력합니다
    AActor* FinalTarget = Cast<AActor>(BBComponent->GetValueAsObject("TargetActor"));
    bool bFinalContact = BBComponent->GetValueAsBool("Contact");

    UE_LOGFMT(LogBaseAIC, Log, "OnPerception 처리 완료 | Contact: {0}, TargetActor: {1}, StimulusType: {2}",
        bFinalContact ? TEXT("true") : TEXT("false"),
        FinalTarget ? FinalTarget->GetName() : TEXT("None"),
        Stimulus.WasSuccessfullySensed() ? TEXT("Detected") : TEXT("Lost"));
}

void ABaseAIController::OnTargetPerceptionLost(AActor* Actor)
{
    if (!Actor) return;

    UE_LOGFMT(LogBaseAIC, Log, "타겟 {0}에 대한 인지가 소실되었습니다.", *Actor->GetName());

    // 현재 블랙보드의 TargetActor와 인지를 잃은 Actor가 동일한지 확인
    if (BBComponent)
    {
        AActor* CurrentTarget = Cast<AActor>(BBComponent->GetValueAsObject("TargetActor"));

        // 인지를 잃은 Actor가 현재 타겟과 동일한 경우에만 블랙보드 초기화
        if (CurrentTarget == Actor)
        {
            // Contact 상태를 false로 설정
            BBComponent->SetValueAsBool("Contact", false);

            // TargetActor를 nullptr로 설정
            BBComponent->SetValueAsObject("TargetActor", nullptr);

            // 이동 목표 위치도 초기화
            BBComponent->ClearValue("MoveToLocation");

            // 타겟 액터의 상태 이벤트 바인딩 해제
            UnbindTargetActorStateEvents(Actor);

            UE_LOGFMT(LogBaseAIC, Log, "블랙보드 값 초기화 완료: Contact=false, TargetActor=nullptr");
        }
        else
        {
            UE_LOGFMT(LogBaseAIC, Log, "인지 소실된 액터({0})가 현재 타겟({1})과 다름 - 블랙보드 유지",
                *Actor->GetName(),
                CurrentTarget ? *CurrentTarget->GetName() : TEXT("None"));
        }
    }

    // 타겟 인지 소실 이벤트 브로드캐스트
    if (AgentPawnRef.IsValid())
    {
        AIDetectsTarget.Broadcast(false, AgentPawnRef.Get(), Actor);
    }

    // Lock On 상태 변경 이벤트 호출
    AILockOnStateChanged.Broadcast(false, nullptr);

    UE_LOGFMT(LogBaseAIC, Log, "AI가 {0}을(를) 놓쳐 Lock On 상태 해제", *Actor->GetName());
}

void ABaseAIController::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
}

void ABaseAIController::OnCharacterStateEventOnGameplay(AActor* Actor, const FGameplayTag& CharacterStateTag)
{
    //@"State.~"
    if (!CharacterStateTag.GetTagName().ToString().StartsWith("State."))
        return;

    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "월드를 찾을 수 없습니다.");
        return;
    }

    //@GameInstance
    UAOWGameInstance* GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(World));
    if (!GameInstance)
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "GameInstance를 가져올 수 없습니다.");
        return;
    }

    //@Save Game
    UAOWSaveGame* SaveGame = GameInstance->GetSaveGameInstance();
    if (!SaveGame)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "세이브 게임 인스턴스를 찾을 수 없습니다.");
        return;
    }

    //@현재 AI 폰 가져오기
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "폰 에이전트가 유효하지 않습니다.");
        return;
    }

    //@상태 이벤트 처리
    HandleCharacterStateEvent(CharacterStateTag);

    //@상태 이력 추가
    SaveGame->AddCharacterStateToHistory(
        CharacterStateTag,
        ControlledPawn,
        AttributeSet.IsValid() ? AttributeSet.Get() : nullptr
    );

    //@상태 이벤트 처리 로그
    UE_LOGFMT(LogBaseAIC, Log,
        "AI 캐릭터 상태 이벤트 처리 | 태그: {0}",
        CharacterStateTag.GetTagName().ToString());
}

void ABaseAIController::OnDamagedEventOnGamePlay(const FGameplayEventData* Payload)
{
    if (!Payload) return;

    //@AI State가 Idle 이면
    if (AIState == EAIState::Idle)
    {
        APawn* const AgentPawn = GetPawn();
        if (!AgentPawn) return; 

        ACharacter* MyCharacter = Cast<ACharacter>(AgentPawn);
        if (!MyCharacter) return;


        //@ 히트 지점으로 돌아볼수 있게 위치 캐싱
        TargetLocationForUpdateRotation = Payload->ContextHandle.GetHitResult()->ImpactPoint;
        //@ Tick에서 회전할수 있게 타입 설정
        CurrentUpdateControlRotationType = EAIUpdateControlRotationType::TargetLocation;

        //@ControllRotation 할것이기에 제어 플레그 true
        UCharacterMovementComponent* CharacterMovementComp = MyCharacter->GetCharacterMovement();
        if (CharacterMovementComp)
        {
            CharacterMovementComp->bUseControllerDesiredRotation = true;
            CharacterMovementComp->bOrientRotationToMovement = false;
        }
        
        UE_LOG(LogBaseAIC, Log, TEXT("OnDamagedEventOnGamePlay [AI] OnDamaged 이벤트 수신 => impactPoint : %s")
            , *TargetLocationForUpdateRotation.ToString());
    }
    else
    {
        UE_LOG(LogBaseAIC, Log, TEXT("OnDamagedEventOnGamePlay [AI] OnDamaged 이벤트 수신 하였지만 AIState가 Idle 아님"));
    }
}

bool ABaseAIController::OnCombatPatternExitComplete()
{
    //@완료 통지가 바인딩되어 있지 않으면 종료
    if (!NotifyCombatPatternExitComplete.IsBound())
    {
        UE_LOGFMT(LogBaseAIC, Warning, "Exit Block 완료 통지 실패: 델리게이트가 바인딩되지 않음");
        return false;
    }

    //@Task에 완료 통지
    UE_LOGFMT(LogBaseAIC, Log, "전투 패턴 Exit Block 완료. Task에 통지");
    return NotifyCombatPatternExitComplete.Execute();
}

void ABaseAIController::OnTargetActorStateChanged(AActor* Actor, const FGameplayTag& StateTag)
{
    // 죽음 상태 태그 체크
    if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead")))
    {
        UE_LOGFMT(LogBaseAIC, Log, "타겟 액터 사망 감지");

        // BB 값 초기화
        if (BBComponent)
        {
            BBComponent->SetValueAsBool("Contact", false);
            BBComponent->SetValueAsObject("TargetActor", nullptr);
            BBComponent->ClearValue("MoveToLocation");
        }

        // Lock On 상태 해제
        AILockOnStateChanged.Broadcast(false, nullptr);

        UE_LOGFMT(LogBaseAIC, Log, "타겟 사망으로 인한 상태 초기화 완료");
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
UAbilitySystemComponent* ABaseAIController::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ABaseAIController::SetAIGroupID(const FGuid& ID)
{
    if (!ID.IsValid())
    {
        return;
    }

    AIGroupID = ID;
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

FGameplayTag ABaseAIController::GetCurrentCharacterStateTag() const
{
    if (!AbilitySystemComponent)
    {
        return FGameplayTag(); // Invalid
    }

    FGameplayEffectQuery Query; // 전체 매칭용 Query
    TArray<FActiveGameplayEffectHandle> Handles = AbilitySystemComponent->GetActiveEffects(Query);

    for (const FActiveGameplayEffectHandle& Handle : Handles)
    {
        const FActiveGameplayEffect* ActiveEffect = AbilitySystemComponent->GetActiveGameplayEffect(Handle);
        if (!ActiveEffect || !ActiveEffect->Spec.Def)
        {
            continue;
        }

        const FGameplayTagContainer& AssetTags = ActiveEffect->Spec.Def->InheritableGameplayEffectTags.Added;

        for (const FGameplayTag& Tag : AssetTags)
        {
            if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag("State")))
            {
                UE_LOGFMT(LogASC, Log, "현재 적용된 상태 태그: {0}", Tag.ToString());
                return Tag;
            }
        }
    }
    
    return FGameplayTag(); // 상태가 없을 경우
}

#pragma endregion