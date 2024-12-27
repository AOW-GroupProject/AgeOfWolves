#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AN_ApplyGameplayEffectToOwner.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAN_ApplyGameplayEffectToOwner, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UGameplayEffect;
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
 *	@UAN_ApplyGameplayEffectToOwner
 * 
 *	GE를 Owner에게 적용합니다.
 */
UCLASS()
class AGEOFWOLVES_API UAN_ApplyGameplayEffectToOwner : public UAnimNotify
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@GE의 Blueprint 클래스
	UPROPERTY(EditAnywhere, Category = "Apply Gameplay Effect 클래스")
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
