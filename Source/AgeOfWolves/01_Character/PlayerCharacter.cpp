
#include "PlayerCharacter.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseCharacterMovementComponent.h"
#include "04_Component/InventoryComponent.h"
#include "04_Component/LockOnComponent.h" 
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"

#include "03_Player/BasePlayerController.h"
#include "04_Component/BaseInputComponent.h"
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

//@Defualt Setting
#pragma region Default Setting
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
		GetCharacterMovement()->bOrientRotationToMovement = true;

		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.f, 0.0f);
		GetCharacterMovement()->JumpZVelocity = 700.f;
		GetCharacterMovement()->AirControl = 0.35f;
		GetCharacterMovement()->MaxWalkSpeed = 200.f;

		GetCharacterMovement()->MaxAcceleration = 1024.f;
		GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
		GetCharacterMovement()->BrakingDecelerationWalking = 2048.f;
		GetCharacterMovement()->GroundFriction = 8.0f;

	}
	// @Camera
	{
		SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
		SpringArm->SetupAttachment(RootComponent);

		// 카메라 거리 조정 (세키로 스타일)
		SpringArm->TargetArmLength = 350.f;
		SpringArm->bUsePawnControlRotation = true;

		// 카메라 래그 설정
		SpringArm->bEnableCameraLag = true;
		SpringArm->bEnableCameraRotationLag = true;
		SpringArm->CameraLagSpeed = 10.f;
		SpringArm->CameraRotationLagSpeed = 8.f;
		SpringArm->CameraLagMaxDistance = 100.f;

		// 스프링암 위치와 회전 조정
		SpringArm->SetRelativeLocation(FVector(0.f, 0.f, 250.f));  // 높이 조정
		SpringArm->SetRelativeRotation(FRotator(-15.f, 0.f, 0.f)); // 아래를 내려다보는 각도

		// 소켓 오프셋 조정으로 카메라 위치 미세 조정
		SpringArm->SocketOffset = FVector(0.f, 0.f, 0.f);

		FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
		FollowCamera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
		FollowCamera->bUsePawnControlRotation = false;
	}
	// @Anim Instance
	{
		//static ConstructorHelpers::FClassFinder<UBaseAnimInstance> animInstance(TEXT("/Game/Blueprints/01_Character/01_AkaOni/AnimationBlueprints/00_UE4Mannequin/ABP_Temp_Base"));
		//if (animInstance.Class != NULL)
		//	GetMesh()->SetAnimInstanceClass(animInstance.Class);

		//AnimInstanceRef = nullptr;
	}
	//@TODO: 추후에 삭제 예정
	//@임시 무기
	{
		//WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
		//WeaponMesh->SetupAttachment(GetMesh(), "TempKatana");
		//WeaponMesh->SetVisibility(false);

		//ShealthedWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShealthedWeaponMesh"));
		//ShealthedWeaponMesh->SetupAttachment(GetMesh(), "TempShealth");
		//ShealthedWeaponMesh->SetVisibility(false);

		//FullWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FullWeaponMesh"));
		//FullWeaponMesh->SetupAttachment(GetMesh(), "TempShealth");
	}

	//@Team ID
	{
		TeamId = 1;
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
	if (!NewController)
	{
		UE_LOGFMT(LogPlayer, Error, "NewController가 유효하지 않습니다.");
		return;
	}

	Super::PossessedBy(NewController);

	//@Base Anim Instance
	UBaseAnimInstance* BaseAnimInstance = Cast<UBaseAnimInstance>(GetMesh()->GetAnimInstance());
	if (!BaseAnimInstance)
	{
		UE_LOGFMT(LogPlayer, Warning, "BaseAnimInstance가 유효하지 않습니다.");
		return;
	}

	AnimInstanceRef = BaseAnimInstance;
	LockOnComponent->LockOnStateChanged.AddUFunction(BaseAnimInstance, "OnLockOnStateChanged");

	//@LockOn Component <-> Input Component
	ABasePlayerController* PC = Cast<ABasePlayerController>(NewController);
	if (!PC)
	{
		UE_LOGFMT(LogPlayer, Warning, "BasePlayerController가 유효하지 않습니다.");
		return;
	}

	UBaseInputComponent* BaseInputComp = PC->GetBaseInputComponent();
	if (!BaseInputComp)
	{
		UE_LOGFMT(LogPlayer, Warning, "BaseInputComponent가 유효하지 않습니다.");
		return;
	}

	// Input Component의 NativeInputTagTriggeredWithValue 이벤트에 LockOn Component의 OnLockOnTargetChanged 바인딩
	BaseInputComp->NativeInputTagTriggeredWithValue.AddUObject(LockOnComponent, &ULockOnComponent::OnLockOnTargetChanged);

	//@ASC
	if (!AbilitySystemComponent.IsValid())
	{
		APlayerStateBase* PS = Cast<APlayerStateBase>(GetPlayerState());
		if (!PS)
		{
			UE_LOGFMT(LogCharacter, Error, "PlayerState가 유효하지 않습니다.");
			return;
		}

		UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
		if (!ASC)
		{
			UE_LOGFMT(LogCharacter, Error, "AbilitySystemComponent가 유효하지 않습니다.");
			return;
		}

		SetAbilitySystemComponent(ASC);
	}

	//@초기화 요청 이벤트
	RequestStartInitByPlayerCharacter.Broadcast(NewController);

	UE_LOGFMT(LogPlayer, Log, "캐릭터가 성공적으로 빙의되었습니다.");
}

void APlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion