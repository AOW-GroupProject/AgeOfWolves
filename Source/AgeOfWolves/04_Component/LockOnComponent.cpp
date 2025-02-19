#include "LockOnComponent.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "04_Component/BaseInputComponent.h"
#include "05_Animation/BaseAnimInstance.h"
#include "03_Player/BasePlayerController.h"
#include "04_Component/BaseCharacterMovementComponent.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/BillboardComponent.h"

DEFINE_LOG_CATEGORY(LogLockOn)

//@Default Setting
#pragma region Default Setting
ULockOnComponent::ULockOnComponent()
{
    PrimaryComponentTick.bCanEverTick = true;

    bLockOn = false;

    NearByEnemies.Reset();
    EnemyMap.Reset();

    TargetEnemyRef.Reset();
    PlayerCharacterRef.Reset();
    BaseAnimInstanceRef.Reset();
    SpringArmComponentRef.Reset();
    FollowCameraComponentRef.Reset();
    BaseInputComponentRef.Reset();
}

void ULockOnComponent::BeginPlay()
{
    Super::BeginPlay();

}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    //@회전 업데이트
    UpdateControllerRotation(DeltaTime);
}

void ULockOnComponent::ExternalBindToInputComp(const AController* Controller)
{
    //@LockOn Component <-> Input Component 바인딩 설정
    auto PC = Cast<ABasePlayerController>(Controller);
    if (!PC)
    {
        UE_LOGFMT(LogPlayer, Warning, "입력 컴포넌트 바인딩 실패: BasePlayerController가 유효하지 않음");
        return;
    }

    UBaseInputComponent* BaseInputComp = PC->GetBaseInputComponent();
    if (!BaseInputComp)
    {
        UE_LOGFMT(LogPlayer, Warning, "입력 컴포넌트 바인딩 실패: BaseInputComponent가 유효하지 않음");
        return;
    }

    //@타겟 변경 입력에 대한 이벤트 바인딩
    BaseInputComp->NativeInputTagTriggeredWithValue.AddUObject(this, &ULockOnComponent::OnLockOnTargetChanged);

    UE_LOGFMT(LogLockOn, Log, "입력 컴포넌트 바인딩 완료");
}

void ULockOnComponent::ExternalBindToASCComp()
{
    //@Owner 캐릭터의 ASC 가져오기
    auto OwnerCharacter = Cast<ACharacterBase>(GetOwner());
    if (!OwnerCharacter)
    {
        UE_LOGFMT(LogLockOn, Warning, "ASC 바인딩 실패: Owner 캐릭터가 유효하지 않음");
        return;
    }

    //@ASC 컴포넌트 가져오기
    auto BaseASC = Cast<UBaseAbilitySystemComponent>(OwnerCharacter->GetAbilitySystemComponent());
    if (!BaseASC)
    {
        UE_LOGFMT(LogLockOn, Warning, "ASC 바인딩 실패: BaseAbilitySystemComponent가 유효하지 않음");
        return;
    }

    //@상태 변화 이벤트 구독
    BaseASC->CharacterStateEventOnGameplay.AddUObject(this, &ULockOnComponent::OnOwnerStateChanged);

    UE_LOGFMT(LogLockOn, Log, "어빌리티 시스템 컴포넌트 바인딩 완료");
}

