#include "Area.h"
#include "Logging/StructuredLog.h"

#include "Components/BoxComponent.h"
#include "04_Component/CrowdControlComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"

#include "10_AI/BaseAIController.h"

DEFINE_LOG_CATEGORY_STATIC(LogArea, Log, All);


//@Defualt Setting
#pragma region Default Setting
AArea::AArea()
{
    PrimaryActorTick.bCanEverTick = false;

    //@Box Component
    AreaBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("AreaBounds"));
    CrowdControlComponent = CreateDefaultSubobject<UCrowdControlComponent>(TEXT("Crowd Control"));
    RootComponent = AreaBounds;

    AreaBounds->SetCollisionProfileName(TEXT("OverlapAll"));
    AreaBounds->SetCollisionResponseToAllChannels(ECR_Overlap);
    AreaBounds->SetBoxExtent(FVector(2000.0f, 2000.0f, 500.0f));

    //@Area ID
    AreaID = FGuid::NewGuid();

    //@초기화
    MAIGroups.Empty();
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
    UnregisterAllPlayer();

    //@모든 AI 등록 해제
    UnregisterAllAI();

    UE_LOGFMT(LogArea, Log, "Area {0} 정리 완료", *AreaID.ToString());

    Super::EndPlay(EndPlayReason);
}

void AArea::PreInitializeComponents()
{
    Super::PreInitializeComponents();
}

void AArea::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    //@컴포넌트의 외부 바인딩...
    CrowdControlComponent->ExternalBindingToArea(this);

    //@비동기 초기화 요청 바인딩...
    RequestStartInitByArea.AddUFunction(CrowdControlComponent, "InitializeCrowdControlComp");
}

void AArea::InternalBindToAI(TWeakObjectPtr<AActor> AIActorPtr)
{
    // Actor가 유효한지 확인
    AActor* AIActor = AIActorPtr.Get();
    if (!IsValid(AIActor))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 바인딩 실패 - 유효하지 않은 AI", *AreaID.ToString());
        return;
    }

    // AI가 속한 그룹 ID 찾기
    FGuid GroupID = GetAIGroupID(AIActor);
    if (!GroupID.IsValid() || !MAIGroups.Contains(GroupID))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 바인딩 실패 - AI {1}이(가) 등록된 그룹에 속해 있지 않음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    // 그룹에서 AI 정보 찾기
    FAIGroupInfo& GroupInfo = MAIGroups[GroupID];
    FAreaAIInfo* AIInfo = nullptr;

    for (auto& MemberInfo : GroupInfo.GroupMembers)
    {
        if (MemberInfo.AIActor.Get() == AIActor)
        {
            AIInfo = &MemberInfo;
            break;
        }
    }

    if (!AIInfo)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 바인딩 실패 - 그룹 {1}에서 AI {2}의 정보를 찾을 수 없음",
            *AreaID.ToString(), *GroupID.ToString(), *AIActor->GetName());
        return;
    }

    // ASC 가져오기
    IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(AIActor);
    if (!AbilityInterface)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}이(가) AbilitySystemInterface를 구현하지 않음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    UBaseAbilitySystemComponent* BaseASC = Cast<UBaseAbilitySystemComponent>(AbilityInterface->GetAbilitySystemComponent());
    if (!BaseASC)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}의 BaseAbilitySystemComponent를 찾을 수 없음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    // Pawn으로 캐스팅 후 Controller 가져오기
    APawn* AIPawn = Cast<APawn>(AIActor);
    if (!AIPawn)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}을(를) Pawn으로 캐스팅할 수 없음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    AController* Controller = AIPawn->GetController();
    if (!IsValid(Controller))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}의 유효한 컨트롤러를 찾을 수 없음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    // BaseAIController로 캐스팅
    ABaseAIController* AIController = Cast<ABaseAIController>(Controller);
    if (!AIController)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}의 컨트롤러를 BaseAIController로 캐스팅할 수 없음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }
    
    //@AI 정보 업데이트
    AIInfo->AIASC = BaseASC;

    //@내부 바인딩...
    AIController->AIDetectsTarget.AddUFunction(this, "OnAIDetectsTarget");
    AIController->SendInfoToBelongingGroup.AddUFunction(this, "OnSendInfoToBelongingGroup");
    BaseASC->CharacterStateEventOnGameplay.AddUFunction(this, "OnAICharacterStateEvent");


    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 이벤트 바인딩 완료",
        *AreaID.ToString(), *AIActor->GetName());

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 락온 상태 이벤트 바인딩 완료",
        *AreaID.ToString(), *AIActor->GetName());
}

void AArea::UnbindFromAI(TWeakObjectPtr<AActor> AIActorPtr)
{
    // Actor가 유효한지 확인
    AActor* AIActor = AIActorPtr.Get();
    if (!IsValid(AIActor))
    {
        UE_LOGFMT(LogArea, Log, "Area {0}: 언바인딩 스킵 - 유효하지 않은 AI", *AreaID.ToString());
        return;
    }

    // AI가 소속된 그룹과 멤버 정보 찾기
    bool bFoundAI = false;
    FAreaAIInfo* AIInfo = nullptr;

    for (auto& GroupPair : MAIGroups)
    {
        for (auto& MemberInfo : GroupPair.Value.GroupMembers)
        {
            if (MemberInfo.AIActor.Get() == AIActor)
            {
                AIInfo = &MemberInfo;
                bFoundAI = true;
                break;
            }
        }

        if (bFoundAI)
            break;
    }

    // AI 정보를 찾지 못한 경우
    if (!bFoundAI || !AIInfo)
    {
        UE_LOGFMT(LogArea, Log, "Area {0}: 언바인딩 스킵 - 등록되지 않은 AI {1}",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    // ASC 가져오기
    UBaseAbilitySystemComponent* BaseASC = AIInfo->AIASC.Get();
    if (!IsValid(BaseASC))
    {
        UE_LOGFMT(LogArea, Log, "Area {0}: 언바인딩 스킵 - AI {1}의 유효한 ASC 없음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    // Pawn으로 캐스팅 후 Controller 가져오기
    APawn* AIPawn = Cast<APawn>(AIActor);
    if (!AIPawn)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}을(를) Pawn으로 캐스팅할 수 없음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    AController* Controller = AIPawn->GetController();
    if (!IsValid(Controller))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}의 유효한 컨트롤러를 찾을 수 없음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    // BaseAIController로 캐스팅
    ABaseAIController* AIController = Cast<ABaseAIController>(Controller);
    if (!AIController)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}의 컨트롤러를 BaseAIController로 캐스팅할 수 없음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    // 바인딩 제거...
    BaseASC->CharacterStateEventOnGameplay.RemoveAll(this);
    AIController->AIDetectsTarget.RemoveAll(this);

    // FAreaAIInfo.AIASC 초기화
    AIInfo->AIASC = nullptr;

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 이벤트 언바인딩 완료",
        *AreaID.ToString(), *AIActor->GetName());
}

void AArea::UnbindFromAllAI()
{
    int32 UnbindCount = 0;

    // 각 그룹별로 AI 처리
    for (auto& GroupPair : MAIGroups)
    {
        // 그룹의 모든 멤버에 대해 바인딩 해제 수행
        for (int32 i = 0; i < GroupPair.Value.GroupMembers.Num(); ++i)
        {
            TWeakObjectPtr<AActor> AIActorPtr = GroupPair.Value.GroupMembers[i].AIActor;

            if (AIActorPtr.IsValid())
            {
                // 바인딩 해제
                UnbindFromAI(AIActorPtr);
                UnbindCount++;
            }
        }

        // 그룹 리더 참조 초기화
        GroupPair.Value.GroupLeader = nullptr;
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

    //@ASC 인터페이스 확인
    if (!Player->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 플레이어 {1}이(가) AbilitySystemInterface를 구현하지 않음",
            *AreaID.ToString(), *Player->GetName());
        return;
    }

    //@ASC 가져오기
    IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Player);
    UBaseAbilitySystemComponent* BaseASC = Cast<UBaseAbilitySystemComponent>(AbilityInterface->GetAbilitySystemComponent());
    if (!BaseASC)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 플레이어 {1}의 BaseAbilitySystemComponent를 찾을 수 없음",
            *AreaID.ToString(), *Player->GetName());
        return;
    }

    //@내부 바인딩...
    BaseASC->CharacterStateEventOnGameplay.AddUFunction(this, "OnPlayerStateEvent");

    UE_LOGFMT(LogArea, Log, "Area {0}: 플레이어 {1} 이벤트 바인딩 완료",
        *AreaID.ToString(), *Player->GetName());
}

