// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "ANS_Deceleration.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_Deceleration, Log, All)

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDecelerationStateChanged, bool, bIsDecelerating);
#pragma endregion

/**
 *	@UANS_Deceleration
 *
 *	감속 적용을 위한 Anim Notify State를 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UANS_Deceleration : public UAnimNotifyState
{
	//@친구 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

		//@Defualt Setting
#pragma region Default Setting
public:
	UANS_Deceleration();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

	//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	// 감속 상태 변경 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "감속")
		FOnDecelerationStateChanged OnDecelerationStateChanged;

private:
	bool bIsDecelerating;
#pragma endregion

	//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	FORCEINLINE bool IsDecelerating() const { return bIsDecelerating; }
#pragma endregion
};