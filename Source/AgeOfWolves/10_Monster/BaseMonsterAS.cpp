// Fill out your copyright notice in the Description page of Project Settings.


#include "10_Monster/BaseMonsterAS.h"

UBaseMonsterAS::UBaseMonsterAS()
{
}

void UBaseMonsterAS::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		
		//ASC OwningActor가 BaseMonster로 cast되면 기믹 어빌리티 실행 되도록
		//기믹 어빌리티에서 블랙보드 키 변경, 애니메이션 실행
	}
}

void UBaseMonsterAS::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
