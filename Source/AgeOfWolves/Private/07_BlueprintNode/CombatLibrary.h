// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

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

#pragma endregion

//@구조체
#pragma region Structs
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
