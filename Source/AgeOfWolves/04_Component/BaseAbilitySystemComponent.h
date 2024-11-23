// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "02_AbilitySystem/AbilityTagRelationshipMapping.h"

#include "BaseAbilitySystemComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogASC, Log, All);

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitySpecGiven, FGameplayAbilitySpec)
#pragma endregion

/**	
 *	@UBaseAbilitySystemComponent
 * 
 *	AOW의 캐릭터 기본 ASC 유형입니다.
 */	
UCLASS()
class AGEOFWOLVES_API UBaseAbilitySystemComponent : public UAbilitySystemComponent
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UBaseAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//@내부 바인딩

protected:
	//@외부 바인딩

protected:
	//@초기화
	virtual void InitializeComponent() override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	//@오버로딩
	FGameplayAbilitySpecHandle GiveAbility(const FGameplayAbilitySpec& AbilitySpec);

protected:
	//@현재 활성화된 GA를 강제 취소합니다.
	virtual void CancelAbilitySpec(FGameplayAbilitySpec& Spec, UGameplayAbility* Ignore) override;
	//@목적: Unblock된 Passive GA의 재 활성화 동작 수행.
	void ReactivateUnblockedPassiveAbility(const FGameplayTagContainer UnblockedAbilityTags);

public:
	FORCEINLINE void GetActivatingAbilityTags(OUT FGameplayTagContainer& OutGameplayTagContainer) const { OutGameplayTagContainer = ActivatingAbilityTags; }

public:
	//@목적: 전달 받은 GA와 관련하여 "Ability Tags To Block"/"Ability Tags To Cancel" Ability Tag를 전달합니다.
	void GetAbilityBlockAndCancelTagsForAbilityTag(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer& OutAbilityTagsToBlock, OUT FGameplayTagContainer& OutAbilityTagsToCancel);
	//@목적: 전달 받은 GA와 관련하여 "Activation Required"/"Activation Blocked" Ability Tag를 전달합니다.
	void GetAbilityRelationshipActivationTags(const FGameplayTagContainer& AbilityTags, OUT FGameplayTagContainer* OutActivationRequired, OUT FGameplayTagContainer* OutActivationBlocked) const;
	//@목적: 전달 받은 GA와 관련하여 "Blocekd"/"Canceled" Ability Tag에 대응되는 GA들에 대한 각각의 조치
	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;

protected:
	UPROPERTY(EditAnywhere)
		TObjectPtr<UAbilityTagRelationshipMapping> AbilityTagRelationshipMapping;

protected:
	//@목적: 현재 활성화 이벤트 처리 중인 GA들의 Gameplay Tag들을 담아둡니다.
	FGameplayTagContainer ActivatingAbilityTags;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@어빌리티 등록 이벤트
	FAbilitySpecGiven AbilitySpecGiven;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@목적: Ability의 활성화 이벤트에 등록할 콜백 함수
	void OnAbilityActivated(UGameplayAbility* Ability);

	//@목적: Ability의 활성화 종료 이벤트에 등록할 콜백 함수
	virtual void OnAbilityEnded(UGameplayAbility* Ability);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	FORCEINLINE void SetAbilityTagRelationshipMapping(UAbilityTagRelationshipMapping* ATRM) { AbilityTagRelationshipMapping = ATRM; }
#pragma endregion
};