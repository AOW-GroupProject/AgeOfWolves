#include "PlayerStateBase.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/PlayerController.h"

#include "01_Character/PawnData.h"
#include "01_Character/CharacterBase.h"
#include "04_Component/BaseCharacterMovementComponent.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "04_Component/PlayerAbilitySystemComponent.h"


DEFINE_LOG_CATEGORY(LogPlayerStateBase)

// @목적 : 로그 메크로입니다. 복사+붙여넣기 활용
// UE_LOGFMT(LogPlayerStateBase, Log, "");

APlayerStateBase::APlayerStateBase()
{
	PawnData = nullptr;
	AbilitySystemComponent = CreateDefaultSubobject<UPlayerAbilitySystemComponent>(TEXT("Ability System Component"));

}

void APlayerStateBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void APlayerStateBase::BeginPlay()
{
	Super::BeginPlay();

}

void APlayerStateBase::InitializePlayerState()
{
	check(PawnData);

	//@AttributeSet, GE, GA
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
					// 캐릭터의 기본 AttributeSet을 ASC에 최초 등록합니다.
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
									FDelegateHandle DelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &APlayerStateBase::OnAttributeValueChanged);
								}
								break;
							}
						}
					}
					// 캐릭터의 기본 GA에 대한 Ability Tag Relationpship을 등록합니다.
					{
						if (PawnData->TagRelationship)
						{
							AbilitySystemComponent->SetAbilityTagRelationshipMapping(PawnData->TagRelationship);
						}
					}
					// 캐릭터의 기본 Gameplay Effect를 ASC에 최초 등록/적용합니다.
					{
						SetToGrant->GiveStartupGameplayEffectToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
					}
					// 캐릭터의 기본 Gameplay Ability를 ASC에 최초 등록/적용합니다.
					{
						SetToGrant->GiveStartupGameplayAbilityToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
					}

					// ASC에 Startup GA, GE, AttributeSet의 등록 완료 이벤트 호출, 임시 주석 처리
					OnAttributeSetInitialized.Broadcast();
				}
			}
		}
	}
}

void APlayerStateBase::LoadGameAbilitySystem()
{
}

void APlayerStateBase::LoadAbilitySystemFromSaveGame(UAOWSaveGame* SaveGame)
{
}

void APlayerStateBase::LoadDefaultAbilitySystemFromAbilityManager(UAbilityManagerSubsystem* AbilityManager)
{
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