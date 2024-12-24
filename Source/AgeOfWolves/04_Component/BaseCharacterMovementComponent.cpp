#include "BaseCharacterMovementComponent.h"

#include "GameFramework/Character.h"
#include "03_Player/PlayerStateBase.h"

//@Defualt Setting
#pragma region Default Setting
UBaseCharacterMovementComponent::UBaseCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UBaseCharacterMovementComponent::OnRegister()
{
	Super::OnRegister();

}

void UBaseCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	if (const auto Owner = CastChecked<ACharacter>(GetOwner()))
	{
		if (auto PS = Owner->GetPlayerState<APlayerStateBase>())
		{
			PS->OnAnyAttributeValueChanged.AddDynamic(this, &UBaseCharacterMovementComponent::MoveSpeedChanged);
		}
	}
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UBaseCharacterMovementComponent::ChangeMoveSpeed(float InOldVal, float InNewVal)
{

	if (InNewVal > InOldVal)
	{
		IsSprinting = true;
		MaxAcceleration = SprintAcceleration;
		MaxWalkSpeed = InNewVal;
	}
	else
	{
		IsSprinting = false;
		MaxAcceleration = WalkAcceleration;
		MaxWalkSpeed = InNewVal;
	}

}
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UBaseCharacterMovementComponent::MoveSpeedChanged(FGameplayAttribute Attribute, float OldValue, float NewValue)
{
	if (Attribute.IsValid() && Attribute.AttributeName == "MoveSpeed")
	{
		ChangeMoveSpeed(OldValue, NewValue);
	}
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion