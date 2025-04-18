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
#include "Components/BillboardComponent.h"

DEFINE_LOG_CATEGORY(LogObjectiveDetection)

//@Defualt Setting
#pragma region Default Setting
UObjectiveDetectionComponent::UObjectiveDetectionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    SetComponentTickEnabled(true);

    // 기본적으로 감지할 상태 태그 설정
    StateTagsToDetect.Add(FGameplayTag::RequestGameplayTag("State.Fragile"));
    StateTagsToDetect.Add(FGameplayTag::RequestGameplayTag("State.Dead"));
    StateTagsToDetect.Add(FGameplayTag::RequestGameplayTag("State.Normal"));

    // 컴포넌트 고유 ID 생성
    ComponentID = FGuid::NewGuid();

    // 배열 초기화
    BoundAreas.Empty();

    //@현재 락온된 타겟 AI
    CurrentTargetAI.Reset();
    //@현재 등을 돌려 잠재 매복 타겟 AI
    AmbushTarget.Reset();
}

void UObjectiveDetectionComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UObjectiveDetectionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 빌보드 업데이트
    if (IndicatorBillboardComponent)
    {
        // 현재 타겟이 있으면 해당 타겟 표시
        if (CurrentTargetAI.IsValid())
        {
            UpdateBillboardComponent(true, false);
        }
        // 현재 타겟이 없지만 처형 타겟이 있는 경우
        else if (ExecutionTarget.IsValid())
        {
            UpdateBillboardComponent(true, false);
        }
        // 현재 타겟이 없지만 AmbushTarget이 있는 경우에도 표시
        else if (AmbushTarget.IsValid())
        {
            UpdateBillboardComponent(true, false);
        }
        else
        {
            // 모두 없으면 빌보드 숨기기
            IndicatorBillboardComponent->SetVisibility(false);
        }
    }

    float CurrentTime = GetWorld()->GetTimeSeconds();

    // 일정 간격으로 후면 노출 체크
    if (CurrentTime - LastBackExposureCheckTime >= BackExposureCheckInterval)
    {
        UpdateAIBackExposureState();
        LastBackExposureCheckTime = CurrentTime;
    }

    // 일정 간격으로 처형 가능 상태 체크
    if (CurrentTime - LastExecutionCheckTime >= ExecutionCheckInterval)
    {
        UpdateExecutionTargetState();
        LastExecutionCheckTime = CurrentTime;
    }
}
void UObjectiveDetectionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    //@타이머 정리
    if (GetWorld())
    {
        GetWorld()->GetTimerManager().ClearTimer(CleanupTimerHandle);
    }

    //@Pawn 이벤트 바인딩 해제
    UnbindFromPawnCapsuleEvents();

    //@모든 Area 바인딩 해제
    TArray<AArea*> AreasToUnbind;
    for (const FAreaBindingInfo& AreaInfo : BoundAreas)
    {
        if (AreaInfo.AreaRef.IsValid())
        {
            AreasToUnbind.Add(AreaInfo.AreaRef.Get());
        }
    }

    //@바인딩 된 Area 정리
    for (AArea* Area : AreasToUnbind)
    {
        UnbindFromAreaEvents(Area);
    }

    //@Bound Areas
    BoundAreas.Empty();

    if (IndicatorBillboardComponent)
    {
        IndicatorBillboardComponent->DestroyComponent();
        IndicatorBillboardComponent = nullptr;
    }

    Super::EndPlay(EndPlayReason);
}

void UObjectiveDetectionComponent::ExternalBindToPawnCapsuleComponent()
{
    //@Capsule Component
    UCapsuleComponent* CapsuleComp = GetPawnCapsuleComponent();
    if (!CapsuleComp)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "Pawn의 CapsuleComponent를 찾을 수 없음");
        return;
    }

    //@외부 바인딩...
    CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &UObjectiveDetectionComponent::OnPawnBeginOverlap);
    CapsuleComp->OnComponentEndOverlap.AddDynamic(this, &UObjectiveDetectionComponent::OnPawnEndOverlap);

    UE_LOGFMT(LogObjectiveDetection, Log, "Pawn({0})의 캡슐 이벤트 바인딩 완료",
        ControlledPawn.IsValid() ? *ControlledPawn->GetName() : TEXT("Unknown"));
}

void UObjectiveDetectionComponent::UnbindFromPawnCapsuleEvents()
{
    //@Capusle Component
    UCapsuleComponent* CapsuleComp = GetPawnCapsuleComponent();
    if (!CapsuleComp)
    {
        return;
    }

    //@외부 바인딩 해제...
    CapsuleComp->OnComponentBeginOverlap.RemoveAll(this);
    CapsuleComp->OnComponentEndOverlap.RemoveAll(this);

    UE_LOGFMT(LogObjectiveDetection, Log, "Pawn의 캡슐 이벤트 언바인딩 완료");
}

void UObjectiveDetectionComponent::ExternalBindToArea(AArea* Area)
{
    //@Area
    if (!Area)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "유효하지 않은 Area와 바인딩 시도");
        return;
    }

    //@FGuid
    FGuid AreaID = Area->GetAreaID();

    for (const FAreaBindingInfo& AreaInfo : BoundAreas)
    {
        if (AreaInfo.AreaID == AreaID)
        {
            UE_LOGFMT(LogObjectiveDetection, Warning, "이미 바인딩된 Area: {0}", *Area->GetName());
            return;
        }
    }

    //@현재 시간
    float CurrentTime = GetWorld()->GetTimeSeconds();

    //@외부 바인딩...
    Area->AreaAIStateChanged.AddUFunction(this, "OnAreaObjectiveStateChanged");
    Area->AIDetectsPlayer.AddUFunction(this, "OnDetectedByAI");

    //@바인딩 정보 생성 및 추가
    FAreaBindingInfo BindingInfo(Area, AreaID, CurrentTime);
    BoundAreas.Add(BindingInfo);

    //@바인딩 이벤트 호출
    PlyaerBoundToArea.Broadcast(BindingInfo, true);

    UE_LOGFMT(LogObjectiveDetection, Log, "Area {0}와 바인딩 완료", *Area->GetName());
}

void UObjectiveDetectionComponent::UnbindFromAreaEvents(AArea* Area)
{
    //@Area
    if (!IsValid(Area))
    {
        return;
    }

    //@FGuid
    FGuid AreaID = Area->GetAreaID();

    //@외부 바인딩 해제...
    Area->AreaAIStateChanged.RemoveAll(this);

    // 제거할 항목 찾기
    int32 IndexToRemove = -1;
    for (int32 i = 0; i < BoundAreas.Num(); i++)
    {
        if (BoundAreas[i].AreaID == AreaID)
        {
            IndexToRemove = i;
            break;
        }
    }

    // 항목이 있으면 이벤트 호출 후 제거
    if (IndexToRemove != -1)
    {
        // 제거 전에 이벤트 호출
        PlyaerBoundToArea.Broadcast(BoundAreas[IndexToRemove], false);
        BoundAreas.RemoveAt(IndexToRemove);
    }

    UE_LOGFMT(LogObjectiveDetection, Log, "Area {0}와 바인딩 해제 완료", *Area->GetName());
}

void UObjectiveDetectionComponent::ExternalBindToLockOnComponent()
{
    //@Player Controller
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOGFMT(LogObjectiveDetection, Error, "Owner가 PlayerController가 아닙니다.");
        return;
    }

    //@Pawn 가져오기
    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "PlayerController의 Pawn이 유효하지 않습니다.");
        return;
    }

    //@LockOnComponent 찾기
    ULockOnComponent* LockOnComp = PlayerPawn->FindComponentByClass<ULockOnComponent>();
    if (!LockOnComp)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "Pawn에서 LockOnComponent를 찾을 수 없습니다.");
        return;
    }

    //@LockOnStateChanged 델리게이트에 콜백 바인딩
    LockOnComp->LockOnStateChanged.AddUObject(this, &UObjectiveDetectionComponent::OnLockOnStateChanged);

    UE_LOGFMT(LogObjectiveDetection, Log, "LockOnComponent의 상태 변경 이벤트에 성공적으로 바인딩되었습니다.");
}

void UObjectiveDetectionComponent::InitializeODComponent()
{
    //@외부 바인딩...

    //@Controller
    AController* Controller = Cast<AController>(GetOwner());
    if (!Controller)
    {
        UE_LOGFMT(LogObjectiveDetection, Error, "ObjectiveDetectionComponent의 소유자가 Controller가 아님");
        return;
    }

    //@Pawn
    APawn* CurrentPawn = Controller->GetPawn();
    if (!CurrentPawn)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "Controller({0})에 연결된 Pawn이 없음", *Controller->GetName());
        return;
    }

    //@Controlled Pawn 업데이트
    ControlledPawn = CurrentPawn;
    UE_LOGFMT(LogObjectiveDetection, Log, "Controlled Pawn 설정: {0}", *CurrentPawn->GetName());

    //@AI
    if (IsOwnerAIController())
    {
        // AI는 시야 체크를 사용하지 않음
        bOnlyDetectInCameraView = false;

        UE_LOGFMT(LogObjectiveDetection, Log, "AI용 ObjectiveDetectionComponent 초기화 (소유자: {0}, Pawn: {1})",
            *Controller->GetName(), *CurrentPawn->GetName());
    }
    //@Player
    else if (IsOwnerPlayerController())
    {
        // PlayerController는 기본적으로 시야 체크 사용
        bOnlyDetectInCameraView = true;

        UE_LOGFMT(LogObjectiveDetection, Log, "플레이어용 ObjectiveDetectionComponent 초기화 (소유자: {0}, Pawn: {1})",
            *Controller->GetName(), *CurrentPawn->GetName());
    }
    else
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "알 수 없는 컨트롤러 유형: {0}", *Controller->GetClass()->GetName());
    }

    // Pawn이 유효한 경우 캡슐 컴포넌트 이벤트 바인딩
    if (!ControlledPawn.IsValid())
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "초기화 중 유효한 Pawn을 찾을 수 없음");
        return;
    }

    // 캡슐 컴포넌트 확인
    UCapsuleComponent* CapsuleComp = GetPawnCapsuleComponent();
    if (!CapsuleComp)
    {
        UE_LOGFMT(LogObjectiveDetection, Error, "Pawn({0})에 CapsuleComponent가 없음", *CurrentPawn->GetName());
        return;
    }

    ExternalBindToPawnCapsuleComponent();
    UE_LOGFMT(LogObjectiveDetection, Log, "초기 Pawn 바인딩 완료: {0} (컴포넌트: {1})",
        *ControlledPawn->GetName(), *CapsuleComp->GetName());

    ExternalBindToLockOnComponent();

    //@Timer
    GetWorld()->GetTimerManager().SetTimer(
        CleanupTimerHandle,
        this,
        &UObjectiveDetectionComponent::CleanupInvalidReferences,
        CleanupInterval,
        true
    );

    UE_LOGFMT(LogObjectiveDetection, Log, "정리 타이머 설정 완료 (간격: {0}초)", CleanupInterval);

    //@Billboard 컴포넌트 등록
    if (!IndicatorBillboardComponent && ControlledPawn.IsValid())
    {
        IndicatorBillboardComponent = NewObject<UBillboardComponent>(ControlledPawn.Get());
        IndicatorBillboardComponent->SetupAttachment(nullptr);
        IndicatorBillboardComponent->SetMobility(EComponentMobility::Movable);
        IndicatorBillboardComponent->RegisterComponent();
    }

    //@Billobard Component 초기화
    if (!LockOnIndicator.IsNull())
    {
        LockOnIndicator.LoadSynchronous();
        if (UTexture2D* LoadedTexture = LockOnIndicator.Get())
        {
            IndicatorBillboardComponent->SetSprite(LoadedTexture);
            // 원래 크기의 0.1배로 설정
            IndicatorBillboardComponent->SetRelativeScale3D(FVector(0.1f));
        }
    }

    IndicatorBillboardComponent->SetVisibility(false);
    IndicatorBillboardComponent->bHiddenInGame = false;

    // 컴포넌트 ID 로그
    UE_LOGFMT(LogObjectiveDetection, Log, "ObjectiveDetectionComponent 초기화 완료 (ID: {0})", *ComponentID.ToString());
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UObjectiveDetectionComponent::UpdateControlledPawn(APawn* NewPawn)
{
    // 이전 Pawn의 이벤트 바인딩 해제
    UnbindFromPawnCapsuleEvents();

    // 새 Pawn 설정
    ControlledPawn = NewPawn;

    // 유효성 검사
    if (!ControlledPawn.IsValid())
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "Pawn이 유효하지 않음");
        return;
    }

    // 새 Pawn의 이벤트 바인딩
    ExternalBindToPawnCapsuleComponent();

    UE_LOGFMT(LogObjectiveDetection, Log, "컨트롤된 Pawn 업데이트: {0}", *NewPawn->GetName());
}

void UObjectiveDetectionComponent::CleanupInvalidReferences()
{
    // 무효한 Area 참조 수집
    int32 RemovedCount = BoundAreas.RemoveAll([](const FAreaBindingInfo& AreaInfo) {
        return !AreaInfo.IsValid();
        });

    if (RemovedCount > 0)
    {
        UE_LOGFMT(LogObjectiveDetection, Log, "참조 정리 완료: 제거된 Area: {0}", RemovedCount);
    }
}

void UObjectiveDetectionComponent::UpdateBillboardComponent(bool bVisible, bool bChangeTransformOnly)
{
    // 기본 유효성 검사
    if (!IndicatorBillboardComponent)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "빌보드 업데이트 실패: 컴포넌트가 유효하지 않음");
        return;
    }

    // 가시성이 false라면 빌보드 숨기고 종료
    if (!bVisible)
    {
        IndicatorBillboardComponent->SetVisibility(false);
        return;
    }

    // 타겟 액터 결정 및 위치 업데이트
    AActor* TargetActor = DetermineTargetActor();
    if (!TargetActor)
    {
        IndicatorBillboardComponent->SetVisibility(false);
        return;
    }

    //@위치 업데이트
    if (!UpdateBillboardPosition(TargetActor))
    {
        //@위치 업데이트 실패 시 가시성 비활성화
        IndicatorBillboardComponent->SetVisibility(false);
        return;
    }

    //@텍스처 업데이트가 필요한 경우
    if (!bChangeTransformOnly)
    {
        UpdateBillboardTexture();
    }

    // 가시성 활성화
    IndicatorBillboardComponent->SetVisibility(true);
}

bool UObjectiveDetectionComponent::UpdateBillboardPosition(AActor* TargetActor)
{
    // 필요한 컴포넌트 확보
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC) return false;

    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn) return false;

    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(PlayerPawn);
    if (!PlayerChar) return false;

    UCameraComponent* CameraComp = PlayerChar->GetCameraComponent();
    if (!CameraComp) return false;

    // 타겟 메시 및 소켓 확인
    USkeletalMeshComponent* TargetMesh = Cast<USkeletalMeshComponent>(
        TargetActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
    if (!TargetMesh) return false;

    FName SpineSocketName = FName("spine_03");
    if (!TargetMesh->DoesSocketExist(SpineSocketName)) return false;

    // 위치 계산
    FVector SocketLocation = TargetMesh->GetSocketLocation(SpineSocketName);
    FVector CameraLocation = CameraComp->GetComponentLocation();
    FVector DirectionToSocket = (SocketLocation - CameraLocation).GetSafeNormal();
    FVector UpVector = FVector(0.0f, 0.0f, 1.0f);
    FVector TargetBillboardLocation = SocketLocation - DirectionToSocket * BillboardForwardOffset + UpVector;

    // 위치 및 회전 보간
    FVector CurrentLocation = IndicatorBillboardComponent->GetComponentLocation();
    FVector InterpolatedLocation = UKismetMathLibrary::VInterpTo(
        CurrentLocation,
        TargetBillboardLocation,
        GetWorld()->GetDeltaSeconds(),
        BillboardInterpolationSpeed
    );

    FRotator TargetRotation = (CameraLocation - SocketLocation).Rotation();
    FRotator CurrentRotation = IndicatorBillboardComponent->GetComponentRotation();
    FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(
        CurrentRotation,
        TargetRotation,
        GetWorld()->GetDeltaSeconds(),
        BillboardInterpolationSpeed
    );

    // 위치, 회전, 크기 적용
    IndicatorBillboardComponent->SetRelativeScale3D(FVector(TextureScale));
    IndicatorBillboardComponent->SetWorldRotation(InterpolatedRotation);
    IndicatorBillboardComponent->SetWorldLocation(InterpolatedLocation);

    return true;
}

