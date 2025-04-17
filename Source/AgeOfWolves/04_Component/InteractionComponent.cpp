#include "InteractionComponent.h"
#include "Logging/StructuredLog.h"

#include "03_Player/BasePlayerController.h"
#include "03_Player/PlayerStateBase.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "04_Component/ObjectiveDetectionComponent.h"

DEFINE_LOG_CATEGORY(LogInteraction)

//@Defualt Setting
#pragma region Default Setting
UInteractionComponent::UInteractionComponent()
{
    PrimaryComponentTick.bCanEverTick = true; 

    //@캐싱
    OwnerPawn.Reset();

    //@Potention Interactions
    PotentialInteractions.Empty();
    MPotentialInteractions.Reset();

}

void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //@매 틱마다 상호작용 거리 업데이트
    CommitInteraction();
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    //@외부 바인딩 해제
    UnbindExeternalBindToASCComp();
    UnbindExternalBindToODComp();

    //@캐싱
    OwnerPawn.Reset();

	Super::EndPlay(EndPlayReason);
}

void UInteractionComponent::ExeternalBindToASComp()
{
    //@PC
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOGFMT(LogInteraction, Error, "ASC 바인딩 실패: 소유자가 PlayerController가 아님");
        return;
    }

    ABasePlayerController* BasePC = Cast< ABasePlayerController>(PC);
    if (!BasePC)
    {
        UE_LOGFMT(LogInteraction, Error, "{0}: Base Player Controller가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    APlayerStateBase* PS = BasePC->GetPlayerState<APlayerStateBase>();
    if (!PS)
    {
        UE_LOGFMT(LogInteraction, Error, "{0}: Player State가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    UBaseAbilitySystemComponent* ASC = Cast<UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent());
    if (!ASC)
    {
        UE_LOGFMT(LogInteraction, Error, "{0}: Base ASC가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    //@외부 바인딩...
    ASC->CharacterStateEventOnGameplay.AddUFunction(this, "OnOwnerStateEventOnGameplay");

    UE_LOGFMT(LogInteraction, Log, "BaseAbilitySystemComponent와 바인딩 완료");
}

void UInteractionComponent::UnbindExeternalBindToASCComp()
{
    //@PC
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOGFMT(LogInteraction, Warning, "ASC 언바인딩: 소유자가 PlayerController가 아님");
        return;
    }

    //@Base PC
    ABasePlayerController* BasePC = Cast<ABasePlayerController>(PC);
    if (!BasePC)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: Base Player Controller가 유효하지 않음", __FUNCDNAME__);
        return;
    }

    //@Player State
    APlayerStateBase* PS = BasePC->GetPlayerState<APlayerStateBase>();
    if (!PS)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: Player State가 유효하지 않음", __FUNCDNAME__);
        return;
    }

    //@Base ASC
    UBaseAbilitySystemComponent* ASC = Cast<UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent());
    if (!ASC)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: Base ASC가 유효하지 않음", __FUNCDNAME__);
        return;
    }

    //@외부 바인딩 해제...
    ASC->CharacterStateEventOnGameplay.RemoveAll(this);

    UE_LOGFMT(LogInteraction, Log, "BaseAbilitySystemComponent와 바인딩 해제 완료");
}

void UInteractionComponent::ExternalBindToODComp()
{
    //@PC
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOGFMT(LogInteraction, Warning, "ASC 바인딩 실패: 소유자가 PlayerController가 아님");
        return;
    }

    //@Objective Detection Component
    auto ODComp = PC->FindComponentByClass<UObjectiveDetectionComponent>();
    if (!ODComp)
    {
        return;
    }

    //@외부 바인딩...
    ODComp->DetectedAIStateChanged.AddUFunction(this, "OnDetectedAIStateChanged");

    ODComp->ExecutionTargetChanged.AddUFunction(this, "OnExecutionTargetChanged");
    ODComp->AmbushTargetChanged.AddUFunction(this, "OnAmbushTargetChanged");
}

void UInteractionComponent::UnbindExternalBindToODComp()
{
    //@PC
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOGFMT(LogInteraction, Warning, "OD 언바인딩: 소유자가 PlayerController가 아님");
        return;
    }

    //@Objective Detection Component
    UObjectiveDetectionComponent* ODComp = PC->FindComponentByClass<UObjectiveDetectionComponent>();
    if (!ODComp)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: ObjectiveDetectionComponent가 유효하지 않음", __FUNCDNAME__);
        return;
    }

    //@외부 바인딩 해제...
    ODComp->DetectedAIStateChanged.RemoveAll(this);

    UE_LOGFMT(LogInteraction, Log, "ObjectiveDetectionComponent와 바인딩 해제 완료");
}

void UInteractionComponent::InitializeInteractionComp()
{
    //@외부 바인딩...
    ExternalBindToODComp();
    ExeternalBindToASComp();

    //@PC
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 거리 업데이트 실패: 소유자가 PlayerController가 아님", __FUNCDNAME__);
        return;
    }

    //@Pawn 캐싱
    OwnerPawn = PC->GetPawn();
    if (!OwnerPawn.IsValid())
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 거리 업데이트 실패: 유효한 플레이어 Pawn이 없음", __FUNCDNAME__);
        return;
    }

    //@맵 초기화
    MPotentialInteractions.Reset();

    UE_LOGFMT(LogInteraction, Log, "{0}: Interaction Component 초기화 완료", __FUNCDNAME__);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UInteractionComponent::RegisterPotentialInteraction(AActor* TargetActor)
{
    //@Target
    if (!TargetActor)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 등록 실패: 유효하지 않은 액터", __FUNCDNAME__);
        return;
    }

    //@Character Tag
    FGameplayTag ObjectTag;
    ACharacterBase* Character = Cast<ACharacterBase>(TargetActor);
    if (Character)
    {
        ObjectTag = Character->GetCharacterTag();
    }

    if (!ObjectTag.IsValid())
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 등록 실패: 액터({1})에서 유효한 캐릭터 태그를 찾을 수 없음",
            __FUNCDNAME__, *TargetActor->GetName());
        return;
    }

    //@MatchingInteractions
    TArray<FPotentialInteraction> MatchingInteractions;
    for (const FPotentialInteraction& Interaction : PotentialInteractions)
    {
        if (Interaction.ObjectTag == ObjectTag)
        {
            MatchingInteractions.Add(Interaction);
        }
    }

    //@MatchingInteractions 0개
    if (MatchingInteractions.Num() == 0)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 등록 실패: 태그({1})에 대한 정의된 상호작용 템플릿이 없음",
            __FUNCDNAME__, *ObjectTag.ToString());
        return;
    }

    //@기존의 상호작용들
    if (MPotentialInteractions.Contains(TargetActor))
    {
        MPotentialInteractions.Remove(TargetActor);
    }

    //@MatchingInteractions 등록
    for (const FPotentialInteraction& Interaction : MatchingInteractions)
    {
        //@Contains?
        if (!MPotentialInteractions.Contains(TargetActor))
        {
            MPotentialInteractions.Add(TargetActor, TMap<EInteractionType, FPotentialInteraction>());
        }

        //@Add
        MPotentialInteractions[TargetActor].Add(Interaction.InteractionType, Interaction);

        UE_LOGFMT(LogInteraction, Log, "{0}: 잠재적 상호작용 등록 - 액터: {1} | 태그: {2} | 타입: {3} | 이벤트: {4}",
            __FUNCDNAME__,
            *TargetActor->GetName(),
            *ObjectTag.ToString(),
            static_cast<uint8>(Interaction.InteractionType),
            *Interaction.EventTag.ToString());
    }

    UE_LOGFMT(LogInteraction, Log, "{0}: 액터({1})에 대해 총 {2}개의 상호작용 유형 등록 완료",
        __FUNCDNAME__, *TargetActor->GetName(), MatchingInteractions.Num());
}

