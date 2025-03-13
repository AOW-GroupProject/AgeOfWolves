#include "Area.h"
#include "Logging/StructuredLog.h"

#include "Components/BoxComponent.h"

#include "01_Character/CharacterBase.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"


DEFINE_LOG_CATEGORY_STATIC(LogArea, Log, All);

//@Defualt Setting
#pragma region Default Setting
AArea::AArea()
{
    PrimaryActorTick.bCanEverTick = false;

    //@Box Component
    AreaBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaBounds"));
    RootComponent = AreaBounds;

    AreaBounds->SetCollisionProfileName(TEXT("OverlapAll"));
    AreaBounds->SetCollisionResponseToAllChannels(ECR_Overlap);
    AreaBounds->SetBoxExtent(FVector(500.0f, 500.0f, 200.0f));

    //@Area ID
    AreaID = FName(*GetName());

    //@초기화
    AIInfoMap.Empty();
    PlayerBindings.Empty();
    LastCleanupTime = 0.0f;
}

void AArea::BeginPlay()
{
    Super::BeginPlay();

    //@Area 초기화
    InitializeArea();
}

void AArea::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    //@Timer
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(CleanupTimerHandle);
    }

    //@모든 플레이어 바인딩 해제
    for (auto& Pair : PlayerBindings)
    {
        if (Pair.Key.IsValid() && Pair.Value.ObjectiveComponent.IsValid())
        {
            UObjectiveDetectionComponent* ObjectiveComp = Pair.Value.ObjectiveComponent.Get();
            if (ObjectiveComp)
            {
                // ObjectiveDetectionComponent에서 Area 언바인딩 메서드 호출
                // ObjectiveComp->UnbindFromArea(this); 
                // 주의: ObjectiveDetectionComponent 구현 후 주석 해제
            }
        }
    }

    //@모든 AI 바인딩 해제
    for (auto& Pair : AIInfoMap)
    {
        if (Pair.Key.IsValid() && Pair.Value.AIASC.IsValid())
        {
            UBaseAbilitySystemComponent* AIComponent = Pair.Value.AIASC.Get();
            if (AIComponent)
            {
                AIComponent->CharacterStateEventOnGameplay.RemoveAll(this);
            }
        }
    }

    //@맵 정리
    AIInfoMap.Empty();
    PlayerBindings.Empty();

    UE_LOGFMT(LogArea, Log, "Area {0} 정리 완료", *AreaID.ToString());

    Super::EndPlay(EndPlayReason);
}

void AArea::InitializeArea()
{
    //@Box Component
    if (!AreaBounds)
    {
        UE_LOGFMT(LogArea, Error, "Area {0}: AreaBounds 컴포넌트가 유효하지 않음", *AreaID.ToString());
        return;
    }

    //@콜리전 이벤트 바인딩
    AreaBounds->OnComponentBeginOverlap.AddDynamic(this, &AArea::OnOverlapBegin);
    AreaBounds->OnComponentEndOverlap.AddDynamic(this, &AArea::OnOverlapEnd);

    //@영역 내 AI 초기화
    InitializeAreaAIInfos();

    //@정리 타이머 설정
    GetWorld()->GetTimerManager().SetTimer(
        CleanupTimerHandle,
        this,
        &AArea::CleanupInvalidReferences,
        CleanupInterval,
        true
    );

    UE_LOGFMT(LogArea, Log, "Area {0} 초기화 완료. 등록된 AI: {1}개", *AreaID.ToString(), AIInfoMap.Num());

#if WITH_EDITOR
    // 에디터에서 디버그 시각화
    if (GetWorld() && GetWorld()->IsPlayInEditor())
    {
        DrawDebugBox(GetWorld(), GetActorLocation(), AreaBounds->GetScaledBoxExtent(), FColor::Green, true, -1.0f, 0, 3.0f);
    }
#endif
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void AArea::InitializeAreaAIInfos()
{
    //@수동 등록된 AI 처리(성능 향상)
    for (AActor* AIActor : RegisteredAIActors)
    {
        if (IsValid(AIActor))
        {
            RegisterAI(AIActor);
        }
    }

    //@자동 감지 활성화된 경우, 주변 AI 탐색(성능 저하)
    if (bAutoDetectAI)
    {
        TArray<AActor*> OverlappingActors;

        //@첫 번째 방법: 현재 박스와 오버랩된 액터 가져오기
        AreaBounds->GetOverlappingActors(OverlappingActors, ACharacterBase::StaticClass());

        //@두 번째 방법: 추가적인 구체 트레이스로 주변 액터 찾기
        if (AIDetectionRadius > 0.0f)
        {
            TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
            ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

            TArray<AActor*> ActorsToIgnore;
            ActorsToIgnore.Add(this);

            TArray<AActor*> FoundActors;
            UKismetSystemLibrary::SphereOverlapActors(
                GetWorld(),
                GetActorLocation(),
                AIDetectionRadius,
                ObjectTypes,
                ACharacterBase::StaticClass(),
                ActorsToIgnore,
                FoundActors
            );

            //@발견된 액터를 기존 목록에 합치기 (중복 제거)
            for (AActor* Actor : FoundActors)
            {
                if (IsValid(Actor) && !OverlappingActors.Contains(Actor))
                {
                    OverlappingActors.Add(Actor);
                }
            }
        }

        //@찾은 모든 AI 액터 등록 (플레이어는 제외)
        for (AActor* Actor : OverlappingActors)
        {
            if (IsValid(Actor) && !Cast<APlayerCharacter>(Actor))
            {
                RegisterAI(Actor);
            }
        }
    }
}

void AArea::RegisterAI(AActor* AIActor)
{
    //@Actor
    if (!IsValid(AIActor))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 유효하지 않은 AI 등록 시도", *AreaID.ToString());
        return;
    }

    //@중복 체크
    TWeakObjectPtr<AActor> AIActorPtr(AIActor);
    if (AIInfoMap.Contains(AIActorPtr))
    {
        UE_LOGFMT(LogArea, Verbose, "Area {0}: 이미 등록된 AI {1}", *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    //@Ability System Insterface
    if (!AIActor->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}이(가) AbilitySystemInterface를 구현하지 않음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    //@ASC
    IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(AIActor);
    UBaseAbilitySystemComponent* AIComponent = Cast<UBaseAbilitySystemComponent>(AbilityInterface->GetAbilitySystemComponent());
    if (!AIComponent)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}의 BaseAbilitySystemComponent를 찾을 수 없음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    //@현재 상태 가져오기 (GlobalStateManager 없이 직접 ASC에서 가져옴)
    FGameplayTag CurrentState = FGameplayTag::EmptyTag;

    //@AI 정보 생성 및 저장
    FAreaAIInfo AIInfo(AIActor, CurrentState, AIComponent);
    AIInfo.StateChangeTime = GetWorld()->GetTimeSeconds();
    AIInfoMap.Add(AIActorPtr, AIInfo);

    //@ASC 이벤트에 바인딩
    AIComponent->CharacterStateEventOnGameplay.AddUFunction(this, "OnAICharacterStateEvent");

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 등록 완료",
        *AreaID.ToString(), *AIActor->GetName());
}

void AArea::UnregisterAI(AActor* AIActor)
{
    //@Actor
    if (!IsValid(AIActor))
    {
        return;
    }

    //@AI 참조 생성
    TWeakObjectPtr<AActor> AIActorPtr(AIActor);

    //@AI 정보 확인
    if (!AIInfoMap.Contains(AIActorPtr))
    {
        return;
    }

    //@ASC 이벤트 바인딩 해제
    const FAreaAIInfo& AIInfo = AIInfoMap[AIActorPtr];
    if (AIInfo.AIASC.IsValid())
    {
        UBaseAbilitySystemComponent* AIComponent = AIInfo.AIASC.Get();
        if (AIComponent)
        {
            AIComponent->CharacterStateEventOnGameplay.RemoveAll(this);
        }
    }

    //@맵에서 제거
    AIInfoMap.Remove(AIActorPtr);

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 등록 해제 완료",
        *AreaID.ToString(), *AIActor->GetName());
}

void AArea::RegisterPlayer(APlayerCharacter* Player)
{
    //@Player Character
    if (!IsValid(Player))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 유효하지 않은 플레이어 등록 시도", *AreaID.ToString());
        return;
    }

    //@TWeakObjectPtr
    TWeakObjectPtr<APlayerCharacter> PlayerPtr(Player);

    //@Objective Detection Component
    UObjectiveDetectionComponent* ObjectiveComp = GetPlayerObjectiveComponent(Player);
    if (!ObjectiveComp)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 플레이어 {1}의 ObjectiveDetectionComponent를 찾을 수 없음",
            *AreaID.ToString(), *Player->GetName());
        return;
    }

    //@중복 체크
    if (PlayerBindings.Contains(PlayerPtr))
    {
        PlayerBindings[PlayerPtr].LastExitTime = 0.0f;
        UE_LOGFMT(LogArea, Verbose, "Area {0}: 플레이어 {1} 바인딩 갱신",
            *AreaID.ToString(), *Player->GetName());
        return;
    }

    //@FPlayerBindingInfo
    FPlayerBindingInfo BindingInfo(Player, ObjectiveComp);
    PlayerBindings.Add(PlayerPtr, BindingInfo);

    // 컴포넌트와 Area 연결
    // ObjectiveComp->SetupWithArea(this); 
    // 주의: ObjectiveDetectionComponent 구현 후 주석 해제

    UE_LOGFMT(LogArea, Log, "Area {0}: 플레이어 {1} 등록 완료",
        *AreaID.ToString(), *Player->GetName());

    //@현재 영역 내 모든 AI 상태 정보를 플레이어에게 알림
    for (auto& Pair : AIInfoMap)
    {
        if (Pair.Key.IsValid() && Pair.Value.CurrentState.IsValid() &&
            Pair.Value.CurrentState.ToString().StartsWith("State."))
        {
            AActor* AIActor = Pair.Key.Get();
            // ObjectiveComp->OnAreaObjectiveStateChanged(AIActor, Pair.Value.CurrentState, this, AreaID);
            // 주의: ObjectiveDetectionComponent 구현 후 주석 해제
        }
    }
}

void AArea::UnregisterPlayer(APlayerCharacter* Player)
{
    //@Player Character
    if (!IsValid(Player))
    {
        return;
    }

    //@TWeakObjectPtr
    TWeakObjectPtr<APlayerCharacter> PlayerPtr(Player);

    //@Contains?
    if (!PlayerBindings.Contains(PlayerPtr))
    {
        return;
    }

    //@현재 시간 가져오기
    float CurrentTime = GetWorld()->GetTimeSeconds();

    //@지연 해제를 위해 시간 업데이트
    PlayerBindings[PlayerPtr].LastExitTime = CurrentTime;

    //@일정 시간 후 정리 작업에서 해제
    UE_LOGFMT(LogArea, Log, "Area {0}: 플레이어 {1} 이탈. 바인딩 해제 지연 시작",
        *AreaID.ToString(), *Player->GetName());
}

void AArea::HandleAIStateChanged(const FGameplayTag& StateTag, AActor* AIActor)
{
    //@Actor
    if (!IsValid(AIActor))
    {
        return;
    }

    //@"State.~"
    if (!StateTag.ToString().StartsWith("State."))
    {
        return;
    }

    //@TWeakObjectPtr
    TWeakObjectPtr<AActor> AIActorPtr(AIActor);

    //@Contains?
    if (!AIInfoMap.Contains(AIActorPtr))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 등록되지 않은 AI {1}의 상태 변경 무시",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    //@이전 상태와 같은지 확인
    FGameplayTag OldState = AIInfoMap[AIActorPtr].CurrentState;
    if (OldState == StateTag)
    {
        return;
    }

    //@상태 정보 업데이트
    AIInfoMap[AIActorPtr].CurrentState = StateTag;
    AIInfoMap[AIActorPtr].StateChangeTime = GetWorld()->GetTimeSeconds();

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 상태 변경: {2} -> {3}",
        *AreaID.ToString(), *AIActor->GetName(),
        OldState.IsValid() ? *OldState.ToString() : TEXT("None"),
        *StateTag.ToString());

    //@플레이어들에게 알림
    for (auto& Pair : PlayerBindings)
    {
        if (Pair.Key.IsValid() && Pair.Value.ObjectiveComponent.IsValid() && Pair.Value.LastExitTime == 0.0f)
        {
            UObjectiveDetectionComponent* ObjectiveComp = Pair.Value.ObjectiveComponent.Get();
            if (ObjectiveComp)
            {
                // ObjectiveComp->OnAreaObjectiveStateChanged(AIActor, StateTag, this, AreaID);
                // 주의: ObjectiveDetectionComponent 구현 후 주석 해제
            }
        }
    }

    //@델리게이트 호출
    AreaAIStateChanged.Broadcast(AIActor, StateTag, this, AreaID);
}

