
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

		GetCharacterMovement()->MaxAcceleration = 600.f;
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

		AnimInstanceRef = nullptr;
	}

}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//@비동기 초기화
	if (!InventoryComponent || !LockOnComponent)
	{
		return;
	}

	RequestStartInitByPlayerCharacter.AddUFunction(InventoryComponent, "InitializeInventory");
	RequestStartInitByPlayerCharacter.AddUFunction(LockOnComponent, "InitializeLockOnComp");
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
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	check(NewController);
	Super::PossessedBy(NewController);

	RequestStartInitByPlayerCharacter.Broadcast(NewController);

	if (UBaseAnimInstance* BaseAnimInstance = Cast<UBaseAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		LockOnComponent->LockOnStateChanged.AddUFunction(BaseAnimInstance, "OnLockOnStateChanged");
		AnimInstanceRef = BaseAnimInstance;
	}

	SetAbilitySystemComponent(Cast<APlayerStateBase>(GetPlayerState())->GetAbilitySystemComponent());
}

void APlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

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
