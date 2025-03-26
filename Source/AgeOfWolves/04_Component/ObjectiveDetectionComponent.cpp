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
        // 현재 타겟이 없지만 AmbushTarget이 있는 경우에도 표시
        else if (AmbushTarget.IsValid())
        {
            UpdateBillboardComponent(true, false);
        }
        else
        {
            // 둘 다 없으면 빌보드 숨기기
            IndicatorBillboardComponent->SetVisibility(false);
        }
    }

    // 일정 간격으로 후면 노출 체크
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastBackExposureCheckTime >= BackExposureCheckInterval)
    {
        UpdateAIBackExposureState();
        LastBackExposureCheckTime = CurrentTime;
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

    //@바인딩 정보 생성 및 추가
    FAreaBindingInfo BindingInfo(Area, AreaID, CurrentTime);
    BoundAreas.Add(BindingInfo);

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

    //@바인딩된 Area 목록에서 제거
    BoundAreas.RemoveAll([AreaID](const FAreaBindingInfo& AreaInfo) {
        return AreaInfo.AreaID == AreaID;
        });

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
    // 가시성 요청이 False인 경우 처리 중단
    if (!bVisible)
    {
        if (IndicatorBillboardComponent)
        {
            IndicatorBillboardComponent->SetVisibility(false);
        }
        return;
    }

    // 타겟 액터 결정 (CurrentTargetAI 우선, 없으면 AmbushTarget 사용)
    AActor* TargetActor = nullptr;
    bool bIsAmbushTarget = false;

    if (CurrentTargetAI.IsValid())
    {
        TargetActor = CurrentTargetAI.Get();
        // 현재 타겟이 AmbushTarget과 동일한지 확인
        bIsAmbushTarget = (AmbushTarget.IsValid() && CurrentTargetAI.Get() == AmbushTarget.Get());
    }
    else if (AmbushTarget.IsValid())
    {
        TargetActor = AmbushTarget.Get();
        bIsAmbushTarget = true;
    }

    if (!TargetActor)
    {
        // 타겟이 없으면 빌보드 숨기기
        if (IndicatorBillboardComponent)
        {
            IndicatorBillboardComponent->SetVisibility(false);
        }
        return;
    }

    // Player Controller 확인
    APlayerController* PC = Cast<APlayerController>(GetOwner());
    if (!PC)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "빌보드 업데이트 실패: 소유자가 PlayerController가 아님");
        return;
    }

    // Pawn 확인
    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "빌보드 업데이트 실패: PlayerController에 연결된 Pawn이 없음");
        return;
    }

    // Player Character 확인
    APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(PlayerPawn);
    if (!PlayerChar)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "빌보드 업데이트 실패: Pawn이 PlayerCharacter가 아님");
        return;
    }

    // Camera Component 확인
    UCameraComponent* CameraComp = PlayerChar->GetCameraComponent();
    if (!CameraComp)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "빌보드 업데이트 실패: 카메라 컴포넌트를 찾을 수 없음");
        return;
    }

    // Skeletal Mesh 확인
    USkeletalMeshComponent* TargetMesh = Cast<USkeletalMeshComponent>(
        TargetActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
    if (!TargetMesh)
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "빌보드 업데이트 실패: 타겟의 스켈레탈 메시를 찾을 수 없음");
        return;
    }

    // Skeletal Mesh의 Socket 위치 확인
    FName SpineSocketName = FName("spine_03");
    if (!TargetMesh->DoesSocketExist(SpineSocketName))
    {
        UE_LOGFMT(LogObjectiveDetection, Warning, "빌보드 업데이트 실패: spine_03 소켓이 존재하지 않음");
        return;
    }

    // 타겟의 소켓 위치 가져오기
    FVector SocketLocation = TargetMesh->GetSocketLocation(SpineSocketName);
    // 카메라 위치 가져오기
    FVector CameraLocation = CameraComp->GetComponentLocation();
    // 카메라에서 소켓으로의 방향 계산
    FVector DirectionToSocket = (SocketLocation - CameraLocation).GetSafeNormal();

    // 소켓 위치에서 카메라 방향의 반대로 약간 앞으로 이동 + 높이 오프셋 적용
    FVector UpVector = FVector(0.0f, 0.0f, 1.0f);
    FVector TargetBillboardLocation = SocketLocation - DirectionToSocket * BillboardForwardOffset + UpVector;

    // 현재 빌보드 위치에서 목표 위치로 부드럽게 보간
    FVector CurrentLocation = IndicatorBillboardComponent->GetComponentLocation();
    FVector InterpolatedLocation = UKismetMathLibrary::VInterpTo(
        CurrentLocation,
        TargetBillboardLocation,
        GetWorld()->GetDeltaSeconds(),
        BillboardInterpolationSpeed
    );

    // 빌보드가 항상 카메라를 향하도록 회전 설정
    FRotator TargetRotation = (CameraLocation - SocketLocation).Rotation();
    FRotator CurrentRotation = IndicatorBillboardComponent->GetComponentRotation();
    FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(
        CurrentRotation,
        TargetRotation,
        GetWorld()->GetDeltaSeconds(),
        BillboardInterpolationSpeed
    );

    // 빌보드 위치 및 회전 설정
    IndicatorBillboardComponent->SetWorldLocation(InterpolatedLocation);
    IndicatorBillboardComponent->SetWorldRotation(InterpolatedRotation);
    // 크기 설정
    IndicatorBillboardComponent->SetRelativeScale3D(FVector(TextureScale));

    // 텍스처 업데이트 및 가시성 처리
    if (!bChangeTransformOnly)
    {
        // 텍스처 업데이트 로직 - 항상 실행되도록 수정
        if (bIsAmbushTarget && !ExecutableIndicator.IsNull())
        {
            // 후면 노출된 타겟은 Executable 인디케이터 사용
            ExecutableIndicator.LoadSynchronous();
            if (UTexture2D* LoadedTexture = ExecutableIndicator.Get())
            {
                SetIndicatorTexture(LoadedTexture);
                UE_LOGFMT(LogObjectiveDetection, Log, "타겟({0})이 후면 노출되어 Executable 인디케이터로 표시", *TargetActor->GetName());
            }
        }
        else if (!LockOnIndicator.IsNull())
        {
            // 일반 타겟은 LockOn 인디케이터 사용
            LockOnIndicator.LoadSynchronous();
            if (UTexture2D* LoadedTexture = LockOnIndicator.Get())
            {
                SetIndicatorTexture(LoadedTexture);
            }
        }

        // 가시성 항상 활성화 - 이 부분 중요!
        IndicatorBillboardComponent->SetVisibility(true);
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

        //@후면 노출 상태 체크
        bool bIsBackExposed = IsActorBackExposed(AIActor);

        //@후면 노출된 경우 AmbushTarget으로 설정
        if (bIsBackExposed)
        {
            AmbushTarget = AIActor;
            UE_LOGFMT(LogObjectiveDetection, Log, "현재 타겟({0})이 후면 노출됨, AmbushTarget으로 설정", *AIActor->GetName());
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
            TArray<AActor*> AreaAIActors = Area->GetAIInArea();

            for (AActor* AIActor : AreaAIActors)
            {
                if (!IsValid(AIActor)) continue;

                //@카메라 시야 내에 있는지 확인
                if (bOnlyDetectInCameraView && !IsActorInCameraView(AIActor))
                    continue;

                //@후면 노출 상태 체크
                bool bIsBackExposed = IsActorBackExposed(AIActor);

                //@가장 가까운 AI 타겟
                if (bIsBackExposed)
                {
                    FVector PawnLocation = GetPawnLocation();
                    FVector AILocation = AIActor->GetActorLocation();
                    float Distance = FVector::Distance(PawnLocation, AILocation);

                    if (Distance < ClosestDistance)
                    {
                        ClosestDistance = Distance;
                        ClosestActor = AIActor;
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
    if (CurrentTargetAI.IsValid()  && CurrentTargetAI.Get() == ObjectiveActor)
    {
        //@Fragile(처형 가능) 상태 태그 확인
        if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Fragile")))
        {
            //@Executable 인디케이터로 변경
            if (IndicatorBillboardComponent && !ExecutableIndicator.IsNull())
            {
                ExecutableIndicator.LoadSynchronous();
                if (UTexture2D* LoadedTexture = ExecutableIndicator.Get())
                {
                    SetIndicatorTexture(LoadedTexture);
                    UE_LOGFMT(LogObjectiveDetection, Log, "타겟 {0}이(가) Fragile 상태로 변경되어 Executable 인디케이터로 변경됨", *ObjectiveActor->GetName());
                }
                else
                {
                    UE_LOGFMT(LogObjectiveDetection, Warning, "ExecutableIndicator 텍스처 로드 실패");
                }
            }
        }
        //@Normal 상태 태그 확인
        else if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Normal")))
        {
            // LockOn 인디케이터로 변경
            if (IndicatorBillboardComponent && !LockOnIndicator.IsNull())
            {
                LockOnIndicator.LoadSynchronous();
                if (UTexture2D* LoadedTexture = LockOnIndicator.Get())
                {
                    SetIndicatorTexture(LoadedTexture);
                    UE_LOGFMT(LogObjectiveDetection, Log, "타겟 {0}이(가) Normal 상태로 변경되어 LockOn 인디케이터로 변경됨", *ObjectiveActor->GetName());
                }
                else
                {
                    UE_LOGFMT(LogObjectiveDetection, Warning, "LockOnIndicator 텍스처 로드 실패");
                }
            }
        }
        //@Dead 상태 태그 확인
        else if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead")))
        {
            // Billboard 컴포넌트 숨기기
            if (IndicatorBillboardComponent)
            {
                UpdateBillboardComponent(false);
            }

            // 타겟 해제
            SetCurrentTargetAI(nullptr);

            UE_LOGFMT(LogObjectiveDetection, Log, "타겟 {0}이(가) 사망하여 인디케이터를 비활성화 및 타겟 해제함", *ObjectiveActor->GetName());
        }
    }

    //@감지 가능한 AI의 상태 변화 이벤트 호출
    DetectedAIStateChanged.Broadcast(StateTag, ObjectiveActor);

    UE_LOGFMT(LogObjectiveDetection, Log, "Area {0}에서 상태 변경 처리 완료: {1} -> {2}",
        *SourceArea->GetName(), *ObjectiveActor->GetName(), *StateTag.ToString());
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
    // 이전 타겟 저장
    AActor* PreviousTarget = CurrentTargetAI.Get();

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