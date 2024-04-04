// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Logging/StructuredLog.h"
#include "Kismet/KismetMathLibrary.h"

#include "04_Component/BaseInputComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "04_Component/BaseCharacterMovementComponent.h"

#include "03_Player/BasePlayerController.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"



DEFINE_LOG_CATEGORY(LogPlayer)

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetMesh()->SetCollisionProfileName("PlayerMesh");

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Replace Input Component to User defined Base Input Component 
	InputComponent = CreateDefaultSubobject<UBaseInputComponent>(TEXT("Input Component"));

	// Configure character movement
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MaxAcceleration = 150.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	SpringArm->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 30.f;
	SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// [24-03-20] TEST : �ӽ� Anim Instance ����
	static ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(TEXT("AnimBlueprint'/Game/Blueprints/01_Character/01_AkaOni/New/AnimationBlueprints/ABP_AkaOni_Base'"));
	if (animInstance.Class != NULL)
		GetMesh()->SetAnimInstanceClass(animInstance.Class);

}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetCharacterMovement()->GetCurrentAcceleration().Length() > 0.f)
	{
		AdjustControllerRotation(DeltaSeconds);
	}

}

void APlayerCharacter::PossessedBy(AController* NewController)
{

	check(NewController);

	Super::PossessedBy(NewController);

	// @TODO : Player State�κ��� ASC�� ���� ������ �����ɴϴ�.
	if (const auto& PlayerController = CastChecked<ABasePlayerController>(NewController))
	{
		if (const auto& PS = PlayerController->GetPlayerState<APlayerStateBase>())
		{
			if (IsValid(PS->GetAbilitySystemComponent()) && PS->GetAbilitySystemComponent()->IsA<UBaseAbilitySystemComponent>())
			{
				AbilitySystemComponent = MakeWeakObjectPtr<UBaseAbilitySystemComponent>(Cast<UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent()));
			}
		}
	}

}

void APlayerCharacter::PawnClientRestart()
{

	Super::PawnClientRestart();

	// @TODO : Event Client Restart

}

void APlayerCharacter::AdjustControllerRotation(float DeltaSeconds)
{

	check(DirectionCurve);

	// ĳ������ ���� ���ӵ� ���͸� ������� �� �����̼��� ���
	FRotator Rotation1 = UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->GetCurrentAcceleration());

	// ĳ���Ͱ� �ٶ󺸴� ������ ���͸� ������� �����̼��� ���
	FRotator Rotation2 = GetControlRotation();

	// �� ���� ������ �ּ� ���� ���̸� ���
	float OffsetAngle = DirectionCurve->GetFloatValue(FMath::FindDeltaAngleDegrees(Rotation1.Yaw, Rotation2.Yaw));

	FRotator Rotaion3 = UKismetMathLibrary::MakeRotator(0.f, 0.f, GetControlRotation().Yaw + OffsetAngle);

	// ���� ����
	FRotator TargetRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), Rotaion3, DeltaSeconds, 10.f);

	SetActorRotation(TargetRotation);

}