void AArea::UnbindFromPlayer(TWeakObjectPtr<APlayerCharacter> Player)
{
    //@유효성 검사
    if (!Player.IsValid())
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 유효하지 않은 플레이어 언바인딩 시도", *AreaID.ToString());
        return;
    }

    //@플레이어 캐스팅
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Player);
    if (!PlayerCharacter)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 액터 {1}을(를) PlayerCharacter로 캐스팅할 수 없음",
            *AreaID.ToString(), *Player->GetName());
        return;
    }

    //@TWeakObjectPtr
    TWeakObjectPtr<APlayerCharacter> PlayerPtr(PlayerCharacter);

    //@바인딩 정보 확인
    if (!MPlayerBindings.Contains(PlayerPtr))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 등록되지 않은 플레이어 {1} 언바인딩 시도",
            *AreaID.ToString(), *Player->GetName());
        return;
    }

    //@ASC 인터페이스 확인
    if (!Player->GetClass()->ImplementsInterface(UAbilitySystemInterface::StaticClass()))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 플레이어 {1}이(가) AbilitySystemInterface를 구현하지 않음",
            *AreaID.ToString(), *Player->GetName());
        return;
    }

    //@ASC 가져오기
    IAbilitySystemInterface* AbilityInterface = Cast<IAbilitySystemInterface>(Player);
    UBaseAbilitySystemComponent* BaseASC = Cast<UBaseAbilitySystemComponent>(AbilityInterface->GetAbilitySystemComponent());
    if (!BaseASC)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 플레이어 {1}의 BaseAbilitySystemComponent를 찾을 수 없음",
            *AreaID.ToString(), *Player->GetName());
        return;
    }

    //@ASC 이벤트에서 언바인딩
    BaseASC->CharacterStateEventOnGameplay.RemoveAll(this);

    //@플레이어의 마지막 퇴장 시간 업데이트
    MPlayerBindings[PlayerPtr].LastExitTime = GetWorld()->GetTimeSeconds();

    UE_LOGFMT(LogArea, Log, "Area {0}: 플레이어 {1} 언바인딩 완료",
        *AreaID.ToString(), *Player->GetName());
}

