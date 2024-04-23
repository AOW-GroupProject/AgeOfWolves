
#include "PlayerStateBase.h"
#include "Logging/StructuredLog.h"

#include "02_GameplayAbility/BaseAttributeSet.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "01_Character/PawnData.h"

#include "01_Character/CharacterBase.h"
#include "04_Component/BaseCharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogPlayerStateBase)

// @���� : �α� ��ũ���Դϴ�. ����+�ٿ��ֱ� Ȱ��
// UE_LOGFMT(LogPlayerStateBase, Log, "");

APlayerStateBase::APlayerStateBase()
{
	PawnData = nullptr;
	AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("Ability System Component"));
}

void APlayerStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void APlayerStateBase::BeginPlay()
{
	Super::BeginPlay();

	InitializeGameplayAbilitySystem();

}

void APlayerStateBase::InitializeGameplayAbilitySystem()
{
	check(PawnData);

	if (const auto& Controller = Cast<AController>(GetOwner()))
	{
		if (const auto& Pawn = Controller->GetPawn())
		{
			AbilitySystemComponent->InitAbilityActorInfo(Pawn, Pawn);

			if (PawnData->IsValidLowLevel())
			{
				UBaseAbilitySet* SetToGrant = PawnData->AbilitySet;
				if (IsValid(SetToGrant))
				{
					// @���� : ĳ������ �⺻ AttributeSet�� ASC�� ���� ����մϴ�.
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
							}
						}
					}
					// @���� : ĳ������ �⺻ Gameplay Effect�� ASC�� ���� ���/�����մϴ�.
					{
						SetToGrant->GiveStartupGameplayEffectToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
					}
					// @���� : ĳ������ �⺻ Gameplay Ability�� ASC�� ���� ���/�����մϴ�.
					{
						SetToGrant->GiveStartupGameplayAbilityToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
					}

					// @TODO : ASC�� Startup GA, GE, AttributeSet�� ��� �Ϸ� �̺�Ʈ ȣ��
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
		OnAnyAttributeValueChanged.Broadcast(Data.Attribute, Data.OldValue, Data.NewValue);	
	}

}