void UObjectiveDetectionComponent::UpdateBillboardTexture()
{
    // 현재 상태에 따라 텍스처 결정
    bool bShouldUseExecutableIndicator = false;

    // 1. 현재 타겟이 처형 가능한 상태인 경우 (정면 노출 + Fragile)
    if (CurrentTargetAI.IsValid() && ExecutionTarget.IsValid() &&
        CurrentTargetAI.Get() == ExecutionTarget.Get())
    {
        bShouldUseExecutableIndicator = true;
    }
    // 2. 현재 타겟이 후면 노출 상태인 경우
    else if (CurrentTargetAI.IsValid() && AmbushTarget.IsValid() &&
        CurrentTargetAI.Get() == AmbushTarget.Get())
    {
        bShouldUseExecutableIndicator = true;
    }
    // 3. 타겟이 없고 처형 가능 타겟이 있는 경우
    else if (!CurrentTargetAI.IsValid() && ExecutionTarget.IsValid())
    {
        bShouldUseExecutableIndicator = true;
    }
    // 4. 타겟이 없고 AmbushTarget이 있는 경우
    else if (!CurrentTargetAI.IsValid() && AmbushTarget.IsValid())
    {
        bShouldUseExecutableIndicator = true;
    }

    // 텍스처 로드 및 설정
    if (bShouldUseExecutableIndicator && !ExecutableIndicator.IsNull())
    {
        ExecutableIndicator.LoadSynchronous();
        if (UTexture2D* LoadedTexture = ExecutableIndicator.Get())
        {
            SetIndicatorTexture(LoadedTexture);
        }
    }
    else if (!LockOnIndicator.IsNull())
    {
        LockOnIndicator.LoadSynchronous();
        if (UTexture2D* LoadedTexture = LockOnIndicator.Get())
        {
            SetIndicatorTexture(LoadedTexture);
        }
    }
}

void UObjectiveDetectionComponent::UpdateExecutionTargetState()
{
    // 이전 타겟 저장 및 현재 타겟 초기화
    AActor* PreviousExecutionTarget = ExecutionTarget.Get();
    ExecutionTarget.Reset();

    // 현재 타겟 AI 처리 (우선순위 높음)
    if (CurrentTargetAI.IsValid() && bIsCurrentTargetFragile)
    {
        AActor* AIActor = CurrentTargetAI.Get();
        TWeakObjectPtr<AActor> AIActorPtr(AIActor);

        // 정면 노출 상태 체크
        if (IsActorFrontExposed(AIActor))
        {
            ExecutionTarget = AIActor;
            UE_LOGFMT(LogObjectiveDetection, Log, "현재 타겟({0})이 정면 노출되어 처형 가능 상태, ExecutionTarget으로 설정",
                *AIActor->GetName());
            goto CHECK_TARGET_CHANGED;
        }

        UE_LOGFMT(LogObjectiveDetection, Warning, "현재 타겟({0})이 정면에 노출되지 않아 처형 불가",
            *AIActor->GetName());
    }

    //@현재 타겟이 없거나 처형 불가능한 경우, 주변 AI 검색
    {
        float ClosestDistance = MAX_FLT;
        AActor* ClosestActor = nullptr;

        for (const FAreaBindingInfo& AreaInfo : BoundAreas)
        {
            if (!AreaInfo.IsValid()) continue;

            AArea* Area = AreaInfo.AreaRef.Get();
            TArray<FAreaAIInfo> AreaAIInfos = Area->GetAreaAIInfos();

            for (auto AIInfo : AreaAIInfos)
            {
                // 기본 조건 검사 (유효성, 상태)
                if (!AIInfo.AIActor.IsValid() ||
                    AIInfo.CurrentState.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead")) ||
                    !AIInfo.CurrentState.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Fragile")))
                {
                    continue;
                }

                auto AI = AIInfo.AIActor.Get();
                TWeakObjectPtr<AActor> AIPtr(AI);

                // 카메라 시야 내 확인
                if (bOnlyDetectInCameraView && !IsActorInCameraView(AI))
                {
                    UE_LOGFMT(LogObjectiveDetection, Warning, "AI({0})가 시야 밖에 있어 처형 대상에서 제외",
                        *AI->GetName());
                    continue;
                }

                // 정면 노출 상태 체크
                if (!IsActorFrontExposed(AI))
                {
                    UE_LOGFMT(LogObjectiveDetection, Warning, "AI({0})가 정면에 노출되지 않아 처형 불가",
                        *AI->GetName());
                    continue;
                }

                // 거리 계산 및 가장 가까운 AI 선택
                FVector PawnLocation = GetPawnLocation();
                FVector AILocation = AI->GetActorLocation();
                float Distance = FVector::Distance(PawnLocation, AILocation);

                if (Distance < ClosestDistance)
                {
                    ClosestDistance = Distance;
                    ClosestActor = AI;
                    UE_LOGFMT(LogObjectiveDetection, Log, "더 가까운 처형 대상 발견: {0}, 거리: {1}",
                        *AI->GetName(), Distance);
                }
            }
        }

        //@가장 가까운 처형 가능 AI 선택
        if (ClosestActor)
        {
            ExecutionTarget = ClosestActor;
            UE_LOGFMT(LogObjectiveDetection, Log, "가장 가까운 처형 가능 AI({0})를 ExecutionTarget으로 설정, 거리: {1}",
                *ClosestActor->GetName(), ClosestDistance);
        }
    }

CHECK_TARGET_CHANGED:
    // 타겟 변경 확인 및 이벤트 발생
    if (ExecutionTarget.Get() != PreviousExecutionTarget)
    {
        UE_LOGFMT(LogObjectiveDetection, Log, "ExecutionTarget 변경: {0} -> {1}",
            PreviousExecutionTarget ? *PreviousExecutionTarget->GetName() : TEXT("없음"),
            ExecutionTarget.IsValid() ? *ExecutionTarget->GetName() : TEXT("없음"));

        ExecutionTargetChanged.Broadcast(ExecutionTarget.Get());
    }
}

void UObjectiveDetectionComponent::UpdateAIBackExposureState()
{
    //@이전 AmbushTarget 초기화
    AActor* PreviousAmbushTarget = AmbushTarget.Get();
    AmbushTarget.Reset();

    //@CurrentTargetAI가 유효한 경우, 해당 타겟만 체크
    if (CurrentTargetAI.IsValid())
    {
        AActor* AIActor = CurrentTargetAI.Get();

        //@AI가 플레이어를 인지 중인지 확인
        TWeakObjectPtr<AActor> AIActorPtr(AIActor);
        if (AIsDetectingPawn.Contains(AIActorPtr))
        {
            return;
        }
        else
        {
            //@후면 노출 상태 체크
            bool bIsBackExposed = IsActorBackExposed(AIActor);

            //@후면 노출된 경우 AmbushTarget으로 설정
            if (bIsBackExposed)
            {
                AmbushTarget = AIActor;
                UE_LOGFMT(LogObjectiveDetection, Log, "현재 타겟({0})이 후면 노출됨, AmbushTarget으로 설정", *AIActor->GetName());
            }
        }
    }
    else
    {
        //@Bound Areas
        float ClosestDistance = MAX_FLT;
        AActor* ClosestActor = nullptr;

        for (const FAreaBindingInfo& AreaInfo : BoundAreas)
        {
            if (!AreaInfo.IsValid()) continue;

            //@Area
            AArea* Area = AreaInfo.AreaRef.Get();
            //@Area AI
            TArray<FAreaAIInfo> AreaAIInfos = Area->GetAreaAIInfos();

            for (auto AIInfo : AreaAIInfos)
            {
                //@AI Actor, Current State != State.Dead
                if (!AIInfo.AIActor.IsValid() || AIInfo.CurrentState.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead"))) continue;

                auto AI = AIInfo.AIActor.Get();

                //@AI가 플레이어를 인지 중인지 확인
                TWeakObjectPtr<AActor> AIPtr(AI);
                if (AIsDetectingPawn.Contains(AIPtr))
                {
                    continue;
                }

                //@카메라 시야 내에 있는지 확인
                if (bOnlyDetectInCameraView && !IsActorInCameraView(AI))
                    continue;

                //@후면 노출 상태 체크
                bool bIsBackExposed = IsActorBackExposed(AI);

                //@가장 가까운 AI 타겟
                if (bIsBackExposed)
                {
                    FVector PawnLocation = GetPawnLocation();
                    FVector AILocation = AI->GetActorLocation();
                    float Distance = FVector::Distance(PawnLocation, AILocation);

                    if (Distance < ClosestDistance)
                    {
                        ClosestDistance = Distance;
                        ClosestActor = AI;
                    }
                }
            }
        }

        //@가장 가까운 AI
        if (ClosestActor)
        {
            AmbushTarget = ClosestActor;
            UE_LOGFMT(LogObjectiveDetection, Log, "가장 가까운 후면 노출 AI({0})를 AmbushTarget으로 설정", *ClosestActor->GetName());
        }
    }

    //@AmbushTarget
    if (AmbushTarget.Get() != PreviousAmbushTarget)
    {
        UE_LOGFMT(LogObjectiveDetection, Log, "AmbushTarget 변경: {0} -> {1}",
            PreviousAmbushTarget ? *PreviousAmbushTarget->GetName() : TEXT("없음"),
            AmbushTarget.IsValid() ? *AmbushTarget->GetName() : TEXT("없음"));

        //@잠재적 매복 암살 타겟 변경 이벤트
        AmbushTargetChanged.Broadcast(AmbushTarget.Get());
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UObjectiveDetectionComponent::OnPawnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
    bool bFromSweep, const FHitResult& SweepResult)
{
    //@Ohter Actor
    if (!IsValid(OtherActor))
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "오버랩 이벤트: 유효하지 않은 액터");
        return;
    }

    UE_LOGFMT(LogObjectiveDetection, Log, "({0})과 충돌!", OtherActor->GetName());

    //@Area 확인
    AArea* Area = Cast<AArea>(OtherActor);
    if (!Area)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "오버랩 이벤트: {0}은(는) Area가 아님 (클래스: {1})",
            *OtherActor->GetName(), *OtherActor->GetClass()->GetName());
        return;
    }

    UE_LOGFMT(LogObjectiveDetection, Log, "Pawn이 Area({0})에 진입, 바인딩 시작", *Area->GetName());

    //@외부 바인딩...
    ExternalBindToArea(Area);
}

