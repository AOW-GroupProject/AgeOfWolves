// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AN_ApplyGEDuringAnimPlaying.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogAN_ApplyGEDuringAnimPlaying, Log, All)

//@���� ����
#pragma region Forward Declaration
class UGameplayEffect;
#pragma endregion

//@������
#pragma region Enums
#pragma endregion

//@����ü
#pragma region Structs
#pragma endregion

//@�̺�Ʈ/��������Ʈ
#pragma region Delegates
#pragma endregion

/**
 *	@UAN_ApplyGEDuringAnimPlaying
 * 
 *	Anim Montage �÷��� �� GE�� ������ �� Ȱ���ϴ� Anim Notify
 */
UCLASS()
class AGEOFWOLVES_API UAN_ApplyGEDuringAnimPlaying : public UAnimNotify
{
	GENERATED_BODY()
	
//@Defualt Setting
#pragma region Default Setting
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@GE�� Blueprint Ŭ����
	UPROPERTY(EditAnywhere, Category = "Apply Gameplay Effect Ŭ����")
		TSubclassOf<UGameplayEffect> ApplyGEClass;
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
