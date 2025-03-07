#include "ANS_AIMotionWarpWithPreserveZ.h"
#include "Logging/StructuredLog.h"

#include "MotionWarpingComponent.h"

DEFINE_LOG_CATEGORY(LogMotionWarpWithPreserveZ)


FTransform URootMotionModifier_SkewWarpWithZ::ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds)
{
    // 부모 클래스의 처리를 가져옴
    FTransform ProcessedRootMotion = Super::ProcessRootMotion(InRootMotion, DeltaSeconds);

    if (bPreserveZAxis)
    {
        // 원래 Z값 추출
        float OriginalZTranslation = InRootMotion.GetTranslation().Z;

        // 처리된 X, Y값 추출
        float ProcessedXTranslation = ProcessedRootMotion.GetTranslation().X;
        float ProcessedYTranslation = ProcessedRootMotion.GetTranslation().Y;

        // 원래 루트 모션의 Z 변위를 보존한 새 변환 설정
        ProcessedRootMotion.SetTranslation(
            FVector(ProcessedXTranslation, ProcessedYTranslation, OriginalZTranslation)
        );

        // 로그 기록
        UE_LOGFMT(LogMotionWarpWithPreserveZ, Log, "Z축 보존 모드: 원본 Z={0}, 최종 변환={1}",
            OriginalZTranslation, ProcessedRootMotion.GetTranslation().ToString());
    }

    return ProcessedRootMotion;
}

UANS_AIMotionWarpWithPreserveZ::UANS_AIMotionWarpWithPreserveZ(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 커스텀 루트 모션 모디파이어 생성
    RootMotionModifier = ObjectInitializer.CreateDefaultSubobject<URootMotionModifier_SkewWarpWithZ>(this, TEXT("RootMotionModifier_SkewWarpWithZ"));

    // 기본값 설정
    bPreserveZAxis = true;

    UE_LOGFMT(LogMotionWarpWithPreserveZ, Log, "Z축 보존 모션 워프 노티파이 생성됨");
}

URootMotionModifier* UANS_AIMotionWarpWithPreserveZ::AddRootMotionModifier_Implementation(
    UMotionWarpingComponent* MotionWarpingComp,
    const UAnimSequenceBase* Animation,
    float StartTime,
    float EndTime) const
{
    // 유효성 검사
    if (!MotionWarpingComp)
    {
        UE_LOGFMT(LogMotionWarpWithPreserveZ, Warning, "모션 워핑 컴포넌트가 유효하지 않음");
        return nullptr;
    }

    if (!RootMotionModifier)
    {
        UE_LOGFMT(LogMotionWarpWithPreserveZ, Warning, "루트 모션 모디파이어가 유효하지 않음");
        return nullptr;
    }

    // 모디파이어 생성
    URootMotionModifier* NewModifier = MotionWarpingComp->AddModifierFromTemplate(RootMotionModifier, Animation, StartTime, EndTime);

    // Z축 보존 설정 적용
    if (URootMotionModifier_SkewWarpWithZ* CustomModifier = Cast<URootMotionModifier_SkewWarpWithZ>(NewModifier))
    {
        CustomModifier->bPreserveZAxis = bPreserveZAxis;
        UE_LOGFMT(LogMotionWarpWithPreserveZ, Log, "Z축 보존 모드 설정: {0}",
            bPreserveZAxis ? TEXT("활성화") : TEXT("비활성화"));
    }
    else
    {
        UE_LOGFMT(LogMotionWarpWithPreserveZ, Warning, "커스텀 모디파이어 캐스팅 실패");
    }

    return NewModifier;
}