void UObjectiveDetectionComponent::OnPawnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    // 충돌한 액터가 유효한지 확인
    if (!IsValid(OtherActor))
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "오버랩 종료 이벤트: 유효하지 않은 액터");
        return;
    }

    UE_LOGFMT(LogObjectiveDetection, Log, "({0})과 충돌 종료", OtherActor->GetName());

    // Area 확인
    AArea* Area = Cast<AArea>(OtherActor);
    if (!Area)
    {
        // Area가 아닌 경우 무시
        UE_LOGFMT(LogObjectiveDetection, Warning, "오버랩 종료 이벤트: {0}은(는) Area가 아님", *OtherActor->GetName());
        return;
    }

    UE_LOGFMT(LogObjectiveDetection, Log, "Pawn이 Area({0})에서 이탈, 바인딩 해제 시작", *Area->GetName());

    // Area 이벤트에서 언바인딩
    UnbindFromAreaEvents(Area);
}

void UObjectiveDetectionComponent::OnLockOnStateChanged(bool bIsLockOn, AActor* TargetActor)
{
    // 기본 상태 변화 로그 - 이벤트 발생 시 항상 출력
    UE_LOGFMT(LogObjectiveDetection, Log, "Lock On 상태 변경: {0}, 타겟: {1}",
        bIsLockOn ? TEXT("활성화") : TEXT("비활성화"),
        TargetActor ? *TargetActor->GetName() : TEXT("없음"));

    //@Billboard 컴포넌트 유효성 확인
    if (!IndicatorBillboardComponent)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "Lock On 표시 실패: Billboard 컴포넌트가 유효하지 않음");
        return;
    }

    //@Lock On 상태에 따른 처리
    if (bIsLockOn && TargetActor)
    {
        //@Lock On 활성화 시 타겟 설정 및 표시 업데이트
        SetCurrentTargetAI(TargetActor);
        UpdateBillboardComponent(true);
    }
    else if (!bIsLockOn)
    {
        //@Lock On 비활성화 시 타겟 제거 및 표시 숨김
        SetCurrentTargetAI(nullptr);
        UpdateBillboardComponent(false);
    }

    UE_LOGFMT(LogObjectiveDetection, Log, "Lock On 상태 처리 완료");
}

void UObjectiveDetectionComponent::OnAreaObjectiveStateChanged(AActor* ObjectiveActor, const FGameplayTag& StateTag, AArea* SourceArea, const FGuid& AreaID)
{
    //@인자 유효성 검사
    if (!IsValid(ObjectiveActor) || !IsValid(SourceArea))
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "유효하지 않은 객체: {0} 또는 {1}",
            ObjectiveActor ? TEXT("유효한 객체") : TEXT("유효하지 않은 객체"),
            SourceArea ? TEXT("유효한 Area") : TEXT("유효하지 않은 Area"));
        return;
    }

    //@StateTagsToDetect 필터링 - 관심 있는 상태 태그만 처리
    if (!StateTagsToDetect.Contains(StateTag))
    {
        return;
    }

    //@시야 내 액터만 감지하는 옵션이 켜져있는 경우, 시야 체크
    if (bOnlyDetectInCameraView && IsOwnerPlayerController())
    {
        if (!IsActorInCameraView(ObjectiveActor))
        {
            UE_LOGFMT(LogObjectiveDetection, Warning, "시야 밖 목표물 무시: {0}", *ObjectiveActor->GetName());
            return;
        }
    }

    //@현재 타겟 액터와 동일한지 확인
    if (CurrentTargetAI.IsValid() && CurrentTargetAI.Get() == ObjectiveActor)
    {
        //@Dead 상태 태그 확인
        if (StateTag.MatchesTag(FGameplayTag::RequestGameplayTag("State.Dead")))
        {
            //@Fragile 여부 해제
            bIsCurrentTargetFragile = false;

            //@타겟 해제
            SetCurrentTargetAI(nullptr);

            //@Billboard 컴포넌트 숨기기
            UpdateBillboardComponent(false);

            UE_LOGFMT(LogObjectiveDetection, Log, "타겟 {0}이(가) 사망하여 인디케이터를 비활성화 및 타겟 해제함", *ObjectiveActor->GetName());
        }
        //@Fragile 상태 변경 시
        else if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Fragile")))
        {
            bIsCurrentTargetFragile = true;
            UE_LOGFMT(LogObjectiveDetection, Log, "타겟 {0}이(가) Fragile 상태로 변경됨", *ObjectiveActor->GetName());

            // Fragile 상태가 되었을 때 즉시 실행처리 하지 않고, 
            // UpdateExecutionTargetState()가 다음 틱에서 처리하도록 함
            // 빌보드 업데이트는 계속 필요하지만 텍스처 변경은 차후 정면 체크 후 결정
            UpdateBillboardComponent(true, true);
        }
        //@Normal 상태 변경 시
        else if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Normal")))
        {
            bIsCurrentTargetFragile = false;

            // ExecutionTarget이 현재 타겟과 동일한 경우 ExecutionTarget 초기화
            if (ExecutionTarget.IsValid() && ExecutionTarget.Get() == ObjectiveActor)
            {
                ExecutionTarget.Reset();
                // 이벤트 발생
                ExecutionTargetChanged.Broadcast(nullptr);
                UE_LOGFMT(LogObjectiveDetection, Log, "타겟 {0}이(가) Normal 상태로 변경되어 처형 가능 타겟에서 제외됨", *ObjectiveActor->GetName());
            }

            UE_LOGFMT(LogObjectiveDetection, Log, "타겟 {0}이(가) Normal 상태로 변경됨", *ObjectiveActor->GetName());

            // 빌보드 업데이트
            UpdateBillboardComponent(true, false);
        }
    }

    //@감지 가능한 AI의 상태 변화 이벤트 호출
    DetectedAIStateChanged.Broadcast(StateTag, ObjectiveActor);

    UE_LOGFMT(LogObjectiveDetection, Log, "Area {0}에서 상태 변경 처리 완료: {1} -> {2}",
        *SourceArea->GetName(), *ObjectiveActor->GetName(), *StateTag.ToString());
}