void AArea::UnbindFromAllPlayer()
{
    //@모든 등록된 플레이어에 대해 언바인딩 수행
    for (auto& Pair : MPlayerBindings)
    {
        if (Pair.Key.IsValid())
        {
            APlayerCharacter* Player = Pair.Key.Get();
            UnbindFromPlayer(Player);
        }
        else
        {
            UE_LOGFMT(LogArea, Warning, "Area {0}: 유효하지 않은 플레이어 발견, 언바인딩 건너뜀", *AreaID.ToString());
        }
    }

    UE_LOGFMT(LogArea, Log, "Area {0}: 모든 플레이어 언바인딩 완료", *AreaID.ToString());
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

    //@비동기 초기화 요청 이벤트
    RequestStartInitByArea.Broadcast(GetAllAIGroupsAsArray());

    UE_LOGFMT(LogArea, Log, "Area {0} 초기화 완료.", *AreaID.ToString());

#if WITH_EDITOR
    // 에디터에서 디버그 시각화
    //if (GetWorld() && GetWorld()->IsPlayInEditor())
    //{
    //    DrawDebugBox(GetWorld(), GetActorLocation(), AreaBounds->GetScaledBoxExtent(), FColor::Green, true, -1.0f, 0, 3.0f);
    //}
#endif
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void AArea::InitializeAreaAIInfos()
{
    //@RegisteredAIGroups
    for (const FAIGroupInfo& RegisteredGroup : RegisteredAIGroups)
    {
        //@Group ID
        FGuid GroupID = RegisteredGroup.GroupID.IsValid() ? RegisteredGroup.GroupID : FGuid::NewGuid();

        //@FAIGroupInfo
        FAIGroupInfo NewGroupInfo;
        NewGroupInfo.GroupID = GroupID;
        NewGroupInfo.GroupName = RegisteredGroup.GroupName.IsEmpty() ?
            FString::Printf(TEXT("Group_%s"), *GroupID.ToString().Left(8)) : RegisteredGroup.GroupName;

        //@FAreaAIInfo
        for (const FAreaAIInfo& MemberInfo : RegisteredGroup.GroupMembers)
        {
            //@사용자 등록이 확인
            if (IsValid(MemberInfo.AIActor.Get()))
            {
                //@FAreaAIInfo
                FAreaAIInfo NewMemberInfo;
                NewMemberInfo.AIActor = MemberInfo.AIActor;
                NewMemberInfo.HierarchyType = MemberInfo.HierarchyType;
                NewMemberInfo.Priority = MemberInfo.Priority;
                NewMemberInfo.CurrentState = FGameplayTag::RequestGameplayTag("State.Alive");
                NewMemberInfo.AIASC = nullptr;
                NewMemberInfo.GroupID = GroupID;
                NewMemberInfo.AlertLevel = 0;

                //@Group Members 추가
                NewGroupInfo.GroupMembers.Add(NewMemberInfo);

            }
        }

        //@MAIGroups
        MAIGroups.Add(GroupID, NewGroupInfo);

        UE_LOGFMT(LogArea, Log, "Area {0}: 그룹 '{1}' (ID: {2}) 등록 완료, 멤버 수: {3}",
            *AreaID.ToString(), *NewGroupInfo.GroupName, *GroupID.ToString(), NewGroupInfo.GroupMembers.Num());

        //@내부 바인딩...
        for (const FAreaAIInfo& MemberInfo : NewGroupInfo.GroupMembers)
        {
            if (MemberInfo.AIActor.IsValid())
            {
                InternalBindToAI(MemberInfo.AIActor);
            }
        }
    }

    //@기본 그룹 생성 (필요한 경우)
    if (bAddUnassignedAIToDefaultGroup)
    {
        //@Group ID
        if (!DefaultGroupID.IsValid())
        {
            DefaultGroupID = FGuid::NewGuid();
        }

        //@MAIGroups
        if (!MAIGroups.Contains(DefaultGroupID))
        {
            FAIGroupInfo DefaultGroup;
            DefaultGroup.GroupID = DefaultGroupID;
            DefaultGroup.GroupName = DefaultGroupName.IsEmpty() ? TEXT("Default Group") : DefaultGroupName;

            //@추가
            MAIGroups.Add(DefaultGroupID, DefaultGroup);

            UE_LOGFMT(LogArea, Log, "Area {0}: 기본 그룹 '{1}' (ID: {2}) 생성됨",
                *AreaID.ToString(), *DefaultGroup.GroupName, *DefaultGroupID.ToString());
        }
    }

    //@자동 감지 활성화된 경우, 주변 AI 탐색
    if (bAutoDetectAI)
    {
        TArray<AActor*> OverlappingActors;

        // 첫 번째 방법: 현재 박스와 오버랩된 액터 가져오기
        AreaBounds->GetOverlappingActors(OverlappingActors, ACharacterBase::StaticClass());

        // 두 번째 방법: 추가적인 구체 트레이스로 주변 액터 찾기
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

            // 발견된 액터를 기존 목록에 합치기 (중복 제거)
            for (AActor* Actor : FoundActors)
            {
                if (IsValid(Actor) && !OverlappingActors.Contains(Actor))
                {
                    OverlappingActors.Add(Actor);
                }
            }
        }

        // 찾은 모든 AI 액터 등록 (플레이어는 제외)
        for (AActor* Actor : OverlappingActors)
        {
            if (IsValid(Actor) && !Cast<APlayerCharacter>(Actor))
            {
                RegisterAI(Actor);

                // RegisterAI에서는 내부 바인딩을 수행하지 않으므로 여기서 추가로 바인딩
                // RegisterAI가 성공했다면 이제 AI는 그룹에 할당되어 있을 것임
                if (GetAIGroupID(Actor).IsValid())
                {
                    InternalBindToAI(Actor);
                }
            }
        }
    }

    //@각 그룹의 리더 업데이트
    for (auto& GroupPair : MAIGroups)
    {
        UpdateAIGroupLeader(GroupPair.Key);
    }
}

