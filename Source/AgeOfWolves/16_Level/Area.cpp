#include "Area.h"
#include "Logging/StructuredLog.h"

#include "Components/BoxComponent.h"

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

    // 더 큰 범위의 Box Extent 설정
    // 가로 2000, 세로 2000, 높이 500 단위로 설정
    AreaBounds->SetBoxExtent(FVector(2000.0f, 2000.0f, 500.0f));

    //@Area ID
    AreaID = FGuid::NewGuid();

    //@초기화
    MAIInfos.Empty();
    MPlayerBindings.Empty();
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

    //@플레이어 등록 해제
    UnregisterPlayer();

    //@모든 AI 등록 해제
    UnregisterAllAI();

    UE_LOGFMT(LogArea, Log, "Area {0} 정리 완료", *AreaID.ToString());

    Super::EndPlay(EndPlayReason);
}

void AArea::InternalBindToAI(TWeakObjectPtr<AActor> AIActorPtr)
{
    //@Actor가 유효한지 확인
    AActor* AIActor = AIActorPtr.Get();
    if (!IsValid(AIActor))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 바인딩 실패 - 유효하지 않은 AI", *AreaID.ToString());
        return;
    }

    //@AI 정보가 존재하는지 확인
    if (!MAIInfos.Contains(AIActorPtr))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 바인딩 실패 - 등록되지 않은 AI {1}",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    //@ASC 가져오기
    IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(AIActor);
    UBaseAbilitySystemComponent* BaseASC = Cast<UBaseAbilitySystemComponent>(AbilityInterface->GetAbilitySystemComponent());
    if (!BaseASC)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}의 BaseAbilitySystemComponent를 찾을 수 없음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    //@AI 정보 업데이트
    FAreaAIInfo& AIInfo = MAIInfos[AIActorPtr];
    AIInfo.AIASC = BaseASC;

    //@ASC 이벤트에 바인딩
    BaseASC->CharacterStateEventOnGameplay.AddUFunction(this, "OnAICharacterStateEvent");

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 이벤트 바인딩 완료",
        *AreaID.ToString(), *AIActor->GetName());
}

void AArea::UnbindFromAI(TWeakObjectPtr<AActor> AIActorPtr)
{
    //@Actor가 유효한지 확인
    AActor* AIActor = AIActorPtr.Get();
    if (!IsValid(AIActor))
    {
        UE_LOGFMT(LogArea, Log, "Area {0}: 언바인딩 스킵 - 유효하지 않은 AI", *AreaID.ToString());
        return;
    }

    //@AI 정보가 존재하는지 확인
    if (!MAIInfos.Contains(AIActorPtr))
    {
        UE_LOGFMT(LogArea, Log, "Area {0}: 언바인딩 스킵 - 등록되지 않은 AI {1}",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    //@AI 정보 가져오기
    FAreaAIInfo& AIInfo = MAIInfos[AIActorPtr];

    //@ASC 가져오기
    UBaseAbilitySystemComponent* BaseASC = AIInfo.AIASC.Get();
    if (!IsValid(BaseASC))
    {
        UE_LOGFMT(LogArea, Log, "Area {0}: 언바인딩 스킵 - AI {1}의 유효한 ASC 없음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    //@Rmove All
    BaseASC->CharacterStateEventOnGameplay.RemoveAll(this);
    
    
    AIInfo.AIASC = nullptr;

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 이벤트 언바인딩 완료",
        *AreaID.ToString(), *AIActor->GetName());
}

void AArea::UnbindFromAllAI()
{
    int32 UnbindCount = 0;

    //@모든 등록된 AI 순회
    for (auto& Pair : MAIInfos)
    {
        TWeakObjectPtr<AActor> AIActorPtr = Pair.Key;
        FAreaAIInfo& AIInfo = Pair.Value;

        //@ASC가 유효한지 확인
        UBaseAbilitySystemComponent* BaseASC = AIInfo.AIASC.Get();
        if (!IsValid(BaseASC))
        {
            continue;
        }

        //@ASC 이벤트에서 언바인딩
        BaseASC->CharacterStateEventOnGameplay.RemoveAll(this);

        //@AI 정보에서 ASC 제거
        AIInfo.AIASC = nullptr;

        UnbindCount++;
    }

    UE_LOGFMT(LogArea, Log, "Area {0}: 모든 AI 이벤트 언바인딩 완료 - 총 {1}개",
        *AreaID.ToString(), UnbindCount);
}

void AArea::InternalBindToPlayer(TWeakObjectPtr<APlayerCharacter> Player)
{
    //@Player 유효성 검사
    if (!Player.IsValid())
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 플레이어 바인딩 실패 - 유효하지 않은 플레이어", *AreaID.ToString());
        return;
    }

    //@이미 등록된 플레이어인지 확인
    if (!MPlayerBindings.Contains(Player))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 플레이어 바인딩 실패 - 등록되지 않은 플레이어 {1}",
            *AreaID.ToString(), *Player->GetName());
        return;
    }

    //@내부 바인딩...

    UE_LOGFMT(LogArea, Log, "Area {0}: 플레이어 {1} 이벤트 바인딩 완료",
        *AreaID.ToString(), *Player->GetName());
}

