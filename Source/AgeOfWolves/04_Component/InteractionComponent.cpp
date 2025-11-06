#include "InteractionComponent.h"
#include "Logging/StructuredLog.h"

#include "03_Player/BasePlayerController.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "04_Component/ObjectiveDetectionComponent.h"
#include "19_Interface/InteractionInterface.h"

DEFINE_LOG_CATEGORY(LogInteraction)

//@Defualt Setting
#pragma region Default Setting
UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UInteractionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //@거리 체크 Interval 기반 최적화
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastDistanceCheckTime >= DistanceCheckInterval)
    {
        CommitInteraction();
        LastDistanceCheckTime = CurrentTime;
    }
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    //@외부 바인딩 해제
    UnbindExeternalBindToASCComp();
    UnbindExternalBindToODComp();

    //@Heap 정리
    InteractionHeap.Reset();

    //@캐시 정리
    OwnerPawn.Reset();

    Super::EndPlay(EndPlayReason);
}
void UInteractionComponent::InitializeInteractionComp()
{
    //@외부 컴포넌트 바인딩
    ExternalBindToODComp();
    ExeternalBindToASComp();

    //@PC 캐싱
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: PC 캐싱 실패", __FUNCDNAME__);
        return;
    }

    //@Pawn 캐싱
    OwnerPawn = PC->GetPawn();
    if (!OwnerPawn.IsValid())
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: Pawn 캐싱 실패", __FUNCDNAME__);
        return;
    }

    //@Heap 초기화
    InteractionHeap.Reset();
    InteractionHeap.Reserve(10);  // 예상 최대 상호작용 개수

    UE_LOGFMT(LogInteraction, Log, "{0}: 초기화 완료", __FUNCDNAME__);
}

void UInteractionComponent::ExeternalBindToASComp()
{
    //@PC 유효성 체크
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOGFMT(LogInteraction, Error, "ASC 바인딩 실패: PC 없음");
        return;
    }

    //@BasePC 캐스팅
    ABasePlayerController* BasePC = Cast<ABasePlayerController>(PC);
    if (!BasePC)
    {
        UE_LOGFMT(LogInteraction, Error, "{0}: BasePC 캐스팅 실패", __FUNCDNAME__);
        return;
    }

    //@PlayerState 가져오기
    APlayerStateBase* PS = BasePC->GetPlayerState<APlayerStateBase>();
    if (!PS)
    {
        UE_LOGFMT(LogInteraction, Error, "{0}: PlayerState 없음", __FUNCDNAME__);
        return;
    }

    //@ASC 가져오기
    UBaseAbilitySystemComponent* ASC = Cast<UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent());
    if (!ASC)
    {
        UE_LOGFMT(LogInteraction, Error, "{0}: ASC 캐스팅 실패", __FUNCDNAME__);
        return;
    }

    //@델리게이트 바인딩
    ASC->CharacterStateEventOnGameplay.AddUFunction(this, "OnOwnerStateEventOnGameplay");

    UE_LOGFMT(LogInteraction, Log, "ASC 바인딩 완료");
}

void UInteractionComponent::UnbindExeternalBindToASCComp()
{
    //@PC 유효성 체크
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC) return;

    //@BasePC 캐스팅
    ABasePlayerController* BasePC = Cast<ABasePlayerController>(PC);
    if (!BasePC) return;

    //@PlayerState 가져오기
    APlayerStateBase* PS = BasePC->GetPlayerState<APlayerStateBase>();
    if (!PS) return;

    //@ASC 가져오기
    UBaseAbilitySystemComponent* ASC = Cast<UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent());
    if (!ASC) return;

    //@델리게이트 언바인딩
    ASC->CharacterStateEventOnGameplay.RemoveAll(this);

    UE_LOGFMT(LogInteraction, Log, "ASC 언바인딩 완료");
}

