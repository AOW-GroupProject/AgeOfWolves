#include "LockOnComponent.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "04_Component/BaseInputComponent.h"
#include "05_Animation/BaseAnimInstance.h"
#include "03_Player/BasePlayerController.h"
#include "04_Component/BaseCharacterMovementComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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

    UpdateControllerRotation(DeltaTime);
}

void ULockOnComponent::InitializeLockOnComp(const AController* Controller)
{
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
void ULockOnComponent::Input_LockOn()
{
    if (!PlayerCharacterRef.IsValid()) return;

    if (bLockOn)
    {
        CancelLockOn();
        UE_LOGFMT(LogLockOn, Log, "Lock On Canceled");
        return;
    }

    StartLockOn();
    UE_LOGFMT(LogLockOn, Log, "Lock On Started");
}

void ULockOnComponent::Input_ChangeLockOnTarget(const FInputActionValue& Value)
{
    if (NearByEnemies.Num() == 0) return;

    int32 TargetIndex = NearByEnemies.IndexOfByKey(TargetEnemyRef);
    FVector2D ValueVector = Value.Get<FVector2D>();

    if (ValueVector.X > 0)
    {
        TargetIndex = FMath::Clamp(TargetIndex + 1, 0, NearByEnemies.Num() - 1);
        TargetEnemyRef = NearByEnemies[TargetIndex];
        UE_LOGFMT(LogLockOn, Log, "Changed Target to next: {0}", *TargetEnemyRef->GetName());
    }
    else
    {
        TargetIndex = FMath::Clamp(TargetIndex - 1, 0, NearByEnemies.Num() - 1);
        TargetEnemyRef = NearByEnemies[TargetIndex];
        UE_LOGFMT(LogLockOn, Log, "Changed Target to previous: {0}", *TargetEnemyRef->GetName());
    }

}

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

    //@Spring Arm(Camera) 설정 업데이트
    UpdateSpringArmSettings(true);

    //@Lock On 상태
    bLockOn = true;

    //@Lock On 상태 이벤트
    LockOnStateChanged.Broadcast(bLockOn);
}

void ULockOnComponent::CancelLockOn()
{
    //@Ref
    if (!PlayerCharacterRef.IsValid() || !BaseAnimInstanceRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "락온 해제 실패: 플레이어 캐릭터가 유효하지 않음");
        return;
    }
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
    float Min = MAX_FLT;

    for (const auto& Hit : HitResults)
    {
        FHitResult LineHitResults;
        bool LineTraceHitResult = UKismetSystemLibrary::LineTraceSingleForObjects(
            GetWorld(),
            PlayerCharacterRef->GetActorLocation(),
            Hit.GetActor()->GetActorLocation(),
            NearByActors,
            false,
            IgnoreActors,
            EDrawDebugTrace::None,
            LineHitResults,
            true
        );

        if (Hit.GetActor() == LineHitResults.GetActor())
        {
            NearByEnemies.AddUnique(Hit.GetActor());

            FVector PlayerCameraLocation = FollowCameraComponentRef->GetComponentTransform().GetTranslation();
            FVector CameraToPlayer = FollowCameraComponentRef->GetForwardVector();
            FVector CameraToEnemy = Hit.GetActor()->GetActorLocation() - PlayerCameraLocation;

            FVector CrossProduct = FVector::CrossProduct(CameraToPlayer, CameraToEnemy);
            float UpDotProduct = FVector::DotProduct(CameraToPlayer, CrossProduct);
            float TempDotProductResult = FVector::DotProduct(CameraToPlayer, CameraToEnemy);

            float Cos = TempDotProductResult / (CameraToPlayer.Length() * CameraToEnemy.Length());
            float HalfFOV = FMath::Cos(FMath::DegreesToRadians(FollowCameraComponentRef->FieldOfView / 1.5));

            if (Cos > HalfFOV)
            {
                EnemyMap.Add(UpDotProduct, Hit.GetActor());
                if (FMath::Abs(Min) > FMath::Abs(UpDotProduct))
                {
                    Min = UpDotProduct;
                }
            }
        }
    }

    if (EnemyMap.IsEmpty())
    {
        UE_LOGFMT(LogLockOn, Log, "No valid targets in FOV");
        return false;
    }

    TArray<float> DotProducts;
    EnemyMap.GenerateKeyArray(DotProducts);
    DotProducts.Sort();
    NearByEnemies.Empty();

    for (float DotProduct : DotProducts)
    {
        NearByEnemies.Add(*EnemyMap.Find(DotProduct));
    }

    TargetEnemyRef = *EnemyMap.Find(Min);
    UE_LOGFMT(LogLockOn, Log, "Found target: {0}", *TargetEnemyRef->GetName());

    return IsValid(TargetEnemyRef.Get());
}

void ULockOnComponent::UpdateSpringArmSettings(bool bIsLockingOn)
{
    if (!SpringArmComponentRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "스프링암 설정 업데이트 실패: 스프링암이 유효하지 않음");
        return;
    }

    SpringArmComponentRef->bUsePawnControlRotation = !bIsLockingOn;
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
    FRotator FinalRotation = UKismetMathLibrary::RInterpTo(StartRotation, TargetRotation, DeltaTime, InterpolationSpeed);

    //@RInterpTo
    PlayerCharacterRef->GetController()->SetControlRotation(FRotator(0.f, FinalRotation.Yaw, 0.f));

    //@Update Spring Arm
    UpdateSpringArmTransform(DeltaTime, Target, FinalRotation);
}

void ULockOnComponent::UpdateSpringArmTransform(float DeltaTime, const FVector& Target, const FRotator& FinalRotation)
{
    //@Spring Arm Comp, Base Anim Intance, Base Input Comp
    if (!SpringArmComponentRef.IsValid() || !BaseAnimInstanceRef.IsValid() || !BaseInputComponentRef.IsValid())
    {
        UE_LOGFMT(LogLockOn, Warning, "스프링암 변환 실패: 필요한 컴포넌트가 유효하지 않음");
        return;
    }

    //@Offset Coefficient
    float SocketOffsetCoefficient = (BaseAnimInstanceRef->GetMovementState() != EMovementState::Sprinting) ? 1.5f : 1.0f;

    //@Input Vector
    if (BaseInputComponentRef->GetInputVector().Y > 0)
    {
        if (SpringArmComponentRef->SocketOffset.Y > -50)
        {
            SpringArmComponentRef->SocketOffset.Y -= (SocketOffsetCoefficient * BaseInputComponentRef->GetInputVector().Y);
        }
    }
    else if (BaseInputComponentRef->GetInputVector().Y < 0)
    {
        if (SpringArmComponentRef->SocketOffset.Y < 50)
        {
            SpringArmComponentRef->SocketOffset.Y -= (SocketOffsetCoefficient * BaseInputComponentRef->GetInputVector().Y);
        }
    }

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
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion