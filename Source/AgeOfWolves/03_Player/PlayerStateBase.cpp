
#include "PlayerStateBase.h"
#include "Logging/StructuredLog.h"

#include "02_GameplayAbility/BaseAttributeSet.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "01_Character/PawnData.h"

#include "01_Character/CharacterBase.h"
#include "04_Component/BaseCharacterMovementComponent.h"

DEFINE_LOG_CATEGORY(LogPlayerStateBase)

// @목적 : 로그 메크로입니다. 복사+붙여넣기 활용
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
					// @설명 : 캐릭터의 기본 AttributeSet을 ASC에 최초 등록합니다.
					{
						SetToGrant->GiveStartupAttributeSetToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

						// 각 Attribute 항목 수치 변화 이벤트에 콜백함수를 등록합니다.
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
					// @설명 : 캐릭터의 기본 Gameplay Effect를 ASC에 최초 등록/적용합니다.
					{
						SetToGrant->GiveStartupGameplayEffectToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
					}
					// @설명 : 캐릭터의 기본 Gameplay Ability를 ASC에 최초 등록/적용합니다.
					{
						SetToGrant->GiveStartupGameplayAbilityToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
					}

					// @TODO : ASC의 Startup Setting의 초기화 작업 완료에 대한 이벤트 호출
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

void APlayerStateBase::OnAnyAttributeChanged(const FOnAttributeChangeData& Data)
{
	check(AttributeSet);

	// #1. Move Speed
	if (Data.Attribute == AttributeSet->GetMoveSpeedAttribute())
	{
		if (const auto& Controller = Cast<AController>(GetOwner()))
		{
			if (const auto& Character = Cast<ACharacterBase>(Controller->GetPawn()))
			{
				// @설명 : GE_StartupAttributeSet을 통해 '초기화'작업이 이루어졌을 경우에만 일시적으로 Player State에서 직접 캐릭터의 MoveSpeed를 조정해줍니다.
				// @설명 : 이후에 발생하는 Move Speed 속성의 수치 변화 이벤트는 Gameplay Ability의 블루프린트에서 작업합니다.(eg. BP_GA_Sprint)
				if (const auto CharacterMovementComponent = CastChecked<UBaseCharacterMovementComponent>(Character->GetCharacterMovement()))
				{
					CharacterMovementComponent->ChangeMoveSpeed(Data.OldValue, Data.NewValue);
				}
			}
		}
	}
	// #2. 그 외 항목
	else if (Data.Attribute == AttributeSet->GetStaminaAttribute())
	{	

	}

}