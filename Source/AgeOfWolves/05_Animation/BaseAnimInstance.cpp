#include "BaseAnimInstance.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogAnimInstance)
// UE_LOGFMT(LogAnimInstance, Log, "");

//@Defualt Setting
#pragma region Default Setting
UBaseAnimInstance::UBaseAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, LastMovementState(EMovementState::Idle)
	, MovementState(EMovementState::Idle)
	, MovementDirection(EMovementDirection::Fwd)
	, bFalling(false)
	, bShouldMove(false)
	, Velocity(FVector::ZeroVector)
	, Speed(0.0f)
	, bLockOn(false)
	, bIsCombatState(false)
	, bModifyBoneTransform(false)
	, BoneTransformLerpSpeed(10.0f)
{

	OwnerCharacterBaseRef.Reset();
}

void UBaseAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

}

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();


	if (const auto character = Cast<ACharacterBase>(TryGetPawnOwner()))
	{
		OwnerCharacterBaseRef = character;
	}
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!OwnerCharacterBaseRef.IsValid()) return;

	if (!OwnerCharacterBaseRef->GetController()) return;

	//@떨어지는 중인가 저장합니다.
	bFalling = OwnerCharacterBaseRef->GetMovementComponent()->IsFalling();
	//@캐릭터의 가속도를 가져옵니다.
	Velocity = OwnerCharacterBaseRef->GetVelocity();
	//@캐릭터으 현재 속도를 정의합니다.
	Speed = OwnerCharacterBaseRef->GetVelocity().Length();
	//@캐릭터의 이동/비이동 상태를 확인합니다.
	bShouldMove = Speed > 3.f && OwnerCharacterBaseRef->GetCharacterMovement()->GetCurrentAcceleration() != FVector::ZeroVector;
	//@캐릭터의 이동 상태를 정의합니다. EMovementState(열거형) 유형의 변수로 나타냅니다.
	FindMovementState();
	//@캐릭터의 이동 방향을 정의합니다.
	FindMovementDirection();
	//@Upper Body Slot 업데이트 여부
	if (bIsCombatState) UpdateUpperBodyAnimation();
}

#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UBaseAnimInstance::FindMovementState()
{

	// @FIX: 240 -> 350 상향
	// #1. 뛰기
	if (OwnerCharacterBaseRef->GetCharacterMovement()->MaxWalkSpeed > 400.f && Speed > 0.05f)
	{
		MovementState = EMovementState::Run;
		LastMovementState = MovementState;
	}
	// #2. 걷기
	else if (OwnerCharacterBaseRef->GetCharacterMovement()->MaxWalkSpeed > 0.f && OwnerCharacterBaseRef->GetCharacterMovement()->MaxWalkSpeed < 600.f && Speed > 10.f)
	{
		MovementState = EMovementState::Walk;
	}
	// #3. 정지
	else
	{
		MovementState = EMovementState::Idle;
	}
}

void UBaseAnimInstance::FindMovementDirection()
{
	//케릭터가 LockOn을 취소하거나, Run동안은 이동 방향을 Fwd로 설정.
	if (bLockOn == false || MovementState == EMovementState::Run)
	{
		MovementDirection = EMovementDirection::Fwd;
		return;
	}

	// 캐릭터의 현재 가속도 벡터를 기반으로 한 로테이션을 계산
	FRotator Rotation1 = UKismetMathLibrary::MakeRotFromX(OwnerCharacterBaseRef->GetCharacterMovement()->GetCurrentAcceleration());

	// 캐릭터가 바라보는 방향의 벡터를 기반으로 로테이션을 계산
	FRotator Rotation2 = OwnerCharacterBaseRef->GetControlRotation();

	// 두 방향 사이의 최소 각도 차이를 계산, 오른손 좌표계 기준 캐릭터의 Yaw 회전 값 기준 시계 방향은 음수, 반 시계 방향은 양수
	float Angle = FMath::FindDeltaAngleDegrees(Rotation1.Yaw, Rotation2.Yaw);

	// #1. Fwd
	if (Angle >= -70.f && Angle <= 70.f)
	{
		MovementDirection = EMovementDirection::Fwd;
	}
	// #2. Left
	else if (Angle > 70.f && Angle < 110.f)
	{
		MovementDirection = EMovementDirection::Left;
	}
	// #3. Right
	else if (Angle > -110.f && Angle < -70.f)
	{
		MovementDirection = EMovementDirection::Right;
	}
	// #4. Back
	else
	{
		MovementDirection = EMovementDirection::Bwd;
	}

}

void UBaseAnimInstance::ChangeCombatState(bool bEnterCombat)
{
	// 상태 변경이 필요한지 체크
	if (bIsCombatState == bEnterCombat)
	{
		UE_LOGFMT(LogAnimInstance, Warning, "전투 상태가 이미 {0} 상태입니다.",
			bEnterCombat ? TEXT("전투") : TEXT("비전투"));
		return;
	}

	// 상태 변경 및 로깅
	bIsCombatState = bEnterCombat;
	UE_LOGFMT(LogAnimInstance, Log, "전투 상태가 {0}(으)로 변경되었습니다.",
		bEnterCombat ? TEXT("전투") : TEXT("비전투"));
}

void UBaseAnimInstance::UpdateUpperBodyAnimation()
{
	// 전제 조건 체크
	if (!ensureMsgf(OwnerCharacterBaseRef.IsValid(),
		TEXT("상반신 애니메이션 업데이트 실패: 캐릭터가 유효하지 않습니다.")))
	{
		return;
	}

	// 애니메이션 업데이트 시작
	UE_LOGFMT(LogAnimInstance, Log, "상반신 애니메이션 업데이트 시작");

	//@TODO: Upper Body Slot의 상반신 애님 몽타주 재생

	UE_LOGFMT(LogAnimInstance, Log, "상반신 애니메이션 업데이트 완료");
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion