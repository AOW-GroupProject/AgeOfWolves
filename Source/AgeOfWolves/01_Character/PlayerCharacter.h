﻿#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"

#include "PlayerCharacter.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPlayer, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UInventoryComponent;
class UUIComponent;
class AController;
class UBaseAnimInstance;
class ULockOnComponent;
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
class AGEOFWOLVES_API APlayerCharacter : public ACharacterBase
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

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		ULockOnComponent* LockComponent;
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
public:
	UFUNCTION(BlueprintCallable)
		FORCEINLINE UCameraComponent* GetCameraComponent() { return FollowCamera; }
	UFUNCTION(BlueprintCallable)
		FORCEINLINE USpringArmComponent* GetSpringArmComponent() { return SpringArm; }
	UFUNCTION(BlueprintCallable)
		FORCEINLINE ULockOnComponent* GetLockOnComponent() { return LockComponent; }
#pragma endregion

};
