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

	/*
	* @목적: 해당 GA와 다른 GA 간의 관계성을 정의하는 FGmaeplayTagContainer 목록을 담은 Data Asset
	* @설명: 해당 GA의 활성화 조건의 기반이 되는 Gameplay Tag 관계성 목록입니다. 이를 통해, 해당 GA의 활성화 혹은 논리적 관계가 있는 다른 GA의 취소/블락 조건을 확인합니다.
	* @참조: AbilityTagRelationshipMapping.h
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data| Ability Tag Relationship Mapping")
		TObjectPtr<UAbilityTagRelationshipMapping> TagRelationship;

	// @설명 : 해당 Pawn과 관련된 Enhanced Input 시스템 정보를 담은 Data Asset입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data | Input Config")
		TObjectPtr<UInputConfig> InputConfig;

};
