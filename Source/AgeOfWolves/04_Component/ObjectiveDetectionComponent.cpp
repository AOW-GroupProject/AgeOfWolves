#include "ObjectiveDetectionComponent.h"
#include "Logging/StructuredLog.h"

#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "07_BlueprintNode/CombatLibrary.h"

#include "10_AI/BaseAIController.h"
#include "GameFramework/Controller.h"
#include "01_Character/PlayerCharacter.h"
#include "GameFramework/PlayerController.h"

#include "Components/CapsuleComponent.h"
#include "04_Component/LockOnComponent.h"

DEFINE_LOG_CATEGORY(LogObjectiveDetection)

#pragma region Default Setting
UObjectiveDetectionComponent::UObjectiveDetectionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetComponentTickEnabled(true);

    StateTagsToDetect.Add(FGameplayTag::RequestGameplayTag("State.Fragile"));
    StateTagsToDetect.Add(FGameplayTag::RequestGameplayTag("State.Dead"));
    StateTagsToDetect.Add(FGameplayTag::RequestGameplayTag("State.Normal"));

    ComponentID = FGuid::NewGuid();
    BoundAreas.Empty();

    CurrentTargetAI.Reset();
    AmbushTarget.Reset();
    ExecutionTarget.Reset();
}

void UObjectiveDetectionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UObjectiveDetectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    bPawnLocationCached = false;

    if (GFrameCounter != LastViewCacheFrame)
    {
        CachedViewResults.Empty();
        LastViewCacheFrame = GFrameCounter;
    }

    float CurrentTime = GetWorld()->GetTimeSeconds();

    if (CurrentTime - LastBackExposureCheckTime >= BackExposureCheckInterval)
    {
        UpdateAIBackExposureState();
        LastBackExposureCheckTime = CurrentTime;
    }

    if (CurrentTime - LastExecutionCheckTime >= ExecutionCheckInterval)
    {
        UpdateExecutionTargetState();
        LastExecutionCheckTime = CurrentTime;
    }

    if (CurrentTime - LastExecutionStructureCheckTime >= ExecutionStructureCheckInterval)
    {
        UpdateDetectionStructure();
        LastExecutionStructureCheckTime = CurrentTime;
    }
}

void UObjectiveDetectionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(CleanupTimerHandle);
    }

    UnbindFromPawnCapsuleEvents();

    TArray<AArea*> AreasToUnbind;
    for (const FAreaBindingInfo& AreaInfo : BoundAreas)
    {
        if (AreaInfo.AreaRef.IsValid())
        {
            AreasToUnbind.Add(AreaInfo.AreaRef.Get());
        }
    }

    for (AArea* Area : AreasToUnbind)
    {
        UnbindFromAreaEvents(Area);
    }

    BoundAreas.Empty();
    CachedAreaAIs.Empty();
    AIsDetectingPawn.Empty();

    Super::EndPlay(EndPlayReason);
}

#if WITH_EDITOR
void UObjectiveDetectionComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UObjectiveDetectionComponent::ExternalBindToPawnCapsuleComponent()
{
    UCapsuleComponent* CapsuleComp = GetPawnCapsuleComponent();
    if (!CapsuleComp)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "Pawn의 CapsuleComponent를 찾을 수 없음");
        return;
    }

    CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &UObjectiveDetectionComponent::OnPawnBeginOverlap);
    CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &UObjectiveDetectionComponent::OnPawnEndOverlap);

    UE_LOGFMT(LogObjectiveDetection, Log, "Pawn({0})의 캡슐 이벤트 바인딩 완료",
        ControlledPawn.IsValid() ? *ControlledPawn->GetName() : TEXT("Unknown"));
}

void UObjectiveDetectionComponent::UnbindFromPawnCapsuleEvents()
{
    UCapsuleComponent* CapsuleComp = GetPawnCapsuleComponent();
    if (!CapsuleComp)
    {
        return;
    }

    CapsuleComp->OnComponentBeginOverlap.RemoveAll(this);
    CapsuleComp->OnComponentEndOverlap.RemoveAll(this);
}

