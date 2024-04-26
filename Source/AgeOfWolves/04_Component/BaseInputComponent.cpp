// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInputComponent.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/Character.h"

#include "03_Player/PlayerStateBase.h"
#include "01_Character/PawnData.h"
#include "EnhancedInputSubsystems.h"
#include "06_Input/InputConfig.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "01_Character/PlayerCharacter.h"

#include "GameplayTagContainer.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseInputComponent)

DEFINE_LOG_CATEGORY(LogInputComponent)


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
		// #2. Controller
		if (AController* Controller = Pawn->GetController())
		{
			// input is a Vector2D
			FVector2D MovementVector = Value.Get<FVector2D>();

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
	NearByEnemies.Empty();
	EnemyMap.Empty();

	TArray<TEnumAsByte<EObjectTypeQuery>> NearByActors;
	TEnumAsByte<EObjectTypeQuery> PawnObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	NearByActors.Add(PawnObjectType);

	TArray<AActor*> IgnoreActors;
	AActor* Character = GetOwner();
	check(Character);
	IgnoreActors.Add(Character);
	
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		IgnoreActors.Add(PlayerCharacter);
	}

	TArray<FHitResult> HitResults;

	bool TraceHitResult = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		Character->GetActorLocation(),
		Character->GetActorLocation() + 100.f,
		DetectionMaxRadius,
		NearByActors,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		HitResults,
		true
	);

	if (TraceHitResult == true)
	{
		for (const auto& Hit : HitResults)
		{
			NearByEnemies.AddUnique(Hit.GetActor());
		}
		float Min = 1000000.f;
		for (int i = 0; i < NearByEnemies.Num(); i++)
		{
			FVector Vector = FVector::CrossProduct(Character->GetActorForwardVector(), NearByEnemies[i]->GetActorLocation() - Character->GetActorLocation());
			float DotProduct = FVector::DotProduct(Character->GetActorUpVector(), Vector);
			EnemyMap.Add(DotProduct, NearByEnemies[i]);
			DrawDebugSphere(GetWorld(), NearByEnemies[i]->GetActorLocation(), 25.f, 12, FColor::Blue, false, 3.f);
			if (FMath::Abs(Min) > FMath::Abs(DotProduct))
			{
				Min = DotProduct;
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Min  : %f"), Min);
		TArray<float> DotProducts;
		EnemyMap.GenerateKeyArray(DotProducts);
		DotProducts.Sort();
		NearByEnemies.Empty();
		for (int i = 0; i < DotProducts.Num(); i ++)
		{
			NearByEnemies.Add(*EnemyMap.Find(DotProducts[i]));
		}
		TargetEnemy = *EnemyMap.Find(Min);
		if (TargetEnemy)
		{
			DrawDebugSphere(GetWorld(), TargetEnemy->GetActorLocation(), 50.f, 12, FColor::Red, false, 1.5f);
		}
	}
}

void UBaseInputComponent::Input_ChangeLockOnTarget(const FInputActionValue& Value)
{
	if (NearByEnemies.Num() == 0) return;
	FVector2D ValueVector = Value.Get<FVector2D>();
	int TargetIndex = NearByEnemies.IndexOfByKey(TargetEnemy);
	if (ValueVector.X > 0) // 마우스 휠 축 위
	{
		TargetIndex = FMath::Clamp(TargetIndex + 1, 0, NearByEnemies.Num() - 1);
		TargetEnemy = NearByEnemies[TargetIndex];
		DrawDebugSphere(GetWorld(), TargetEnemy->GetActorLocation(), 50.f, 12, FColor::Red, false, 1.5f);
	}
	else // 마우스 휠 축 아래
	{
		TargetIndex = FMath::Clamp(TargetIndex - 1, 0, NearByEnemies.Num() - 1);
		TargetEnemy = NearByEnemies[TargetIndex];
		DrawDebugSphere(GetWorld(), TargetEnemy->GetActorLocation(), 50.f, 12, FColor::Red, false, 1.5f);
	}
	APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
	FVector Start = CameraManager->GetCameraLocation();
	FVector Target = TargetEnemy->GetActorLocation();

	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	APlayerCharacter* PlayerCharacter = CastChecked<APlayerCharacter>(GetOwner());
	PlayerCharacter->GetController()->SetControlRotation(FRotator(0.f, Rotation.Yaw, 0.f));
}



void UBaseInputComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	//UE_LOGFMT(LogInputComponent, Log, "Pressed!");

	//UE_LOGFMT(LogInputComponent, Log, "Input Tag : {0}", InputTag.ToString());

	if (const APawn* Pawn = Cast<APawn>(GetOwner()))
	{
		if (const auto& PS = CastChecked<APlayerStateBase>(Pawn->GetPlayerState()))
		{
			if (const auto& ASC = CastChecked< UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
			{
				ASC->AbilityInputTagPressed(InputTag);
			}
		}
	}

}

void UBaseInputComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	//UE_LOGFMT(LogInputComponent, Log, "Released!");

	//UE_LOGFMT(LogInputComponent, Log, "Input Tag : {0}", InputTag.ToString());

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
