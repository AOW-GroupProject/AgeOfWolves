#include "ANS_PlayingStopAnimation.h"

#include "Animation/AnimInstance.h"
#include "05_Animation/BaseAnimInstance.h"

//@Defualt Setting
#pragma region Default Setting
void UANS_PlayingStopAnimation::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
	{
		if (auto BaseAnimInstance = Cast<UBaseAnimInstance>(AnimInstance))
		{
			BaseAnimInstance->SetIsPlayingRootMotionMontageWithFullBodySlot(true);
		}
	}
}

void UANS_PlayingStopAnimation::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
	{
		if (auto BaseAnimInstance = Cast<UBaseAnimInstance>(AnimInstance))
		{
			BaseAnimInstance->SetIsPlayingRootMotionMontageWithFullBodySlot(false);
		}
	}
}

FString UANS_PlayingStopAnimation::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("정지 애니메이션 진행 중 이동 불가"));

}
#pragma endregion