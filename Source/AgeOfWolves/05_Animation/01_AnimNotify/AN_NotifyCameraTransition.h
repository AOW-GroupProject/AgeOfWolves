// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "04_Component/DynamicCameraComponent.h"
#include "AN_NotifyCameraTransition.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAN_NotifyCameraTransition, Log, All)

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
 *	@UAN_NotifyCameraTransition
 * 
 *	Camera 전환 데이터를 알립니다.
 */
UCLASS()
class AGEOFWOLVES_API UAN_NotifyCameraTransition : public UAnimNotify
{
	//@친추 클래스
#pragma region Friend Class
#pragma endregion
	
	GENERATED_BODY()
	//@Defualt Setting
#pragma region Default Setting
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Transition")
	FCameraTransitionInfo TransitionInfo;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

	//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
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