void UInteractionComponent::ExternalBindToODComp()
{
    //@PC 유효성 체크
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOGFMT(LogInteraction, Warning, "OD 바인딩 실패: PC 없음");
        return;
    }

    //@OD 컴포넌트 찾기
    UObjectiveDetectionComponent* ODComp = PC->FindComponentByClass<UObjectiveDetectionComponent>();
    if (!ODComp) return;

    //@델리게이트 바인딩
    ODComp->DetectedAIStateChanged.AddUFunction(this, "OnDetectedAIStateChanged");
    ODComp->ExecutionTargetChanged.AddUFunction(this, "OnExecutionTargetChanged");
    ODComp->AmbushTargetChanged.AddUFunction(this, "OnAmbushTargetChanged");
    ODComp->DetectedStructureChanged.AddUFunction(this, "OnDetectedStructureChanged");
}

void UInteractionComponent::UnbindExternalBindToODComp()
{
    //@PC 유효성 체크
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC) return;

    //@OD 컴포넌트 찾기
    UObjectiveDetectionComponent* ODComp = PC->FindComponentByClass<UObjectiveDetectionComponent>();
    if (!ODComp) return;

    //@델리게이트 언바인딩
    ODComp->DetectedAIStateChanged.RemoveAll(this);
    ODComp->ExecutionTargetChanged.RemoveAll(this);
    ODComp->AmbushTargetChanged.RemoveAll(this);
    ODComp->DetectedStructureChanged.RemoveAll(this);

    UE_LOGFMT(LogInteraction, Log, "OD 언바인딩 완료");
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UInteractionComponent::RegisterPotentialInteraction(AActor* TargetActor)
{
    if (!TargetActor) return;

    //@ObjectTag 가져오기 (Character 타입)
    FGameplayTag ObjectTag;
    if (ACharacterBase* Character = Cast<ACharacterBase>(TargetActor))
    {
        ObjectTag = Character->GetCharacterTag();
    }

    if (!ObjectTag.IsValid()) return;

    //@매칭되는 상호작용 템플릿 찾기
    for (const FPotentialInteraction& Interaction : PotentialInteractions)
    {
        if (Interaction.ObjectTag == ObjectTag)
        {
            //@타입별로 개별 등록
            RegisterPotentialInteraction(TargetActor, Interaction.InteractionType);
        }
    }
}

void UInteractionComponent::RegisterPotentialInteraction(AActor* TargetActor, EInteractionType InteractionType)
{
    //@유효성 체크
    if (!TargetActor || InteractionType == EInteractionType::None || InteractionType >= EInteractionType::MAX)
        return;

    //@ObjectTag 가져오기
    FGameplayTag ObjectTag;
    if (ACharacterBase* Character = Cast<ACharacterBase>(TargetActor))
    {
        ObjectTag = Character->GetCharacterTag();
    }
    else if (IInteractionInterface* InteractionInterface = Cast<IInteractionInterface>(TargetActor))
    {
        ObjectTag = InteractionInterface->GetObjectTag();
    }

    if (!ObjectTag.IsValid()) return;

    //@중복 체크
    for (const FPotentialInteraction& Existing : InteractionHeap)
    {
        if (Existing.TargetActor == TargetActor && Existing.InteractionType == InteractionType)
        {
            UE_LOGFMT(LogInteraction, Warning, "이미 등록된 상호작용: {0} - {1}",
                *TargetActor->GetName(), static_cast<uint8>(InteractionType));
            return;
        }
    }

    //@템플릿에서 매칭되는 상호작용 찾기
    const FPotentialInteraction* FoundInteraction = nullptr;
    for (const FPotentialInteraction& Interaction : PotentialInteractions)
    {
        if (Interaction.ObjectTag == ObjectTag && Interaction.InteractionType == InteractionType)
        {
            FoundInteraction = &Interaction;
            break;
        }
    }

    if (!FoundInteraction) return;

    //@새 상호작용 생성 (Actor 포함)
    FPotentialInteraction NewInteraction = *FoundInteraction;
    NewInteraction.TargetActor = TargetActor;

    //@Heap에 추가 (TArray::HeapPush 사용)
    InteractionHeap.HeapPush(NewInteraction);

    UE_LOGFMT(LogInteraction, Log, "상호작용 등록: {0} - Type: {1} | Priority: {2}",
        *TargetActor->GetName(), static_cast<uint8>(InteractionType), NewInteraction.Priority);
}

void UInteractionComponent::RemovePotentialInteraction(AActor* TargetActor)
{
    if (!TargetActor) return;

    bool bRemoved = false;

    //@역순으로 순회하며 제거 (인덱스 안전)
    for (int32 i = InteractionHeap.Num() - 1; i >= 0; --i)
    {
        if (InteractionHeap[i].TargetActor == TargetActor)
        {
            InteractionHeap.HeapRemoveAt(i);
            bRemoved = true;
        }
    }

    if (bRemoved)
    {
        UE_LOGFMT(LogInteraction, Log, "모든 상호작용 제거 - 액터: {0}", *TargetActor->GetName());
        UpdateCurrentPriorityInteraction();
    }
}

void UInteractionComponent::RemovePotentialInteraction(AActor* TargetActor, EInteractionType InteractionType)
{
    //@유효성 체크
    if (!TargetActor || InteractionType == EInteractionType::None || InteractionType >= EInteractionType::MAX)
        return;

    //@매칭되는 항목 찾기
    for (int32 i = 0; i < InteractionHeap.Num(); ++i)
    {
        const FPotentialInteraction& Interaction = InteractionHeap[i];

        if (Interaction.TargetActor == TargetActor && Interaction.InteractionType == InteractionType)
        {
            //@Heap에서 제거
            InteractionHeap.HeapRemoveAt(i);

            UE_LOGFMT(LogInteraction, Log, "상호작용 제거: {0} - Type: {1}",
                *TargetActor->GetName(), static_cast<uint8>(InteractionType));

            //@우선순위 재평가
            UpdateCurrentPriorityInteraction();
            return;
        }
    }
}

void UInteractionComponent::CommitInteraction()
{
    //@거리 조건 업데이트
    UpdateDistanceConditions();

    //@우선순위 재평가
    UpdateCurrentPriorityInteraction();
}

void UInteractionComponent::UpdateDistanceConditions()
{
    if (!OwnerPawn.IsValid()) return;

    FVector PlayerLocation = OwnerPawn->GetActorLocation();
    bool bAnyChanged = false;

    //@모든 상호작용의 거리 조건 업데이트
    for (FPotentialInteraction& Interaction : InteractionHeap)
    {
        if (!Interaction.TargetActor.IsValid()) continue;

        //@거리 제곱 비교 (최적화)
        float DistSq = FVector::DistSquared(PlayerLocation, Interaction.TargetActor->GetActorLocation());
        bool bInRange = (DistSq <= FMath::Square(Interaction.RequiredDistance));

        //@조건 변경 감지
        if (Interaction.bAdditionalConditionsMet != bInRange)
        {
            Interaction.bAdditionalConditionsMet = bInRange;
            bAnyChanged = true;
        }
    }

    //@변경 사항이 있으면 Heap 재정렬
    if (bAnyChanged)
    {
        InteractionHeap.Heapify();
    }
}

