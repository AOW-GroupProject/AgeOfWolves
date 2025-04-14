#include "LocationDataComponent.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogLocationData)

//@Defualt Setting
#pragma region Default Setting
ULocationDataComponent::ULocationDataComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

    ExecutionLocation = CreateDefaultSubobject<USceneComponent>(TEXT("ExecutionLocation"));
    ExecutionLocation->SetupAttachment(this);

    AmbushLocation = CreateDefaultSubobject<USceneComponent>(TEXT("AmbushLocation"));
    AmbushLocation->SetupAttachment(this);
}

void ULocationDataComponent::BeginPlay()
{
	Super::BeginPlay();

}
void ULocationDataComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
FVector ULocationDataComponent::GetLocationForInteractionType(EInteractionType Type) const
{
    AActor* Owner = GetOwner();
    FVector OwnerLocation = Owner ? Owner->GetActorLocation() : FVector::ZeroVector;
    FVector ResultLocation = FVector::ZeroVector;

    switch (Type)
    {
    case EInteractionType::Execution:
        if (ExecutionLocation)
        {
            ResultLocation = ExecutionLocation->GetComponentLocation();


            return ResultLocation;
        }
        break;
    case EInteractionType::Ambush:
        if (AmbushLocation)
        {
            ResultLocation = AmbushLocation->GetComponentLocation();
            return ResultLocation;
        }
        break;
    default:
        UE_LOGFMT(LogLocationData, Warning, "지원되지 않는 상호작용 타입: {0}", UEnum::GetValueAsString(Type));
        break;
    }

    UE_LOGFMT(LogLocationData, Warning, "유효한 위치를 찾을 수 없음 - 타입: {0} | 소유자: {1}",
        UEnum::GetValueAsString(Type),
        Owner ? Owner->GetName() : "없음");
    return FVector::ZeroVector;
}

FRotator ULocationDataComponent::GetRotationForInteractionType(EInteractionType Type) const
{
    AActor* Owner = GetOwner();
    FRotator OwnerRotation = Owner ? Owner->GetActorRotation() : FRotator::ZeroRotator;
    FRotator ResultRotation = FRotator::ZeroRotator;

    switch (Type)
    {
    case EInteractionType::Execution:
        if (ExecutionLocation && !ExecutionLocation->IsPendingKill())
        {
            ResultRotation = ExecutionLocation->GetComponentRotation();
            return ResultRotation;
        }
        break;
    case EInteractionType::Ambush:
        if (AmbushLocation && !AmbushLocation->IsPendingKill())
        {
            ResultRotation = AmbushLocation->GetComponentRotation();
            return ResultRotation;
        }
        break;
    default:
        UE_LOGFMT(LogLocationData, Warning, "지원되지 않는 상호작용 타입: {0}", UEnum::GetValueAsString(Type));
        break;
    }

    UE_LOGFMT(LogLocationData, Warning, "유효한 회전을 찾을 수 없음 - 타입: {0} | 소유자: {1}",
        UEnum::GetValueAsString(Type),
        Owner ? Owner->GetName() : "없음");
    return FRotator::ZeroRotator;
}
#pragma endregion