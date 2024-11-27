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
* @���� : ASC�� ��ϵ� AttributeSet�� �� Attribute ���� �ʱ�ȭ �̺�Ʈ
* @���� : Attribute �� �ʱ�ȭ �̺�Ʈ �߻� �� �̸� UI �� �پ��� ���� �˸��� ����
* @���� : -
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttributeSetInitialized);

/*
* @���� : ASC�� ��ϵ� AttributeSet�� �� Attribute �� ��ȭ �̺�Ʈ�� �����ϴ� �̺�Ʈ
* @���� : Attribute �� ��ȭ �̺�Ʈ �߻� �� �̸� UI �� �پ��� ���� �˸��� ����
* @���� : -
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAnyAttributeValueChanged, FGameplayAttribute, Attribute, float, OldValue, float, NewValue);

class ABasePlayerController;
class UPawnData;
class UBaseAttributeSet;
class UPlayerAbilitySystemComponent; 
class UUserWidget;
class UAOWSaveGame;
class UAbilityManagerSubsystem;

/**
 * Player State contaions pawn's info interacting with others
 */
UCLASS()
class AGEOFWOLVES_API APlayerStateBase : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	// Player Controller�� friend Ŭ������ ����
	friend class ABasePlayerController;

#pragma region Default Setting
public:
	APlayerStateBase();

protected:
	//~APlayerState Interface
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	//~End Of APlayerState Interface
protected:
	

public:
	UFUNCTION()
		void InitializePlayerState();
#pragma endregion

#pragma region Gameplay Ability System
public:
	//@�ε� �۾�
	UFUNCTION()
		void LoadGameAbilitySystem();
	//@Inventory�� Item ������ Save File�κ��� Load�մϴ�.
	void LoadAbilitySystemFromSaveGame(UAOWSaveGame* SaveGame);
	//@Inventory�� Item ������ Item Manager�κ��� Load�մϴ�.
	void LoadDefaultAbilitySystemFromAbilityManager(UAbilityManagerSubsystem* AbilityManager);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability System|Pawn Data")
		TObjectPtr<UPawnData> PawnData;
	FBaseAbilitySet_GrantedHandles* SetGrantedHandles;

	UPROPERTY()
		UPlayerAbilitySystemComponent* AbilitySystemComponent;

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
	* @���� : ĳ������ Attribute ��ġ ��ȭ �̺�Ʈ�� ����� �ݹ� �Լ�
	* @���� : Ability System Component���� �����ϴ� Attribute �׸��� ��ġ ��ȭ �̺�Ʈ�� ����� �ݹ� �Լ��Դϴ�.
	*		  HUD ������ ���� PS���� �����ϴ� AttributeBase ���� �������̽��� Ȱ�� �����մϴ�(C++ȯ��).
	* @���� : APlayerStateBase::InitializeGameplayAbilitySystem()
	*/
	void OnAttributeValueChanged(const FOnAttributeChangeData& Data);

public:
	/*
	* @���� : ASC�� ��ϵ� AttributeSet�� �� Attribute �� �ʱ�ȭ �̺�Ʈ�� �����ϴ� �̺�Ʈ
	* @���� : Attribute �� ��ȭ �̺�Ʈ �߻� �� �̸� UI �� �پ��� ���� �˸��� ����
	* @���� : -
	*/
	FAttributeSetInitialized OnAttributeSetInitialized;
	/*
	* @���� : ASC�� ��ϵ� AttributeSet�� �� Attribute �� ��ȭ �̺�Ʈ�� �����ϴ� �̺�Ʈ
	* @���� : Attribute �� ��ȭ �̺�Ʈ �߻� �� �̸� UI �� �پ��� ���� �˸��� ����
	* @���� : -
	*/
	FAnyAttributeValueChanged OnAnyAttributeValueChanged;
#pragma endregion

#pragma region Getter&Setter
public:
	/*
	* @����: Attribute�� ���� ��ġ ���� �����ϴ� Getter�Դϴ�.
	* @����: Attribute �׸��� �߰��� ���� Getter �Լ� ���� �ڵ嵵 �����Ͽ�, �ߺ��� �����ϱ� ���� ���ø� �Լ��� �����մϴ�.
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
