
// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseCharacterMovementComponent.h"
#include "04_Component/InventoryComponent.h"
#include "04_Component/LockOnComponent.h" 
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"

#include "03_Player/BasePlayerController.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "05_Animation/BaseAnimInstance.h"
#include "00_GameInstance/AOWGameInstance.h"
#include "02_AbilitySystem/AOWGameplayTags.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogPlayer)
// UE_LOGFMT(LogPlayer, Log, "");

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName)
	)
{

	PrimaryActorTick.bCanEverTick = true;
	// @Input Component 
	{
		InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));
		LockOnComponent = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOn Component"));
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
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.0f); // ...at this rotation rate
		GetCharacterMovement()->JumpZVelocity = 700.f;
		GetCharacterMovement()->AirControl = 0.35f;
		GetCharacterMovement()->MaxWalkSpeed = 200.f;
		// @FIX: 150 -> 500 상향
		GetCharacterMovement()->MaxAcceleration = 800.f;
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

		SpringArm->CameraLagSpeed = 10.f;
		SpringArm->CameraRotationLagSpeed = 8.f;
		SpringArm->CameraLagMaxDistance = 100.f;

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

	if (InventoryComponent)
	{
		RequestStartInitByPlayerCharacter.AddUFunction(InventoryComponent, "InitializeInventory");
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	SetAbilitySystemComponent(Cast<APlayerStateBase>(GetPlayerState())->GetAbilitySystemComponent());
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}


void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	check(NewController);

	Super::PossessedBy(NewController);

	//@초기화 알림 이벤트
	RequestStartInitByPlayerCharacter.Broadcast(NewController);

}

void APlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

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

void APlayerCharacter::Die()
{
	
}

void APlayerCharacter::HitReact(FGameplayTag HitDirectionTag)
{
	FGameplayEventData GameplayEventData;
	GameplayEventData.EventTag = AOWGameplayTags::Ability_Active_HitReact;
	GameplayEventData.Instigator = this;
	GameplayEventData.Target = this; 
	GameplayEventData.OptionalObject = nullptr; 
	GameplayEventData.ContextHandle = FGameplayEffectContextHandle(); 
	GameplayEventData.TargetTags.AddTag(HitDirectionTag); // HitDirectionTag 전달
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, AOWGameplayTags::Ability_Active_HitReact, GameplayEventData);
}
