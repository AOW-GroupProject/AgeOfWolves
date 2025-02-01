#include "CharacterBase.h"
#include "Logging/StructuredLog.h"

#include "05_Animation/BaseAnimInstance.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "MotionWarpingComponent.h"

#include "10_AI/BaseAIController.h"

DEFINE_LOG_CATEGORY(LogCharacter)

//@Defualt Setting
#pragma region Default Setting
ACharacterBase::ACharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TeamId(FGenericTeamId::NoTeam)
{
	PrimaryActorTick.bCanEverTick = true;

	MotionWarpComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("Motion Warp Component"));

}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ACharacterBase::PossessedBy(AController* NewController)
{

	Super::PossessedBy(NewController);

	if (!NewController)
	{
		UE_LOGFMT(LogCharacter, Error, "NewController가 유효하지 않습니다.");
		return;
	}

	if (!AbilitySystemComponent.IsValid())
	{
		//@Base AI Controller
		auto BaseAIC = Cast<ABaseAIController>(NewController);
		if (!BaseAIC)
		{
			return;
		}

		//@ASC
		UAbilitySystemComponent* ASC = BaseAIC->GetAbilitySystemComponent();
		if (!ASC)
		{
			UE_LOGFMT(LogCharacter, Error, "AbilitySystemComponent가 유효하지 않습니다.");
			return;
		}

		//@Set ASC
		SetAbilitySystemComponent(ASC);
	}

}

bool ACharacterBase::CanBeSeenFrom(const FVector& ObserverLocation, FVector& OutSeenLocation, int32& NumberOfLoSChecksPerformed, float& OutSightStrength, const AActor* IgnoreActor, const bool* bWasVisible, int32* UserData) const
{
	static const FName NAME_AILineOfSight = FName(TEXT("TestPawnLineOfSight"));
	FHitResult HitResult;
	FVector SocketLocation = GetMesh()->GetSocketLocation(PerceptionTarget);

	const bool bHitSocket = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, SocketLocation,
		FCollisionObjectQueryParams(ECC_TO_BITFIELD
		(ECC_WorldStatic) | ECC_TO_BITFIELD(ECC_WorldDynamic)),
		FCollisionQueryParams(NAME_AILineOfSight, true,
			IgnoreActor));

	NumberOfLoSChecksPerformed++;

	if (bHitSocket == false || (HitResult.GetActor() && HitResult.GetActor()->IsOwnedBy(this)))
	{
		OutSeenLocation = SocketLocation;
		OutSightStrength = 1;

		return true;
	}

	const bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, ObserverLocation, GetActorLocation(),
		FCollisionObjectQueryParams(ECC_TO_BITFIELD(ECC_WorldStatic) |
			ECC_TO_BITFIELD(ECC_WorldDynamic)), FCollisionQueryParams
			(NAME_AILineOfSight, true, IgnoreActor));

	NumberOfLoSChecksPerformed++;

	if (bHit == false || (HitResult.GetActor() && HitResult.GetActor()->IsOwnedBy(this)))
	{
		OutSeenLocation = GetActorLocation();
		OutSightStrength = 1;

		return true;
	}

	OutSightStrength = 0;
	return false;
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.IsValid() ? AbilitySystemComponent.Get() : nullptr;
}

void ACharacterBase::SetAbilitySystemComponent(UAbilitySystemComponent* ASC)
{
	check(ASC)

		AbilitySystemComponent = ASC;
}
#pragma endregion