// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "MonsterData.generated.h"


UENUM(BlueprintType)
enum class EMonsterName : uint8
{
	Default UMETA(DisplayName = "Default"),
	Wolf UMETA(DisplayName = "Wolf"),
	WolfBoss UMETA(DisplayName = "WolfBoss"),
	Human UMETA(DisplayName = "Human"),
	HumanBoss UMETA(DisplayName = "HumanBoss")

};

//�� Ÿ�Ը��� �⺻ ���ݷ�, ������� �ٸ�. 
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

//�������Ʈ���� �� Ÿ�Ը��� �Է����� ���� ����(���ݷ�, �����). ���� ������ 2���ۿ� ���ٸ� MonsterData�� ���Ĺ����°� ������. Ȥ�� Ÿ�Ը��� �߰��� ���� ������ ���� ���� ����Ͽ� ������. 
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


	//���� ��ų ID����
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
	TMap<int, FMonsterSkillData> MonsterSkillData; //FSkillData �����ϸ� �Է��� ������ ���� �����
	

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "SingleMonsterData")
	FString MonsterName = FString(TEXT(""));


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