void UInteractionComponent::RegisterPotentialInteraction(AActor* TargetActor, EInteractionType InteractionType)
{
    //@Target
    if (!TargetActor)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 등록 실패: 유효하지 않은 액터", __FUNCDNAME__);
        return;
    }

    //@Interaction Type
    if (InteractionType == EInteractionType::None || InteractionType >= EInteractionType::MAX)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 등록 실패: 유효하지 않은 상호작용 유형", __FUNCDNAME__);
        return;
    }

    //@Object Tag
    FGameplayTag ObjectTag;
    ACharacterBase* Character = Cast<ACharacterBase>(TargetActor);
    if (Character)
    {
        ObjectTag = Character->GetCharacterTag();
    }

    if (!ObjectTag.IsValid())
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 등록 실패: 액터({1})에서 유효한 캐릭터 태그를 찾을 수 없음",
            __FUNCDNAME__, *TargetActor->GetName());
        return;
    }

    //@FoundInteraction
    FPotentialInteraction FoundInteraction;
    bool bFoundMatch = false;

    for (int32 i = 0; i < PotentialInteractions.Num(); i++)
    {
        if (PotentialInteractions[i].ObjectTag == ObjectTag && PotentialInteractions[i].InteractionType == InteractionType)
        {
            FoundInteraction = PotentialInteractions[i];
            bFoundMatch = true;
            break;
        }
    }

    //@Found?
    if (!bFoundMatch)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 등록 실패: 태그({1})와 타입({2})에 대한 정의된 상호작용 템플릿이 없음",
            __FUNCDNAME__, *ObjectTag.ToString(), static_cast<uint8>(InteractionType));
        return;
    }

    //@Contains?
    if (!MPotentialInteractions.Contains(TargetActor))
    {
        MPotentialInteractions.Add(TargetActor, TMap<EInteractionType, FPotentialInteraction>());
    }

    //@Add
    MPotentialInteractions[TargetActor].Add(InteractionType, FoundInteraction);

    UE_LOGFMT(LogInteraction, Log, "{0}: 잠재적 상호작용 등록 - 액터: {1} | 태그: {2} | 타입: {3} | 이벤트: {4}",
        __FUNCDNAME__,
        *TargetActor->GetName(),
        *ObjectTag.ToString(),
        static_cast<uint8>(InteractionType),
        *FoundInteraction.EventTag.ToString());
}

void UInteractionComponent::RemovePotentialInteraction(AActor* TargetActor)
{
    //@Target
    if (!TargetActor)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 제거 실패: 유효하지 않은 액터", __FUNCDNAME__);
        return;
    }

    //@Remove
    if (MPotentialInteractions.Remove(TargetActor) <= 0)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 제거할 상호작용을 찾을 수 없음 - 액터: {1}",
            __FUNCDNAME__, *TargetActor->GetName());
        return;
    }

    UE_LOGFMT(LogInteraction, Log, "{0}: 모든 잠재적 상호작용 제거 - 액터: {1}",
        __FUNCDNAME__, *TargetActor->GetName());

    //@IsFullyAvailable?
    if (CurrentPriorityInteraction.IsFullyAvailable())
    {
        CurrentPriorityInteraction = FPotentialInteraction();
        PotentialInteractionChanged.Broadcast(nullptr, CurrentPriorityInteraction);
    }
}

void UInteractionComponent::RemovePotentialInteraction(AActor* TargetActor, EInteractionType InteractionType)
{
    //@Target
    if (!TargetActor)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 제거 실패: 유효하지 않은 액터", __FUNCDNAME__);
        return;
    }

    //@Interaction Type
    if (InteractionType == EInteractionType::None || InteractionType >= EInteractionType::MAX)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 제거 실패: 유효하지 않은 상호작용 유형", __FUNCDNAME__);
        return;
    }

    //@ActorInteractions
    TMap<EInteractionType, FPotentialInteraction>* ActorInteractions = MPotentialInteractions.Find(TargetActor);
    if (!ActorInteractions)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 제거할 상호작용을 찾을 수 없음 - 액터: {1}",
            __FUNCDNAME__, *TargetActor->GetName());
        return;
    }

    //@Remove
    if (ActorInteractions->Remove(InteractionType) <= 0)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 제거할 특정 상호작용 타입을 찾을 수 없음 - 액터: {1} | 타입: {2}",
            __FUNCDNAME__, *TargetActor->GetName(), static_cast<uint8>(InteractionType));
        return;
    }

    UE_LOGFMT(LogInteraction, Log, "{0}: 특정 잠재적 상호작용 제거 - 액터: {1} | 타입: {2}",
        __FUNCDNAME__, *TargetActor->GetName(), static_cast<uint8>(InteractionType));

    //@Actor에 대한 잠재적 상호작용 더이상 없을 경우, 아예 삭제
    if (ActorInteractions->Num() == 0)
    {
        MPotentialInteractions.Remove(TargetActor);
    }

    //@현재 우선순위 상호작용 업데이트
    if (CurrentPriorityInteraction.IsFullyAvailable() &&
        CurrentPriorityInteraction.InteractionType == InteractionType)
    {
        CurrentPriorityInteraction = FPotentialInteraction();
        PotentialInteractionChanged.Broadcast(nullptr, CurrentPriorityInteraction);
    }
}

