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
		
		//ASC OwningActor�� BaseMonster�� cast�Ǹ� ��� �����Ƽ ���� �ǵ���
		//��� �����Ƽ���� ������ Ű ����, �ִϸ��̼� ����
	}
}

void UBaseMonsterAS::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}