void ULockOnComponent::InitializeLockOnComp(const AController* Controller)
{
    //@외부 바인딩...
    ExternalBindToInputComp(Controller);
    ExternalBindToASCComp();

    //@컴포넌트 등록
    if (!LockOnBillboardComponent)
    {
        LockOnBillboardComponent = NewObject<UBillboardComponent>(GetOwner());
        if (!LockOnBillboardComponent)
        {
            UE_LOGFMT(LogLockOn, Warning, "빌보드 컴포넌트 생성 실패");
            return;
        }
        LockOnBillboardComponent->RegisterComponent();
    }


    //@Billobard Component 초기화
    if (!LockOnTexture.IsNull())
    {
        LockOnTexture.LoadSynchronous();
        if (UTexture2D* LoadedTexture = LockOnTexture.Get())
        {
            LockOnBillboardComponent->SetSprite(LoadedTexture);
            // 원래 크기의 0.1배로 설정
            LockOnBillboardComponent->SetRelativeScale3D(FVector(0.1f));
        }
    }

    LockOnBillboardComponent->SetVisibility(false);
    LockOnBillboardComponent->bHiddenInGame = false;

    if (!Controller)
    {
        UE_LOGFMT(LogLockOn, Warning, "컴포넌트 초기화 실패: 컨트롤러가 유효하지 않음");
        return;
    }


    const auto Owner = GetOwner();
    if (!Owner)
    {
        UE_LOGFMT(LogLockOn, Warning, "Owner Actor가 유효하지 않습니다.");
        return;
    }

    PlayerCharacterRef = Cast<APlayerCharacter>(Owner);
    if (!PlayerCharacterRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "컴포넌트 초기화 실패: 플레이어 캐릭터가 유효하지 않음");
        return;
    }

    SpringArmComponentRef = PlayerCharacterRef->GetSpringArmComponent();
    if (!SpringArmComponentRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "컴포넌트 초기화 실패: 스프링암이 유효하지 않음");
        return;
    }

    FollowCameraComponentRef = PlayerCharacterRef->GetCameraComponent();
    if (!FollowCameraComponentRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "컴포넌트 초기화 실패: 카메라가 유효하지 않음");
        return;
    }

    BaseAnimInstanceRef = Cast<UBaseAnimInstance>(PlayerCharacterRef->GetMesh()->GetAnimInstance());
    if (!BaseAnimInstanceRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "컴포넌트 초기화 실패: 애님 인스턴스가 유효하지 않음");
        return;
    }

    ABasePlayerController* PlayerController = Cast<ABasePlayerController>(PlayerCharacterRef->GetController());
    if (!PlayerController)
    {
        UE_LOGFMT(LogLockOn, Warning, "컴포넌트 초기화 실패: 플레이어 컨트롤러가 유효하지 않음");
        return;
    }

    BaseInputComponentRef = PlayerController->GetBaseInputComponent();
    if (!BaseInputComponentRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "컴포넌트 초기화 실패: 입력 컴포넌트가 유효하지 않음");
        return;
    }



    UE_LOGFMT(LogLockOn, Log, "락온 컴포넌트 초기화 완료");
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void ULockOnComponent::StartLockOn()
{
    //@Ref 
    if (!PlayerCharacterRef.IsValid() || !BaseAnimInstanceRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "락온 시작 실패: 플레이어 캐릭터가 유효하지 않음");
        return;
    }

    //@Target Enemy 찾기
    if (!FindTargetEnemy())
    {
        UE_LOGFMT(LogLockOn, Warning, "락온 시작 실패: 적절한 타겟을 찾을 수 없음");
        return;
    }

    //@현재 타겟의 상태 변화 이벤트에 바인딩 수행
    BindCurrentTargetStateEvents();

    //@시각 효과
    if (!LockOnBillboardComponent)
    {
        UE_LOGFMT(LogLockOn, Warning, "락온 시작 실패: 빌보드 컴포넌트가 유효하지 않음");
        return;
    }

    if (!TargetEnemyRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "락온 시작 실패: 타겟이 유효하지 않음");
        return;
    }

    USceneComponent* TargetRoot = TargetEnemyRef->GetRootComponent();
    if (!TargetRoot)
    {
        UE_LOGFMT(LogLockOn, Warning, "락온 시작 실패: 타겟의 루트 컴포넌트가 유효하지 않음");
        return;
    }

    //@Billboard Component 부착
    if (LockOnBillboardComponent && PlayerCharacterRef.IsValid() && TargetEnemyRef.IsValid())
    {
        UpdateBillboardComponent(true);
    }

    //@Spring Arm(Camera) 설정 업데이트
    UpdateSpringArmSettings(true);

    //@Lock On 상태
    bLockOn = true;

    //@Lock On 상태 이벤트
    LockOnStateChanged.Broadcast(bLockOn);

    UE_LOGFMT(LogLockOn, Log, "Lock On 시작: 타겟 = {0}", *TargetEnemyRef->GetName());
}