void UInteractionComponent::CommitInteraction()
{
    //@PC
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 거리 업데이트 실패: 소유자가 PlayerController가 아님", __FUNCDNAME__);
        return;
    }

    //@Pawn
    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 거리 업데이트 실패: 유효한 플레이어 Pawn이 없음", __FUNCDNAME__);
        return;
    }

    //@플레이어 위치
    FVector PlayerLocation = PlayerPawn->GetActorLocation();

    //@우선순위 상호작용 추적을 위한 변수
    FPotentialInteraction NewPriorityInteraction;
    bool bFoundPriority = false;
    AActor* NewPriorityActor = nullptr;

    //@모든 등록된 액터에 대한 상호작용 거리 업데이트
    TArray<TWeakObjectPtr<AActor>> ActorsToRemove;

    for (auto& ActorInteractionPair : MPotentialInteractions)
    {
        //@상호작용 대상 액터
        TWeakObjectPtr<AActor> TargetActorWeak = ActorInteractionPair.Key;
        if (!TargetActorWeak.IsValid())
        {
            //@유효하지 않은 액터는 제거 목록에 추가
            ActorsToRemove.Add(TargetActorWeak);
            continue;
        }

        AActor* TargetActor = TargetActorWeak.Get();

        //@타겟 위치
        FVector TargetLocation = TargetActor->GetActorLocation();

        //@플레이어와 타겟 간 거리 계산
        float DistanceSq = FVector::DistSquared(PlayerLocation, TargetLocation);

        //@액터의 모든 상호작용에 대한 처리
        TMap<EInteractionType, FPotentialInteraction>& ActorInteractions = ActorInteractionPair.Value;
        TArray<EInteractionType> InteractionTypesToRemove;

        for (auto& TypeInteractionPair : ActorInteractions)
        {
            EInteractionType InteractionType = TypeInteractionPair.Key;
            FPotentialInteraction& Interaction = TypeInteractionPair.Value;

            //@필요 거리의 제곱값 (성능 최적화를 위해 제곱 비교)
            float RequiredDistanceSq = Interaction.RequiredDistance * Interaction.RequiredDistance;

            //@거리 조건 만족 여부 - 이전 상태와 비교하여 변경 감지
            bool bWasAvailable = Interaction.bAdditionalConditionsMet;
            bool bIsAvailable = (DistanceSq <= RequiredDistanceSq);

            //@거리 조건 업데이트
            Interaction.bAdditionalConditionsMet = bIsAvailable;

            //@상태 변경 시 로그 출력
            if (bWasAvailable != bIsAvailable)
            {
                UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 거리 조건 변경 - 액터: {1} | 타입: {2} | 가능 여부: {3} | 거리: {4} | 필요 거리: {5}",
                    __FUNCDNAME__,
                    *TargetActor->GetName(),
                    static_cast<uint8>(InteractionType),
                    bIsAvailable ? TEXT("가능") : TEXT("불가능"),
                    FMath::Sqrt(DistanceSq),
                    Interaction.RequiredDistance);

                //@현재 우선순위 상호작용이 불가능하게 되었으면 취소 이벤트 발생
                if (!bIsAvailable && CurrentPriorityInteraction == Interaction)
                {
                    UE_LOGFMT(LogInteraction, Log, "{0}: 현재 우선순위 상호작용 불가능으로 변경 - 액터: {1} | 타입: {2}",
                        __FUNCDNAME__, *TargetActor->GetName(), static_cast<uint8>(InteractionType));

                    CancelInteractionActivated(TargetActor, Interaction);

                    //@현재 우선순위 상호작용 초기화
                    CurrentPriorityInteraction = FPotentialInteraction();
                }
            }

            //@완전히 사용 가능한 상호작용이면서 현재까지의 최우선순위보다 높은 경우 업데이트
            if (Interaction.IsFullyAvailable() && (!bFoundPriority || Interaction.Priority > NewPriorityInteraction.Priority))
            {
                NewPriorityInteraction = Interaction;
                NewPriorityActor = TargetActor;
                bFoundPriority = true;
            }
        }

        //@제거 대상 상호작용 타입 처리
        for (EInteractionType TypeToRemove : InteractionTypesToRemove)
        {
            ActorInteractions.Remove(TypeToRemove);
        }

        //@모든 상호작용이 제거된 경우, 액터도 제거 목록에 추가
        if (ActorInteractions.Num() == 0)
        {
            ActorsToRemove.Add(TargetActorWeak);
        }
    }

    //@유효하지 않거나 빈 액터 항목 제거
    for (const TWeakObjectPtr<AActor>& ActorToRemove : ActorsToRemove)
    {
        MPotentialInteractions.Remove(ActorToRemove);
    }

    //@현재 사용 가능한 모든 상호작용 로그 출력
    UE_LOGFMT(LogInteraction, Log, "{0}: === 현재 사용 가능한 상호작용 목록 ===", __FUNCDNAME__);
    int32 AvailableCount = 0;

    for (const auto& ActorInteractionPair : MPotentialInteractions)
    {
        TWeakObjectPtr<AActor> TargetActorWeak = ActorInteractionPair.Key;
        if (!TargetActorWeak.IsValid())
        {
            continue;
        }

        AActor* TargetActor = TargetActorWeak.Get();
        const TMap<EInteractionType, FPotentialInteraction>& ActorInteractions = ActorInteractionPair.Value;

        for (const auto& TypeInteractionPair : ActorInteractions)
        {
            const FPotentialInteraction& Interaction = TypeInteractionPair.Value;

            //@상호작용 가능한 상태
            if (Interaction.IsFullyAvailable())
            {
                AvailableCount++;

                //@타겟 위치와 플레이어 위치 계산
                FVector TargetLocation = TargetActor->GetActorLocation();
                const float CurrentDistance = FVector::Dist(PlayerLocation, TargetLocation);
                const float RequiredDistance = Interaction.RequiredDistance;

                UE_LOGFMT(LogInteraction, Log, "   - 액터: {0} | 태그: {1} | 타입: {2} | 우선순위: {3} | 현재 거리: {4} | 요구 거리: {5}",
                    *TargetActor->GetName(),
                    *Interaction.ObjectTag.ToString(),
                    static_cast<uint8>(Interaction.InteractionType),
                    Interaction.Priority,
                    CurrentDistance,
                    RequiredDistance);
            }
        }
    }

    if (AvailableCount == 0)
    {
        UE_LOGFMT(LogInteraction, Log, "사용 가능한 상호작용이 없습니다.");
    }

    //@새로운 우선순위 상호작용이 현재와 다를 경우 업데이트
    if (bFoundPriority && (CurrentPriorityInteraction != NewPriorityInteraction))
    {
        //@우선순위 상호작용 변경
        CurrentPriorityInteraction = NewPriorityInteraction;

        //@새로운 우선순위 상호작용 이벤트 발생
        TryActivateInteraction(NewPriorityActor, CurrentPriorityInteraction);
    }
    //@사용 가능한 상호작용이 없는데 현재 우선순위 상호작용이 있을 경우 초기화
    else if (!bFoundPriority && CurrentPriorityInteraction.ObjectTag.IsValid())
    {
        //@가능한 상호작용이 없어짐
        CancelInteractionActivated(nullptr, CurrentPriorityInteraction);
        CurrentPriorityInteraction = FPotentialInteraction();
    }
}

