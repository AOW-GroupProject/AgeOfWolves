#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"

#include "CombatLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCombatLibrary, Log, All);

//@전방 선언
#pragma region Forward Declaration
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
    Left    UMETA(DisplayName = "Left"),
    Right   UMETA(DisplayName = "Right"),
    Forward UMETA(DisplayName = "Forward"),
    Backward UMETA(DisplayName = "Backward"),
    Max     UMETA(DisplayName = "Max")
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

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    //@피격 반응 방향을 계산합니다.
    UFUNCTION(BlueprintCallable, Category = "Combat | 피격 반응")
        static EHitReactDirection CalculateHitDirection(const FVector& ImpactLocation, const AActor* HitActor);

    //@피격 반응 방향을 계산합니다.
    UFUNCTION(BlueprintCallable, Category = "Combat | 피격 반응")
        static FHitDirectionResult  CalculateHitDirectionWithHitResult(const AActor* Instigator, const FHitResult& HitResult);
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
