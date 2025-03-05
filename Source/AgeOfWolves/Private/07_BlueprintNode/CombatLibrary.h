#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

#include "CombatLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCombatLibrary, Log, All);

//@전방 선언
#pragma region Forward Declaration
class AActor;
struct FHitResult;
#pragma endregion

//@열거형
#pragma region Enums
/*
*   @EHitReactDirection
*
*   피격 반응 방향
*/
UENUM(BlueprintType)
enum class EHitReactDirection : uint8
{
    Left     UMETA(DisplayName = "Left"),
    Right    UMETA(DisplayName = "Right"),
    Forward  UMETA(DisplayName = "Forward"),
    Backward UMETA(DisplayName = "Backward"),
    FL       UMETA(DisplayName = "Forward Left"), 
    FR       UMETA(DisplayName = "Forward Right"),
    Max      UMETA(DisplayName = "Max")
};

/*
*   @EHitHeight
*
*   공격 높이 결정
*/
UENUM(BlueprintType)
enum class EHitHeight : uint8
{
    Upper    UMETA(DisplayName = "Upper"),
    Middle   UMETA(DisplayName = "Middle"),
    Lower    UMETA(DisplayName = "Lower"),
    Max      UMETA(Hidden)
};

/*
*   @EHitImpactLocation
*
*   공격 받은 위치
*/
UENUM(BlueprintType)
enum class EHitImpactLocation : uint8
{
    Center    UMETA(DisplayName = "Center"),
    Front     UMETA(DisplayName = "Front"),
    Back      UMETA(DisplayName = "Back"),
    Left      UMETA(DisplayName = "Left"),
    Right     UMETA(DisplayName = "Right"),
    Max       UMETA(Hidden)
};
#pragma endregion

//@구조체
#pragma region Structs
/*
*   @FHitDirectionResult
*
*   피격 방향 관련 정보를 담아놓은 구조체
*/
USTRUCT(BlueprintType)
struct FHitDirectionResult
{
    GENERATED_BODY()

        FHitDirectionResult()
        : Direction(EHitReactDirection::Max)
        , Height(EHitHeight::Max)
        , ImpactLocation(EHitImpactLocation::Max)
    {
    }

    UPROPERTY(BlueprintReadOnly, Category = "Hit Direction")
        EHitReactDirection Direction;

    UPROPERTY(BlueprintReadOnly, Category = "Hit Direction")
        EHitHeight Height;

    UPROPERTY(BlueprintReadOnly, Category = "Hit Direction")
        EHitImpactLocation ImpactLocation;

    // Getter 함수들
    EHitReactDirection GetDirection() const { return Direction; }
    EHitHeight GetHeight() const { return Height; }
    EHitImpactLocation GetImpactLocation() const { return ImpactLocation; }

    // 디버그 정보 출력을 위한 ToString 함수
    FString ToString() const
    {
        return FString::Printf(TEXT("Direction: %d, Height: %d, ImpactLocation: %d"),
            static_cast<uint8>(Direction),
            static_cast<uint8>(Height),
            static_cast<uint8>(ImpactLocation));
    }
};

/*
*   @FSlashGameplayCueParams
* 
*   Slash 이팩트를 다루는 Gameplay Cue의 Execute 를 위해 필요한 정보를 담은 구조체
*/
USTRUCT(BlueprintType)
struct FSlashGameplayCueParams
{
    GENERATED_BODY()

        UPROPERTY(BlueprintReadOnly)
        FRotator SlashRotation;

    UPROPERTY(BlueprintReadOnly)
        FVector SpawnLocation;

    UPROPERTY(BlueprintReadOnly)
        FVector ImpactNormal;
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UCombatLibrary
 *
 *	전투 시스템에서 활용 가능한 블루프린트 노드를 정의합니다.
 */
UCLASS()
class UCombatLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

 //@Defualt Setting
#pragma region Default Setting
public:
    //@피격 반응 방향을 계산합니다.
    UFUNCTION(BlueprintCallable, Category = "Combat | 피격 반응")
        static EHitReactDirection CalculateHitDirection(const FVector& ImpactLocation, const AActor* HitActor);

    //@피격 반응 방향을 계산합니다.
    UFUNCTION(BlueprintCallable, Category = "Combat | 피격 반응")
        static FHitDirectionResult CalculateHitDirectionWithHitResult(const AActor* Instigator, const FHitResult& HitResult);

public:
    /**
     * 대상 액터에게 GameplayEvent를 전송합니다.
     * @param EventTag - 전송할 이벤트의 태그
     * @param TargetActor - 이벤트를 받을 대상 액터
     * @param InstigatorActor - 이벤트를 발생시킨 액터
     * @param HitResult - 히트 결과 (옵션)
     * @param Magnitude - 이벤트의 크기값 (옵션)
     * @param OptionalObject - 추가 전달할 오브젝트 (옵션)
     * @param OptionalObject2 - 추가 전달할 오브젝트2 (옵션)
     * @return bool - 이벤트 전송 성공 여부
     */
    UFUNCTION(BlueprintCallable, Category = "Combat | Gameplay Event", Meta = (GameplayTagFilter = "EventTag"))
        static bool SendGameplayEventToTarget(
            FGameplayTag EventTag,
            AActor* TargetActor,
            AActor* InstigatorActor,
            const FHitResult& HitResult = FHitResult(),
            float Magnitude = 0.0f,
            UObject* OptionalObject = nullptr,
            UObject* OptionalObject2 = nullptr);

public:
    /**
     * @brief 슬래시 GameplayCue 실행에 필요한 파라미터들을 준비합니다.
     * @param InActor - 이펙트를 실행할 액터
     * @param HitResult - 공격 히트 결과
     * @return 계산된 GameplayCue 파라미터들
     */
    UFUNCTION(BlueprintCallable, Category = "Combat | GameplayCue")
        static FSlashGameplayCueParams PrepareSlashGameplayCueParameters(AActor* InActor, const FHitResult& HitResult);
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
#pragma endregion
};