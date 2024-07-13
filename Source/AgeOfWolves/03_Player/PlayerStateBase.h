// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "02_AbilitySystem/BaseAbilitySet.h"

#include "PlayerStateBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerStateBase, Log, All)

/*
* @목적 : ASC에 등록된 AttributeSet의 각 Attribute 값의 초기화 이벤트
* @설명 : Attribute 값 초기화 이벤트 발생 시 이를 UI 등 다양한 곳에 알리기 위함
* @참조 : -
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttributeSetInitialized);

/*
* @목적 : ASC에 등록된 AttributeSet의 각 Attribute 값 변화 이벤트를 전파하는 이벤트
* @설명 : Attribute 값 변화 이벤트 발생 시 이를 UI 등 다양한 곳에 알리기 위함
* @참조 : -
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAnyAttributeValueChanged, FGameplayAttribute, Attribute, float, OldValue, float, NewValue);

class ABasePlayerController;
class UPawnData;
class UBaseAttributeSet;
class UBaseAbilitySystemComponent; 
class UUserWidget;

/**
 * Player State contaions pawn's info interacting with others
 */
UCLASS()
class AGEOFWOLVES_API APlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	// Player Controller의 friend 클래스로 설정
	friend class ABasePlayerController;

#pragma region Default Setting
public:
	APlayerStateBase();

protected:
	//~APlayerState Interface
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	//~End Of APlayerState Interface
#pragma endregion

#pragma region Gameplay Ability System
public:
	/*
	* @목적: PawnData를 통해 캐릭터의 최초(게임 시작 시점) 기본 GA, GE, AttributeSet을 ASC에 등록하는 작업을 수행합니다.
	* @설명: 등록 순서는 AttributeBase -> GE -> GA 순서를 유지합니다. AttributeBase의 각 Attribute의 초기 값을 GE를 통해 초기화하기 때문입니다!
	* @참고: Inventory -> HUD -> GAS(AS->GE->GA)
	*/
	UFUNCTION()
		void InitializePlayerSystem();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability System|Pawn Data")
		TObjectPtr<UPawnData> PawnData;
	FBaseAbilitySet_GrantedHandles* SetGrantedHandles;

	UPROPERTY()
		UBaseAbilitySystemComponent* AbilitySystemComponent;
	UPROPERTY()
		TSoftObjectPtr<UBaseAttributeSet> AttributeSet;

public:
	UPawnData* GetPawnData() const;
	//~IAbilitySystemInterface Interface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End Of IAbilitySystemInterface Interface
	TSoftObjectPtr<UBaseAttributeSet> GetAttributeSet() const;
#pragma endregion

#pragma region Attribute Callbacks
protected:
	/*
	* @목적 : 캐릭터의 Attribute 수치 변화 이벤트에 등록할 콜백 함수
	* @설명 : Ability System Component에서 관리하는 Attribute 항목의 수치 변화 이벤트에 등록할 콜백 함수입니다.
	*		  HUD 구현을 위해 PS에서 제공하는 AttributeBase 관련 인터페이스로 활용 가능합니다(C++환경).
	* @참조 : APlayerStateBase::InitializeGameplayAbilitySystem()
	*/
	void OnAttributeValueChanged(const FOnAttributeChangeData& Data);

public:
	/*
	* @목적 : ASC에 등록된 AttributeSet의 각 Attribute 값 초기화 이벤트를 전파하는 이벤트
	* @설명 : Attribute 값 변화 이벤트 발생 시 이를 UI 등 다양한 곳에 알리기 위함
	* @참조 : -
	*/
	FAttributeSetInitialized OnAttributeSetInitialized;
	/*
	* @목적 : ASC에 등록된 AttributeSet의 각 Attribute 값 변화 이벤트를 전파하는 이벤트
	* @설명 : Attribute 값 변화 이벤트 발생 시 이를 UI 등 다양한 곳에 알리기 위함
	* @참조 : -
	*/
	FAnyAttributeValueChanged OnAnyAttributeValueChanged;
#pragma endregion

#pragma region Getter&Setter
public:
	/*
	* @목적: Attribute의 현재 수치 값을 제공하는 Getter입니다.
	* @설명: Attribute 항목이 추가될 수록 Getter 함수 관련 코드도 증가하여, 중복을 방지하기 위해 템플릿 함수로 정의합니다.
	*/
	template<typename T>
	T GetAttributeValue(FString FindingAttriubteName)
	{
		check(AttributeSet.Get());

		TArray<FGameplayAttribute> Attributes = AttributeSet.Get()->GetAllAttributes();
		for (const auto Attribute : Attributes)
		{
			if (Attribute.IsValid() && Attribute.AttributeName == FindingAttriubteName)
			{
				return Attribute.GetNumericValue(AttributeSet.Get());
			}
		}
		return -1;
	}
#pragma endregion
	
};
