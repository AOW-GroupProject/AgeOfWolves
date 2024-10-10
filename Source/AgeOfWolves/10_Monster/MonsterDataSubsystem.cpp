// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Monster/MonsterDataSubsystem.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "02_AbilitySystem/02_GameplayAbility/BaseGameplayAbility.h"

UMonsterDataSubsystem::UMonsterDataSubsystem(const FObjectInitializer& ObjectInitializer)
{
	static ConstructorHelpers::FObjectFinder<UPrimaryDataAsset> dataAsset(TEXT("/Script/AgeOfWolves.MonsterData'/Game/Blueprints/10_Monster/MonsterData.MonsterData'"));
	if (dataAsset.Object)
	{
		MonsterData = Cast<UMonsterData>(dataAsset.Object)->MonsterData;
	}

}

void UMonsterDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
}

void UMonsterDataSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UMonsterDataSubsystem::CustomFunction(EMonsterName name, FSingleMonsterData &SingleMonsterData, UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles)
{
	if (MonsterData.Find((name)))
	{
		FSingleMonsterData SingleMonsterDataTemp = *MonsterData.Find(name);
		SingleMonsterData = SingleMonsterDataTemp;

		CurrentMonsterData = SingleMonsterDataTemp;
		GiveStartupAttributeSetToAbilitySystem(ASC, OutGrantedHandles, SingleMonsterDataTemp.AttributeSets);
		GiveStartupGameplayEffectToAbilitySystem(ASC, OutGrantedHandles, SingleMonsterDataTemp.GameplayEffects);
		GiveStartupGameplayAbilityToAbilitySystem(ASC, OutGrantedHandles, SingleMonsterDataTemp.GameplayAbilities);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("SingleMonsterData"));
	}

}

void UMonsterDataSubsystem::DecideEnemyOrElse(EMonsterName OwnerName, EMonsterName TargetName, bool &IsEnemy, bool &IsFriend)
{
	if (MonsterData.Find(OwnerName))
	{
		FSingleMonsterData& SingleMonsterDataTemp = *MonsterData.Find(OwnerName);
		for (auto i : SingleMonsterDataTemp.EnemyNameList)
		{
			if (i == TargetName)
			{
				IsEnemy = true;
				return;
			}
		}
		for (auto i : SingleMonsterDataTemp.FriendNameList)
		{
			if (i == TargetName)
			{
				IsFriend = true;
				return;
			}
		}
	}
}

void UMonsterDataSubsystem::AddEnemyName(EMonsterName OwnerName, EMonsterName EnemyName)
{
	if (MonsterData.Find(OwnerName))
	{
		FSingleMonsterData &SingleMonsterDataTemp = *MonsterData.Find(OwnerName);
		SingleMonsterDataTemp.EnemyNameList.Add(EnemyName);
	}
}

void UMonsterDataSubsystem::AddFriendName(EMonsterName OwnerName, EMonsterName FriendName)
{
	if (MonsterData.Find(OwnerName))
	{
		FSingleMonsterData& SingleMonsterDataTemp = *MonsterData.Find(OwnerName);
		SingleMonsterDataTemp.FriendNameList.Add(FriendName);
	}
}

void UMonsterDataSubsystem::GiveStartupAttributeSetToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, TArray<FBaseAbilitySet_AttributeSet>& AttributeSets) const
{
	check(ASC);
	
	// Attribute Set
	for (int32 SetIndex = 0; SetIndex < AttributeSets.Num(); ++SetIndex)
	{
		const FBaseAbilitySet_AttributeSet& SetToGrant = AttributeSets[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOGFMT(LogAbilitySet, Error, "Ability Set의 Attribute Set이 유효하지 않습니다!");
			continue;
		}
		// #1. ASC에 AttributeSet 등록
		
		UBaseAttributeSet* NewSet = NewObject<UBaseAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet);

		// 왜 안되지
		NewSet->InitHealth(CurrentMonsterData.PermanentStat.PHealthStat);
		NewSet->InitMaxHealth(CurrentMonsterData.PermanentStat.PHealthStat);
		NewSet->InitDamage(CurrentMonsterData.PermanentStat.PDamageStat);

		//UBaseAttributeSet* NewSet2 = Cast<UBaseAttributeSet>(ASC->GetAttributeSet(TSubclassOf<UBaseAttributeSet>()));

		//여기에 방어력, 처치시 추가되는 돈 등 현재는 기획에 없어서 안 넣은 수치들 나중에 초기화 해주기
		
		ASC->AddAttributeSetSubobject(NewSet);

		/*bool IsFound;
		ASC->GetGameplayAttributeValue(NewSet->GetHealthAttribute(), IsFound);
		if (IsFound)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Debug %f"), ASC->GetGameplayAttributeValue(NewSet->GetHealthAttribute(), IsFound)));
		}*/

		// #2. BaseAilitySet에 GrantedHandle 추가
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}

void UMonsterDataSubsystem::GiveStartupGameplayEffectToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, TArray<FBaseAbilitySet_GameplayEffect>& GameplayEffects) const
{
	check(ASC);

	// GE
	for (int32 EffectIndex = 0; EffectIndex < GameplayEffects.Num(); ++EffectIndex)
	{
		const FBaseAbilitySet_GameplayEffect& EffectToGrant = GameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOGFMT(LogAbilitySet, Error, "Ability Set의 {0}번째 Gameplay Effect가 유효하지 않습니다!", FString::FromInt(EffectIndex));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}
}

void UMonsterDataSubsystem::GiveStartupGameplayAbilityToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, TArray<FBaseAbilitySet_GameplayAbility>& GameplayAbilities) const
{
	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GameplayAbilities.Num(); ++AbilityIndex)
	{
		const FBaseAbilitySet_GameplayAbility& AbilityToGrant = GameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOGFMT(LogAbilitySet, Error, "Ability Set의 {0}번째 Gameplay Ability가 유효하지 않습니다!", FString::FromInt(AbilityIndex));
			continue;
		}

		// 1. Ability CDO
		UBaseGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UBaseGameplayAbility>();

		// 2. AbilitySpec 구성
		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		//AbilitySpec.SourceObject = SourceObject;

		// 3. Active GA는 별도의 Input Tag를 AbilitySpec에 저장합니다. 
		if (AbilityToGrant.bActive)
			AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		// 4. ASC 등록
		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);

		// 5. Passive GA

		// 6. Handle 저장
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}
}