void UObjectiveDetectionComponent::ExternalBindToArea(AArea* Area)
{
    if (!Area)
    {
        return;
    }

    FGuid AreaID = Area->GetAreaID();

    if (IsAreaBound(AreaID))
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "이미 바인딩된 Area: {0}", *Area->GetName());
        return;
    }

    float CurrentTime = GetWorld()->GetTimeSeconds();

    Area->AreaAIStateChanged.AddUFunction(this, "OnAreaObjectiveStateChanged");
    Area->AIDetectsPlayer.AddUFunction(this, "OnDetectedByAI");

    FAreaBindingInfo BindingInfo(Area, AreaID, CurrentTime);
    BoundAreas.Add(BindingInfo);

    CachedAreaAIs.Remove(AreaID);

    PlyaerBoundToArea.Broadcast(BindingInfo, true);
}

void UObjectiveDetectionComponent::UnbindFromAreaEvents(AArea* Area)
{
    if (!IsValid(Area))
    {
        return;
    }

    FGuid AreaID = Area->GetAreaID();

    Area->AreaAIStateChanged.RemoveAll(this);
    Area->AIDetectsPlayer.RemoveAll(this);

    int32 IndexToRemove = -1;
    for (int32 i = 0; i < BoundAreas.Num(); i++)
    {
        if (BoundAreas[i].AreaID == AreaID)
        {
            IndexToRemove = i;
            break;
        }
    }

    if (IndexToRemove != -1)
    {
        PlyaerBoundToArea.Broadcast(BoundAreas[IndexToRemove], false);
        BoundAreas.RemoveAt(IndexToRemove);
    }

    CachedAreaAIs.Remove(AreaID);
}

void UObjectiveDetectionComponent::ExternalBindToLockOnComponent()
{
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        return;
    }

    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn)
    {
        return;
    }

    ULockOnComponent* LockOnComp = PlayerPawn->FindComponentByClass<ULockOnComponent>();
    if (!LockOnComp)
    {
        return;
    }

    LockOnComp->LockOnStateChanged.AddUObject(this, &UObjectiveDetectionComponent::OnLockOnStateChanged);
}

void UObjectiveDetectionComponent::InitializeODComponent()
{
    AController* Controller = Cast<AController>(GetOwner());
    if (!Controller)
    {
        UE_LOGFMT(LogObjectiveDetection, Error, "ObjectiveDetectionComponent의 소유자가 Controller가 아님");
        return;
    }

    APawn* CurrentPawn = Controller->GetPawn();
    if (!CurrentPawn)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "Controller({0})에 연결된 Pawn이 없음", *Controller->GetName());
        return;
    }

    ControlledPawn = CurrentPawn;

    if (IsOwnerAIController())
    {
        bOnlyDetectInCameraView = false;
    }
    else if (IsOwnerPlayerController())
    {
        bOnlyDetectInCameraView = true;
    }

    if (!ControlledPawn.IsValid())
    {
        return;
    }

    UCapsuleComponent* CapsuleComp = GetPawnCapsuleComponent();
    if (!CapsuleComp)
    {
        return;
    }

    ExternalBindToPawnCapsuleComponent();
    ExternalBindToLockOnComponent();

    GetWorld()->GetTimerManager().SetTimer(
        CleanupTimerHandle,
        this,
        &UObjectiveDetectionComponent::CleanupInvalidReferences,
        CleanupInterval,
        true
    );
}
#pragma endregion

#pragma region Property or Subwidgets or Infos...etc
void UObjectiveDetectionComponent::UpdateControlledPawn(APawn* NewPawn)
{
    UnbindFromPawnCapsuleEvents();

    ControlledPawn = NewPawn;

    if (!ControlledPawn.IsValid())
    {
        return;
    }

    bPawnLocationCached = false;
    CachedAreaAIs.Empty();

    ExternalBindToPawnCapsuleComponent();
}

