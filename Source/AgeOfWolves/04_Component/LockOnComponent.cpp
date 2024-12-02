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
}

void ULockOnComponent::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacterRef = Cast<APlayerCharacter>(GetOwner());
    if (!PlayerCharacterRef.IsValid()) return;

    SpringArmComponentRef = PlayerCharacterRef->GetSpringArmComponent();
    if (!SpringArmComponentRef.IsValid()) return;

    FollowCameraComponentRef = PlayerCharacterRef->GetCameraComponent();
    if (!FollowCameraComponentRef.IsValid()) return;

    BaseAnimInstanceRef = Cast<UBaseAnimInstance>(PlayerCharacterRef->GetMesh()->GetAnimInstance());
    if (!BaseAnimInstanceRef.IsValid()) return;

    ABasePlayerController* PlayerController = Cast<ABasePlayerController>(PlayerCharacterRef->GetController());
    if (!PlayerController) return;

    BaseInputComponentRef = PlayerController->GetBaseInputComponent();
    if (!BaseInputComponentRef.IsValid()) return;
}

void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    AdjustCameraTransform(DeltaTime);
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

void ULockOnComponent::StartLockOn()
{
    if (!PlayerCharacterRef.IsValid()) return;
    if (!FindTargetEnemy()) return;

    PlayerCharacterRef->GetCharacterMovement()->bOrientRotationToMovement = false;
    PlayerCharacterRef->GetCharacterMovement()->bUseControllerDesiredRotation = true;

    if (SpringArmComponentRef.IsValid())
    {
        SpringArmComponentRef->bUsePawnControlRotation = false;
        SpringArmComponentRef->CameraLagSpeed = 5;
        SpringArmComponentRef->CameraRotationLagSpeed = 17.5;
    }

    bLockOn = true;
    if (BaseAnimInstanceRef.IsValid())
    {
        BaseAnimInstanceRef->SetbLockOn(true);
    }

    SetControllerRotationTowardTarget();
}

void ULockOnComponent::CancelLockOn()
{
    if (!PlayerCharacterRef.IsValid()) return;

    PlayerCharacterRef->GetCharacterMovement()->bOrientRotationToMovement = true;
    PlayerCharacterRef->GetCharacterMovement()->bUseControllerDesiredRotation = false;

    if (SpringArmComponentRef.IsValid())
    {
        SpringArmComponentRef->bUsePawnControlRotation = true;
        SpringArmComponentRef->CameraLagSpeed = 10;
        SpringArmComponentRef->CameraRotationLagSpeed = 30;
    }

    if (BaseAnimInstanceRef.IsValid())
    {
        BaseAnimInstanceRef->SetbLockOn(false);
    }

    InputVector = FVector2D::ZeroVector;
    NearByEnemies.Empty();
    EnemyMap.Empty();
    TargetEnemy = nullptr;
    bLockOn = false;
}

void ULockOnComponent::Input_ChangeLockOnTarget(const FInputActionValue& Value)
{
    if (NearByEnemies.Num() == 0) return;

    int32 TargetIndex = NearByEnemies.IndexOfByKey(TargetEnemy);
    FVector2D ValueVector = Value.Get<FVector2D>();

    if (ValueVector.X > 0)
    {
        TargetIndex = FMath::Clamp(TargetIndex + 1, 0, NearByEnemies.Num() - 1);
        TargetEnemy = NearByEnemies[TargetIndex];
        UE_LOGFMT(LogLockOn, Log, "Changed Target to next: {0}", *TargetEnemy->GetName());
    }
    else
    {
        TargetIndex = FMath::Clamp(TargetIndex - 1, 0, NearByEnemies.Num() - 1);
        TargetEnemy = NearByEnemies[TargetIndex];
        UE_LOGFMT(LogLockOn, Log, "Changed Target to previous: {0}", *TargetEnemy->GetName());
    }

    SetControllerRotationTowardTarget();
}

void ULockOnComponent::SetControllerRotationTowardTarget()
{
    if (!PlayerCharacterRef.IsValid() || !TargetEnemy) return;

    FVector Start = PlayerCharacterRef->GetActorLocation();
    FVector Target = TargetEnemy->GetActorLocation();
    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
    FRotator CurrentRotation = PlayerCharacterRef->GetControlRotation();

    FRotator FinalRotation = UKismetMathLibrary::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 10.f);
    PlayerCharacterRef->GetController()->SetControlRotation(FRotator(0.f, FinalRotation.Yaw, 0.f));

    UE_LOGFMT(LogLockOn, Verbose, "Set Controller Rotation - Target: {0}, Final Yaw: {1}", *TargetEnemy->GetName(), FinalRotation.Yaw);
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

    TargetEnemy = *EnemyMap.Find(Min);
    UE_LOGFMT(LogLockOn, Log, "Found target: {0}", *TargetEnemy->GetName());

    return IsValid(TargetEnemy);
}

void ULockOnComponent::AdjustCameraTransform(float DeltaTime)
{
    if (!bLockOn || !IsValid(TargetEnemy) || !PlayerCharacterRef.IsValid() || !SpringArmComponentRef.IsValid())
    {
        CancelLockOn();
        return;
    }

    bool bCloseToEnemy = (PlayerCharacterRef->GetActorLocation() - TargetEnemy->GetActorLocation()).Length() < MaxLockOnDistance;
    if (!bCloseToEnemy)
    {
        UE_LOGFMT(LogLockOn, Log, "Target out of range, canceling lock on");
        CancelLockOn();
        return;
    }

    SpringArmComponentRef->bUsePawnControlRotation = true;
    SpringArmComponentRef->bEnableCameraRotationLag = true;
    SpringArmComponentRef->CameraRotationLagSpeed = 17.5f;

    FVector Start = PlayerCharacterRef->GetActorLocation();
    FVector Target = TargetEnemy->GetActorLocation();
    FRotator StartRotation = PlayerCharacterRef->GetControlRotation();
    FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);

    FRotator FinalRotation = UKismetMathLibrary::RInterpTo(StartRotation, TargetRotation, DeltaTime, 20.f);
    PlayerCharacterRef->GetController()->SetControlRotation(FRotator(0.f, FinalRotation.Yaw, 0.f));

    if (!BaseAnimInstanceRef.IsValid() || !BaseInputComponentRef.IsValid()) return;

    float SocketOffsetCoefficient = (BaseAnimInstanceRef->GetMovementState() != EMovementState::Sprinting) ? 1.5f : 1.0f;

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

    float DistanceFromTargetEnemy = (PlayerCharacterRef->GetActorLocation() - Target).Length();
    DistanceFromTargetEnemy = FMath::Clamp((6000 / DistanceFromTargetEnemy) + 20, 0, 30);
    FRotator DistanceRotation = FRotator(-DistanceFromTargetEnemy, 0, 0);

    FRotator CameraFinalRotation = DistanceRotation + FinalRotation;
    FRotator SpringArmRotator = UKismetMathLibrary::RInterpTo(FinalRotation, CameraFinalRotation, DeltaTime, 5.f);

    SpringArmComponentRef->SocketOffset.X = FMath::Lerp(0, -200, DistanceFromTargetEnemy / 70);
    SpringArmComponentRef->SetWorldRotation(CameraFinalRotation);
}
#pragma endregion