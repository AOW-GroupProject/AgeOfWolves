// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"

#include "BaseAnimInstance.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAnimInstance, Log, All)

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
/*
*	@EMovementState
* 
*	캐릭터 이동 상태의 종류를 정의하는 열거형
*/
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle = 0	UMETA(DisplayName = "Idle"),
	Walk		UMETA(DisplayName = "Walk"),
	Run			UMETA(DisplayName = "Run"),
	Pivoting	UMETA(DisplayName = "Pivoting"),
	MAX			UMETA(DisplayName = "MAX"),
};

/*
*	@EMovementDirection
* 
*	캐릭터 이동 방향의 종류를 정의하는 열거형
*/
UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	Fwd = 0		UMETA(DisplayName = "Fwd"),
	Bwd			UMETA(DisplayName = "Bwd"),
	Left		UMETA(DisplayName = "Left"),
	Right		UMETA(DisplayName = "Right"),
	MAX			UMETA(DisplayName = "MAX"),
};
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion


/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UBaseAnimInstance : public UAnimInstance
{
//@친추 클래스
#pragma region Friend Class
	friend class UBaseGameplayAbility;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UBaseAnimInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@현재 Movement State를 찾습니다.
	UFUNCTION(BlueprintCallable)
		void FindMovementState();
	//@현재 Movement Direction을 찾습니다.
	UFUNCTION(BlueprintCallable)
		void FindMovementDirection();

protected:
	//@Combat State를 변경합니다.
	UFUNCTION(BlueprintCallable)
		void ChangeCombatState(bool bEnterCombat);

	//@캐릭터의 상반신 애니메이션을 업데이트합니다.
	void UpdateUpperBodyAnimation();

protected:
	UFUNCTION(BlueprintNativeEvent)
		void ChangeBoneTransform(float DeltaTime);
	virtual void ChangeBoneTransform_Implementation(float DeltaTime) { }

	UFUNCTION(BlueprintNativeEvent)
		void ClearBoneTransform(float DeltaTime);
	virtual void ClearBoneTransform_Implementation(float DeltaTime) { }

protected:
	//@이전 이동 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement State", meta = (AlloPrivateAccess = "true"))
		EMovementState LastMovementState = EMovementState::Idle;

	//@현재 이동 상태
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement State", meta = (AlloPrivateAccess = "true"))
		EMovementState MovementState = EMovementState::Idle;

	//@현재 이동 방향
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement Direction", meta = (AlloPrivateAccess = "true"))
		EMovementDirection MovementDirection = EMovementDirection::Fwd;

protected:
	//@현재 공중에 있는지 체크합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bFalling = false;
	//@현재 이동중인지 체크합니다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bShouldMove = false;
	//@현재 캐릭터에 가해진 가속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		FVector Velocity = FVector();
	//@현재 캐릭터의 속도
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		float Speed;
	//@락온 활성화 여부
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bLockOn = false;

	//@전투/비전투 여부
	UPROPERTY(Transient, EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool bIsCombatState = false;

protected:
	//@본 수정 여부
	UPROPERTY(Transient, BlueprintReadOnly)
		bool bModifyBoneTransform;
	//@보간 속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float BoneTransformLerpSpeed = 10.f;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	//@캐싱
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		TWeakObjectPtr<class ACharacterBase> OwnerCharacterBaseRef;

public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE EMovementState GetMovementState() const { return MovementState; }

	UFUNCTION(BlueprintCallable)
		FORCEINLINE EMovementDirection GetMovementDirection() const { return MovementDirection; }

public:
	FORCEINLINE void SetbLockOn(bool LockOn) { bLockOn = LockOn; }
#pragma endregion

};