void ULockOnComponent::CancelLockOn()
{
    //@Ref
    if (!PlayerCharacterRef.IsValid() || !BaseAnimInstanceRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "락온 해제 실패: 플레이어 캐릭터가 유효하지 않음");
        return;
    }

    //@타겟의 상태 변화 이벤트 언바인딩
    UnbindCurrentTargetStateEvents();

    //@Billboard
    UpdateBillboardComponent(false);

    //@Spring Arm 설정 업데이트
    UpdateSpringArmSettings(false);

    //@Lock On 상태
    bLockOn = false;

    //@Lock On 상태 이벤트
    LockOnStateChanged.Broadcast(bLockOn);

    //@초기화
    NearByEnemies.Empty();
    EnemyMap.Empty();
    TargetEnemyRef.Reset();
}

bool ULockOnComponent::FindTargetEnemy()
{
    if (!PlayerCharacterRef.IsValid()) return false;
    if (!FollowCameraComponentRef.IsValid()) return false;

    TArray<TEnumAsByte<EObjectTypeQuery>> NearByActors;
    NearByActors.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

    TArray<AActor*> IgnoreActors;
    IgnoreActors.Add(PlayerCharacterRef.Get());

    // 첫 번째: 구체 트레이스로 범위 내 모든 액터 찾기
    TArray<FHitResult> HitResults;
    bool SphereTraceHitResult = UKismetSystemLibrary::SphereTraceMultiForObjects(
        GetWorld(),
        PlayerCharacterRef->GetActorLocation(),
        PlayerCharacterRef->GetActorLocation(),
        MaxDetectRadius,
        NearByActors,
        false,
        IgnoreActors,
        EDrawDebugTrace::None,
        HitResults,
        true
    );

    if (!SphereTraceHitResult)
    {
        UE_LOGFMT(LogLockOn, Log, "No targets found in range");
        return false;
    }

    NearByEnemies.Empty();
    EnemyMap.Empty();
    float CenterDot = MAX_FLT;

    const FVector CameraLocation = FollowCameraComponentRef->GetComponentTransform().GetTranslation();
    const FVector CameraForward = FollowCameraComponentRef->GetForwardVector();
    const FVector CameraRight = FollowCameraComponentRef->GetRightVector();
    const FVector CameraUp = FollowCameraComponentRef->GetUpVector();
    const float HalfFOV = FMath::Cos(FMath::DegreesToRadians(FollowCameraComponentRef->FieldOfView * 0.5f));

    bool bHasTargetInFOV = false;

    // 두 번째: 각 액터에 대해 FOV와 가시성, 콜리전 체크
    for (const auto& Hit : HitResults)
    {
        AActor* HitActor = Hit.GetActor();

        //@TODO: 임시적으로, Target의 생사여부를 충돌 활성화 여부로 체크
        if (!IsValid(HitActor) || !HitActor->GetActorEnableCollision())
        {
            UE_LOGFMT(LogLockOn, Verbose, "Skipping actor {0} - collision disabled",
                IsValid(HitActor) ? *HitActor->GetName() : TEXT("Invalid"));
            continue;
        }

        // 시야 체크를 위한 Line Trace
        FHitResult LineHitResults;
        bool LineTraceHitResult = UKismetSystemLibrary::LineTraceSingleForObjects(
            GetWorld(),
            CameraLocation,
            HitActor->GetActorLocation(),
            NearByActors,
            false,
            IgnoreActors,
            EDrawDebugTrace::None,
            LineHitResults,
            true
        );

        // Line Trace로 장애물 체크
        if (HitActor == LineHitResults.GetActor())
        {
            // 카메라에서 적까지의 방향 벡터
            FVector DirectionToEnemy = (HitActor->GetActorLocation() - CameraLocation).GetSafeNormal();

            // 화면 중앙으로부터의 각도 계산 (내적)
            float ForwardDot = FVector::DotProduct(CameraForward, DirectionToEnemy);

            // FOV 체크
            if (ForwardDot > HalfFOV)
            {
                bHasTargetInFOV = true;

                // 좌우 위치 판단을 위한 외적과 내적
                FVector Cross = FVector::CrossProduct(CameraForward, DirectionToEnemy);
                float RightDot = FVector::DotProduct(Cross, CameraUp);

                NearByEnemies.AddUnique(HitActor);
                EnemyMap.Add(RightDot, HitActor);

                // 화면 중앙에 가장 가까운 적 찾기
                if (FMath::Abs(RightDot) < FMath::Abs(CenterDot))
                {
                    CenterDot = RightDot;
                }
            }
        }
    }

    // FOV 내에 타겟이 없으면 Lock On 실패
    if (!bHasTargetInFOV || EnemyMap.IsEmpty())
    {
        UE_LOGFMT(LogLockOn, Log, "No valid targets in camera FOV");
        return false;
    }

    // 좌우 위치에 따라 정렬
    TArray<float> DotProducts;
    EnemyMap.GenerateKeyArray(DotProducts);
    DotProducts.Sort();

    // 정렬된 순서대로 NearByEnemies 재구성
    NearByEnemies.Empty();
    for (float DotProduct : DotProducts)
    {
        NearByEnemies.Add(*EnemyMap.Find(DotProduct));
    }

    // 화면 중앙에 가장 가까운 적을 첫 타겟으로 설정
    TargetEnemyRef = *EnemyMap.Find(CenterDot);
    UE_LOGFMT(LogLockOn, Log, "Found target in FOV: {0}", *TargetEnemyRef->GetName());

    return IsValid(TargetEnemyRef.Get());
}

void ULockOnComponent::BindCurrentTargetStateEvents()
{
    //@Target Enemy
    if (!TargetEnemyRef.IsValid()) return;

    //@CharacterBase로 캐스팅
    ACharacterBase* Character = Cast<ACharacterBase>(TargetEnemyRef.Get());
    if (!Character) return;

    //@ASC
    if (UBaseAbilitySystemComponent* TargetASC = Cast<UBaseAbilitySystemComponent>(Character->GetAbilitySystemComponent()))
    {
        //@상태 변화 이벤트 바인딩
        TargetASC->CharacterStateEventOnGameplay.AddUObject(this, &ULockOnComponent::OnTargetStateChanged);

        UE_LOGFMT(LogLockOn, Log, "타겟 {0}에 대한 상태 이벤트 바인딩 완료", *TargetEnemyRef->GetName());
    }
}

void ULockOnComponent::UnbindCurrentTargetStateEvents()
{
    if (!TargetEnemyRef.IsValid()) return;

    ACharacterBase* Character = Cast<ACharacterBase>(TargetEnemyRef.Get());
    if (!Character) return;

    if (UBaseAbilitySystemComponent* TargetASC = Cast<UBaseAbilitySystemComponent>(Character->GetAbilitySystemComponent()))
    {
        TargetASC->CharacterStateEventOnGameplay.RemoveAll(this);
        UE_LOGFMT(LogLockOn, Log, "타겟 {0}에 대한 상태 이벤트 바인딩 해제", *TargetEnemyRef->GetName());
    }
}

void ULockOnComponent::UpdateSpringArmSettings(bool bIsLockingOn)
{
    if (!SpringArmComponentRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "스프링암 설정 업데이트 실패: 스프링암이 유효하지 않음");
        return;
    }

    SpringArmComponentRef->bUsePawnControlRotation = true;

    SpringArmComponentRef->CameraLagSpeed = bIsLockingOn ? 5.0f : 10.0f;
    SpringArmComponentRef->CameraRotationLagSpeed = bIsLockingOn ? 17.5f : 30.0f;
}

void ULockOnComponent::UpdateControllerRotation(float DeltaTime)
{
    //@락온 체크, Target Enemy 체크, Player Character 유효성 체크, Spring Arm Component 유효성 체크
    if (!bLockOn || !TargetEnemyRef.IsValid() || !PlayerCharacterRef.IsValid() || !SpringArmComponentRef.IsValid())
    {
        return;
    }

    //@Max Lock On Distance
    bool bCloseToEnemy = (PlayerCharacterRef->GetActorLocation() - TargetEnemyRef->GetActorLocation()).Length() < MaxLockOnDistance;
    if (!bCloseToEnemy)
    {
        CancelLockOn();
        return;
    }

    //@Base Anim Instance, Base Input Component
    if (!BaseAnimInstanceRef.IsValid() || !BaseInputComponentRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "카메라 변환 실패: 애님 인스턴스 또는 입력 컴포넌트 유효하지 않음");

        return;
    }

    //@Location
    FVector Start = PlayerCharacterRef->GetActorLocation();
    FVector Target = TargetEnemyRef->GetActorLocation();
    
    //@Rotation
    FRotator StartRotation = PlayerCharacterRef->GetControlRotation();
    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
    
    //@Final Rotation
    FinalRotation = UKismetMathLibrary::RInterpTo(StartRotation, TargetRotation, DeltaTime, InterpolationSpeed);

    //@RInterpTo
    PlayerCharacterRef->GetController()->SetControlRotation(FRotator(0.f, FinalRotation.Yaw, 0.f));

    //@Update Spring Arm
    //UpdateSpringArmTransform(DeltaTime, Target, FinalRotation);

    //@시각 효과
    if (LockOnBillboardComponent && TargetEnemyRef.IsValid())
    {
        UpdateBillboardComponent(true, true);
    }
}

