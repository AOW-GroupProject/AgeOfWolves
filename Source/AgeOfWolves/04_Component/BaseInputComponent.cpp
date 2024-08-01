// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInputComponent.h"
#include "Logging/StructuredLog.h"

#include "03_Player/BasePlayerController.h"

#include "01_Character/PawnData.h"
#include "EnhancedInputSubsystems.h"
#include "14_Subsystem/InputManagerSubsystem.h"

#include "01_Character/PlayerCharacter.h"
#include "03_Player/PlayerStateBase.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "04_Component/LockOnComponent.h"


#include "GameplayTagContainer.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(BaseInputComponent)

DEFINE_LOG_CATEGORY(LogInputComponent)
// UE_LOGFMT(LogInputComponent, Log, "");


#pragma region Default Setting
UBaseInputComponent::UBaseInputComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;//Initialize Component 활용에 필요 
}

void UBaseInputComponent::OnRegister()
{
	Super::OnRegister();

}

void UBaseInputComponent::OnUnregister()
{
	Super::OnUnregister();

}

void UBaseInputComponent::InitializeComponent()
{
	Super::InitializeComponent();

}

void UBaseInputComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);

}

void UBaseInputComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UBaseInputComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UBaseInputComponent::InitializeInputComponent()
{
	//@Player Controller
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOGFMT(LogInputComponent, Error, "Owner가 PlayerController가 아닙니다.");
		return;
	}
	//@Binding
	InternalBindToInputActions(PC);
	//@Local Player
	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOGFMT(LogInputComponent, Error, "LocalPlayer is null in UIComponent::InitializeComponent");
		return;
	}
	//@Input Manager Subsystem
	UInputManagerSubsystem* InputManagerSubsystem = LocalPlayer->GetSubsystem<UInputManagerSubsystem>();
	if (!InputManagerSubsystem)
	{
		UE_LOGFMT(LogInputComponent, Error, "입력 설정 로드 실패: InputManagerSubsystem을 찾을 수 없습니다.");
		return;
	}
	//@Input Configs
	const TArray<UInputConfig*>& InputConfigs = InputManagerSubsystem->GetInputConfigs();
	//@Add IMC
	for (auto InputConfig : InputConfigs)
	{
		InputManagerSubsystem->AddMappingContext(InputConfig->InputMappingContext, InputConfig->MappingPriority);
	}
	//@Delegate
	NotifyInputComponentInitFinished.Broadcast();

	UE_LOGFMT(LogInputComponent, Log, "플레이어 입력 초기화가 완료되었습니다.");
}

void UBaseInputComponent::InternalBindToInputActions(const APlayerController* PC)
{

	//@Local Player
	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOGFMT(LogInputComponent, Error, "LocalPlayer is null in UIComponent::InitializeComponent");
		return ;
	}
	//@Input Manager Subsystem
	UInputManagerSubsystem* InputManagerSubsystem = LocalPlayer->GetSubsystem<UInputManagerSubsystem>();
	if (!InputManagerSubsystem)
	{
		UE_LOGFMT(LogInputComponent, Error, "입력 설정 로드 실패: InputManagerSubsystem을 찾을 수 없습니다.");
		return ;
	}
	//@Input Configs
	const TArray<UInputConfig*>& InputConfigs = InputManagerSubsystem->GetInputConfigs();
	//@Binding
	for (UInputConfig* InputConfig : InputConfigs)
	{
		//@Native IA
		BindNativeInputActions(InputConfig);
		//@Ability IA
		BindAbilityInputActions(InputConfig);
		//@UI IA
		BindUIInputActions(InputConfig);
	}

}

void UBaseInputComponent::BindNativeInputActions(const UInputConfig* InputConfig)
{
	UE_LOGFMT(LogInputComponent, Log, "기본 입력 액션을 바인딩합니다.");
	BindNativeInputAction(InputConfig, FGameplayTag::RequestGameplayTag(FName("Input.Native.Move")), ETriggerEvent::Triggered, this, &UBaseInputComponent::Input_Move);
	BindNativeInputAction(InputConfig, FGameplayTag::RequestGameplayTag(FName("Input.Native.Looking")), ETriggerEvent::Triggered, this, &UBaseInputComponent::Input_Look);
	BindNativeInputAction(InputConfig, FGameplayTag::RequestGameplayTag(FName("Input.Native.LockOn")), ETriggerEvent::Triggered, this, &UBaseInputComponent::Input_LockOn);
}

