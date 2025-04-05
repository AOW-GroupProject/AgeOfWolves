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
    Super::UpdateControlRotation(DeltaTime, false);

    APawn* const AgentPawn = GetPawn();
    if (!AgentPawn) return;

    //@BB의 타겟 액터 확인
    AActor* TargetActor = nullptr;
    if (BBComponent)
    {
        TargetActor = Cast<AActor>(BBComponent->GetValueAsObject("TargetActor"));
    }

    //@타겟이 없으면 회전 업데이트 하지 않음
    if (!TargetActor) return;

    //@현재 위치와 회전
    FVector Start = AgentPawn->GetActorLocation();
    FRotator CurrentRotation = GetControlRotation();

    //@타겟을 향한 회전 계산 (GetFocalPoint 대신 직접 타겟 위치 사용)
    TargetRotation = UKismetMathLibrary::FindLookAtRotation(Start, TargetActor->GetActorLocation());

    //@Yaw만 사용
    TargetRotation.Pitch = 0.0f;
    TargetRotation.Roll = 0.0f;

    //@보간된 최종 회전 계산
    FRotator FinalRotation = UKismetMathLibrary::RInterpTo(
        CurrentRotation,
        TargetRotation,
        DeltaTime,
        SmootRotationSpeed
    );

    //@컨트롤러 회전 설정
    SetControlRotation(FinalRotation);
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
    float ValidTime)
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

    UE_LOGFMT(LogBaseAIC, Log, "그룹원 사망 정보 수신: 발신자={0}, 요청={1}",
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

    //@CrowdControl.Threatened : 대상에 대한 '공포' 반응
    if (SharingInfo.ResultTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("CrowdControl.Threatened")))
    {
        //@현재 전투 중인지 확인
        AActor* CurrentTarget = nullptr;
        if (BBComponent)
        {
            CurrentTarget = Cast<AActor>(BBComponent->GetValueAsObject("TargetActor"));
        }

        //@타겟이 있는 경우만 이벤트 발생
        if (!IsValid(CurrentTarget))
        {
            return;
        }

        //@군중 제어 발생 이벤트 호출
        CrowdControlEventTriggered.ExecuteIfBound(SharingInfo.ResultTag);

        UE_LOGFMT(LogBaseAIC, Log, "위협 상태에서 전투 중 - 군중 제어 이벤트 발생 | 타겟: {0}, 우선순위: {1}",
            *CurrentTarget->GetName(), SharingInfo.Priority);
    }

    UE_LOGFMT(LogBaseAIC, Log, "군중 제어 이벤트 발생 | 발신자: {0}, 군중 제어 요청: {1}, 우선순위: {2}",
        *SenderAI->GetName(), *SharingInfo.ResultTag.ToString(), SharingInfo.Priority);
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void ABaseAIController::OnPerception(AActor* Actor, FAIStimulus Stimulus)
{
    //@Perception 주체
    ACharacterBase* OwningCharacter = Cast<ACharacterBase>(GetPawn());
    //@Perception 대상
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

        //@Target Actor의 상태 변화 이벤트에 바인딩 수행
        BindTargetActorStateEvents(Actor);

        //@타겟 인지 성공 이벤트
        if (!!AgentPawnRef.IsValid())
        {
            AIDetectsTarget.Broadcast(true, AgentPawnRef.Get(), Actor);
        }
        else
        {
            UE_LOGFMT(LogBaseAIC, Warning, "유효하지 않음!");
        }

        //@Lock On 이벤트 호출
        AILockOnStateChanged.Broadcast(true, Actor);
    }
}

void ABaseAIController::OnTargetPerceptionLost(AActor* Actor)
{
    if (!Actor) return;

    UE_LOGFMT(LogBaseAIC, Log, "타겟 {0}에 대한 인지가 소실되었습니다.", *Actor->GetName());

    //@타겟 인지 성공 이벤트
    if (!!AgentPawnRef.IsValid())
    {
        AIDetectsTarget.Broadcast(false, AgentPawnRef.Get(), Actor);
    }
     
    //@Lock On 상태 변경 이벤트 호출
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
#pragma endregion