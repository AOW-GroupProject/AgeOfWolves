// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "18_Structure/StructureBase.h"
#include "StructureSavePoint.generated.h"


//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
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
class AGEOFWOLVES_API AStructureSavePoint : public AStructureBase
{
	//@친추 클래스
#pragma region Friend Class
#pragma endregion
	
	GENERATED_BODY()

	//@Defualt Setting
#pragma region Default Setting
public:
	AStructureSavePoint();

public:
	virtual void SetStructureActive(bool IsActive) override;
	
#pragma endregion


#pragma region Property or Subwidgets or Infos...etc
	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StructureSavePointActivatedMesh;
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