void UBaseInputComponent::BindAbilityInputActions(const UInputConfig* InputConfig)
{
	if (!InputConfig->AbilityInputActions.IsEmpty())
	{
		UE_LOGFMT(LogInputComponent, Log, "어빌리티 입력 액션을 바인딩합니다.");
		BindInputActions(InputConfig->AbilityInputActions, this, &ThisClass::OnAbilityInputTagTriggered, &ThisClass::OnAbilityInputTagReleased);
	}
	else
	{
		UE_LOGFMT(LogInputComponent, Warning, "바인딩할 어빌리티 입력 액션이 없습니다.");
	}
}

void UBaseInputComponent::BindUIInputActions(const UInputConfig* InputConfig)
{
	if (!InputConfig->UIInputActions.IsEmpty())
	{
		BindInputActions(InputConfig->UIInputActions, this, &ThisClass::OnUIInputTagTriggered, &ThisClass::OnUIInputTagReleased);
	}
}

void UBaseInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
#pragma endregion 

#pragma region IMC(Input Mapping Context)
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
#pragma endregion

#pragma region Callbacks
void UBaseInputComponent::Input_Move(const FInputActionValue& Value)
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			FVector2D MovementVector = Value.Get<FVector2D>();
			InputVector = MovementVector;

			const FRotator Rotation = PC->GetControlRotation();
			const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

			// Forward, Backward
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			Pawn->AddMovementInput(ForwardDirection, MovementVector.X);

			// Right, Left
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			Pawn->AddMovementInput(RightDirection, MovementVector.Y);
		}
	}
}

void UBaseInputComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (APawn* Pawn = PC->GetPawn())
		{
			APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(Pawn);
			if (PlayerCharacter && PlayerCharacter->GetLockOnComponent()->GetbLockOn() == true) return;

			const FVector2D Value = InputActionValue.Get<FVector2D>();
			if (Value.X != 0.0f)
			{
				PC->AddYawInput(Value.X);
			}

			if (Value.Y != 0.0f)
			{
				PC->AddPitchInput(Value.Y);
			}
		}
	}
}

void UBaseInputComponent::Input_LockOn(const FInputActionValue& Value)
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PC->GetPawn()))
		{
			ULockOnComponent* LockOnComponent = PlayerCharacter->GetLockOnComponent();
			if (LockOnComponent->GetbLockOn() == true)
			{
				LockOnComponent->CancelLockOn();
			}
			else
			{
				LockOnComponent->StartLockOn();
			}
		}
	}
}

void UBaseInputComponent::Input_LeftMousePressed(const FInputActionValue& Value)
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PC->GetPawn()))
		{
			APlayerStateBase* PlayerStateBase = Cast<APlayerStateBase>(PlayerCharacter->GetPlayerState());
			if (PlayerStateBase && PlayerStateBase->GetAbilitySystemComponent())
			{
				FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Ability.Active.Attack"));
				PlayerStateBase->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(FGameplayTagContainer(AttackTag));
			}
		}
	}
}

void UBaseInputComponent::OnAbilityInputTagTriggered(FGameplayTag InputTag)
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (APlayerStateBase* PS = Cast<APlayerStateBase>(PC->PlayerState))
		{
			if (UBaseAbilitySystemComponent* ASC = Cast<UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
			{
				ASC->AbilityInputTagTriggered(InputTag);
			}
		}
	}
}

void UBaseInputComponent::OnAbilityInputTagReleased(FGameplayTag InputTag)
{
	if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
	{
		if (APlayerStateBase* PS = Cast<APlayerStateBase>(PC->PlayerState))
		{
			if (UBaseAbilitySystemComponent* ASC = Cast<UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
			{
				ASC->AbilityInputTagReleased(InputTag);
			}
		}
	}
}

void UBaseInputComponent::OnUIInputTagTriggered(FGameplayTag InputTag)
{
	UE_LOGFMT(LogInputComponent, Log, "{0}", InputTag.GetTagName().ToString());

	UIInputTagTriggered.ExecuteIfBound(InputTag);

}

void UBaseInputComponent::OnUIInputTagReleased(FGameplayTag InputTag)
{
	UE_LOGFMT(LogInputComponent, Log, "{0}", InputTag.GetTagName().ToString());

	UIInputTagReleased.ExecuteIfBound(InputTag);

}
#pragma endregion