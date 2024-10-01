// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"

#include "10_Monster/MonsterData.h"
#include <AbilitySystemBlueprintLibrary.h>

#include "02_AbilitySystem/BaseAbilitySet.h"

#include "BaseMonster.generated.h" 



/*
* @목적 : ASC에 등록된 AttributeSet의 각 Attribute 값의 초기화 이벤트
* @설명 : Attribute 값 초기화 이벤트 발생 시 이를 UI 등 다양한 곳에 알리기 위함
* @참조 : -
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMonsterAttributeSetInitialized);

/*
* @목적 : ASC에 등록된 AttributeSet의 각 Attribute 값 변화 이벤트를 전파하는 이벤트
* @설명 : Attribute 값 변화 이벤트 발생 시 이를 UI 등 다양한 곳에 알리기 위함
* @참조 : -
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAnyMonsterAttributeValueChanged, FGameplayAttribute, Attribute, float, OldValue, float, NewValue);

class UBaseAbilitySet;
class UBaseAttributeSet;
class UBaseAbilitySystemComponent;
class UBaseGameplayAbility;

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	Patrol UMETA(DisplayName = "patrol"),
	Strafe UMETA(DisplayName = "strafe"),
	Attacking UMETA(DisplayName = "attacking"), //공격 애니메이션 재생 중
	DetectingPlayer UMETA(DisplayName = "detectingPlayer"), //플레이어 감지 중
	Stunned UMETA(DisplayName = "stunned")

};



UCLASS()
class AGEOFWOLVES_API ABaseMonster : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseMonster();

protected:


	virtual void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void PossessedBy(AController* NewController) override;


#pragma region State

protected:
	//현재 상태 저장
	EMonsterState CurrentState;

public:
	UFUNCTION(BlueprintCallable)
	EMonsterState GetCurrentState();
	/*
	* @목적 : State를 변경하고 WhenEndState를 실행해야 하는데 하나의 함수로 하기 위해서 제작.
	* @설명 : State를 바꾸고 스테이트가 끝날 때 각 스테이트마다 실행해줘야 하는 내용을 담은 함수인 WhenEndState를 호출한다.
	* @예시 : AttackMontage 마지막에 State를 Patrol중이었을 때 DetectingPlayer로 바꾸게 되는 상황이라면 
	*/
	UFUNCTION(BlueprintCallable)
	void ChangeState(EMonsterState inValue);

	/*
	* @목적 : 특정 State가 끝났을 때 해줘야 하는 작업들을 담은 함수.
	* @설명 : switch로 각 State마다 코드를 작성한다. 자식 클래스에서 override해서 사용할 수 있도록 한다.
	*/
	UFUNCTION(BlueprintCallable)
	virtual void WhenEndState();


#pragma endregion

#pragma region CombatSystem
public:

#pragma endregion

#pragma region Rotation

protected:
	virtual void ControllRotation();

#pragma endregion


#pragma region MotionWarping
public:
	//MotionWarping
	UPROPERTY(BlueprintReadWrite, Category = "MotionWarping")
	TObjectPtr<class UMotionWarpingComponent> MotionWarpingComp;


	/*
	* @목적 : 모션워핑을 사용하는 애니메이션 재생 시 바인드해줄 MontageEnded 함수
	* @설명 : Ability System Component에서 관리하는 Attribute 항목의 수치 변화 이벤트에 등록할 콜백 함수입니다.
	*		  HUD 구현을 위해 PS에서 제공하는 AttributeBase 관련 인터페이스로 활용 가능합니다(C++환경).
	* @참조 : APlayerStateBase::InitializeGameplayAbilitySystem()
	*/
	UFUNCTION()
	void OnWarpMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	void SetWarpTarget(FName name, FVector vector);

#pragma endregion




#pragma region Gameplay Ability System
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TObjectPtr<UBaseAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UAbilityTagRelationshipMapping> TagRelationship;

protected:


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UBaseAbilitySet> AbilitySet;

	UPROPERTY(VisibleAnywhere, Category = "Ability")
	TSoftObjectPtr<UBaseAttributeSet> AttributeSet;

	//UPROPERTY(VisibleAnywhere, Category = "Ability")
	FBaseAbilitySet_GrantedHandles* SetGrantedHandles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	TArray<TSubclassOf<UBaseGameplayAbility>> GameplayAbilities;

	

	UPROPERTY()
	uint8 bAbilitiesInitialized : 1;

public:
	/*
	* @설명: Possess됐을 시 어트리뷰트, 어빌리티, 이펙트 ASC에 등록.
	*/
	UFUNCTION()
	void InitializeGameplayAbilitySystem();



public:
	/*
	* @설명 : IAbilitySystemInterface 인터페이스 클래스의 인터페이스 함수를 오버라이딩 합니다.
	*/
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;



#pragma endregion


#pragma region Data

public:


	UPROPERTY(EditAnywhere)
	EMonsterName MonsterName;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FSingleMonsterData SingleMonsterData;

	UFUNCTION(BlueprintCallable)
	FSingleMonsterData GetSingleMonsterData();


#pragma endregion


#pragma region Attribute Callbacks
public:
	/*
	* @목적 : 캐릭터의 Attribute 수치 변화 이벤트에 등록할 콜백 함수
	* @설명 : Ability System Component에서 관리하는 Attribute 항목의 수치 변화 이벤트에 등록할 콜백 함수입니다.
	*		  HUD 구현을 위해 PS에서 제공하는 AttributeBase 관련 인터페이스로 활용 가능합니다(C++환경).
	* @참조 : APlayerStateBase::InitializeGameplayAbilitySystem()
	*/
	void OnAttributeValueChanged(const FOnAttributeChangeData& Data);

public:
	/*
	* @목적 : ASC에 등록된 AttributeSet의 각 Attribute 값 초기화 이벤트를 전파하는 이벤트
	* @설명 : Attribute 값 변화 이벤트 발생 시 이를 UI 등 다양한 곳에 알리기 위함
	* @참조 : -
	*/
	FMonsterAttributeSetInitialized OnMonsterAttributeSetInitialized;
	/*
	* @목적 : ASC에 등록된 AttributeSet의 각 Attribute 값 변화 이벤트를 전파하는 이벤트
	* @설명 : Attribute 값 변화 이벤트 발생 시 이를 UI 등 다양한 곳에 알리기 위함
	* @참조 : -
	*/
	FAnyMonsterAttributeValueChanged OnAnyMonsterAttributeValueChanged; //기믹과 같이 값이 바뀔 때마다 체크해줘야 하는 것이 있다면 함수를 AddDynamic으로 추가하기
#pragma endregion


};