void UInteractionComponent::UpdateCurrentPriorityInteraction()
{
    //@이전 상호작용 백업
    FPotentialInteraction PreviousInteraction = CurrentPriorityInteraction;
    AActor* PreviousActor = PreviousInteraction.TargetActor.Get();

    //@새로운 최고 우선순위 찾기
    AActor* NewPriorityActor = nullptr;
    CurrentPriorityInteraction = FindHighestPriorityInteraction(NewPriorityActor);

    //@우선순위 변경 감지
    bool bPriorityChanged = (CurrentPriorityInteraction.ObjectTag != PreviousInteraction.ObjectTag ||
        CurrentPriorityInteraction.InteractionType != PreviousInteraction.InteractionType);

    if (!bPriorityChanged) return;

    //@이전 상호작용 취소
    if (PreviousInteraction.ObjectTag.IsValid() && PreviousActor)
    {
        CancelInteractionActivated(PreviousActor, PreviousInteraction);
    }

    //@새 상호작용 활성화
    if (CurrentPriorityInteraction.ObjectTag.IsValid() && NewPriorityActor)
    {
        TryActivateInteraction(NewPriorityActor, CurrentPriorityInteraction);
    }
}

void UInteractionComponent::TryActivateInteraction(AActor* TargetActor, const FPotentialInteraction& Interaction)
{
    //@유효성 체크
    if (!Interaction.IsFullyAvailable() || !TargetActor) return;

    //@델리게이트 브로드캐스트
    PotentialInteractionChanged.Broadcast(TargetActor, Interaction);

    UE_LOGFMT(LogInteraction, Log, "{0}: 상호작용 활성화 - 액터: {1} | 타입: {2}",
        __FUNCDNAME__, *TargetActor->GetName(), static_cast<uint8>(Interaction.InteractionType));
}

void UInteractionComponent::CancelInteractionActivated(AActor* TargetActor, const FPotentialInteraction& Interaction)
{
    if (!Interaction.ObjectTag.IsValid()) return;

    //@취소된 상호작용 생성 (bAdditionalConditionsMet = false)
    FPotentialInteraction CancelledInteraction = Interaction;
    CancelledInteraction.bAdditionalConditionsMet = false;

    //@델리게이트 브로드캐스트
    PotentialInteractionChanged.Broadcast(TargetActor, CancelledInteraction);

    UE_LOGFMT(LogInteraction, Log, "{0}: 상호작용 취소 - 액터: {1} | 타입: {2}",
        __FUNCDNAME__,
        TargetActor ? *TargetActor->GetName() : TEXT("없음"),
        static_cast<uint8>(Interaction.InteractionType));
}

void UInteractionComponent::HandleTargetTransition(AActor* NewTarget, EInteractionType TargetType)
{
    //@역순으로 순회하며 해당 타입 제거
    for (int32 i = InteractionHeap.Num() - 1; i >= 0; --i)
    {
        if (InteractionHeap[i].InteractionType == TargetType)
        {
            InteractionHeap.HeapRemoveAt(i);
        }
    }

    //@새 타겟 등록
    if (NewTarget)
    {
        RegisterPotentialInteraction(NewTarget, TargetType);
    }
}

