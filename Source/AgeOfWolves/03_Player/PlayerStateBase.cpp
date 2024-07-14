#include "PlayerStateBase.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/PlayerController.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "01_Character/PawnData.h"

#include "01_Character/CharacterBase.h"
#include "04_Component/BaseCharacterMovementComponent.h"

#include "08_UI/StateBars.h"


DEFINE_LOG_CATEGORY(LogPlayerStateBase)

// @���� : �α� ��ũ���Դϴ�. ����+�ٿ��ֱ� Ȱ��
// UE_LOGFMT(LogPlayerStateBase, Log, "");

APlayerStateBase::APlayerStateBase()
{
	PawnData = nullptr;
	AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("Ability System Component"));

	HUDClass = nullptr;
	HUD = nullptr;
}

void APlayerStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void APlayerStateBase::BeginPlay()
{
	Super::BeginPlay();

	//if (auto PC = CastChecked<ABasePlayerController>(GetPlayerController()))
	//{
	//	PC->OnControllerPossessCharacter.BindUFunction(this, "InitializeGameplayAbilitySystem");
	//}
}

void APlayerStateBase::InitializeGameplayAbilitySystem()
{
	check(PawnData);

	// @TODO: Inventory �ʱ�ȭ, ���� ������
	
	// #2. HUD �ʱ�ȭ
	if (HUDClass->IsValidLowLevel())
	{
		if (auto PC = GetPlayerController())
		{
			HUD = CreateWidget<UUserWidget>(PC, HUDClass);
			if (IsValid(HUD))
			{
				HUD->AddToViewport();
			}
		}
	}

	// #2. GAS �ʱ�ȭ
	if (const auto& Controller = Cast<AController>(GetOwner()))
	{
		if (const auto& Pawn = Controller->GetPawn())
		{
			// Actor Info
			AbilitySystemComponent->InitAbilityActorInfo(Pawn, Pawn);

			if (PawnData->IsValidLowLevel())
			{
				UBaseAbilitySet* SetToGrant = PawnData->AbilitySet;
				if (IsValid(SetToGrant))
				{
					// ĳ������ �⺻ AttributeSet�� ASC�� ���� ����մϴ�.
					{
						SetToGrant->GiveStartupAttributeSetToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

						// �� Attribute �׸� ��ġ ��ȭ �̺�Ʈ�� �ݹ��Լ��� ����մϴ�.
						for (auto& AS : AbilitySystemComponent->GetSpawnedAttributes())
						{
							if (IsValid(AS))
							{
								AttributeSet = AS;
								TArray<FGameplayAttribute> Attributes = AttributeSet->GetAllAttributes();
								for (const FGameplayAttribute& Attribute : Attributes)
								{
									FDelegateHandle DelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &APlayerStateBase::OnAttributeValueChanged);
								}
								break;
							}
						}
					}
					// ĳ������ �⺻ GA�� ���� Ability Tag Relationpship�� ����մϴ�.
					{
						if (PawnData->TagRelationship)
						{
							AbilitySystemComponent->SetAbilityTagRelationshipMapping(PawnData->TagRelationship);
						}
					}
					// ĳ������ �⺻ Gameplay Effect�� ASC�� ���� ���/�����մϴ�.
					{
						SetToGrant->GiveStartupGameplayEffectToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
					}
					// ĳ������ �⺻ Gameplay Ability�� ASC�� ���� ���/�����մϴ�.
					{
						SetToGrant->GiveStartupGameplayAbilityToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
					}

					// ASC�� Startup GA, GE, AttributeSet�� ��� �Ϸ� �̺�Ʈ ȣ��, �ӽ� �ּ� ó��
					//OnAttributeSetInitialized.Broadcast();
				}
			}
		}
	}
}

UAbilitySystemComponent* APlayerStateBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

TSoftObjectPtr<UBaseAttributeSet> APlayerStateBase::GetAttributeSet() const
{
	return AttributeSet;
}

UPawnData* APlayerStateBase::GetPawnData() const
{
	return PawnData.Get();
}

void APlayerStateBase::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
	if (OnAnyAttributeValueChanged.IsBound())
	{
		if(Data.Attribute == AttributeSet->GetMoveSpeedAttribute())
			 UE_LOGFMT(LogPlayerStateBase, Log, "{0}:", FString::SanitizeFloat(Data.NewValue));

		OnAnyAttributeValueChanged.Broadcast(Data.Attribute, Data.OldValue, Data.NewValue);	
	}

}