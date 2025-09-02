#include "BaseInputComponent.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"

#include "03_Player/PlayerStateBase.h"
#include "03_Player/BasePlayerController.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "04_Component/LockOnComponent.h"
#include "04_Component/UIComponent.h"

#include "14_Subsystem/InputManagerSubsystem.h"

#include "GameplayTagContainer.h"
#include "EnhancedInputSubsystems.h"

#include "05_Animation/BaseAnimInstance.h" // 헤더 추가

DEFINE_LOG_CATEGORY(LogInputComponent)
// UE_LOGFMT(LogInputComponent, Log, "");

//@Defualt Setting
#pragma region Default Setting
UBaseInputComponent::UBaseInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, bBlockMovementInput(false) // 추가
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
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

	//@External Binding
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
	//@Add IMC and find highest priority
	int32 HighestPriority = TNumericLimits<int32>::Lowest();
	for (auto InputConfig : InputConfigs)
	{
		InputManagerSubsystem->AddMappingContext(InputConfig->InputMappingContext, InputConfig->MappingPriority);
		if (InputConfig->MappingPriority > HighestPriority)
		{
			HighestPriority = InputConfig->MappingPriority;
			CurrentIMCTag = InputConfig->IMCTag;
		}
	}
	//@Delegate
	NotifyInputComponentInitFinished.Broadcast();

	UE_LOGFMT(LogInputComponent, Log, "플레이어 입력 초기화가 완료되었습니다. 현재 활성화된 IMC: {0}", CurrentIMCTag.ToString());
}

void UBaseInputComponent::InternalBindToInputActions(const APlayerController* PC)
{

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
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UBaseInputComponent::BindNativeInputActions(const UInputConfig* InputConfig)
{
	UE_LOGFMT(LogInputComponent, Log, "기본 입력 액션을 바인딩합니다.");

	//@이동 
	BindNativeInputAction(InputConfig,
		FGameplayTag::RequestGameplayTag(FName("Input.Native.Move")),
		ETriggerEvent::Triggered,
		this,
		&UBaseInputComponent::Input_Move);

	//@회전
	BindNativeInputAction(InputConfig,
		FGameplayTag::RequestGameplayTag(FName("Input.Native.Looking")),
		ETriggerEvent::Triggered,
		this,
		&UBaseInputComponent::Input_Look);

	//@락온
	BindNativeInputAction(InputConfig,
		FGameplayTag::RequestGameplayTag(FName("Input.Native.LockOn")),
		ETriggerEvent::Triggered,
		this,
		&UBaseInputComponent::Input_LockOn);

	//@락온 타겟 변경 - Axis1D
	BindAxis1DNativeInputAction(InputConfig,
		FGameplayTag::RequestGameplayTag(FName("Input.Native.ChangeLockOnTarget")),
		ETriggerEvent::Triggered);
}

void UBaseInputComponent::BindAbilityInputActions(const UInputConfig* InputConfig)
{
	if (!InputConfig->AbilityInputActions.IsEmpty())
	{
		UE_LOGFMT(LogInputComponent, Log, "어빌리티 입력 액션을 바인딩합니다.");
		BindInputActions(InputConfig->AbilityInputActions, this, &ThisClass::OnAbilityInputTagPressed, &ThisClass::OnAbilityInputTagReleased);
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

void UBaseInputComponent::SwapMappings(const FGameplayTag& NewIMCTag)
{
	if (CurrentIMCTag == NewIMCTag)
	{
		UE_LOGFMT(LogInputComponent, Log, "이미 {0} IMC가 활성화되어 있습니다.", NewIMCTag.ToString());
		return;
	}

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		UE_LOGFMT(LogInputComponent, Error, "Owner가 PlayerController가 아닙니다.");
		return;
	}

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOGFMT(LogInputComponent, Error, "LocalPlayer를 찾을 수 없습니다.");
		return;
	}

	UInputManagerSubsystem* InputManagerSubsystem = LocalPlayer->GetSubsystem<UInputManagerSubsystem>();
	if (!InputManagerSubsystem)
	{
		UE_LOGFMT(LogInputComponent, Error, "InputManagerSubsystem을 찾을 수 없습니다.");
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem)
	{
		UE_LOGFMT(LogInputComponent, Error, "EnhancedInputLocalPlayerSubsystem을 찾을 수 없습니다.");
		return;
	}

	const TArray<UInputConfig*>& InputConfigs = InputManagerSubsystem->GetInputConfigs();
	UInputConfig* CurrentConfig = nullptr;
	UInputConfig* NewConfig = nullptr;

	for (UInputConfig* Config : InputConfigs)
	{
		if (Config->IMCTag == CurrentIMCTag)
		{
			CurrentConfig = Config;
		}
		else if (Config->IMCTag == NewIMCTag)
		{
			NewConfig = Config;
		}

		if (CurrentConfig && NewConfig)
		{
			break;
		}
	}

	if (CurrentConfig)
	{
		InputSubsystem->RemoveMappingContext(CurrentConfig->InputMappingContext);
	}

	if (NewConfig)
	{
		InputSubsystem->AddMappingContext(NewConfig->InputMappingContext, NewConfig->MappingPriority);
		CurrentIMCTag = NewIMCTag;
		UE_LOGFMT(LogInputComponent, Log, "{0} IMC로 전환되었습니다.", NewIMCTag.ToString());
	}
	else
	{
		UE_LOGFMT(LogInputComponent, Warning, "{0} IMC를 찾을 수 없습니다.", NewIMCTag.ToString());
	}
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UBaseInputComponent::Input_Move(const FInputActionValue& Value)
{
	// 입력 허용 상태 체크
	if (!IsInputAllowed()) return;

	// === 새로 추가: 이동 입력 블록 체크 ===
	if (bBlockMovementInput)
	{
		UE_LOGFMT(LogInputComponent, Warning, "이동 입력이 블록되었습니다 (발도술 자세 등)");
		return;
	}

	if (CurrentIMCTag == FGameplayTag::RequestGameplayTag(FName("Input.IMC.PlayerOnGround")))
	{
		if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
		{
			if (APawn* Pawn = PC->GetPawn())
			{
				// BaseAnimInstance 체크 (기존 Root Motion 체크 유지)
				if (USkeletalMeshComponent* MeshComp = Pawn->FindComponentByClass<USkeletalMeshComponent>())
				{
					if (UBaseAnimInstance* BaseAnimInstance = Cast<UBaseAnimInstance>(MeshComp->GetAnimInstance()))
					{
						if (BaseAnimInstance->GetIsPlayingRootMotionMontageWithFullBodySlot())
						{
							return;
						}
					}
				}

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
}

void UBaseInputComponent::Input_Look(const FInputActionValue& InputActionValue)
{
	//@입력 허용 상태 체크 - 한 줄 추가
	if (!IsInputAllowed()) return;

	if (CurrentIMCTag == FGameplayTag::RequestGameplayTag(FName("Input.IMC.PlayerOnGround")))
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
}

void UBaseInputComponent::Input_LockOn(const FInputActionValue& Value)
{
	//@입력 허용 상태 체크 - 한 줄 추가
	if (!IsInputAllowed()) return;

	if (CurrentIMCTag != FGameplayTag::RequestGameplayTag(FName("Input.IMC.PlayerOnGround")))
		return;

	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
		return;

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(PC->GetPawn());
	if (!PlayerCharacter)
		return;

	ULockOnComponent* LockOnComponent = PlayerCharacter->GetLockOnComponent();
	if (!LockOnComponent)
		return;

	if (LockOnComponent->GetbLockOn())
	{
		LockOnComponent->CancelLockOn();
	}
	else
	{
		LockOnComponent->StartLockOn();
	}
}

void UBaseInputComponent::OnAbilityInputTagPressed(FGameplayTag InputTag)
{
	//@입력 허용 상태 체크 - 한 줄 추가
	if (!IsInputAllowed()) return;

	if (CurrentIMCTag == FGameplayTag::RequestGameplayTag(FName("Input.IMC.PlayerOnGround")))
	{
		if (APlayerController* PC = Cast<APlayerController>(GetOwner()))
		{
			if (APlayerStateBase* PS = Cast<APlayerStateBase>(PC->PlayerState))
			{
				if (UBaseAbilitySystemComponent* ASC = Cast<UBaseAbilitySystemComponent>(PS->GetAbilitySystemComponent()))
				{
					ASC->AbilityInputTagPressed(InputTag);
				}
			}
		}
	}
}

void UBaseInputComponent::OnAbilityInputTagReleased(FGameplayTag InputTag)
{
	//@입력 허용 상태 체크 - 한 줄 추가
	if (!IsInputAllowed()) return;

	if (CurrentIMCTag == FGameplayTag::RequestGameplayTag(FName("Input.IMC.PlayerOnGround")))
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
}

void UBaseInputComponent::OnUIInputTagTriggered(FGameplayTag InputTag)
{
	//UE_LOGFMT(LogInputComponent, Log, "{0}", InputTag.GetTagName().ToString());

	UIInputTagTriggered.Broadcast(InputTag);

}

void UBaseInputComponent::OnUIInputTagReleased(FGameplayTag InputTag)
{
	//UE_LOGFMT(LogInputComponent, Log, "{0}", InputTag.GetTagName().ToString());

	UIInputTagReleased.Broadcast(InputTag);

}

void UBaseInputComponent::OnNativeInputTagValueTriggered(const FInputActionValue& Value, FGameplayTag InputTag)
{
	const float AxisValue = Value.Get<float>();
	UE_LOGFMT(LogInputComponent, Log, "Native Input Value Triggered - Tag: {0}, Value: {1}",
		InputTag.ToString(), AxisValue);

	NativeInputTagTriggeredWithValue.Broadcast(InputTag, AxisValue);
}

void UBaseInputComponent::OnUIInputTagValueTriggered(const FInputActionValue& Value, FGameplayTag InputTag)
{
	const float AxisValue = Value.Get<float>();
	UE_LOGFMT(LogInputComponent, Log, "UI Input Value Triggered - Tag: {0}, Value: {1}", InputTag.ToString(), AxisValue);

	UIInputTagTriggeredWithValue.Broadcast(InputTag, AxisValue);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
bool UBaseInputComponent::IsInputAllowed() const
{
	//@Owner가 PlayerController인지 확인
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		return false;
	}

	//@PlayerController의 입력 활성화 상태 체크
	if (!PC->InputEnabled())
	{
		return false;
	}

	return true;
}

void UBaseInputComponent::SetBlockMovementInput(bool bBlock)
{
	if (bBlockMovementInput == bBlock) return;

	bBlockMovementInput = bBlock;

	UE_LOGFMT(LogInputComponent, Log, "이동 입력 블록 상태 변경: {0}",
		bBlock ? TEXT("블록됨") : TEXT("허용됨"));
}
#pragma endregion