void AArea::CleanupInvalidReferences()
{
    //@현재 시간
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // 1. 무효한 AI 참조 정리
    TArray<TWeakObjectPtr<AActor>> InvalidAIRefs;

    for (auto& Pair : AIInfoMap)
    {
        if (!Pair.Key.IsValid())
        {
            InvalidAIRefs.Add(Pair.Key);
        }
    }

    for (auto& Ref : InvalidAIRefs)
    {
        AIInfoMap.Remove(Ref);
    }

    // 2. 무효하거나 지연 시간이 만료된 플레이어 참조 정리
    TArray<TWeakObjectPtr<APlayerCharacter>> PlayersToRemove;

    for (auto& Pair : PlayerBindings)
    {
        // 무효한 참조이거나, 이탈 후 일정 시간이 지난 경우
        if (!Pair.Key.IsValid() ||
            (Pair.Value.LastExitTime > 0.0f && !Pair.Value.ShouldDelayUnbind(CurrentTime)))
        {
            PlayersToRemove.Add(Pair.Key);
        }
    }

    for (auto& PlayerRef : PlayersToRemove)
    {
        // 컴포넌트가 유효하면 언바인딩 처리
        if (PlayerBindings.Contains(PlayerRef) &&
            PlayerBindings[PlayerRef].ObjectiveComponent.IsValid())
        {
            UObjectiveDetectionComponent* ObjectiveComp = PlayerBindings[PlayerRef].ObjectiveComponent.Get();
            if (ObjectiveComp)
            {
                // ObjectiveComp->UnbindFromArea(this);
                // 주의: ObjectiveDetectionComponent 구현 후 주석 해제
            }
        }

        PlayerBindings.Remove(PlayerRef);
    }

    if (InvalidAIRefs.Num() > 0 || PlayersToRemove.Num() > 0)
    {
        UE_LOGFMT(LogArea, Log, "Area {0}: 정리 완료. 제거된 AI: {1}, 제거된 플레이어: {2}",
            *AreaID.ToString(), InvalidAIRefs.Num(), PlayersToRemove.Num());
    }

    LastCleanupTime = CurrentTime;
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void AArea::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    // 액터 유효성 검사
    if (!IsValid(OtherActor))
    {
        return;
    }

    // 플레이어 검사
    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (Player)
    {
        RegisterPlayer(Player);
        return;
    }

    // AI 검사 (플레이어가 아니고 캐릭터인 경우)
    ACharacterBase* AICharacter = Cast<ACharacterBase>(OtherActor);
    if (AICharacter && !Cast<APlayerCharacter>(AICharacter))
    {
        RegisterAI(AICharacter);
        return;
    }
}

void AArea::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // 액터 유효성 검사
    if (!IsValid(OtherActor))
    {
        return;
    }

    // 플레이어 검사
    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (Player)
    {
        UnregisterPlayer(Player);
        return;
    }

    // AI 검사
    ACharacterBase* AICharacter = Cast<ACharacterBase>(OtherActor);
    if (AICharacter && !Cast<APlayerCharacter>(AICharacter))
    {
        // AI 등록 해제는 지연시키지 않고 바로 처리
        UnregisterAI(AICharacter);
        return;
    }
}

void AArea::OnAICharacterStateEvent(const FGameplayTag& StateTag, AActor* SourceActor)
{
    //@상태 변경 처리 함수 호출
    HandleAIStateChanged(StateTag, SourceActor);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
TArray<AActor*> AArea::GetAIInArea() const
{
    TArray<AActor*> Result;

    for (auto& Pair : AIInfoMap)
    {
        if (Pair.Key.IsValid())
        {
            Result.Add(Pair.Key.Get());
        }
    }

    return Result;
}

UObjectiveDetectionComponent* AArea::GetPlayerObjectiveComponent(APlayerCharacter* Player)
{
    if (!IsValid(Player))
    {
        return nullptr;
    }

    //@Ojbective Detection Component
    UObjectiveDetectionComponent* ObjectiveComp = Player->FindComponentByClass<UObjectiveDetectionComponent>();
    if (!ObjectiveComp)
    {
        UE_LOGFMT(LogArea, Warning, "플레이어 {0}에서 ObjectiveDetectionComponent를 찾을 수 없음", *Player->GetName());
    }

    return ObjectiveComp;
}
#pragma endregion