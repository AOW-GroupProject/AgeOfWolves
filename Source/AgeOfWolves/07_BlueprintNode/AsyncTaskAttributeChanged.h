// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"

#include "AsyncTaskAttributeChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttributeValueChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

/**
 * @목적 : ASC의 Attribute 항목의 수치 값 변화 이벤트에 귀기울이는 잠복성 블루프린트 노드입니다.
 * @설명 : UI 구현에 사용되며, Blueprint 환경에서 활용되는 노드를 C++ 환경에서 구현한 후, Editor에 노출시킵니다.
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class AGEOFWOLVES_API UAsyncTaskAttributeChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

#pragma region Attribute Value Change Interface
public:
	UPROPERTY(BlueprintAssignable)
		FAttributeValueChanged OnAttributeValueChanged;
	/*
	* @목적 : Attribute 항목의 수치 값 변화 이벤트에 등록되는 콜백 함수를 가진 AsyncAction의 생성자
	* @설명 : 해당 콜백 함수는 ASC의 데이터 멤버, FActiveGameplayEffectsContainer,에서 관리하는 AttributeValueChangeDelegates(FOnGameplayAttributeValueChange) 목록에 추가됩니다.
	*		 어떤 GE에 의해 ASC에서 관리하는 Attribute 항목의 수치 값에 변화가 발생하면, AttributeValueChangeDelegates 목록에 저장된 콜백 함수들을 호출합니다.
	* @참조 : -
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UAsyncTaskAttributeChanged* ListenToAttributeChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);
	/*
	* @목적 : 두 개 이상의 Attribute 항목의 수치 값 변화 이벤트에 등록되는 콜백 함수를 갖는 AsyncAction의 생성자
	* @설명 : 위와 동일
	* @참조 : -
	*/
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UAsyncTaskAttributeChanged* ListenToAttributesChange(UAbilitySystemComponent* AbilitySystemComponent, TArray<FGameplayAttribute>& Attributes);

	/*
	* @목적 : Async Task의 메모리 해제
	* @설명 : ListenToAttributeChange/ListenToAttributesChange 정적 함수 호출 시 매번 새로운 AsyncTask 객체를 생성하여, 메모리 관리가 중요합니다.
	*		 모든 동작을 완료하고, 해당 AsyncTask 객체의 메모리 해제를 위해 아래 함수를 호출해주어야합니다.
	* @참조 : -
	*/
	UFUNCTION(BlueprintCallable)
		void EndTask();
#pragma endregion

#pragma region Attribute Value Change Internal Settings
protected:
	/*
	* @목적 : Attribute 항목의 수치 값 변화 이벤트에 등록되는 콜백 함수
	* @설명 : 해당 AsyncAction이 생성 시점에 전달받은 FGameplayAttribute 정보를 통해 ASC의 특정 Attribute 혹은 Attributes의 수치 값 변화 이벤트에 등록되는 콜백함수
	* @참조 : -
	*/
	void AttributeChanged(const FOnAttributeChangeData& Data);

	/*
	* @목적 : 해당 AsyncTask의 소임을 다하고, Attribute 수치 값 변화 이벤트에 등록된 콜백 함수를 정리하고, 메모리 누수를 방지하기 위해 ASC의 참조를 들고있습니다.
	* @설명 : -
	* @참조 : -
	*/
	UPROPERTY()
		UAbilitySystemComponent* ASC;

	FGameplayAttribute AttributeListenTo;
	TArray<FGameplayAttribute> AttributesListenTo;
#pragma endregion

	
};
