
// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseInputComponent.h"
#include "04_Component/BaseCharacterMovementComponent.h"
#include "04_Component/InventoryComponent.h"
#include "04_Component/UIComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"

#include "03_Player/BasePlayerController.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "05_Animation/BaseAnimInstance.h"
#include "00_GameInstance/AOWGameInstance.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogPlayer)
// UE_LOGFMT(LogPlayer, Log, "");

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)
	)
{
	// @Component 
	{
		InputComponent = CreateDefaultSubobject<UBaseInputComponent>(TEXT("Input Component"));
		InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
		UIComponent = CreateDefaultSubobject<UUIComponent>(TEXT("UI Component"));
	}
	// @Capsule
	{
		GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		GetMesh()->SetCollisionProfileName("PlayerMesh");
	}
	// @Rotation
	{
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
	}
	// @Character Movement
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
		// @FIX: 300 -> 800 상향
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 800.f, 0.0f); // ...at this rotation rate
		GetCharacterMovement()->JumpZVelocity = 700.f;
		GetCharacterMovement()->AirControl = 0.35f;
		GetCharacterMovement()->MaxWalkSpeed = 400.f;
		// @FIX: 150 -> 500 상향
		GetCharacterMovement()->MaxAcceleration = 500.f;
		GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
		GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
		GetCharacterMovement()->GroundFriction = 8.0f;
	}
	// @Camera
	{
		SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
		SpringArm->SetupAttachment(RootComponent);
		SpringArm->TargetArmLength = 400.0f; 
		SpringArm->bUsePawnControlRotation = true; 
		SpringArm->bEnableCameraLag = true;
		SpringArm->bEnableCameraRotationLag = true;
		SpringArm->CameraRotationLagSpeed = 30.f;
		SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 10.f));

		FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
		FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
		FollowCamera->bUsePawnControlRotation = false;
	}
	// @Anim Instance
	{
		static ConstructorHelpers::FClassFinder<UAnimInstance> animInstance(TEXT("AnimBlueprint'/Game/Blueprints/01_Character/01_AkaOni/AnimationBlueprints/ABP_AkaOni_Base'"));
		if (animInstance.Class != NULL)
			GetMesh()->SetAnimInstanceClass(animInstance.Class);
	}

}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//@의존 관계는 '->' 통해 명시하며, 좌항의 Component가 우항의 Component에 의존적이며 좌항의 Comp는 우항 Comp에서 제공하는 Delegate에 Callback 함수 등록
	//@UI -> Inventory

	//@TODO: Inventory의 아이템 추가/제거 작업 이벤트에 콜백 함수 바인딩
	if(UIComponent)
	{
		//@->InventoryComp
		if(InventoryComponent)
		{
			//@TODO: Binding Callbacks to Inven Comp Delegates
		}
		//@Load
		UIComponent->LoadUI();
	}

	//@Inventory
	if (InventoryComponent)
	{

		//@Load
		InventoryComponent->LoadInventory();
	}

}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//@TODO: Refactoring
	BaseInputComponent = Cast<UBaseInputComponent>(InputComponent);
	check(BaseInputComponent);
	BaseAnimInstance = Cast<UBaseAnimInstance>(GetMesh()->GetAnimInstance());
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	AdjustCameraTransform(DeltaSeconds);
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	check(NewController);

	Super::PossessedBy(NewController);

	// @Player State Base 초기화
	if (const auto& PlayerController = CastChecked<ABasePlayerController>(NewController))
	{
		if (const auto& PS = PlayerController->GetPlayerState<APlayerStateBase>())
		{
			if (IsValid(PS->GetAbilitySystemComponent()) && PS->GetAbilitySystemComponent()->IsA<UBaseAbilitySystemComponent>())
			{
				PS->InitializePlayerSystem();

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
	if (BaseInputComponent->GetbLockOn() == false) return;

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

void APlayerCharacter::AdjustCameraTransform(float DeltaSeconds)
{
	
	if (BaseInputComponent->GetbLockOn() == true)
	{
		SpringArm->bUsePawnControlRotation = false;
		AActor* TargetEnemy = BaseInputComponent->GetTargetEnemy();
		// MaxLockOnDistance 보다 가까우면 true
		bool bCloseToEnemy = (GetActorLocation() - TargetEnemy->GetActorLocation()).Length() < BaseInputComponent->GetMaxLockOnDistance();
		// 거리가 가깝거나 TargetEnemy가 존재하는 경우 true
		if (IsValid(TargetEnemy) && bCloseToEnemy)
		{
			APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
			FVector CameraStart = CameraManager->GetCameraLocation();
			FVector CharacterStart = GetActorLocation();
			FVector TargetPosition = TargetEnemy->GetActorLocation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CharacterStart, TargetPosition);
				
			float SocketOffsetCoefficient = 1.0f;

			GetController()->SetControlRotation(LookAtRotation);
			// 달리지 않는 동안에만 ActorRotation을 LockOn방향으로 설정
			if (!(BaseAnimInstance->GetMovementState() == EMovementState::Run))
			{
				FRotator ActorRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
				SetActorRotation(ActorRotation);
				SocketOffsetCoefficient = 1.5f;
			}
			// 오른쪽, 왼쪽으로 이동하는 경우 스프링암의 Y오프셋을 조절
			if (BaseInputComponent->GetInputVector().Y > 0) // 오른쪽으로 이동
			{
				if (SpringArm->SocketOffset.Y > -50)
				{
					SpringArm->SocketOffset.Y -= (SocketOffsetCoefficient * BaseInputComponent->GetInputVector().Y);
				}
			}
			else if (BaseInputComponent->GetInputVector().Y < 0) // 왼쪽으로 이동
			{
				if (SpringArm->SocketOffset.Y < 50)
				{
					SpringArm->SocketOffset.Y -= (SocketOffsetCoefficient * BaseInputComponent->GetInputVector().Y);
				}
			}
			// TargetEnemy와 거리에 따라 카메라를 위로 이동 시킴
			float DistanceFromTargetEnemy = (GetActorLocation() - TargetPosition).Length();
			DistanceFromTargetEnemy = FMath::Clamp((6000 / DistanceFromTargetEnemy) + 20, 0, 70);
			FRotator DistanceRotation = FRotator(-DistanceFromTargetEnemy, 0, 0);
	
			FRotator FinalRotation = DistanceRotation + LookAtRotation;
			FRotator SpringArmRotator = UKismetMathLibrary::RInterpTo(LookAtRotation, FinalRotation, DeltaSeconds, 10.f);

			SpringArm->SocketOffset.X = FMath::Lerp(0, -200, DistanceFromTargetEnemy / 70);
			SpringArm->SetWorldRotation(FinalRotation);
		}
		else if (!bCloseToEnemy)// TargetEnemy와 너무 멀어진경우 LockOn을 취소한다.
		{
			TargetEnemy = nullptr;
			SpringArm->bUsePawnControlRotation = true;
			SpringArm->SocketOffset.Y = 0;
			BaseInputComponent->CancelLockOn();
		}
		else if (!IsValid(TargetEnemy)) // Target이 죽어 유효하지 않는 경우 LockOn을 취소하고, 다음 Target을 찾는다.
		{
			TargetEnemy = nullptr;
			SpringArm->bUsePawnControlRotation = true;
			SpringArm->SocketOffset.Y = 0;
			BaseInputComponent->CancelLockOn();
			BaseInputComponent->StartLockOn();
		}
	}
	else
	{
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->SocketOffset.Y = 0;
		BaseInputComponent->CancelLockOn();
	}
	
}
