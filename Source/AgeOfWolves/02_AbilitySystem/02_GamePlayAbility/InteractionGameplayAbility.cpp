#include "InteractionGameplayAbility.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "04_Component/InteractionComponent.h"
#include "04_Component/ObjectiveDetectionComponent.h"
#include "14_Subsystem/InteractionManagerSubsystem.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "07_BlueprintNode/CombatLibrary.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "02_AbilitySystem/06_AbilityTask/AT_MoveToInteractionZone.h"

DEFINE_LOG_CATEGORY(LogInteractionGA)

//@Default Setting
#pragma region Default Setting
UInteractionGameplayAbility::UInteractionGameplayAbility(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    //@Interaction Data
    InteractionData = nullptr;
    //@Target
    TargetActor.Reset();
    //@Interaction 성공 여부
    bInteractionCompleted = false;
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
//@구현부
void UInteractionGameplayAbility::MoveToInteractionTarget(FName SocketName, float Duration)
{
    AActor* Target = GetTargetActor();
    if (!Target)
    {
        UE_LOGFMT(LogInteractionGA, Warning, "상호작용 이동 실패 - Target Actor가 유효하지 않음");
        return;
    }

    //@기존 Task 정리
    if (CurrentMoveTask && CurrentMoveTask->IsActive())
    {
        CurrentMoveTask->EndTask();
    }

    //@새로운 Interaction Zone Task 생성
    CurrentMoveTask = UAT_MoveToInteractionZone::MoveToInteractionZone(
        this,
        Target,
        SocketName,
        Duration
    );

    if (!CurrentMoveTask)
    {
        UE_LOGFMT(LogInteractionGA, Warning, "상호작용 이동 Task 생성 실패");
        return;
    }

    //@콜백 바인딩 (델리게이트 이름 변경)
    CurrentMoveTask->OnInteractionZoneReached.AddDynamic(this, &UInteractionGameplayAbility::OnInteractionMoveCompleted);

    //@Task 활성화
    CurrentMoveTask->ReadyForActivation();

    UE_LOGFMT(LogInteractionGA, Log, "상호작용 영역으로 이동 시작 - 목표: {0}", *Target->GetName());
}

void UInteractionGameplayAbility::SendInteractionEvent()
{
    //@Target Actor
    AActor* LocalTargetActor = GetTargetActor();
    if (!LocalTargetActor)
    {
        UE_LOGFMT(LogInteractionGA, Warning, "SendInteractionEvent 실패 - 사유: TargetActor가 유효하지 않음");
        return;
    }

    //@Source Actor
    AActor* SourceActor = GetAvatarActorFromActorInfo();
    if (!SourceActor)
    {
        UE_LOGFMT(LogInteractionGA, Warning, "SendInteractionEvent 실패 - 사유: Source Actor가 유효하지 않음");
        return;
    }

    //@이벤트 태그 검사
    if (!InteractionData->GetEventTag().IsValid())
    {
        UE_LOGFMT(LogInteractionGA, Warning, "SendInteractionEvent 실패 - 사유: 이벤트 태그가 유효하지 않음");
        return;
    }

    //@페어링 애니메이션 상호작용인 경우 InteractionManagerSubsystem에 참여자 등록
    RegisterPairedAnimationParticipants();

    //@GameplayEventData 구성
    FGameplayEventData EventData;
    EventData.Instigator = SourceActor;
    EventData.Target = LocalTargetActor;
    EventData.OptionalObject = InteractionData;

    //@이벤트 전송
    bool bSuccess = UCombatLibrary::SendGameplayEventToTarget(
        InteractionData->GetEventTag(),
        LocalTargetActor,
        SourceActor,
        FHitResult(), // 빈 HitResult 사용
        0.0f,
        InteractionData,
        nullptr
    );

    if (!bSuccess)
    {
        UE_LOGFMT(LogInteractionGA, Warning, "SendInteractionEvent 실패 - Target: {0}, 사유: 이벤트 전송 실패",
            *LocalTargetActor->GetName());
        return;
    }

    UE_LOGFMT(LogInteractionGA, Log, "상호작용 이벤트 전송 완료 - Target: {0}, Instigator: {1}, 상호작용 타입: {2}",
        *LocalTargetActor->GetName(), *SourceActor->GetName(), static_cast<int32>(InteractionData->GetInteractionType()));
}

void UInteractionGameplayAbility::RegisterPairedAnimationParticipants()
{
    if (!InteractionData || !InteractionData->IsPairedAnimationInteraction())
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogInteractionGA, Warning, "RegisterPairedAnimationParticipants 실패 - 사유: World가 유효하지 않음");
        return;
    }

    UInteractionManagerSubsystem* InteractionManager = World->GetGameInstance()->GetSubsystem<UInteractionManagerSubsystem>();
    if (!InteractionManager)
    {
        UE_LOGFMT(LogInteractionGA, Warning, "RegisterPairedAnimationParticipants 실패 - 사유: InteractionManagerSubsystem을 찾을 수 없음");
        return;
    }

    //@세션 생성 또는 기존 세션 사용
    FGuid SessionID = InteractionData->GetSessionID();
    if (!SessionID.IsValid())
    {
        SessionID = InteractionManager->CreateInteractionSession();
        UE_LOGFMT(LogInteractionGA, Log, "새 상호작용 세션 생성 - SessionID: {0}", *SessionID.ToString());
    }

    //@플레이어만 자신의 몽타주 등록 (Player 역할)
    AActor* SourceActor = GetAvatarActorFromActorInfo();
    ACharacterBase* PlayerCharacter = Cast<ACharacterBase>(SourceActor);
    if (PlayerCharacter)
    {
        UAnimMontage* PlayerMontage = InteractionData->GetPlayerMontage().LoadSynchronous();
        if (PlayerMontage)
        {
            //@새로운 RegisterPlayerParticipant 함수 사용
            bool bPlayerRegistered = InteractionManager->RegisterPlayerParticipant(
                SessionID, 
                PlayerCharacter, 
                PlayerMontage, 
                1.0f, 
                NAME_None
            );

            if (bPlayerRegistered)
            {
                UE_LOGFMT(LogInteractionGA, Log, "플레이어 참여자 등록 성공 - SessionID: {0}", *SessionID.ToString());
            }
        }
    }

    //@SessionID를 InteractionData에 설정
    FPotentialInteraction UpdatedInteraction = InteractionData->InteractionData;
    UpdatedInteraction.SessionID = SessionID;
    InteractionData->SetInteractionData(UpdatedInteraction);
}

#pragma endregion

//@Callbacks
#pragma region Callbacks
void UInteractionGameplayAbility::OnChainActionActivated_Implementation(FGameplayTag ChainActionEventTag)
{
    Super::OnChainActionActivated_Implementation(ChainActionEventTag);

    UE_LOGFMT(LogInteractionGA, Log, "체인 액션 활성화 이벤트 호출 - Ability: {0} | Event Tag: {1}",
        *GetName(),
        *ChainActionEventTag.ToString());

}

void UInteractionGameplayAbility::OnChainActionFinished_Implementation(FGameplayTag ChainActionEventTag)
{
    Super::OnChainActionFinished_Implementation(ChainActionEventTag);

    UE_LOGFMT(LogInteractionGA, Log, "체인 액션 종료 이벤트 호출 - Ability: {0} | Event Tag: {1}",
        *GetName(),
        *ChainActionEventTag.ToString());
}

void UInteractionGameplayAbility::OnInteractionMoveCompleted_Implementation()
{
    UE_LOGFMT(LogInteractionGA, Log, "상호작용 위치 이동 완료");
    CurrentMoveTask = nullptr;

    //@애니메이션 재생 등 후속 작업 수행
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
ACharacterBase* UInteractionGameplayAbility::GetCharacterFromActorInfo() const
{
    return Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
}

bool UInteractionGameplayAbility::CanInteractWith(AActor* Target) const
{
    if (!Target)
        return false;

    // 기본적인 예시: 오브젝트 태그가 지정된 경우 태그 확인
    if (InteractionData->GetObjectTag().IsValid())
    {
        // 대상 액터가 태그를 갖고 있는지 확인
        ACharacterBase* Character = Cast<ACharacterBase>(GetTargetActor());
        if (Character)
        {
            FGameplayTag CharacterTag = Character->GetCharacterTag();
            if (!CharacterTag.IsValid() || !CharacterTag.MatchesTag(InteractionData->GetObjectTag()))
                return false;
        }
    }

    return true;
}
#pragma endregion