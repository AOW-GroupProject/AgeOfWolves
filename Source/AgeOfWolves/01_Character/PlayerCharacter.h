#pragma once

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
class UDynamicCameraComponent;
class USpringArmComponent;
class UCameraComponent;
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
	// (현재 사용되지 않음)

protected:
	//@외부 바인딩
	// (현재 사용되지 않음)

protected:
	//@초기화
	// (현재 사용되지 않음)

#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void UpdateFacingTarget();

protected:
	/** 백 카메라를 위한 스프링 암 컴포넌트 */
	UPROPERTY(VisibleAnywhere, Category = Camera, meta = (AllowPrivateAccess = "true"))
		USpringArmComponent* SA_Back;

	/** 플레이어를 따라가는 카메라 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FollowCamera;

	/** 인벤토리 관리 컴포넌트 */
	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UInventoryComponent* InventoryComponent;

	/** 락온 시스템 컴포넌트 */
	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		ULockOnComponent* LockComponent;

	/** 동적 카메라 제어 컴포넌트 */
	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UDynamicCameraComponent* DynamicCameraComponent;
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
	/** 카메라 컴포넌트 반환 */
	UFUNCTION(BlueprintCallable, Category = "Player Character")
		FORCEINLINE UCameraComponent* GetCameraComponent() const { return FollowCamera; }
	
	/** 스프링 암 컴포넌트 반환 */
	UFUNCTION(BlueprintCallable, Category = "Player Character")
		FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SA_Back; }
	
	/** 인벤토리 컴포넌트 반환 */
	UFUNCTION(BlueprintCallable, Category = "Player Character")
		FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	
	/** 락온 컴포넌트 반환 */
	UFUNCTION(BlueprintCallable, Category = "Player Character")
		FORCEINLINE ULockOnComponent* GetLockOnComponent() const { return LockComponent; }
	
	/** 동적 카메라 컴포넌트 반환 */
	UFUNCTION(BlueprintCallable, Category = "Player Character")
		FORCEINLINE UDynamicCameraComponent* GetDynamicCameraComponent() const { return DynamicCameraComponent; }
#pragma endregion

};