void UObjectiveDetectionComponent::OnDetectedByAI(bool bIsDetected, AActor* AI, APlayerCharacter* DetectedPlayer)
{
    //@인자 유효성 검사
    if (!IsValid(AI) || !IsValid(DetectedPlayer))
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "ComponentID: {0} - 유효하지 않은 AI 또는 플레이어", ComponentID.ToString());
        return;
    }

    //@ControlledPawn
    APawn* CurrentPawn = GetControlledPawn();
    if (!IsValid(CurrentPawn))
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "ComponentID: {0} - 유효하지 않은 ControlledPawn", ComponentID.ToString());
        return;
    }

    //@ControlledPawn == DetectedPlayer?
    APlayerCharacter* PlayerPawn = Cast<APlayerCharacter>(CurrentPawn);
    if (!PlayerPawn || PlayerPawn != DetectedPlayer)
    {
        return;
    }

    //@AI의 인지 상태에 따라 목록 업데이트
    TWeakObjectPtr<AActor> AIPtr(AI);

    if (bIsDetected)
    {
        //@인지 중인 AI 목록에 추가
        AIsDetectingPawn.AddUnique(AIPtr);

        UE_LOGFMT(LogObjectiveDetection, Log, "ComponentID: {0} - AI {1}가 플레이어를 인지함",
            ComponentID.ToString(), AI->GetName());
    }
    else
    {
        //@인지가 해제된 경우 목록에서 제거
        AIsDetectingPawn.Remove(AIPtr);

        UE_LOGFMT(LogObjectiveDetection, Log, "ComponentID: {0} - AI {1}가 플레이어 인지를 해제함",
            ComponentID.ToString(), AI->GetName());
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
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
    // 소유자가 컨트롤러인지 확인
    AController* Controller = Cast<AController>(GetOwner());
    if (!Controller)
    {
        return nullptr;
    }

    // 컨트롤러의 Pawn 반환
    return Controller->GetPawn();
}

FVector UObjectiveDetectionComponent::GetPawnLocation() const
{
    // ControlledPawn이 유효한 경우 해당 위치 반환
    if (ControlledPawn.IsValid())
    {
        return ControlledPawn->GetActorLocation();
    }

    // 그렇지 않으면 현재 컨트롤 중인 Pawn의 위치 계산
    APawn* CurrentPawn = GetControlledPawn();
    if (CurrentPawn)
    {
        return CurrentPawn->GetActorLocation();
    }

    // 실패 시 Zero 반환
    return FVector::ZeroVector;
}

UCapsuleComponent* UObjectiveDetectionComponent::GetPawnCapsuleComponent() const
{
    //@ControlledPawn이 유효한지 확인
    if (!ControlledPawn.IsValid())
    {
        return nullptr;
    }

    // Pawn의 캡슐 컴포넌트 찾기
    return ControlledPawn->FindComponentByClass<UCapsuleComponent>();
}

UCameraComponent* UObjectiveDetectionComponent::GetPlayerCameraComponent() const
{
    // 플레이어 컨트롤러 확인
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        return nullptr;
    }

    // 컨트롤된 폰 확인
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

    // 카메라 컴포넌트 직접 찾기
    return PlayerCharacter->GetCameraComponent();
}