void UInteractionComponent::CleanupEmptyActorEntries()
{
    InteractionHeap.Empty();
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UInteractionComponent::OnOwnerStateEventOnGameplay(AActor* OwnerActor, const FGameplayTag& StateTag)
{
    //@현재 미사용
}

void UInteractionComponent::OnDetectedAIStateChanged(const FGameplayTag& StateTag, AActor* ObjectiveActor)
{
    //@유효성 체크
    if (!ObjectiveActor || !StateTag.IsValid()) return;

    //@Dead 상태인 경우 모든 상호작용 제거
    if (StateTag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Dead")))
    {
        RemovePotentialInteraction(ObjectiveActor);
    }
}

void UInteractionComponent::OnExecutionTargetChanged(AActor* PotentialExecutionTarget)
{
    //@처형 타겟 전체 교체
    HandleTargetTransition(PotentialExecutionTarget, EInteractionType::Execution);
}

void UInteractionComponent::OnAmbushTargetChanged(AActor* PotentialAmbushTarget)
{
    //@암살 타겟 전체 교체
    HandleTargetTransition(PotentialAmbushTarget, EInteractionType::Ambush);
}

void UInteractionComponent::OnDetectedStructureChanged(AActor* DetectedStructureActor, bool isEnteredDetection)
{
    //@감지 벗어남: 상호작용 제거
    if (DetectedStructureActor && !isEnteredDetection)
    {
        RemovePotentialInteraction(DetectedStructureActor, EInteractionType::Shrine);
        UE_LOGFMT(LogInteraction, Log, "구조물 감지 해제");
        return;
    }

    //@감지 진입: 상호작용 등록
    if (DetectedStructureActor && isEnteredDetection)
    {
        RegisterPotentialInteraction(DetectedStructureActor, EInteractionType::Shrine);
        UE_LOGFMT(LogInteraction, Log, "구조물 감지 등록 - 액터: {0}", *DetectedStructureActor->GetName());
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
bool UInteractionComponent::IsInteractionTypeAvailable(EInteractionType InteractionType) const
{
    //@해당 타입의 가용 상호작용 찾기
    for (const FPotentialInteraction& Interaction : InteractionHeap)
    {
        if (Interaction.InteractionType == InteractionType &&
            Interaction.IsFullyAvailable() &&
            Interaction.TargetActor.IsValid())
        {
            return true;
        }
    }

    return false;
}

TArray<FPotentialInteraction> UInteractionComponent::FindInteraction(OUT AActor* TargetActor)
{
    TArray<FPotentialInteraction> Result;
    if (!TargetActor) return Result;

    //@해당 Actor의 모든 상호작용 수집
    for (const FPotentialInteraction& Interaction : InteractionHeap)
    {
        if (Interaction.TargetActor == TargetActor)
        {
            Result.Add(Interaction);
        }
    }

    return Result;
}

FPotentialInteraction* UInteractionComponent::FindInteraction(OUT AActor* TargetActor, EInteractionType Type)
{
    if (!TargetActor) return nullptr;

    //@해당 Actor의 특정 타입 상호작용 찾기
    for (FPotentialInteraction& Interaction : InteractionHeap)
    {
        if (Interaction.TargetActor == TargetActor && Interaction.InteractionType == Type)
        {
            return &Interaction;
        }
    }

    return nullptr;
}

FPotentialInteraction UInteractionComponent::FindHighestPriorityInteraction(OUT AActor*& OutActor)
{
    OutActor = nullptr;

    //@Heap이 비어있으면 빈 상호작용 반환
    if (InteractionHeap.Num() == 0)
    {
        return FPotentialInteraction();
    }

    //@Top부터 순회하며 가용한 상호작용 찾기
    for (int32 i = 0; i < InteractionHeap.Num(); ++i)
    {
        const FPotentialInteraction& Interaction = InteractionHeap[i];

        //@Actor 유효성 체크
        if (!Interaction.TargetActor.IsValid())
        {
            continue;
        }

        //@완전 가용 상태 확인
        if (Interaction.IsFullyAvailable())
        {
            OutActor = Interaction.TargetActor.Get();
            return Interaction;
        }
    }

    return FPotentialInteraction();
}

FPotentialInteraction UInteractionComponent::GetHighestPriorityInteraction() const
{
    return CurrentPriorityInteraction;
}

FPotentialInteraction UInteractionComponent::GetHighestPriorityInteractionForActor(AActor* TargetActor)
{
    if (!TargetActor) return FPotentialInteraction();

    FPotentialInteraction HighestPriority;
    bool bFound = false;

    //@해당 Actor의 모든 상호작용 중 최고 우선순위 찾기
    for (const FPotentialInteraction& Interaction : InteractionHeap)
    {
        if (Interaction.TargetActor == TargetActor &&
            Interaction.IsFullyAvailable() &&
            (!bFound || Interaction.Priority > HighestPriority.Priority))
        {
            HighestPriority = Interaction;
            bFound = true;
        }
    }

    return bFound ? HighestPriority : FPotentialInteraction();
}
#pragma endregion