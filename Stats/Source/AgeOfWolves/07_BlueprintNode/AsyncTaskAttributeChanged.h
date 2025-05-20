#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AbilitySystemComponent.h"

#include "AsyncTaskAttributeChanged.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAttributeValueChanged, FGameplayAttribute, Attribute, float, NewValue, float, OldValue);

 /*
 *	@UAsyncTaskAttributeChanged
 *	
 *	ASC의 Attribute 항목의 수치 값 변화 이벤트에 귀기울이는 잠복성 블루프린트 노드입니다.
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = AsyncTask))
class AGEOFWOLVES_API UAsyncTaskAttributeChanged : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

#pragma region Attribute Value Change Interface
public:
	//@속성 수치 변화 이벤트
	UPROPERTY(BlueprintAssignable)
		FAttributeValueChanged OnAttributeValueChanged;
	//@목적 : Attribute 항목의 수치 값 변화 이벤트에 등록되는 콜백 함수를 가진 AsyncAction의 생성자
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UAsyncTaskAttributeChanged* ListenToAttributeValueChange(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAttribute Attribute);
	//@목적 : 두 개 이상의 Attribute 항목의 수치 값 변화 이벤트에 등록되는 콜백 함수를 갖는 AsyncAction의 생성자
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
		static UAsyncTaskAttributeChanged* ListenToAttributesValueChange(UAbilitySystemComponent* AbilitySystemComponent, TArray<FGameplayAttribute>& Attributes);

	//@목적 : Async Task의 메모리 해제
	UFUNCTION(BlueprintCallable)
		void EndTask();
#pragma endregion

#pragma region Attribute Value Change Internal Settings
protected:
	void AttributeChanged(const FOnAttributeChangeData& Data);

	//@목적 : 해당 AsyncTask의 소임을 다하고, Attribute 수치 값 변화 이벤트에 등록된 콜백 함수를 정리하고, 메모리 누수를 방지하기 위해 ASC의 참조를 들고있습니다.
	TWeakObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayAttribute AttributeListenTo;
	TArray<FGameplayAttribute> AttributesListenTo;
#pragma endregion

	
};