void UObjectiveDetectionComponent::CleanupInvalidReferences()
{
    int32 RemovedAreas = BoundAreas.RemoveAll([](const FAreaBindingInfo& AreaInfo) {
        return !AreaInfo.IsValid();
        });

    int32 RemovedAIs = 0;
    for (auto It = AIsDetectingPawn.CreateIterator(); It; ++It)
    {
        if (!It->IsValid())
        {
            It.RemoveCurrent();
            RemovedAIs++;
        }
    }

    if (CurrentTargetAI.IsValid() && !IsValid(CurrentTargetAI.Get()))
    {
        SetCurrentTargetAI(nullptr);
    }

    if (AmbushTarget.IsValid() && !IsValid(AmbushTarget.Get()))
    {
        AmbushTarget.Reset();
        AmbushTargetChanged.Broadcast(nullptr);
    }

    if (ExecutionTarget.IsValid() && !IsValid(ExecutionTarget.Get()))
    {
        ExecutionTarget.Reset();
        ExecutionTargetChanged.Broadcast(nullptr);
    }

    TArray<FGuid> InvalidCacheKeys;
    for (auto& Pair : CachedAreaAIs)
    {
        if (!FindAreaByGuid(Pair.Key))
        {
            InvalidCacheKeys.Add(Pair.Key);
        }
    }

    for (const FGuid& Key : InvalidCacheKeys)
    {
        CachedAreaAIs.Remove(Key);
    }

    if (RemovedAreas > 0 || RemovedAIs > 0 || InvalidCacheKeys.Num() > 0)
    {
        UE_LOGFMT(LogObjectiveDetection, Log, "참조 정리 완료 - Area: {0}, AI: {1}, 캐시: {2}",
            RemovedAreas, RemovedAIs, InvalidCacheKeys.Num());
    }
}

void UObjectiveDetectionComponent::UpdateExecutionTargetState()
{
    AActor* PreviousExecutionTarget = ExecutionTarget.Get();
    ExecutionTarget.Reset();

    //1단계: '락온' 타겟이 있다면, 최우선 순위
    if (CurrentTargetAI.IsValid() && bIsCurrentTargetFragile)
    {
        AActor* AIActor = CurrentTargetAI.Get();

        if (IsActorFrontExposed(AIActor))
        {
            ExecutionTarget = AIActor;

            if (ExecutionTarget.Get() != PreviousExecutionTarget)
            {
                ExecutionTargetChanged.Broadcast(ExecutionTarget.Get());
            }
            return;
        }
    }

    FVector PawnLocation = GetPawnLocation();
    float ClosestDistance = MAX_FLT;
    AActor* ClosestActor = nullptr;

    for (const FAreaBindingInfo& AreaInfo : BoundAreas)
    {
        if (!AreaInfo.IsValid()) continue;

        FGuid AreaID = AreaInfo.AreaID;

        if (!CachedAreaAIs.Contains(AreaID))
        {
            AArea* Area = AreaInfo.AreaRef.Get();
            CachedAreaAIs.Add(AreaID, Area->GetAreaAIInfos());
        }

        const TArray<FAreaAIInfo>& AreaAIs = CachedAreaAIs[AreaID];

        for (const FAreaAIInfo& AIInfo : AreaAIs)
        {
            if (!AIInfo.AIActor.IsValid()) continue;

            AActor* AI = AIInfo.AIActor.Get();

            //2단계: 최소 요구조건 체크(죽음 상태인지, 카메라 뷰에 보이는지)
            if (!MeetsMinimumDetectionConditions(AI, AIInfo.CurrentState))
            {
                continue;
            }

            //3단계: '취약'상태 체크
            if (!AIInfo.CurrentState.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Fragile")))
            {
                continue;
            }

            //4단계: 캐릭터와 마주보고 있는지 체크
            if (!IsActorFrontExposed(AI))
            {
                continue;
            }

            float Distance = FVector::Distance(PawnLocation, AI->GetActorLocation());

            //5단계: 가장 가까이 있는 타겟 업데이트
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestActor = AI;
            }
        }
    }

    ExecutionTarget = ClosestActor;

    if (ExecutionTarget.Get() != PreviousExecutionTarget)
    {
        ExecutionTargetChanged.Broadcast(ExecutionTarget.Get());
    }
}

void UObjectiveDetectionComponent::UpdateAIBackExposureState()
{
    AActor* PreviousAmbushTarget = AmbushTarget.Get();
    AmbushTarget.Reset();

    if (CurrentTargetAI.IsValid())
    {
        AActor* AIActor = CurrentTargetAI.Get();
        TWeakObjectPtr<AActor> AIPtr(AIActor);

        if (!AIsDetectingPawn.Contains(AIPtr) && IsActorBackExposed(AIActor))
        {
            AmbushTarget = AIActor;
        }
    }
    else
    {
        FVector PawnLocation = GetPawnLocation();
        float ClosestDistance = MAX_FLT;
        AActor* ClosestActor = nullptr;

        for (const FAreaBindingInfo& AreaInfo : BoundAreas)
        {
            if (!AreaInfo.IsValid()) continue;

            FGuid AreaID = AreaInfo.AreaID;

            if (!CachedAreaAIs.Contains(AreaID))
            {
                AArea* Area = AreaInfo.AreaRef.Get();
                CachedAreaAIs.Add(AreaID, Area->GetAreaAIInfos());
            }

            const TArray<FAreaAIInfo>& AreaAIs = CachedAreaAIs[AreaID];

            for (const FAreaAIInfo& AIInfo : AreaAIs)
            {
                if (!AIInfo.AIActor.IsValid()) continue;

                AActor* AI = AIInfo.AIActor.Get();

                //2단계: 최소 요구 조건 체크
                if (!MeetsMinimumDetectionConditions(AI, AIInfo.CurrentState))
                {
                    continue;
                }

                TWeakObjectPtr<AActor> AIPtr(AI);
                if (AIsDetectingPawn.Contains(AIPtr))
                {
                    continue;
                }

                //3단계: 등을 보이고 있는지 체크
                if (!IsActorBackExposed(AI))
                {
                    continue;
                }

                float Distance = FVector::Distance(PawnLocation, AI->GetActorLocation());

                //4단계: 가장 가까운 AI로 타겟을 업데이트
                if (Distance < ClosestDistance)
                {
                    ClosestDistance = Distance;
                    ClosestActor = AI;
                }
            }
        }

        AmbushTarget = ClosestActor;
    }

    if (AmbushTarget.Get() != PreviousAmbushTarget)
    {
        AmbushTargetChanged.Broadcast(AmbushTarget.Get());
    }
}

