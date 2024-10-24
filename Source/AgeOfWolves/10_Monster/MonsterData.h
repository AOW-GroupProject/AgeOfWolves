// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"

#include "02_AbilitySystem/BaseAbilitySet.h"

#include "MonsterData.generated.h"

// @수정: 태그 사용 가능
UENUM(BlueprintType)
enum class EMonsterName : uint8
{
	Default UMETA(DisplayName = "Default"), 
	Player UMETA(DisplayName = "Player"), //BaseMonster로 캐스팅이 안됐을 시 이걸로 전달
	Wolf UMETA(DisplayName = "Wolf"),
	WolfBoss UMETA(DisplayName = "WolfBoss"),
	Human UMETA(DisplayName = "Human"),
	HumanBoss UMETA(DisplayName = "HumanBoss")

};

//각 타입마다 기본 공격력, 생명력이 다름. 
UENUM(BlueprintType)
enum class EMonsterType : uint8
{
	Normal UMETA(DisplayName = "normal"),
	NormalA UMETA(DisplayName = "normalA"),
	Elite UMETA(DisplayName = "elite"),
	Boss UMETA(DisplayName = "boss")

};

UENUM(BlueprintType)
enum class ESkillType : uint8
{
	Normal UMETA(DisplayName = "normal"),
	Gimmick UMETA(DisplayName = "gimmick"),
	Passive UMETA(DisplayName = "passive")
};

//블루프린트에서 각 타입마다 입력해줄 고정 스텟(공격력, 생명력). 고정 스탯이 2개밖에 없다면 MonsterData에 합쳐버리는게 나을듯. 혹시 타입마다 추가될 고정 스탯이 있을 것을 고려하여 구분함. 
USTRUCT(Atomic, BlueprintType)
struct FPermanentStat : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	float PDamageStat = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	float PHealthStat = 0.f;
};

USTRUCT(Atomic, BlueprintType)
struct FMonsterSkillData
{
	GENERATED_USTRUCT_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	int SkillID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	FGameplayTagContainer SkillTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	FString SkillName = FString(TEXT(""));

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	UAnimMontage* Montage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	ESkillType SkillType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	float SkillProbability;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	int SkillOrder;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	int SkillCoolDown;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	float MaxDistance;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	float MinDistance;


	//연계 스킬 ID저장
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	int ConnectedSkillID;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	float ConnectedSkillProbability;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillData")
	FString SkillDescription;


};

USTRUCT(Atomic, BlueprintType)
struct FSingleMonsterData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SingleMonsterData")
	EMonsterType MonsterType;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SingleMonsterData")
	TMap<int, FMonsterSkillData> MonsterSkillData; //FSkillData 수정하면 입력한 데이터 전부 사라짐

	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = GameplayAbility))
	TArray<FBaseAbilitySet_GameplayAbility> GameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta = (TitleProperty = GameplayEffect))
	TArray<FBaseAbilitySet_GameplayEffect> GameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta = (TitleProperty = AttributeSet))
	TArray<FBaseAbilitySet_AttributeSet> AttributeSets;
	

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SingleMonsterData")
	FString MonsterName = FString(TEXT(""));

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SingleMonsterData")
	FPermanentStat PermanentStat;

	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SingleMonsterData")
	TArray<EMonsterName> EnemyNameList;

	//EnemyName과 FriendName에도 없다면 중립 반환, Friend는 같은 팀을 돕는 몬스터를 위해 제작해놓음
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SingleMonsterData")
	TArray<EMonsterName> FriendNameList;




};


/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UMonsterData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UMonsterData(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "MonsterData")
	TMap<EMonsterName, FSingleMonsterData> MonsterData;
	
};