void UInteractionComponent::TryActivateInteraction(AActor* TargetActor, const FPotentialInteraction& Interaction)
{
    if (!Interaction.IsFullyAvailable() || !TargetActor)
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상호작용 활성화 실패: 유효하지 않은 파라미터", __FUNCDNAME__);
        return;
    }

    //@이벤트 발생 (델리게이트 호출)
    PotentialInteractionChanged.Broadcast(TargetActor, Interaction);

    UE_LOGFMT(LogInteraction, Log, "{0}: 상호작용 활성화 - 액터: {1} | 태그: {2} | 타입: {3} | 이벤트: {4}",
        __FUNCDNAME__,
        *TargetActor->GetName(),
        *Interaction.ObjectTag.ToString(),
        static_cast<uint8>(Interaction.InteractionType),
        *Interaction.EventTag.ToString());
}

void UInteractionComponent::CancelInteractionActivated(AActor* TargetActor, const FPotentialInteraction& Interaction)
{
    if (!Interaction.ObjectTag.IsValid())
    {
        return;
    }

    //@취소된 상호작용 생성 (bAdditionalConditionsMet = false)
    FPotentialInteraction CancelledInteraction = Interaction;
    CancelledInteraction.bAdditionalConditionsMet = false;

    //@이벤트 발생 (델리게이트 호출)
    PotentialInteractionChanged.Broadcast(TargetActor, CancelledInteraction);

    UE_LOGFMT(LogInteraction, Log, "{0}: 상호작용 취소 - 액터: {1} | 태그: {2} | 타입: {3}",
        __FUNCDNAME__,
        TargetActor ? *TargetActor->GetName() : TEXT("없음"),
        *Interaction.ObjectTag.ToString(),
        static_cast<uint8>(Interaction.InteractionType));
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UInteractionComponent::OnOwnerStateEventOnGameplay(AActor* OwnerActor, const FGameplayTag& StateTag)
{
}

void UInteractionComponent::OnDetectedAIStateChanged(const FGameplayTag& StateTag, AActor* ObjectiveActor)
{
    //@Objective Actor, State Tag
    if (!ObjectiveActor || !StateTag.IsValid())
    {
        UE_LOGFMT(LogInteraction, Warning, "{0}: 상태 변경 이벤트 처리 실패: 유효하지 않은 매개변수", __FUNCDNAME__);
        return;
    }

    UE_LOGFMT(LogInteraction, Log, "{0}: 상태 변경 감지 - 액터: {1} | 상태: {2}",
        __FUNCDNAME__, *ObjectiveActor->GetName(), *StateTag.ToString());

    // Fragile 상태 처리는 제거하고 Dead 상태만 처리
    if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead")))
    {
        RemovePotentialInteraction(ObjectiveActor);
    }

    // State.Normal 처리도 제거 (ExecutionTargetChanged에서 처리)
}

void UInteractionComponent::OnExecutionTargetChanged(AActor* PotentialExecutionTarget)
{
    // 처형 타겟이 없으면 모든 처형 상호작용 제거
    if (!PotentialExecutionTarget)
    {
        TArray<TWeakObjectPtr<AActor>> ActorsToCheck;
        MPotentialInteractions.GetKeys(ActorsToCheck);

        for (const auto& ActorWeak : ActorsToCheck)
        {
            if (ActorWeak.IsValid())
            {
                RemovePotentialInteraction(ActorWeak.Get(), EInteractionType::Execution);
            }
        }

        UE_LOGFMT(LogInteraction, Log, "{0}: 처형 타겟 초기화 - 모든 처형 상호작용 제거", __FUNCDNAME__);
        return;
    }

    UE_LOGFMT(LogInteraction, Log, "{0}: 처형 타겟 변경 감지 - 액터: {1}",
        __FUNCDNAME__, *PotentialExecutionTarget->GetName());

    //@새로운 처형 타겟 등록
    RegisterPotentialInteraction(PotentialExecutionTarget, EInteractionType::Execution);

    //@다른 액터들의 처형 상호작용 제거
    TArray<TWeakObjectPtr<AActor>> ActorsToCheck;
    MPotentialInteractions.GetKeys(ActorsToCheck);

    for (const auto& ActorWeak : ActorsToCheck)
    {
        if (ActorWeak.IsValid() && ActorWeak.Get() != PotentialExecutionTarget)
        {
            RemovePotentialInteraction(ActorWeak.Get(), EInteractionType::Execution);
        }
    }

    UE_LOGFMT(LogInteraction, Log, "{0}: 처형 타겟 처리 완료 - 액터: {1}에 대한 처형 상호작용 등록",
        __FUNCDNAME__, *PotentialExecutionTarget->GetName());
}

void UInteractionComponent::OnAmbushTargetChanged(AActor* PotentialAmbushTarget)
{
    //@PotentialAmbushTarget
    if (!PotentialAmbushTarget)
    {
        TArray<TWeakObjectPtr<AActor>> ActorsToCheck;
        MPotentialInteractions.GetKeys(ActorsToCheck);

        for (const auto& ActorWeak : ActorsToCheck)
        {
            //@Remove
            if (ActorWeak.IsValid())
            {
                RemovePotentialInteraction(ActorWeak.Get(), EInteractionType::Ambush);
            }
        }

        UE_LOGFMT(LogInteraction, Log, "{0}: 암살 타겟 초기화 - 모든 암살 상호작용 제거", __FUNCDNAME__);
        return;
    }

    UE_LOGFMT(LogInteraction, Log, "{0}: 암살 타겟 변경 감지 - 액터: {1}",
        __FUNCDNAME__, *PotentialAmbushTarget->GetName());

    //@새로운 잠재적 암살 타겟 등록
    RegisterPotentialInteraction(PotentialAmbushTarget, EInteractionType::Ambush);

    //@다른 액터들의 암살 유형 상호작용 제거 
    TArray<TWeakObjectPtr<AActor>> ActorsToCheck;
    MPotentialInteractions.GetKeys(ActorsToCheck);

    for (const auto& ActorWeak : ActorsToCheck)
    {
        //@Remove
        if (ActorWeak.IsValid() && ActorWeak.Get() != PotentialAmbushTarget)
        {
            // 해당 액터의 암살 유형 상호작용 제거
            RemovePotentialInteraction(ActorWeak.Get(), EInteractionType::Ambush);
        }
    }

    UE_LOGFMT(LogInteraction, Log, "{0}: 암살 타겟 처리 완료 - 액터: {1}에 대한 암살 상호작용 등록",
        __FUNCDNAME__, *PotentialAmbushTarget->GetName());
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility

bool UInteractionComponent::IsInteractionTypeAvailable(EInteractionType InteractionType) const
{
	return false;
}

TArray<FPotentialInteraction> UInteractionComponent::FindInteraction(AActor* TargetActor)
{
    TArray<FPotentialInteraction> Result;

    if (!TargetActor)
    {
        return Result;
    }

    TMap<EInteractionType, FPotentialInteraction>* ActorInteractions = MPotentialInteractions.Find(TargetActor);
    if (!ActorInteractions)
    {
        return Result;
    }

    // 액터에 대한 모든 상호작용 정보 수집
    for (const auto& Pair : *ActorInteractions)
    {
        Result.Add(Pair.Value);
    }

    return Result;
}

FPotentialInteraction* UInteractionComponent::FindInteraction(AActor* TargetActor, EInteractionType Type)
{
    if (!TargetActor)
    {
        return nullptr;
    }

    TMap<EInteractionType, FPotentialInteraction>* ActorInteractions = MPotentialInteractions.Find(TargetActor);
    if (!ActorInteractions)
    {
        return nullptr;
    }

    return ActorInteractions->Find(Type);
}

FPotentialInteraction UInteractionComponent::GetHighestPriorityInteraction() const
{
	return FPotentialInteraction();
}

FPotentialInteraction UInteractionComponent::GetHighestPriorityInteractionForActor(AActor* TargetActor)
{
    if (!TargetActor)
    {
        return FPotentialInteraction();
    }

    TMap<EInteractionType, FPotentialInteraction>* ActorInteractions = MPotentialInteractions.Find(TargetActor);
    if (!ActorInteractions || ActorInteractions->Num() == 0)
    {
        return FPotentialInteraction();
    }

    FPotentialInteraction HighestPriority;
    bool bFound = false;

    for (const auto& Pair : *ActorInteractions)
    {
        const FPotentialInteraction& Interaction = Pair.Value;
        if (Interaction.IsFullyAvailable() && (!bFound || Interaction.Priority > HighestPriority.Priority))
        {
            HighestPriority = Interaction;
            bFound = true;
        }
    }

    return bFound ? HighestPriority : FPotentialInteraction();
}
#pragma endregion