
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
			// @설명 : ASC에 Actor Info 등록
			AbilitySystemComponent->InitAbilityActorInfo(Pawn, Pawn);

			// @TODO : Refactoring, 사용자 정의 Init 함수로 옮기기
			// @설명 : PawnData의 AbilitySet을 통해 Pawn의 ASC에 Startup AttributeSet을  등록합니다. 
			if (PawnData->IsValidLowLevel())
			{
				UBaseAbilitySet* SetToGrant = PawnData->AbilitySet;
				if (IsValid(SetToGrant))
				{
					// #1. Startup AttributeSet
					{
						// #1. 사용자의 Startup AttributeSet을 ASC에 등록합니다.
						SetToGrant->GiveStartupAttributeSetToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

						// #2. 새로운 AttributeSet의 각 Attribute 항목 수치 변화 이벤트에 콜백함수를 등록합니다.
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
						// #1. 사용자의 Startup Gameplay Effect를 ASC에 적용합니다.(eg. GE_StartupAttributeSet)
						SetToGrant->GiveStartupGameplayEffectToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
					}
					// #3. Startup GA
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

// @TODO : Widget과 상호작용.
// @TODO : Widge은 Ability Task를 통해 ASC의 AttributeSet의 각 Attribute 항목의 수치 변화 이벤트를 Listen해도 될텐데... 필요할까?
void APlayerStateBase::OnAnyAttributeChanged(const FOnAttributeChangeData& Data)
{
	check(AttributeSet);

	// @TODO :	Gameplay Effect의 적용으로 변경된 AttributeSet의 각 Attribute 항목의 수치 값 변화 이벤트에 대한 처리
	// #1. Move Speed
	if (Data.Attribute == AttributeSet->GetMoveSpeedAttribute())
	{
		if (const auto& Controller = Cast<AController>(GetOwner()))
		{
			if (const auto& Character = Cast<ACharacterBase>(Controller->GetPawn()))
			{
				// @설명 : GE_StartupAttributeSet을 통해 '초기화'작업이 이루어졌을 경우에만 일시적으로 Player State에서 직접 캐릭터의 MoveSpeed를 조정해줍니다.
				// @설명 : 이후에 Move Speed 속성의 수치 변화 이벤트는 Gameplay Ability의 블루프린트에서 작업합니다.(eg. BP_GA_Sprint)
				if (const auto InputComp = CastChecked<UBaseCharacterMovementComponent>(Character->GetCharacterMovement()))
				{
					InputComp->ChangeMoveSpeed(Data.OldValue, Data.NewValue);
				}
			}
		}
	}

}