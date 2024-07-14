// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInputComponent.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/Character.h"

#include "03_Player/PlayerStateBase.h"
#include "01_Character/PawnData.h"
#include "EnhancedInputSubsystems.h"
#include "06_Input/InputConfig.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "04_Component/CombatComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "01_Character/PlayerCharacter.h"
#include "05_Animation/BaseAnimInstance.h"

#include "GameplayTagContainer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseInputComponent)

DEFINE_LOG_CATEGORY(LogInputComponent)
// UE_LOGFMT(LogInputComponent, Log, "");

UBaseInputComponent::UBaseInputComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UBaseInputComponent::OnRegister()
{
	Super::OnRegister();

}

void UBaseInputComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializePlayersInputActionsSetup();
}

void UBaseInputComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UBaseInputComponent::InitializePlayersInputActionsSetup()
{
	const APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		return;
	}

	const APlayerController* PC = Pawn->GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = PC->GetLocalPlayer();
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	if (const APlayerStateBase* PS = PC->GetPlayerState<APlayerStateBase>())
	{
		if (const UPawnData* PawnData = PS->GetPawnData())
		{
			if (const UInputConfig* InputConfig = PawnData->InputConfig)
			{
				// #1. IMC to Subsystem
				AddInputMappings(InputConfig, Subsystem);
			

				// #2. Native Input Actions
				BindNativeInputAction(InputConfig, FGameplayTag::RequestGameplayTag(FName("Input.Native.Move")), ETriggerEvent::Triggered, this, &UBaseInputComponent::Input_Move);
				BindNativeInputAction(InputConfig, FGameplayTag::RequestGameplayTag(FName("Input.Native.Looking")), ETriggerEvent::Triggered, this, &UBaseInputComponent::Input_Look);
				BindNativeInputAction(InputConfig, FGameplayTag::RequestGameplayTag(FName("Input.Native.LockOn")), ETriggerEvent::Triggered, this, &UBaseInputComponent::Input_LockOn);
				BindNativeInputAction(InputConfig, FGameplayTag::RequestGameplayTag(FName("Input.Native.ChangeLockOnTarget")), ETriggerEvent::Triggered, this, &UBaseInputComponent::Input_ChangeLockOnTarget);
				// BindNativeInputAction(InputConfig, FGameplayTag::RequestGameplayTag(FName("Input.Native.CountMouseInput")), ETriggerEvent::Triggered, this, &UBaseInputComponent::Input_CountMouseLeftInput);

				// #3. Ability Input Actions
				TArray<uint32> BindHandles;
				BindAbilityInputActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

				// #4. Delegate Executed
				OnPlayerInputInitFinished.ExecuteIfBound();
			}
		}
	}

}

void UBaseInputComponent::AddInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	ULocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<ULocalPlayer>();
	check(LocalPlayer);

	InputSubsystem->AddMappingContext(InputConfig->InputMappingContext, InputConfig->MappingPriority);

}

void UBaseInputComponent::RemoveInputMappings(const UInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	ULocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<ULocalPlayer>();
	check(LocalPlayer);

	InputSubsystem->RemoveMappingContext(InputConfig->InputMappingContext);

}

void UBaseInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}

void UBaseInputComponent::Input_Move(const FInputActionValue& Value)
{
	// #1. Pawn
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{	
		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();
		InputVector = MovementVector;
		// #2. Controller
		if (AController* Controller = Pawn->GetController())
		{
			if (Controller != nullptr)
			{
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

				// Forward, Backward
				const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				Pawn->AddMovementInput(ForwardDirection, MovementVector.X);

				// Right, Left
				const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				Pawn->AddMovementInput(RightDirection, MovementVector.Y);
			}
		}
	}
}

void UBaseInputComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	if (APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (bLockOn) return;
		const FVector2D Value = InputActionValue.Get<FVector2D>();

		if (Value.X != 0.0f)
		{
			Pawn->AddControllerYawInput(Value.X);
		}

		if (Value.Y != 0.0f)
		{
			Pawn->AddControllerPitchInput(Value.Y);
		}
	}

}



void UBaseInputComponent::Input_LockOn(const FInputActionValue& Value)
{
	if (bLockOn == true) // LockOn 중일 때, 마우스 휠 입력시 LockOn 종료
	{
		CancelLockOn();
	}
	else // LockOn 중이 아닐 때, 마우스 휠 입력시 LockOn 시작
	{
		StartLockOn();
	}
}

void UBaseInputComponent::StartLockOn()
{

	UBaseAnimInstance* BaseAnimInstance = Cast<UBaseAnimInstance>(Cast<APlayerCharacter>(GetOwner())->GetMesh()->GetAnimInstance());
	check(BaseAnimInstance)
	// TargetEnemy를 찾고, 찾았다면 LockOn에 쓰이는 변수들을 설정함.
	if (FindTargetEnemy() == true)
	{
		bLockOn = true;
		USpringArmComponent* SpringArm = GetOwner()->FindComponentByClass<USpringArmComponent>();
		// To do : 기존 값을 불러오게
		SpringArm->CameraLagSpeed = 5;
		SpringArm->CameraRotationLagSpeed = 17.5;
		BaseAnimInstance->SetbLockOn(true);
		SetControllerRotationTowardTarget();
	}

}

void UBaseInputComponent::CancelLockOn()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	check(PlayerCharacter);
	UBaseAnimInstance* BaseAnimInstance = Cast<UBaseAnimInstance>(Cast<APlayerCharacter>(GetOwner())->GetMesh()->GetAnimInstance());
	check(BaseAnimInstance)

	// SpringArmComponent 멤버 변수 초기화
	USpringArmComponent* SpringArm = GetOwner()->FindComponentByClass<USpringArmComponent>();
	// To do : 기존 값을 불러오게
	SpringArm->CameraLagSpeed = 10;
	SpringArm->CameraRotationLagSpeed = 30;
	// BaseAnimInstance  멤버 변수 초기화
	BaseAnimInstance->SetbLockOn(false);

	// BaseInputComponent 멤버 변수 초기화
	InputVector = FVector2D(0, 0);
	NearByEnemies.Empty();
	EnemyMap.Empty();
	TargetEnemy = nullptr;
	bLockOn = false;
}

void UBaseInputComponent::SetControllerRotationTowardTarget()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	check(PlayerCharacter);
	FVector Start = PlayerCharacter->GetActorLocation();
	FVector Target = TargetEnemy->GetActorLocation();
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	//PlayerCharacter->SetActorRotation(FRotator(0.f, Rotation.Yaw, 0.f));
	PlayerCharacter->GetController()->SetControlRotation(FRotator(0.f, Rotation.Yaw, 0.f));
	DrawDebugSphere(GetWorld(), TargetEnemy->GetActorLocation(), 30, 12, FColor::Red, false, 1.5f);
	
}

