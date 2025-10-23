// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CameraAngleSettings.generated.h"

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형

#pragma region Enums
UENUM(BlueprintType)
enum class ECameraAngle : uint8
{
	BackView	   UMETA(DisplayName = "BackView"),
	LeftShoulder   UMETA(DisplayName = "LeftShoulder"),
	RightShoulder  UMETA(DisplayName = "RightShoulder"),
	CloseUp        UMETA(DisplayName = "CloseUp"),
	BustShot       UMETA(DisplayName = "BustShot"),
	FullShot       UMETA(DisplayName = "FullShot"),
	WideShot       UMETA(DisplayName = "WideShot"),
	LeftSide       UMETA(DisplayName = "LeftSide"),
	RightSide      UMETA(DisplayName = "RightSide"),
	HighAngle      UMETA(DisplayName = "HighAngle"),
	MAX
};
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion


/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UCameraAngleSettings : public UPrimaryDataAsset
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion
	
	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	TMap<ECameraAngle, FName> GetCameraAngleNameMap() { return CameraAngleNameMap;}
protected:
	//@ UDynamicCameraComponent에서 SpringArm 매핑 시킴 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CameraAngle")
	TMap<ECameraAngle, FName> CameraAngleNameMap;
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