void ULockOnComponent::UpdateSpringArmTransform(float DeltaTime, const FVector& Target, const FRotator& TargetRotation)
{
    //@Spring Arm Comp, Base Anim Intance, Base Input Comp
    if (!SpringArmComponentRef.IsValid() || !BaseAnimInstanceRef.IsValid() || !BaseInputComponentRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "스프링암 변환 실패: 필요한 컴포넌트가 유효하지 않음");
        return;
    }

    //@Offset Coefficient
    float SocketOffsetCoefficient = (BaseAnimInstanceRef->GetMovementState() != EMovementState::Sprinting) ? 1.5f : 1.0f;

    //@Distance to Rotation
    float DistanceFromTargetEnemy = (PlayerCharacterRef->GetActorLocation() - Target).Length();
    DistanceFromTargetEnemy = FMath::Clamp((6000 / DistanceFromTargetEnemy) + 20, 0, 30);
    FRotator DistanceRotation = FRotator(-DistanceFromTargetEnemy, 0, 0);

    //@Final Rotation
    FRotator CameraFinalRotation = DistanceRotation + FinalRotation;

    //@RInterpTo
    FRotator SpringArmRotator = UKismetMathLibrary::RInterpTo(FinalRotation, CameraFinalRotation, DeltaTime, InterpolationSpeed);

    // 스프링암 최종 변환 적용
    SpringArmComponentRef->SocketOffset.X = FMath::Lerp(0, -200, DistanceFromTargetEnemy / 70);
    SpringArmComponentRef->SetWorldRotation(CameraFinalRotation);
}

