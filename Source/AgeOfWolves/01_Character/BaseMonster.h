// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "AbilitySystemInterface.h"

#include "10_Monster/MonsterData.h"

#include "BaseMonster.generated.h" 


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

class UBaseAbilitySystemComponent;

UCLASS()
class AGEOFWOLVES_API ABaseMonster : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

#pragma region State

protected:
	//현재 상태 저장
	EMonsterState CurrentState;


#pragma endregion

#pragma region Rotation

protected:
	virtual void ControllRotation();

#pragma endregion


public:	

	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region Gameplay Ability System
protected:
	UPROPERTY()
	UBaseAbilitySystemComponent* AbilitySystemComponent;
public:
	/*
	* @목적 : IAbilitySystemInterface 인터페이스 클래스의 인터페이스 함수를 오버라이딩 합니다.
	* @설명 : -
	* @참조 : -
	*/
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;
#pragma endregion

#pragma region Data

protected:
	/*
	* @설명 : 몬스터 데이터 초기화해주는 함수.
	* @참조 : -
	*/
	virtual void InitializeMonsterData();
public:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UMonsterData> MonsterDataFile;

	UPROPERTY(EditAnywhere)
	EMonsterName MonsterName;

	UPROPERTY(VisibleAnywhere)
	FSingleMonsterData SingleMonsterData;

#pragma endregion

	


};