void AArea::UnbindFromPlayer(TWeakObjectPtr<APlayerCharacter> Player)
{
    //@Player 유효성 검사
    if (!Player.IsValid())
    {
        UE_LOGFMT(LogArea, Log, "Area {0}: 플레이어 언바인딩 스킵 - 유효하지 않은 플레이어", *AreaID.ToString());
        return;
    }

    //@등록된 플레이어인지 확인
    if (!MPlayerBindings.Contains(Player))
    {
        UE_LOGFMT(LogArea, Log, "Area {0}: 플레이어 언바인딩 스킵 - 등록되지 않은 플레이어 {1}",
            *AreaID.ToString(), *Player->GetName());
        return;
    }

    //@내부 바인딩 해제...

    UE_LOGFMT(LogArea, Log, "Area {0}: 플레이어 {1} 이벤트 언바인딩 완료",
        *AreaID.ToString(), *Player->GetName());
}

void AArea::UnbindFromPlayer()
{
    //@IsEmpty?
    if (MPlayerBindings.IsEmpty())
    {
        UE_LOG(LogArea, Warning, TEXT("[AArea::UnbindFromPlayer] 등록된 플레이어가 없습니다. AreaID: %s"), *AreaID.ToString());
        return;
    }

    //@MPlayerBindings
    for (auto& PlayerBinding : MPlayerBindings)
    {
        if (PlayerBinding.Key.IsValid())
        {
            
            UE_LOG(LogArea, Log, TEXT("[AArea::UnbindFromPlayer] 플레이어 바인딩 해제: %s, AreaID: %s"),
                *PlayerBinding.Key->GetName(), *AreaID.ToString());
            
            //@내부 바인딩 해제...
            UnbindFromPlayer(PlayerBinding.Key);
        }
    }

    UE_LOG(LogArea, Log, TEXT("[AArea::UnbindFromPlayer] 모든 플레이어 바인딩 해제 완료. AreaID: %s"), *AreaID.ToString());
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

    UE_LOGFMT(LogArea, Log, "Area {0} 초기화 완료. 등록된 AI: {1}개", *AreaID.ToString(), MAIInfos.Num());

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
    if (MAIInfos.Contains(AIActorPtr))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 이미 등록된 AI {1}", *AreaID.ToString(), *AIActor->GetName());
        return;
    }
    //@Ability System Insterface
    if (!AIActor->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}이(가) AbilitySystemInterface를 구현하지 않음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }


    //@현재 상태 가져오기 (GlobalStateManager 없이 직접 ASC에서 가져옴)
    FGameplayTag CurrentState = FGameplayTag::RequestGameplayTag("State.Alive");

    //@AI 정보 생성 및 저장
    FAreaAIInfo AIInfo(AIActor, CurrentState, nullptr);
    AIInfo.StateChangeTime = GetWorld()->GetTimeSeconds();
    MAIInfos.Add(AIActorPtr, AIInfo);

    //@내부 바인딩...
    InternalBindToAI(AIActorPtr);


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
    if (!MAIInfos.Contains(AIActorPtr))
    {
        return;
    }

    //@내부 바인딩 해제...
    UnbindFromAI(AIActorPtr);

    //@맵에서 제거
    MAIInfos.Remove(AIActorPtr);

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 등록 해제 완료",
        *AreaID.ToString(), *AIActor->GetName());
}