void AArea::RegisterAI(AActor* AIActor)
{
    //@AI Actor
    if (!IsValid(AIActor))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 유효하지 않은 AI 등록 시도", *AreaID.ToString());
        return;
    }

    //@AI 참조 생성
    TWeakObjectPtr<AActor> AIActorPtr(AIActor);

    //@에디터에서 등록된 그룹 정보 확인
    bool bGroupAssigned = false;

    //@RegisteredAIGroups
    for (const FAIGroupInfo& GroupInfo : RegisteredAIGroups)
    {
        //@각 그룹의 멤버 목록에서 현재 AI 검색
        for (const FAreaAIInfo& MemberInfo : GroupInfo.GroupMembers)
        {
            if (MemberInfo.AIActor.Get() == AIActor)
            {
                //@AI가 속한 그룹 발견
                bGroupAssigned = true;

                // 이미 MAIGroups에 해당 그룹이 등록되어 있는지 확인
                if (!MAIGroups.Contains(GroupInfo.GroupID))
                {
                    //@새 그룹 등록
                    MAIGroups.Add(GroupInfo.GroupID, GroupInfo);

                    //@그룹 등록 완료 이벤트
                    AIRegisteredToAIGroup.Broadcast(AIActor, GroupInfo.GroupID);

                    UE_LOGFMT(LogArea, Log, "Area {0}: 그룹 '{1}' (ID: {2}) 등록 완료",
                        *AreaID.ToString(), *GroupInfo.GroupName, GroupInfo.GroupID);
                }

                // 이미 그룹 리더가 설정되어 있지 않으면 리더 업데이트
                if (!MAIGroups[GroupInfo.GroupID].GroupLeader.IsValid())
                {
                    UpdateAIGroupLeader(GroupInfo.GroupID);
                }

                break;
            }
        }

        if (bGroupAssigned)
            break;
    }

    //@Default Group?
    if (!bGroupAssigned && bAddUnassignedAIToDefaultGroup)
    {
        //@Default Group 생성
        if (!MAIGroups.Contains(DefaultGroupID))
        {
            FAIGroupInfo DefaultGroup(DefaultGroupID, DefaultGroupName);
            MAIGroups.Add(DefaultGroupID, DefaultGroup);

            UE_LOGFMT(LogArea, Log, "Area {0}: 기본 그룹 '{1}' (ID: {2}) 생성됨",
                *AreaID.ToString(), *DefaultGroupName, DefaultGroupID);
        }

        //@Default Gorup에 추가
        FAIGroupInfo& DefaultGroup = MAIGroups[DefaultGroupID];

        //@AI 계층 타입과 우선순위 기본값 설정
        FAreaAIInfo NewAIInfo(AIActor, FGameplayTag::RequestGameplayTag("State.Alive"), nullptr);
        DefaultGroup.GroupMembers.Add(NewAIInfo);

        //@리더 업데이트
        if (!DefaultGroup.GroupLeader.IsValid())
        {
            UpdateAIGroupLeader(DefaultGroupID);
        }

        UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 기본 그룹에 할당됨 (ID: {2})",
            *AreaID.ToString(), *AIActor->GetName(), DefaultGroupID);
    }

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 그룹 등록 처리 완료",
        *AreaID.ToString(), *AIActor->GetName());
}

void AArea::UnregisterAI(AActor* AIActor)
{
    //@AI
    if (!IsValid(AIActor))
    {
        return;
    }

    //TWeakObjectPtr
    TWeakObjectPtr<AActor> AIActorPtr(AIActor);

    //@Group ID
    FGuid GroupID = GetAIGroupID(AIActor);
    
    if (!GroupID.IsValid() || !MAIGroups.Contains(GroupID))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI {1}은(는) 등록되지 않았거나 유효한 그룹에 속해있지 않음",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    //@FAIGroupInof
    FAIGroupInfo& GroupInfo = MAIGroups[GroupID];

    //@Group Leader?
    bool bWasLeader = (GroupInfo.GroupLeader.Get() == AIActor);

    //@Remove
    bool bFound = false;
    for (int32 i = 0; i < GroupInfo.GroupMembers.Num(); i++)
    {
        if (GroupInfo.GroupMembers[i].AIActor.Get() == AIActor)
        {
            GroupInfo.GroupMembers.RemoveAt(i);
            bFound = true;

            UE_LOGFMT(LogArea, Log, "Area {0}: 그룹 {1}에서 AI {2} 제거됨",
                *AreaID.ToString(), *GroupID.ToString(), *AIActor->GetName());

            break;
        }
    }

    //@Not Found 종료
    if (!bFound)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 그룹 {1}에서 AI {2}를 찾을 수 없음",
            *AreaID.ToString(), *GroupID.ToString(), *AIActor->GetName());
        return;
    }

    //@Update Group Leader
    if (bWasLeader)
    {
        UpdateAIGroupLeader(GroupID);

        if (GroupInfo.GroupLeader.IsValid())
        {
            UE_LOGFMT(LogArea, Log, "Area {0}: 그룹 {1}의 새 리더로 {2} 선정됨",
                *AreaID.ToString(), *GroupID.ToString(), *GroupInfo.GroupLeader->GetName());

            //@그룹 상태 변화 이벤트
            GroupStatusChanged.Broadcast(GroupID, FString::Printf(TEXT("새 리더 지정: %s"),
                *GroupInfo.GroupLeader->GetName()));
        }
        else
        {
            UE_LOGFMT(LogArea, Log, "Area {0}: 그룹 {1}의 리더 없음",
                *AreaID.ToString(), *GroupID.ToString());

            //@그룹 상태 변화 이벤트
            GroupStatusChanged.Broadcast(GroupID, TEXT("리더 없음"));
        }
    }

    //@Group이 비어있을 경우
    if (GroupInfo.GroupMembers.Num() == 0)
    {
        UE_LOGFMT(LogArea, Log, "Area {0}: 그룹 {1}에 멤버가 없음",
            *AreaID.ToString(), *GroupID.ToString());

        //@제거
        if (GroupID != DefaultGroupID || !bAddUnassignedAIToDefaultGroup)
        {
            MAIGroups.Remove(GroupID);

            UE_LOGFMT(LogArea, Log, "Area {0}: 빈 그룹 {1} 제거됨",
                *AreaID.ToString(), *GroupID.ToString());

            //@그룹 상태 변화 이벤트
            GroupStatusChanged.Broadcast(GroupID, TEXT("그룹 제거됨"));
        }
    }

    //@Unbind
    UnbindFromAI(AIActorPtr);

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 등록 해제 완료",
        *AreaID.ToString(), *AIActor->GetName());
}