void UObjectiveDetectionComponent::UpdateDetectionStructure()
{
<<<<<<< HEAD
    FVector PawnLocation = GetPawnLocation();
=======
    FVector OwnerLocation;
    FVector OwnerForwardDir  ;
    if (const APlayerController* PC = Cast<APlayerController>(GetOwner()))
    {
        if (const APawn* P = PC->GetPawn())
        {
            OwnerLocation = P->GetActorLocation();
            OwnerForwardDir = P->GetActorForwardVector();
        }
    }
>>>>>>> origin/develop

    const float HalfAngleDeg = FMath::Max(0.f, DetectionStructureTotalAngleDegrees * 0.5f);
    const float CosThreshold = FMath::Cos(FMath::DegreesToRadians(HalfAngleDeg));
    const float DistLimit = DetectionStructureDistance;

    AActor* ClosestStructure = nullptr;
    float LowestDistSq = FLT_MAX;
    bool HasFound = false;

    for (const FAreaBindingInfo& AreaInfo : BoundAreas)
    {
        if (!AreaInfo.IsValid()) continue;

        TArray<FStructureData> StructureDatas = AreaInfo.AreaRef.Get()->GetStructureDatas();

        for (const FStructureData& StructureData : StructureDatas)
        {
            AActor* Target = StructureData.GetStructureActor();
            if (!IsValid(Target)) continue;

<<<<<<< HEAD
            const FVector ToOwner = PawnLocation - Target->GetActorLocation();
            float DistSq = ToOwner.SizeSquared();

            //1단계: 최대 거리 내 캐릭터가 존재하는지 체크
            if (DistSq > (DistLimit * DistLimit))
                continue;

            FVector TargetForwardDir = Target->GetActorForwardVector();
            FVector OwnerToTargetDir = ToOwner.GetSafeNormal();
=======
            
            //@ 감지 거리 체크
            const FVector ToTarget =  Target->GetActorLocation() - OwnerLocation;
            float DistSq  = FVector::Dist(Target->GetActorLocation(), OwnerLocation);
            if (DistSq > DistLimit)
                continue;

            FVector TargetForwardDir = Target->GetActorForwardVector();
            FVector OwerToTargetDir = ToTarget;
>>>>>>> origin/develop

            if (!TargetForwardDir.Normalize())
                continue;

<<<<<<< HEAD
            //2단계: 내적, 지정 각도 내 캐릭터가 위치하는지 체크
            const float CosAngle = FVector::DotProduct(TargetForwardDir, OwnerToTargetDir);
=======
            //@구조물 보는 방향 x각도 이내에 있는지 체크
            const float CosAngle = FVector::DotProduct(OwnerForwardDir, OwerToTargetDir);
>>>>>>> origin/develop
            if (CosAngle < CosThreshold)
                continue;

            if (DistSq >= LowestDistSq) continue;

            HasFound = true;
            LowestDistSq = DistSq;
            ClosestStructure = Target;
        }
    }

    if (!HasFound)
    {
        if (DetectedStructureActor.IsValid())
        {
            DetectedStructureChanged.Broadcast(DetectedStructureActor.Get(), false);
        }
        DetectedStructureActor = nullptr;
    }
    else if (DetectedStructureActor.Get() != ClosestStructure)
    {
        DetectedStructureActor = ClosestStructure;
        DetectedStructureChanged.Broadcast(DetectedStructureActor.Get(), true);
    }
}
#pragma endregion

#pragma region Callbacks
void UObjectiveDetectionComponent::OnPawnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    if (!IsValid(OtherActor))
    {
        return;
    }

    AArea* Area = Cast<AArea>(OtherActor);
    if (!Area)
    {
        return;
    }

    ExternalBindToArea(Area);
}

void UObjectiveDetectionComponent::OnPawnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (!IsValid(OtherActor))
    {
        return;
    }

    AArea* Area = Cast<AArea>(OtherActor);
    if (!Area)
    {
        return;
    }

    UnbindFromAreaEvents(Area);
}

void UObjectiveDetectionComponent::OnLockOnStateChanged(bool bIsLockOn, AActor* TargetActor)
{
    if (bIsLockOn && TargetActor)
    {
        SetCurrentTargetAI(TargetActor);
    }
    else if (!bIsLockOn)
    {
        SetCurrentTargetAI(nullptr);
    }
}

void UObjectiveDetectionComponent::OnAreaObjectiveStateChanged(AActor* ObjectiveActor, const FGameplayTag& StateTag, AArea* SourceArea, const FGuid& AreaID)
{
    if (!IsValid(ObjectiveActor) || !IsValid(SourceArea))
    {
        return;
    }

    if (!StateTagsToDetect.Contains(StateTag))
    {
        return;
    }

    if (bOnlyDetectInCameraView && IsOwnerPlayerController())
    {
        if (!IsInCameraView(ObjectiveActor))
        {
            return;
        }
    }

    CachedAreaAIs.Remove(AreaID);

    if (CurrentTargetAI.IsValid() && CurrentTargetAI.Get() == ObjectiveActor)
    {
        if (StateTag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Dead")))
        {
            bIsCurrentTargetFragile = false;
            SetCurrentTargetAI(nullptr);
        }
        else if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Fragile")))
        {
            bIsCurrentTargetFragile = true;
        }
        else if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Normal")))
        {
            bIsCurrentTargetFragile = false;

            if (ExecutionTarget.IsValid() && ExecutionTarget.Get() == ObjectiveActor)
            {
                ExecutionTarget.Reset();
                ExecutionTargetChanged.Broadcast(nullptr);
            }
        }
    }

    if (AmbushTarget.IsValid() && AmbushTarget.Get() == ObjectiveActor)
    {
        if (StateTag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Dead")))
        {
            AmbushTarget.Reset();
            AmbushTargetChanged.Broadcast(nullptr);
        }
    }

    if (ExecutionTarget.IsValid() && ExecutionTarget.Get() == ObjectiveActor)
    {
        bool bShouldClear = StateTag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Dead")) ||
            StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Normal"));

        if (bShouldClear)
        {
            ExecutionTarget.Reset();
            ExecutionTargetChanged.Broadcast(nullptr);
        }
    }

    DetectedAIStateChanged.Broadcast(StateTag, ObjectiveActor);
}

void UObjectiveDetectionComponent::OnDetectedByAI(bool bIsDetected, AActor* AI, APlayerCharacter* DetectedPlayer)
{
    if (!IsValid(AI) || !IsValid(DetectedPlayer))
    {
        return;
    }

    APawn* CurrentPawn = GetControlledPawn();
    if (!IsValid(CurrentPawn))
    {
        return;
    }

    APlayerCharacter* PlayerPawn = Cast<APlayerCharacter>(CurrentPawn);
    if (!PlayerPawn || PlayerPawn != DetectedPlayer)
    {
        return;
    }

    TWeakObjectPtr<AActor> AIPtr(AI);

    if (bIsDetected)
    {
        AIsDetectingPawn.Add(AIPtr);
    }
    else
    {
        AIsDetectingPawn.Remove(AIPtr);
    }
}
#pragma endregion

#pragma region Utility
bool UObjectiveDetectionComponent::IsOwnerAIController() const
{
    return GetOwner() && GetOwner()->IsA(ABaseAIController::StaticClass());
}

bool UObjectiveDetectionComponent::IsOwnerPlayerController() const
{
    return GetOwner() && GetOwner()->IsA(APlayerController::StaticClass());
}

APawn* UObjectiveDetectionComponent::GetControlledPawn() const
{
    AController* Controller = Cast<AController>(GetOwner());
    if (!Controller)
    {
        return nullptr;
    }

    return Controller->GetPawn();
}

FVector UObjectiveDetectionComponent::GetPawnLocation()
{
    if (bPawnLocationCached)
    {
        return CachedPawnLocation;
    }

    if (ControlledPawn.IsValid())
    {
        CachedPawnLocation = ControlledPawn->GetActorLocation();
        bPawnLocationCached = true;
        return CachedPawnLocation;
    }

    APawn* CurrentPawn = GetControlledPawn();
    if (CurrentPawn)
    {
        CachedPawnLocation = CurrentPawn->GetActorLocation();
        bPawnLocationCached = true;
        return CachedPawnLocation;
    }

    return FVector::ZeroVector;
}

UCapsuleComponent* UObjectiveDetectionComponent::GetPawnCapsuleComponent() const
{
    if (!ControlledPawn.IsValid())
    {
        return nullptr;
    }

    return ControlledPawn->FindComponentByClass<UCapsuleComponent>();
}

UCameraComponent* UObjectiveDetectionComponent::GetPlayerCameraComponent() const
{
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        return nullptr;
    }

    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn)
    {
        return nullptr;
    }

    auto PlayerCharacter = Cast<APlayerCharacter>(PlayerPawn);
    if (!PlayerCharacter)
    {
        return nullptr;
    }

    return PlayerCharacter->GetCameraComponent();
}

bool UObjectiveDetectionComponent::MeetsMinimumDetectionConditions(AActor* AIActor, const FGameplayTag& StateTag) const
{
    if (!AIActor->IsValidLowLevel())
    {
        return false;
    }

    if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead")))
    {
        return false;
    }

    if (bOnlyDetectInCameraView && !const_cast<UObjectiveDetectionComponent*>(this)->IsInCameraView(AIActor))
    {
        return false;
    }

    return true;
}

bool UObjectiveDetectionComponent::IsInCameraView(AActor* Actor)
{
    if (!IsValid(Actor))
    {
        return false;
    }

    TWeakObjectPtr<AActor> ActorPtr(Actor);
    if (const bool* CachedResult = CachedViewResults.Find(ActorPtr))
    {
        return *CachedResult;
    }

    if (!IsOwnerPlayerController())
    {
        return true;
    }

    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC || !ControlledPawn.IsValid())
    {
        return false;
    }

    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(ControlledPawn.Get());
    if (!PlayerChar)
    {
        return false;
    }

    UCameraComponent* Camera = PlayerChar->FindComponentByClass<UCameraComponent>();
    if (!Camera)
    {
        return false;
    }

    FVector CameraLocation = Camera->GetComponentLocation();
    FVector CameraForward = Camera->GetForwardVector();
    FVector DirectionToActor = (Actor->GetActorLocation() - CameraLocation).GetSafeNormal();

    float DotProduct = FVector::DotProduct(CameraForward, DirectionToActor);
    float HalfFOV = FMath::Cos(FMath::DegreesToRadians((Camera->FieldOfView + FOVExpansion) * 0.5f));

    bool bInView = DotProduct > HalfFOV;

    CachedViewResults.Add(ActorPtr, bInView);

    return bInView;
}

AArea* UObjectiveDetectionComponent::FindAreaByGuid(const FGuid& AreaGuid) const
{
    for (const FAreaBindingInfo& AreaInfo : BoundAreas)
    {
        if (AreaInfo.AreaID == AreaGuid && AreaInfo.AreaRef.IsValid())
        {
            return AreaInfo.AreaRef.Get();
        }
    }
    return nullptr;
}

TArray<FAreaBindingInfo> UObjectiveDetectionComponent::GetBoundAreas() const
{
    return BoundAreas;
}

bool UObjectiveDetectionComponent::IsAreaBound(const FGuid& AreaID) const
{
    for (const FAreaBindingInfo& AreaInfo : BoundAreas)
    {
        if (AreaInfo.AreaID == AreaID)
        {
            return true;
        }
    }
    return false;
}

FGuid UObjectiveDetectionComponent::GetComponentID() const
{
    return ComponentID;
}

bool UObjectiveDetectionComponent::IsActorFrontExposed(AActor* Actor) const
{
    if (!bPawnLocationCached || !IsValid(Actor))
    {
        return false;
    }

    FVector ActorLocation = Actor->GetActorLocation();
    FVector DirectionToActor = (ActorLocation - CachedPawnLocation).GetSafeNormal();
    FVector ActorForward = Actor->GetActorForwardVector();

    float DotProduct = FVector::DotProduct(DirectionToActor, ActorForward);

    return FMath::Abs(DotProduct) >= ExecutionAngleThreshold;
}

bool UObjectiveDetectionComponent::IsActorBackExposed(AActor* TargetActor) const
{
    if (!ControlledPawn.IsValid() || !IsValid(TargetActor))
    {
        return false;
    }

    return UCombatLibrary::IsActorBackExposed(ControlledPawn.Get(), TargetActor);
}

void UObjectiveDetectionComponent::SetCurrentTargetAI(AActor* NewTargetActor)
{
    AActor* PreviousTarget = CurrentTargetAI.Get();

    if (PreviousTarget != NewTargetActor)
    {
        bIsCurrentTargetFragile = false;
    }

    CurrentTargetAI = NewTargetActor;

    if (PreviousTarget != NewTargetActor)
    {
        CurrentTargetChanged.Broadcast(NewTargetActor);
    }
}

AActor* UObjectiveDetectionComponent::GetCurrentTargetAI() const
{
    return CurrentTargetAI.IsValid() ? CurrentTargetAI.Get() : nullptr;
}
#pragma endregion