void AArea::UnregisterAllAI()
{
    // AI 등록 정보가 비어있으면 얼리 리턴
    if (MAIInfos.IsEmpty())
    {
        UE_LOG(LogArea, Warning, TEXT("[AArea::UnregisterAllAI] 등록된 AI가 없습니다. AreaID: %s"), *AreaID.ToString());
        return;
    }

    UE_LOG(LogArea, Log, TEXT("[AArea::UnregisterAllAI] 영역 내 모든 AI 등록 해제 시작. AreaID: %s, AI 개수: %d"),
        *AreaID.ToString(), MAIInfos.Num());

    //@모든 AI에 대하여 바인딩 해제
    UnbindFromAllAI();

    //@MAIInfos 초기화
    MAIInfos.Empty();

    UE_LOG(LogArea, Log, TEXT("[AArea::UnregisterAllAI] 영역 내 모든 AI 등록 해제 완료. AreaID: %s"), *AreaID.ToString());
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

    //@중복 체크
    if (MPlayerBindings.Contains(PlayerPtr))
    {
        MPlayerBindings[PlayerPtr].LastExitTime = 0.0f;
        UE_LOGFMT(LogArea, Warning, "Area {0}: 플레이어 {1} 바인딩 갱신",
            *AreaID.ToString(), *Player->GetName());
        return;
    }

    //@FPlayerBindingInfo
    FPlayerBindingInfo BindingInfo(Player);
    MPlayerBindings.Add(PlayerPtr, BindingInfo);

    //@내부 바인딩...
    InternalBindToPlayer(PlayerPtr);

    UE_LOGFMT(LogArea, Log, "Area {0}: 플레이어 {1} 등록 완료",
        *AreaID.ToString(), *Player->GetName());

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
    if (!MPlayerBindings.Contains(PlayerPtr))
    {
        return;
    }

    //@내부 바인딩 해제...
    UnbindFromPlayer(PlayerPtr);

    //@현재 시간 가져오기
    float CurrentTime = GetWorld()->GetTimeSeconds();

    //@지연 해제를 위해 시간 업데이트
    MPlayerBindings[PlayerPtr].LastExitTime = CurrentTime;

    //@일정 시간 후 정리 작업에서 해제
    UE_LOGFMT(LogArea, Log, "Area {0}: 플레이어 {1} 이탈. 바인딩 해제 지연 시작",
        *AreaID.ToString(), *Player->GetName());
}

void AArea::UnregisterPlayer()
{
    // MPlayerBindings이 비어있으면 얼리 리턴
    if (MPlayerBindings.IsEmpty())
    {
        UE_LOG(LogArea, Warning, TEXT("[AArea::UnregisterPlayer] 등록된 플레이어가 없습니다. AreaID: %s"), *AreaID.ToString());
        return;
    }

    //@바인딩 해제
    UnbindFromPlayer();

    //@MPlayerBindings 초기화
    MPlayerBindings.Empty();

    UE_LOG(LogArea, Log, TEXT("[AArea::UnregisterPlayer] 모든 플레이어 등록 해제 완료. AreaID: %s"), *AreaID.ToString());
}

void AArea::HandleAIStateChanged(AActor* AIActor, const FGameplayTag& StateTag)
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
    if (!MAIInfos.Contains(AIActorPtr))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 등록되지 않은 AI {1}의 상태 변경 무시",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    //@이전 상태와 같은지 확인
    FGameplayTag OldState = MAIInfos[AIActorPtr].CurrentState;
    if (OldState == StateTag)
    {
        return;
    }

    //@상태 정보 업데이트
    MAIInfos[AIActorPtr].CurrentState = StateTag;
    MAIInfos[AIActorPtr].StateChangeTime = GetWorld()->GetTimeSeconds();

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 상태 변경: {2} -> {3}",
        *AreaID.ToString(), *AIActor->GetName(),
        OldState.IsValid() ? *OldState.ToString() : TEXT("None"),
        *StateTag.ToString());

    //@델리게이트 호출
    AreaAIStateChanged.Broadcast(AIActor, StateTag, this, AreaID);
}

void AArea::CleanupInvalidReferences()
{
    //@현재 시간
    float CurrentTime = GetWorld()->GetTimeSeconds();

    // 1. 무효한 AI 참조 정리
    TArray<TWeakObjectPtr<AActor>> InvalidAIRefs;

    for (auto& Pair : MAIInfos)
    {
        if (!Pair.Key.IsValid())
        {
            InvalidAIRefs.Add(Pair.Key);
        }
    }

    for (auto& Ref : InvalidAIRefs)
    {
        MAIInfos.Remove(Ref);
    }

    // 2. 무효하거나 지연 시간이 만료된 플레이어 참조 정리
    TArray<TWeakObjectPtr<APlayerCharacter>> PlayersToRemove;

    for (auto& Pair : MPlayerBindings)
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
        if (MPlayerBindings.Contains(PlayerRef))
        {

        }

        MPlayerBindings.Remove(PlayerRef);
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
}

void AArea::OnAICharacterStateEvent(AActor* Actor, const FGameplayTag& StateTag)
{
    //@상태 변경 처리 함수 호출
    HandleAIStateChanged(Actor, StateTag);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
TArray<AActor*> AArea::GetAIInArea() const
{
    TArray<AActor*> Result;

    for (auto& Pair : MAIInfos)
    {
        if (Pair.Key.IsValid())
        {
            Result.Add(Pair.Key.Get());
        }
    }

    return Result;
}

TArray<FAreaAIInfo> AArea::GetAreaAIInfos() const
{
    TArray<FAreaAIInfo> AIInfos;

    for (auto& AIInfoPair : MAIInfos)
    {
        if (AIInfoPair.Key.IsValid())
        {
            AIInfos.Add(AIInfoPair.Value);
        }
    }

    return AIInfos;
}
#pragma endregion