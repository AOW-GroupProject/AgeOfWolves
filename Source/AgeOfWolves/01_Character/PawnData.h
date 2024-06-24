// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "02_AbilitySystem/BaseAbilitySet.h"
#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"
#include "06_Input/InputConfig.h"

#include "PawnData.generated.h"

/**
 * @목적 : Pawn 생성에 필요한 정보들을 한데 묶어 놓은 Data Asset
 * @설명 : Pawn과 관련된 데이터들을 한데 묶어놓음으로써, 수월한 관리를 위함입니다.
 * @참조 : -
 */
UCLASS()
class AGEOFWOLVES_API UPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPawnData(const FObjectInitializer& ObjectInitializer);

public:

	// @설명 : 해당 DA와 관련된 Pawn의 BP 클래스입니다. (eg. BP_AkaOni)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data|Pawn")
		TSubclassOf<APawn> PawnClass;

	// @설명 : 해당 Pawn과 관련된 Ability Set 정보를 담은 Data Asset입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data| Ability Set")
		TObjectPtr<UBaseAbilitySet> AbilitySet;

	// @설명 : 해당 Pawn과 관련된 Ability Tag 관계성 정보를 담은 Data Asset입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data| Ability Tag Relationship Mapping")
		TObjectPtr<UAbilityTagRelationshipMapping> TagRelationshipMapping;

	// @설명 : 해당 Pawn과 관련된 Enhanced Input 시스템 정보를 담은 Data Asset입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data | Input Config")
		TObjectPtr<UInputConfig> InputConfig;

};
