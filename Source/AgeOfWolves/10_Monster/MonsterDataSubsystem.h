// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "MonsterData.h"

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MonsterDataSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UMonsterDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

#pragma region Default

public:

    
    UMonsterDataSubsystem(const FObjectInitializer& ObjectInitializer);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    virtual void Deinitialize() override;

#pragma endregion
    
#pragma region Data
public:
    // @설명: 각 캐릭터에게서 이름을 받아와 해당 MonsterData를 복사해준다.
	// 
    void CustomFunction(EMonsterName name, FSingleMonsterData &SingleMonsterData, UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles);


    TMap<EMonsterName, FSingleMonsterData> MonsterData;

	/*
	* @목적 : 상대가 적인지 아군인지 판단해주는 함수입니다.
	* @설명 : FSingleMonsterData의 EnemyNameList와 FriendNameList를 사용합니다.
	* @참조 : -
	*/
	void DecideEnemyOrElse(EMonsterName OwnerName, EMonsterName TargetName, bool &IsEnemy, bool &IsFriend);

	UFUNCTION(BlueprintCallable, Category = "Data")
	void AddEnemyName(EMonsterName OwnerName, EMonsterName EnemyName);

	UFUNCTION(BlueprintCallable, Category = "Data")
	void AddFriendName(EMonsterName OwnerName, EMonsterName FriendName);


#pragma endregion

#pragma region Ability

	/*
	* @목적 : 사용자 캐릭터의 기본 AttributeSet, Gameplay Ability, Gameplay Effect를 해당 캐릭터 객체의 ASC에 등록하는 함수들입니다.
	* @설명 : 각자의 방식으로 ASC에 등록한 후, 해당 등록 이벤트 관련한 Out Param으로 전달 받은 GrantHandle을 반환합니다.
	* @참조 : -
	*/
	void GiveStartupAttributeSetToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles,TArray<FBaseAbilitySet_AttributeSet> &AttributeSets) const;
	void GiveStartupGameplayEffectToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, TArray<FBaseAbilitySet_GameplayEffect> &GameplayEffects) const;
	void GiveStartupGameplayAbilityToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, TArray<FBaseAbilitySet_GameplayAbility> &GameplayAbilities) const;

#pragma endregion
};
