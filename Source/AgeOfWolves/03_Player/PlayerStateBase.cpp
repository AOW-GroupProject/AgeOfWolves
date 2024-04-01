
#include "PlayerStateBase.h"
#include "Logging/StructuredLog.h"

#include "02_GameplayAbility/BaseAttributeSet.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "01_Character/PawnData.h"

#include "01_Character/CharacterBase.h"
#include "04_Component/BaseCharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogPlayerStateBase)

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
			// @���� : ASC�� Actor Info ���
			AbilitySystemComponent->InitAbilityActorInfo(Pawn, Pawn);

			// @TODO : Refactoring, ����� ���� Init �Լ��� �ű��
			// @���� : PawnData�� AbilitySet�� ���� Pawn�� ASC�� Startup AttributeSet��  ����մϴ�. 
			if (PawnData->IsValidLowLevel())
			{
				UBaseAbilitySet* SetToGrant = PawnData->AbilitySet;
				if (IsValid(SetToGrant))
				{
					// #1. Startup AttributeSet
					{
						// #1. ������� Startup AttributeSet�� ASC�� ����մϴ�.
						SetToGrant->GiveStartupAttributeSetToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

						// #2. ���ο� AttributeSet�� �� Attribute �׸� ��ġ ��ȭ �̺�Ʈ�� �ݹ��Լ��� ����մϴ�.
						for (auto& AS : AbilitySystemComponent->GetSpawnedAttributes())
						{
							if (IsValid(AS))
							{
								AttributeSet = AS;
								TArray<FGameplayAttribute> Attributes = AttributeSet->GetAllAttributes();
								for (const FGameplayAttribute& Attribute : Attributes)
								{
									FDelegateHandle DelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &APlayerStateBase::OnAnyAttributeChanged);
								}
							}
						}
					}
					// #2. Startup GE
					{
						// #1. ������� Startup Gameplay Effect�� ASC�� �����մϴ�.(eg. GE_StartupAttributeSet)
						SetToGrant->GiveStartupGameplayEffectToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
					}
					// #3. Startup GA
					{
						SetToGrant->GiveStartupGameplayAbilityToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
					}

					// @TODO : ASC�� Startup Setting�� �ʱ�ȭ �۾� �Ϸῡ ���� �̺�Ʈ ȣ��
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

// @TODO : Widget�� ��ȣ�ۿ�.
// @TODO : Widge�� Ability Task�� ���� ASC�� AttributeSet�� �� Attribute �׸��� ��ġ ��ȭ �̺�Ʈ�� Listen�ص� ���ٵ�... �ʿ��ұ�?
void APlayerStateBase::OnAnyAttributeChanged(const FOnAttributeChangeData& Data)
{
	check(AttributeSet);

	// @TODO :	Gameplay Effect�� �������� ����� AttributeSet�� �� Attribute �׸��� ��ġ �� ��ȭ �̺�Ʈ�� ���� ó��
	// #1. Move Speed
	if (Data.Attribute == AttributeSet->GetMoveSpeedAttribute())
	{
		if (const auto& Controller = Cast<AController>(GetOwner()))
		{
			if (const auto& Character = Cast<ACharacterBase>(Controller->GetPawn()))
			{
				// @���� : GE_StartupAttributeSet�� ���� '�ʱ�ȭ'�۾��� �̷������ ��쿡�� �Ͻ������� Player State���� ���� ĳ������ MoveSpeed�� �������ݴϴ�.
				// @���� : ���Ŀ� Move Speed �Ӽ��� ��ġ ��ȭ �̺�Ʈ�� Gameplay Ability�� �������Ʈ���� �۾��մϴ�.(eg. BP_GA_Sprint)
				if (const auto InputComp = CastChecked<UBaseCharacterMovementComponent>(Character->GetCharacterMovement()))
				{
					InputComp->ChangeMoveSpeed(Data.OldValue, Data.NewValue);
				}
			}
		}
	}

}