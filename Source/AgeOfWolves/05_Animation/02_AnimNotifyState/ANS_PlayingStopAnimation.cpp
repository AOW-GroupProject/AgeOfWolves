#include "ANS_PlayingStopAnimation.h"
#include "Animation/AnimInstance.h"
#include "05_Animation/BaseAnimInstance.h"
// BaseAnimInstance 헤더 include 필요

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