#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"

#include "CharacterBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UBaseAttributeSet;
class UBaseAbilitySystemComponent;
class UMotionWarpingComponent;
class UNiagaraComponent;
class UBaseAnimInstance;
class UStaticMeshComponent;
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

UCLASS()
class AGEOFWOLVES_API ACharacterBase : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	ACharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	//~UObject interface
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual bool CanBeSeenFrom(
		const FVector& ObserverLocation, FVector& OutSeenLocation,
		int32& NumberOfLoSChecksPerformed, float& OutSightStrength,
		const AActor* IgnoreActor = nullptr, const bool* bWasVisible = nullptr,
		int32* UserData = nullptr) const;
	//~End Of UObject Interface
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Component | Skeletal FX")
		UNiagaraComponent* SkeletalFXComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Component | Motion Warp")
		UMotionWarpingComponent* MotionWarpComponent;

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* KatanaMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* SayaMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
		UStaticMeshComponent* FullMesh;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "팀 설정")
		FGameplayTag CharacterTag;

	UPROPERTY(EditDefaultsOnly, Category = "팀 설정")
		FGenericTeamId TeamId;

private:
	//@인지 타겟
	FName PerceptionTarget = FName("spine_02");
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
	UPROPERTY()
		TObjectPtr<UBaseAnimInstance> AnimInstanceRef;

protected:
	//@ASC 캐싱
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

public:
	UFUNCTION(BlueprintCallable)
		UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpComponent; }

public:
	FORCEINLINE UStaticMeshComponent* GetWeaponMesh() { return KatanaMesh; }
	FORCEINLINE UStaticMeshComponent* GetSayaMesh() { return SayaMesh; }
	FORCEINLINE UStaticMeshComponent* GetFullWeaponMesh() { return FullMesh; }
	FORCEINLINE UNiagaraComponent* GetSkeletalFXComponent() { return SkeletalFXComponent; }

public:
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
	void SetAbilitySystemComponent(UAbilitySystemComponent* ASC);

public:
	FORCEINLINE FGameplayTag GetCharacterTag() const { return CharacterTag; }

public:
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamId; }
	void SetTeamId(const FGenericTeamId& NewTeamId)
	{
		TeamId = NewTeamId;
	}
#pragma endregion
};

