// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "02_AbilitySystem/BaseAbilitySet.h"
#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"
#include "06_Input/InputConfig.h"

#include "PawnData.generated.h"

/**
 * @���� : Pawn ������ �ʿ��� �������� �ѵ� ���� ���� Data Asset
 * @���� : Pawn�� ���õ� �����͵��� �ѵ� ����������ν�, ������ ������ �����Դϴ�.
 * @���� : -
 */
UCLASS()
class AGEOFWOLVES_API UPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// @���� : �ش� DA�� ���õ� Pawn�� BP Ŭ�����Դϴ�. (eg. BP_AkaOni)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data|Pawn")
		TSubclassOf<APawn> PawnClass;

	// @���� : �ش� Pawn�� ���õ� Ability Set ������ ���� Data Asset�Դϴ�.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data| Ability Set")
		TObjectPtr<UBaseAbilitySet> AbilitySet;

	// @���� : �ش� Pawn�� ���õ� Ability Tag ���輺 ������ ���� Data Asset�Դϴ�.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data| Ability Tag Relationship Mapping")
		TObjectPtr<UAbilityTagRelationshipMapping> TagRelationshipMapping;

	// @���� : �ش� Pawn�� ���õ� Enhanced Input �ý��� ������ ���� Data Asset�Դϴ�.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data | Input Config")
		TObjectPtr<UInputConfig> InputConfig;

};
