// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncTaskAttributeChanged.h"

UAsyncTaskAttributeChanged* UAsyncTaskAttributeChanged::ListenToAttributeValueChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute)
{

	if (!IsValid(AbilitySystemComponent) && !Attribute.IsValid())
	{
		return nullptr;
	}

	// AsyncTask의 인스턴스 생성
	UAsyncTaskAttributeChanged* AsyncTask = NewObject<UAsyncTaskAttributeChanged>();
	AsyncTask->ASC = AbilitySystemComponent;
	AsyncTask->AttributeListenTo = Attribute;
	// AsyncTask의 콜백 함수를 ASC의 Attribute 수치 값 변화 이벤트에 등록
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(AsyncTask, &UAsyncTaskAttributeChanged::AttributeChanged);

	return AsyncTask;
}

UAsyncTaskAttributeChanged* UAsyncTaskAttributeChanged::ListenToAttributesValueChange(UAbilitySystemComponent* AbilitySystemComponent, TArray<FGameplayAttribute>& Attributes)
{
	if (!IsValid(AbilitySystemComponent))
	{
		return nullptr;
	}

	for (const auto& Attribute : Attributes)
	{
		if (!Attribute.IsValid())
		{
			return nullptr;
		}
	}

	// AsyncTask의 인스턴스 생성
	UAsyncTaskAttributeChanged* AsyncTask = NewObject<UAsyncTaskAttributeChanged>();
	AsyncTask->ASC = AbilitySystemComponent;
	AsyncTask->AttributesListenTo = Attributes;
	// AsyncTask의 콜백 함수를 ASC의 Attribute 수치 값 변화 이벤트에 등록
	for (const auto Attribute : AsyncTask->AttributesListenTo)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(AsyncTask, &UAsyncTaskAttributeChanged::AttributeChanged);
	}

	return AsyncTask;
}

void UAsyncTaskAttributeChanged::EndTask()
{
	// 수치 값 변화 이벤트를 구독한 콜백 함수를 모두 삭제합니다.
	if (IsValid(ASC.Get()))
	{
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeListenTo).RemoveAll(this);

		for (auto& Attribute : AttributesListenTo)
		{
			ASC->GetGameplayAttributeValueChangeDelegate(Attribute).RemoveAll(this);
		}
	}

	SetReadyToDestroy();
	MarkAsGarbage();
}

void UAsyncTaskAttributeChanged::AttributeChanged(const FOnAttributeChangeData& Data)
{
	OnAttributeValueChanged.Broadcast(Data.Attribute, Data.OldValue, Data.NewValue);
}
