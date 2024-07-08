// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "AbilitySystemInterface.h"

#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"
#include "10_Monster/BaseMonsterAbilitySet.h"
#include "10_Monster/BaseMonsterAttributeSet.h"
#include "10_Monster/MonsterData.h"
#include "10_Monster/MonsterInterface_GAS.h"

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


UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	Patrol UMETA(DisplayName = "patrol"),
	Attack UMETA(DisplayName = "attack"),
	DetectingPlayer UMETA(DisplayName = "detectingPlayer") //플레이어 감지 중

};



USTRUCT(Atomic, BlueprintType)
struct FMonsterData_Struct : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	


};

class UBaseMonsterASC;
class UBaseGameplayAbility;

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


#pragma endregion

#pragma region Rotation

protected:
	virtual void ControllRotation();

#pragma endregion




#pragma region MonsterInterface

	//virtual UBaseMonsterASC* GetMonsterASC() override;


#pragma endregion

#pragma region Gameplay Ability System
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	TObjectPtr<UBaseMonsterASC> AbilitySystemComponent;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	TObjectPtr<UBaseMonsterAbilitySet> AbilitySet;

	UPROPERTY(VisibleAnywhere, Category = "Ability")
	TSoftObjectPtr<UBaseMonsterAttributeSet> AttributeSet;

	

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

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMonsterData> MonsterDataFile;

	UPROPERTY(EditAnywhere)
	EMonsterName MonsterName;

	UPROPERTY(VisibleAnywhere)
	FSingleMonsterData SingleMonsterData;

#pragma endregion


#pragma region Attribute Callbacks
public:
	/*
	* @목적 : 캐릭터의 Attribute 수치 변화 이벤트에 등록할 콜백 함수
	* @설명 : Ability System Component에서 관리하는 Attribute 항목의 수치 변화 이벤트에 등록할 콜백 함수입니다.
	*		  HUD 구현을 위해 PS에서 제공하는 AttributeBase 관련 인터페이스로 활용 가능합니다(C++환경).
	* @참조 : APlayerStateBase::InitializeGameplayAbilitySystem()
	*/
	void OnAttributeValueChanged(FOnAttributeChangeData& Data);

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
	FAnyMonsterAttributeValueChanged OnAnyMonsterAttributeValueChanged;
#pragma endregion


};
