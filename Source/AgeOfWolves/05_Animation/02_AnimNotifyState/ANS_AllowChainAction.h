#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"

#include "ANS_AllowChainAction.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_AllowChainAction, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UBaseAbilitySystemComponent;
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
 *  @UANS_AllowChainAction
 * 
 *  Chain Action을 특정 구간 동안 허용하는 Anim Notify State를 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UANS_AllowChainAction : public UAnimNotifyState
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UANS_AllowChainAction();

protected:
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    //@체인 시스템 활성화 도중에 입력 받을 어빌리티 태그
    UPROPERTY(EditAnywhere, Category = "Chain Action")
        FGameplayTag TagToChain;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
    UPROPERTY()
        TWeakObjectPtr<UBaseAbilitySystemComponent> BaseASCRef;
#pragma endregion
};