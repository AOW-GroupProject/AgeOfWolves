#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"

#include "ANS_ActivateAfterImageFX.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogANS_AfterImageFX, Log, All);

//@전방 선언
#pragma region Forward Declaration
class USkeletalMeshComponent;
class UAnimSequenceBase;
class ACharacterBase;
#pragma endregion

/**
 *	@UANS_ActivateAfterImageFX
 *
 *	애니메이션 특정 재생 구간 동안 캐릭터의 SkeletalFXComponent를 활성화합니다.
 */
UCLASS(meta = (DisplayName = "잔상 효과 활성화"))
class AGEOFWOLVES_API UANS_ActivateAfterImageFX : public UAnimNotifyState
{
    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    //@생성자
    UANS_ActivateAfterImageFX();

public:
    //@애니메이션 노티파이 인터페이스 구현
    virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
    virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
    //@캐릭터 베이스 참조 획득
    ACharacterBase* GetCharacterBase(USkeletalMeshComponent* MeshComp);
#pragma endregion

    //@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    virtual FString GetNotifyName_Implementation() const override;
#pragma endregion
};