bool UObjectiveDetectionComponent::IsActorInCameraView(AActor* Actor) const
{
    // 유효성 검사
    if (!IsValid(Actor))
    {
        return false;
    }

    // PlayerController가 아니면 항상 true 반환
    if (!IsOwnerPlayerController())
    {
        return true;
    }

    // 플레이어 컨트롤러와 폰 가져오기
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC || !ControlledPawn.IsValid())
    {
        return false;
    }

    // 플레이어 캐릭터 확인
    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(ControlledPawn.Get());
    if (!PlayerChar)
    {
        return false;
    }

    // 카메라 컴포넌트 가져오기
    UCameraComponent* Camera = PlayerChar->FindComponentByClass<UCameraComponent>();
    if (!Camera)
    {
        return false;
    }

    // 카메라 위치와 전방 벡터 가져오기
    FVector CameraLocation = Camera->GetComponentLocation();
    FVector CameraForward = Camera->GetForwardVector();

    // 카메라에서 액터까지의 방향 벡터 계산
    FVector DirectionToActor = (Actor->GetActorLocation() - CameraLocation).GetSafeNormal();

    // 카메라 시야 내에 있는지 확인 (내적 사용)
    float DotProduct = FVector::DotProduct(CameraForward, DirectionToActor);

    // FOV 확장을 적용한 시야각 계산
    float HalfFOV = FMath::Cos(FMath::DegreesToRadians((Camera->FieldOfView + FOVExpansion) * 0.5f));

    // 시야 내에 있으면 true 반환
    return DotProduct > HalfFOV;
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
    if (!ControlledPawn.IsValid() || !IsValid(Actor))
    {
        return false;
    }

    //@플레이어 위치
    FVector PlayerLocation = ControlledPawn->GetActorLocation();
    //@AI 위치
    FVector ActorLocation = Actor->GetActorLocation();

    //@플레이어에서 AI로 향하는 방향 벡터
    FVector DirectionToActor = (ActorLocation - PlayerLocation).GetSafeNormal();

    //@AI의 전방 벡터
    FVector ActorForward = Actor->GetActorForwardVector();

    //@두 벡터 간의 각도 계산 (내적 사용)
    float DotProduct = FVector::DotProduct(DirectionToActor, ActorForward);

    //@지정된 임계값보다 크면 정면에서 바라보는 것으로 판단 (cos 값이 클수록 각도가 작음)
    //@ExecutionAngleThreshold는 0~1 사이 값 (1에 가까울수록 엄격한 각도)
    bool bIsFrontExposed = FMath::Abs(DotProduct) >= ExecutionAngleThreshold;

    UE_LOGFMT(LogObjectiveDetection, Log, "정면 노출 체크 - 액터: {0}, 결과: {1}, 내적값: {2}, 임계값: {3}",
        *Actor->GetName(), bIsFrontExposed ? TEXT("노출됨") : TEXT("노출 안됨"),
        DotProduct, ExecutionAngleThreshold);

    return bIsFrontExposed;
}

bool UObjectiveDetectionComponent::IsActorBackExposed(AActor* TargetActor) const
{
    if (!ControlledPawn.IsValid() || !IsValid(TargetActor))
    {
        return false;
    }

    return UCombatLibrary::IsActorBackExposed(ControlledPawn.Get(), TargetActor);
}

AActor* UObjectiveDetectionComponent::DetermineTargetActor()
{
    // 1. 현재 LockOn된 타겟이 있으면 우선 사용
    if (CurrentTargetAI.IsValid())
    {
        return CurrentTargetAI.Get();
    }
    // 2. 처형 가능 타겟이 있으면 다음 우선순위
    else if (ExecutionTarget.IsValid())
    {
        return ExecutionTarget.Get();
    }
    // 3. 매복 가능 타겟이 있으면 마지막 우선순위
    else if (AmbushTarget.IsValid())
    {
        return AmbushTarget.Get();
    }

    return nullptr;
}

void UObjectiveDetectionComponent::SetCurrentTargetAI(AActor* NewTargetActor)
{
    // 이전 타겟 저장
    AActor* PreviousTarget = CurrentTargetAI.Get();

    // 타겟이 변경될 경우 상태 초기화
    if (PreviousTarget != NewTargetActor)
    {
        // 타겟이 변경되면 Fragile 상태 초기화
        bIsCurrentTargetFragile = false;
    }

    // 새 타겟 설정
    CurrentTargetAI = NewTargetActor;

    // 로그 출력
    if (PreviousTarget != NewTargetActor)
    {
        UE_LOGFMT(LogObjectiveDetection, Log, "타겟 변경: {0} -> {1}",
            PreviousTarget ? *PreviousTarget->GetName() : TEXT("없음"),
            NewTargetActor ? *NewTargetActor->GetName() : TEXT("없음"));
    }
}

AActor* UObjectiveDetectionComponent::GetCurrentTargetAI() const
{
    return CurrentTargetAI.IsValid() ? CurrentTargetAI.Get() : nullptr;
}

void UObjectiveDetectionComponent::SetIndicatorTexture(UTexture2D* NewTexture)
{
    // Billboard 컴포넌트 유효성 확인
    if (!IndicatorBillboardComponent || !NewTexture)
    {
        return;
    }

    // 텍스처 설정
    IndicatorBillboardComponent->SetRelativeScale3D(FVector(TextureScale));
    IndicatorBillboardComponent->SetSprite(NewTexture);
}
#pragma endregion