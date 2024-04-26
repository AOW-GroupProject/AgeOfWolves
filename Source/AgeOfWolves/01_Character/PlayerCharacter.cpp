
// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseInputComponent.h"
#include "04_Component/BaseCharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"

#include "03_Player/BasePlayerController.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "Kismet/KismetMathLibrary.h"

// @설명 : UI 구현을 위한 테스트 코드, 실제 UI구현시 아래 헤더 파일 삭제 후 진행
#include "08_UI/TestWidget.h"

DEFINE_LOG_CATEGORY(LogPlayer)
// UE_LOGFMT(LogPlayer, LoG, "");

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)
	)
{
	PrimaryActorTick.bCanEverTick = true;

	// Replace Input Component to User defined Base Input Component
	InputComponent = CreateDefaultSubobject<UBaseInputComponent>(TEXT("Input Component"));

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetMesh()->SetCollisionProfileName("PlayerMesh");

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 300.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 230.f;
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

	// [24-03-20] TEST : 임시 Anim Instance 적용
	static ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(TEXT("AnimBlueprint'/Game/Blueprints/01_Character/01_AkaOni/AnimationBlueprints/ABP_AkaOni_Base'"));
	if (animInstance.Class != NULL)
		GetMesh()->SetAnimInstanceClass(animInstance.Class);

	// @목적: UI관련 테스트 진행 코드, 실제 UI 구현 시 아래 코드 삭제후 진행
	WidgetClass = nullptr;
	HUD = nullptr;

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
	AdjustControllerRotation(DeltaSeconds);
}

void APlayerCharacter::PossessedBy(AController* NewController)
{

	check(NewController);

	Super::PossessedBy(NewController);

	// @설명 : Player State로부터 ASC에 대한 참조를 가져옵니다.
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

	// @설명 : UI 관련 테스트 코드, 임시 코드, 삭제 예정
	if (IsLocallyControlled() && WidgetClass->IsValidLowLevel())
	{
		if (ABasePlayerController* PC = Cast<ABasePlayerController>(GetController()))
		{
			HUD = CreateWidget<UTestWidget>(PC, WidgetClass);
			if (IsValid(HUD))
			{
				HUD->AddToViewport();
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
	
	// 캐릭터의 현재 가속도 벡터를 기반으로 한 로테이션을 계산
	FRotator Rotation1 = UKismetMathLibrary::MakeRotFromX(GetCharacterMovement()->GetCurrentAcceleration());

	// 캐릭터가 바라보는 방향의 벡터를 기반으로 로테이션을 계산
	FRotator Rotation2 = GetControlRotation();

	// 두 방향 사이의 최소 각도 차이를 계산
	float OffsetAngle = DirectionCurve->GetFloatValue(FMath::FindDeltaAngleDegrees(Rotation1.Yaw, Rotation2.Yaw));

	FRotator Rotaion3 = UKismetMathLibrary::MakeRotator(0.f, 0.f, GetControlRotation().Yaw + OffsetAngle);

	// 선형 보간
	FRotator TargetRotation = UKismetMathLibrary::RInterpTo(GetActorRotation(), Rotaion3, DeltaSeconds, 10.f);

	// 결과 값을 액터 객체의 회전 값으로 설정
	SetActorRotation(TargetRotation);

}