void AArea::UnregisterAllAI()
{
    //@AI 그룹 정보가 비어있으면 얼리 리턴
    if (MAIGroups.IsEmpty())
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 등록된 AI 그룹이 없습니다.", *AreaID.ToString());
        return;
    }

    //@그룹 내 모든 AI 수 계산
    int32 TotalAICount = 0;
    for (const auto& GroupPair : MAIGroups)
    {
        TotalAICount += GroupPair.Value.GroupMembers.Num();
    }

    UE_LOGFMT(LogArea, Log, "Area {0}: 영역 내 모든 AI 등록 해제 시작. 그룹 개수: {1}, AI 총 개수: {2}",
        *AreaID.ToString(), MAIGroups.Num(), TotalAICount);

    //@모든 AI에 대하여 바인딩 해제
    UnbindFromAllAI();

    //@MAIGroups 초기화
    MAIGroups.Empty();

    UE_LOGFMT(LogArea, Log, "Area {0}: 영역 내 모든 AI 등록 해제 완료", *AreaID.ToString());
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

void AArea::UnregisterAllPlayer()
{
    // MPlayerBindings이 비어있으면 얼리 리턴
    if (MPlayerBindings.IsEmpty())
    {
        UE_LOG(LogArea, Warning, TEXT("[AArea::UnregisterPlayer] 등록된 플레이어가 없습니다. AreaID: %s"), *AreaID.ToString());
        return;
    }

    //@바인딩 해제
    UnbindFromAllPlayer();

    //@MPlayerBindings 초기화
    MPlayerBindings.Empty();

    UE_LOG(LogArea, Log, TEXT("[AArea::UnregisterPlayer] 모든 플레이어 등록 해제 완료. AreaID: %s"), *AreaID.ToString());
}

void AArea::UpdateAIGroupLeader(const FGuid& GroupID)
{
    //@Group ID
    if (!MAIGroups.Contains(GroupID))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 리더 업데이트 실패 - 존재하지 않는 그룹 ID {1}",
            *AreaID.ToString(), *GroupID.ToString());
        return;
    }

    //@FAIGroupInfo
    FAIGroupInfo& GroupInfo = MAIGroups[GroupID];

    AActor* LeaderCandidate = nullptr;
    EAIHierarchyType HighestRank = EAIHierarchyType::Regular; 
    int32 HighestPriority = -1;

    //@FAreaAIInfo
    for (const FAreaAIInfo& MemberInfo : GroupInfo.GroupMembers)
    {
        //@AI Actor
        if (!MemberInfo.AIActor.IsValid())
            continue;

        AActor* AIActor = MemberInfo.AIActor.Get();

        //@Is Higerr Rank?
        bool bIsHigherRank = false;

        //@Hierarchy Rank
        if (static_cast<int32>(MemberInfo.HierarchyType) < static_cast<int32>(HighestRank))
        {
            bIsHigherRank = true;
        }
        //@Priority
        else if (MemberInfo.HierarchyType == HighestRank && MemberInfo.Priority > HighestPriority)
        {
            bIsHigherRank = true;
        }

        if (bIsHigherRank)
        {
            LeaderCandidate = AIActor;
            HighestRank = MemberInfo.HierarchyType;
            HighestPriority = MemberInfo.Priority;
        }
    }

    //@Prev Leader
    AActor* PreviousLeader = GroupInfo.GroupLeader.Get();

    //@Leader
    if (LeaderCandidate)
    {
        GroupInfo.GroupLeader = LeaderCandidate;

        if (PreviousLeader != LeaderCandidate)
        {
            UE_LOGFMT(LogArea, Log, "Area {0}: 그룹 ID {1}의 리더가 {2}에서 {3}로 변경됨 (계층: {4})",
                *AreaID.ToString(), *GroupID.ToString(),
                PreviousLeader ? *PreviousLeader->GetName() : TEXT("없음"),
                *LeaderCandidate->GetName(),
                *UEnum::GetValueAsString(HighestRank));

            //@그룹 상태 변화 이벤트
            GroupStatusChanged.Broadcast(GroupID, FString::Printf(TEXT("리더 변경: %s (계층: %s)"),
                *LeaderCandidate->GetName(), *UEnum::GetValueAsString(HighestRank)));
        }
    }
    else
    {
        //@리더가 없을 경우
        GroupInfo.GroupLeader = nullptr;

        if (PreviousLeader)
        {
            UE_LOGFMT(LogArea, Log, "Area {0}: 그룹 ID {1}의 리더가 제거됨 (이전: {2})",
                *AreaID.ToString(), *GroupID.ToString(), *PreviousLeader->GetName());

            //@그룹 상태 변경 이벤트
            GroupStatusChanged.Broadcast(GroupID, TEXT("리더 없음"));
        }
    }
}

void AArea::HandleAIStateChanged(AActor* AIActor, const FGameplayTag& StateTag)
{
    //@AI Actor
    if (!IsValid(AIActor))
    {
        return;
    }

    //@State Tag
    if (!StateTag.ToString().StartsWith("State."))
    {
        return;
    }

    //@TWeakObjectPtr
    TWeakObjectPtr<AActor> AIActorPtr(AIActor);

    //@Group ID
    FGuid GroupID = GetAIGroupID(AIActor);

    if (!GroupID.IsValid() || !MAIGroups.Contains(GroupID))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 등록되지 않은 AI {1}의 상태 변경 무시",
            *AreaID.ToString(), *AIActor->GetName());
        return;
    }

    //@FAIGroupInfo
    FAIGroupInfo& GroupInfo = MAIGroups[GroupID];
    //@FAreaAIInfo
    FAreaAIInfo* AIInfo = nullptr;

    for (auto& MemberInfo : GroupInfo.GroupMembers)
    {
        if (MemberInfo.AIActor.Get() == AIActor)
        {
            AIInfo = &MemberInfo;
            break;
        }
    }

    //@AI Info
    if (!AIInfo)
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 그룹 {1}에서 AI {2}의 정보를 찾을 수 없음",
            *AreaID.ToString(), *GroupID.ToString(), *AIActor->GetName());
        return;
    }

    //@Current State
    FGameplayTag OldState = AIInfo->CurrentState;
    if (OldState == StateTag)
    {
        return;
    }

    //@상태 정보 업데이트
    AIInfo->CurrentState = StateTag;

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1} 상태 변경: {2} -> {3}",
        *AreaID.ToString(), *AIActor->GetName(),
        OldState.IsValid() ? *OldState.ToString() : TEXT("None"),
        *StateTag.ToString());

    //@Area AI의 상태 변화 이벤트
    AreaAIStateChanged.Broadcast(AIActor, StateTag, this, AreaID);
}

void AArea::CleanupInvalidReferences()
{
 
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

    //@플레이어 검사
    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (Player)
    {
        RegisterPlayer(Player);
        return;
    }

    //@AI 검사 (플레이어가 아니고 캐릭터인 경우)
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
    //@Other Actor
    if (!IsValid(OtherActor))
    {
        return;
    }

    //@Player Character
    APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
    if (!Player)
    {
        return;
    }

    //@등록 해제
    UnregisterPlayer(Player);
}

void AArea::OnAICharacterStateEvent(AActor* Actor, const FGameplayTag& StateTag)
{
    //@상태 변경 처리 함수 호출
    HandleAIStateChanged(Actor, StateTag);
}

void AArea::OnPlayerStateEvent(AActor* Actor, const FGameplayTag& StateTag)
{

}

void AArea::OnAIDetectsTarget(bool bLockOn, AActor* AI, AActor* DetectedTarget)
{
    // Actor 유효성 검사
    if (!IsValid(AI) || !IsValid(DetectedTarget))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI 인지 이벤트 처리 실패 - 유효하지 않은 액터", *AreaID.ToString());
        return;
    }

    // AI가 등록되어 있는지 확인 (그룹 ID로 확인)
    FGuid GroupID = GetAIGroupID(AI);
    if (!GroupID.IsValid())
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI 인지 이벤트 처리 실패 - 등록되지 않은 AI {1}",
            *AreaID.ToString(), *AI->GetName());
        return;
    }

    // 감지된 대상이 플레이어인지 확인
    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(DetectedTarget);
    if (!PlayerCharacter)
    {
        return;
    }

    // 플레이어가 등록되어 있는지 확인
    TWeakObjectPtr<APlayerCharacter> PlayerPtr(PlayerCharacter);
    if (!MPlayerBindings.Contains(PlayerPtr))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: AI 인지 이벤트 처리 실패 - 등록되지 않은 플레이어 {1}",
            *AreaID.ToString(), *PlayerCharacter->GetName());
        return;
    }

    // AI의 Player 인지 이벤트 호출
    AIDetectsPlayer.Broadcast(bLockOn, AI, PlayerCharacter);

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1}이(가) 플레이어 {2}를 인지함 (락온: {3})",
        *AreaID.ToString(), *AI->GetName(), *PlayerCharacter->GetName(), bLockOn ? TEXT("True") : TEXT("False"));
}

void AArea::OnSendInfoToBelongingGroup(AActor* AI, FSharingInfoWithGroup SharingInfo)
{
    //@AI 유효성 검사
    if (!IsValid(AI))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 그룹 정보 전달 실패 - 유효하지 않은 AI", *AreaID.ToString());
        return;
    }

    //@상태 태그 유효성 검사
    if (!SharingInfo.StateTag.IsValid())
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 그룹 정보 전달 실패 - 유효하지 않은 상태 태그", *AreaID.ToString());
        return;
    }

    //@AI가 속한 그룹 ID 찾기
    FGuid GroupID = GetAIGroupID(AI);
    if (!GroupID.IsValid())
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 그룹 정보 전달 실패 - AI {1}이(가) 속한 그룹을 찾을 수 없음",
            *AreaID.ToString(), *AI->GetName());
        return;
    }

    //@그룹 존재 확인
    if (!MAIGroups.Contains(GroupID))
    {
        UE_LOGFMT(LogArea, Warning, "Area {0}: 그룹 정보 전달 실패 - 그룹 ID {1}이(가) 존재하지 않음",
            *AreaID.ToString(), *GroupID.ToString());
        return;
    }

    //@Group ID 업데이트
    SharingInfo.GroupID = GroupID;

    //@그룹 정보 전달 이벤트 호출
    NotifyGroupToShareInfo.Broadcast(AI, SharingInfo);

    UE_LOGFMT(LogArea, Log, "Area {0}: AI {1}의 그룹 {2}에 정보 전달 - 상태: {3}, 우선순위: {4}",
        *AreaID.ToString(), *AI->GetName(), *GroupID.ToString(), *SharingInfo.StateTag.ToString(), SharingInfo.Priority);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
TArray<FAreaAIInfo> AArea::GetAreaAIInfos() const
{
    TArray<FAreaAIInfo> AIInfos;

    //@모든 그룹을 순회
    for (const auto& GroupPair : MAIGroups)
    {
        const FAIGroupInfo& GroupInfo = GroupPair.Value;

        //@각 그룹의 모든 멤버를 결과 배열에 추가
        for (const FAreaAIInfo& MemberInfo : GroupInfo.GroupMembers)
        {
            //@유효한 AI만 추가
            if (MemberInfo.AIActor.IsValid())
            {
                AIInfos.Add(MemberInfo);
            }
        }
    }

    return AIInfos;
}

FGuid AArea::GetAIGroupID(AActor* AIActor) const
{
    if (!IsValid(AIActor))
        return FGuid();

    TWeakObjectPtr<AActor> AIActorPtr(AIActor);

    // 등록된 모든 그룹을 검사
    for (const auto& GroupPair : MAIGroups)
    {
        auto GroupID = GroupPair.Key;
        auto GroupInfo = GroupPair.Value;

        //@그룹의 멤버 목록에서 AIActor 찾기
        for (const FAreaAIInfo& MemberInfo : GroupInfo.GroupMembers)
        {
            if (MemberInfo.AIActor.Get() == AIActor)
            {
                return GroupID;
            }
        }
    }

    return FGuid();
}

TArray<FAIGroupInfo> AArea::GetAllAIGroupsAsArray() const
{
    TArray<FAIGroupInfo> GroupInfos;

    for (const auto& Pair : MAIGroups)
    {
        GroupInfos.Add(Pair.Value);
    }

    return GroupInfos;
}

TArray<AActor*> AArea::GetGroupMembers(const FGuid& GroupID) const
{
    TArray<AActor*> Members;

    // 그룹 ID가 존재하는지 확인
    if (!MAIGroups.Contains(GroupID))
    {
        return Members;
    }

    const FAIGroupInfo& GroupInfo = MAIGroups[GroupID];

    // 그룹 멤버 목록에서 유효한 액터만 추가
    for (const FAreaAIInfo& MemberInfo : GroupInfo.GroupMembers)
    {
        if (MemberInfo.AIActor.IsValid())
        {
            Members.Add(MemberInfo.AIActor.Get());
        }
    }

    return Members;
}
#pragma endregion