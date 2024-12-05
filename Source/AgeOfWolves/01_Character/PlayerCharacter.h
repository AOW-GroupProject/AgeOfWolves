// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"

#include "GenericTeamAgentInterface.h"
#include "CombatInterface.h"

#include "PlayerCharacter.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogPlayer, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UInventoryComponent;
class UUIComponent;
class AController;
class UBaseAnimInstance;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE_OneParam(FRequestStartInitByPlayerCharacter, const AController*);
#pragma endregion

/**
 * @APlayerCharacter
 * 
 * 사용자 캐릭터를 구현하는 ACharacterBase 유형의 객체
 */
UCLASS()
class AGEOFWOLVES_API APlayerCharacter : public ACharacterBase, public IGenericTeamAgentInterface, public ICombatInterface
{
	
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PawnClientRestart() override;

public:
	//@Interface of ICombatInterface
	virtual void Die() override;
	virtual void HitReact(FGameplayTag HitDirectionTag) override;
	//@End Of ICombatInterface

protected:
	//@내부 바인딩

protected:
	//@외부 바인딩

protected:
	//@초기화

#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateFacingTarget();

protected:
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UInventoryComponent* InventoryComponent;

	UPROPERTY(VisibleDefaultsOnly)
		class ULockOnComponent* LockOnComponent;

protected:
	public:
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
			int32 ID = 1;

protected:
	FGenericTeamId TeamId;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	FRequestStartInitByPlayerCharacter RequestStartInitByPlayerCharacter;
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	UPROPERTY()
		TObjectPtr<UBaseAnimInstance> AnimInstanceRef;

public:
	FORCEINLINE UCameraComponent* GetCameraComponent() { return FollowCamera; }
	FORCEINLINE USpringArmComponent* GetSpringArmComponent() { return SpringArm; }
	FORCEINLINE ULockOnComponent* GetLockOnComponent() { return LockOnComponent; }

public:
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
#pragma endregion

};