bool UBaseInputComponent::FindTargetEnemy()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	check(PlayerCharacter);

	UCameraComponent* FollowCamera = PlayerCharacter->GetCameraComponent();

	TArray<TEnumAsByte<EObjectTypeQuery>> NearByActors;
	TEnumAsByte<EObjectTypeQuery> PawnObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	NearByActors.Add(PawnObjectType);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerCharacter);

	// PlayerCharacter를 무시한 MaxDetectRadius을 반지름으로 하는 원형 트레이스를 실행함.
	TArray<FHitResult> HitResults;
	bool SphereTraceHitResult = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		PlayerCharacter->GetActorLocation(),
		PlayerCharacter->GetActorLocation() + 100.f,
		MaxDetectRadius,
		NearByActors,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		HitResults, true
	);
	if (SphereTraceHitResult == true)
	{
		// LineTraceSingle을 통해, LockOn이 가능한지 판단
		FHitResult LineHitResults;
		for (const auto& Hit : HitResults)
		{
			bool LineTraceHitResult = UKismetSystemLibrary::LineTraceSingleForObjects(
				GetWorld(),
				PlayerCharacter->GetActorLocation(),
				Hit.GetActor()->GetActorLocation(),  // + 100 * (Hit.GetActor()->GetActorLocation() - PlayerCharacter->GetActorLocation()).Normalize(),
				NearByActors,
				false,
				IgnoreActors,
				EDrawDebugTrace::None,
				LineHitResults, true
			);
			if (Hit.GetActor() == LineHitResults.GetActor())
			{
				NearByEnemies.AddUnique(Hit.GetActor());
			}
		}
		float Min = 1000000.f;
		// 외적/내적을 통해 거리와 각도에 따른 값을 계산해 이를 key, Enemy를 Value로 Map을 설정함.
		for (int i = 0; i < NearByEnemies.Num(); i++)
		{
			// Option 1 : Player Character의 Forward Vector 기준으로 LockOn
			// Option 2 : Camera에 보이는 Enemy를 (Camera의 Forward Vector 기준으로) LockOn <- 현재 선택됨

			// FVector PlayerForwardVector = PlayerCharacter->GetActorForwardVector();
			// FVector CameraToPlayer = PlayerCharacter->GetActorLocation() - PlayerCameraLocation;

			FVector PlayerCameraLocation = FollowCamera->GetComponentTransform().GetTranslation();
			FVector CameraToPlayer = FollowCamera->GetForwardVector();

			FVector CameraToEnemy = NearByEnemies[i]->GetActorLocation() - PlayerCameraLocation;
			FVector PlayerToEnemy = NearByEnemies[i]->GetActorLocation() - PlayerCharacter->GetActorLocation();

			FVector CrossProduct = FVector::CrossProduct(CameraToPlayer, CameraToEnemy);
			// FVector CrossProduct = FVector::CrossProduct(PlayerForwardVector, PlayerToEnemy);
			float UpDotProduct = FVector::DotProduct(CameraToPlayer, CrossProduct);
			// float UpDotProduct = FVector::DotProduct(PlayerForwardVector, CrossProduct);
		
			float TempDotProductResult = FVector::DotProduct(CameraToPlayer, CameraToEnemy);

			// FOV 안에 있는지 계산한다.
			float Cos = TempDotProductResult / (CameraToPlayer.Length() * CameraToEnemy.Length());
			float HalfFOV = FMath::Cos(FMath::DegreesToRadians(FollowCamera->FieldOfView/1.5));
			if (Cos > HalfFOV)
			{
				EnemyMap.Add(UpDotProduct, NearByEnemies[i]);
				// DrawDebugSphere(GetWorld(), NearByEnemies[i]->GetActorLocation(), 25.f, 12, FColor::Blue, false, 3.f);
				// 가장 가까운 적을 찾기 위해 min을 계산.
				if (FMath::Abs(Min) > FMath::Abs(UpDotProduct))
				{
					Min = UpDotProduct;
				}
			}
		}
		if (EnemyMap.IsEmpty())
		{
			return false;
		}

		// Target Enemy 전환을 위해 EnemyMap을 정렬함.
		TArray<float> DotProducts;
		EnemyMap.GenerateKeyArray(DotProducts);
		DotProducts.Sort();
		NearByEnemies.Empty();
		for (int i = 0; i < DotProducts.Num(); i++)
		{
			NearByEnemies.Add(*EnemyMap.Find(DotProducts[i]));
		}
		// 가장 가까운 적을 Target Enemy로 설정함.
		TargetEnemy = *EnemyMap.Find(Min);
		if (IsValid(TargetEnemy)) return true;
		else return false;
	}
	else
	{
		return false;
	}
}

void UBaseInputComponent::Input_ChangeLockOnTarget(const FInputActionValue& Value)
{
	if (NearByEnemies.Num() == 0) return;
	FVector2D ValueVector = Value.Get<FVector2D>();
	int TargetIndex = NearByEnemies.IndexOfByKey(TargetEnemy);
	if (ValueVector.X > 0) // 마우스 휠 축 위 입력
	{
		TargetIndex = FMath::Clamp(TargetIndex + 1, 0, NearByEnemies.Num() - 1);
		TargetEnemy = NearByEnemies[TargetIndex];
	}
	else // 마우스 휠 축 아래 입력
	{
		TargetIndex = FMath::Clamp(TargetIndex - 1, 0, NearByEnemies.Num() - 1);
		TargetEnemy = NearByEnemies[TargetIndex];
	}
	SetControllerRotationTowardTarget();
}




void UBaseInputComponent::Input_CountMouseLeftInput()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	check(PlayerCharacter);
	//PlayerCharacter->GetCombatComponent()->IncrementInputCount();
}

void UBaseInputComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (const auto& PS = CastChecked<APlayerStateBase>(Pawn->GetPlayerState()))
		{
			if (const auto& ASC = CastChecked< UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
			{
				ASC->AbilityInputTagTriggered(InputTag);
			}
		}
	}

}

void UBaseInputComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{

	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (const auto& PS = CastChecked<APlayerStateBase>(Pawn->GetPlayerState()))
		{
			if (const auto& ASC = CastChecked< UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
			{
				ASC->AbilityInputTagReleased(InputTag);
			}
		}
	}

}