void ULockOnComponent::UpdateBillboardComponent(bool bVisible, bool bChangeTransformOnly)
{
    if (!LockOnBillboardComponent || !TargetEnemyRef.IsValid() || !PlayerCharacterRef.IsValid() || !FollowCameraComponentRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "빌보드 업데이트 실패: 필요한 컴포넌트가 유효하지 않음");
        return;
    }

    if (!bVisible)
    {
        LockOnBillboardComponent->SetVisibility(false);
        return;
    }

    // Scale
    LockOnBillboardComponent->SetRelativeScale3D(FVector(TextureScale));

    // 타겟의 월드 위치를 뷰포트 좌표로 변환
    APlayerController* PlayerController = PlayerCharacterRef->GetController<APlayerController>();
    if (!PlayerController) return;

    FVector2D ScreenPosition;
    FVector TargetLocation = TargetEnemyRef->GetActorLocation();

    if (PlayerController->ProjectWorldLocationToScreen(TargetLocation, ScreenPosition))
    {
        // 카메라에서 타겟 방향으로의 벡터 계산
        FVector CameraLocation = FollowCameraComponentRef->GetComponentLocation();
        FVector DirectionToTarget = (TargetLocation - CameraLocation).GetSafeNormal();

        // 타겟 위치에서 카메라 방향의 반대로 약간 앞으로 이동
        FVector AdjustedLocation = TargetLocation - DirectionToTarget * BillboardForwardOffset;

        LockOnBillboardComponent->SetWorldLocation(AdjustedLocation);
        LockOnBillboardComponent->SetWorldRotation(FollowCameraComponentRef->GetComponentRotation());
    }

    if (!bChangeTransformOnly)
    {
        LockOnBillboardComponent->SetVisibility(true);
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void ULockOnComponent::OnLockOnTargetChanged(const FGameplayTag& InputTag, const float Value)
{
    //@태그 체크
    if (InputTag != FGameplayTag::RequestGameplayTag(FName("Input.Native.ChangeLockOnTarget")))
    {
        return;
    }

    //@Lock On 상태 체크
    if (!bLockOn)
    {
        return;
    }

    //@타겟 체크
    if (!TargetEnemyRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "타겟 변경 실패: 현재 타겟이 유효하지 않음");
        return;
    }

    //@주변 적 체크
    if (NearByEnemies.Num() <= 1)
    {
        return;
    }

    //@현재 타겟의 인덱스 찾기
    const int32 CurrentIndex = NearByEnemies.Find(TargetEnemyRef.Get());
    if (CurrentIndex == INDEX_NONE)
    {
        UE_LOGFMT(LogLockOn, Warning, "타겟 변경 실패: 현재 타겟의 인덱스를 찾을 수 없음");
        return;
    }

    // 이전 타겟의 이벤트 바인딩 해제
    UnbindCurrentTargetStateEvents();

    //@새 인덱스 계산
    int32 NewIndex;
    if (Value < 0.0f)  //@휠 다운(음수) -> 오른쪽으로 이동
    {
        NewIndex = (CurrentIndex + 1) % NearByEnemies.Num();
    }
    else  //@휠 업(양수) -> 왼쪽으로 이동
    {
        NewIndex = (CurrentIndex - 1 + NearByEnemies.Num()) % NearByEnemies.Num();
    }

    //@새로운 타겟 설정
    AActor* NewTarget = NearByEnemies[NewIndex];
    if (!NewTarget)
    {
        UE_LOGFMT(LogLockOn, Warning, "타겟 변경 실패: 새로운 타겟이 유효하지 않음");
        return;
    }

    //@Billboard Component 업데이트
    if (!LockOnBillboardComponent)
    {
        UE_LOGFMT(LogLockOn, Warning, "타겟 변경 실패: 빌보드 컴포넌트가 유효하지 않음");
        return;
    }

    USceneComponent* NewTargetRoot = NewTarget->GetRootComponent();
    if (!NewTargetRoot)
    {
        UE_LOGFMT(LogLockOn, Warning, "타겟 변경 실패: 새 타겟의 루트 컴포넌트가 유효하지 않음");
        return;
    }

    UpdateBillboardComponent(true, true);

    TargetEnemyRef = NewTarget;

    // 새로운 타겟에 대한 이벤트 바인딩
    BindCurrentTargetStateEvents();

    UE_LOGFMT(LogLockOn, Log, "Lock On 타겟이 변경되었습니다: {0}", *NewTarget->GetName());
}

void ULockOnComponent::OnOwnerStateChanged(const FGameplayTag& StateTag)
{
    //@죽음 상태 태그 체크
    if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead")))
    {
        UE_LOGFMT(LogLockOn, Log, "오너 캐릭터 사망 감지");

        //@Lock On 상태가 아니면 처리하지 않음
        if (!bLockOn)
        {
            UE_LOGFMT(LogLockOn, Log, "Lock On 상태가 아니므로 처리를 종료합니다.");
            return;
        }

        if (!PlayerCharacterRef.IsValid())
        {
            return;
        }

        ACharacterBase* Character = Cast<ACharacterBase>(PlayerCharacterRef.Get());
        if (!Character)
        {
            return;
        }

        if (UBaseAbilitySystemComponent* OwnerASC = Cast<UBaseAbilitySystemComponent>(Character->GetAbilitySystemComponent()))
        {
            OwnerASC->CharacterStateEventOnGameplay.RemoveAll(this);
            UE_LOGFMT(LogLockOn, Log, "오너 캐릭터에 대한 상태 이벤트 바인딩 해제");
        }

        UE_LOGFMT(LogLockOn, Log, "Lock On 상태이므로 Lock On을 취소합니다.");
        CancelLockOn();
    }
}

void ULockOnComponent::OnTargetStateChanged(const FGameplayTag& StateTag)
{
    // 죽음 상태 태그 체크
    if (StateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead")))
    {
        UE_LOGFMT(LogLockOn, Log, "타겟 {0}이(가) 사망하여 Lock On을 취소합니다.", *TargetEnemyRef->GetName());
        CancelLockOn();
    }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion