// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "08_UI/AbilitySlot.h"

#include "HUD_QuickSlotsUI_AbilitySlot.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogQuickSlotsAbilitySlot, Log, All)

//@���� ����
#pragma region Forward Declaration
class UEditableTextBox;
#pragma endregion

//@������
#pragma region Enums
#pragma endregion

//@����ü
#pragma region Structs
#pragma endregion

//@�̺�Ʈ/��������Ʈ
#pragma region Delegates
#pragma endregion

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API UHUD_QuickSlotsUI_AbilitySlot : public UAbilitySlot
{
//@ģ�� Ŭ����
#pragma region Friend Class
	friend class UHUD_QuickSlotsUI;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UHUD_QuickSlotsUI_AbilitySlot(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized() override; // �޸� �Ҵ� �Ϸ�, ȭ�鿡 �����Ǳ� ���� ȣ���
	//~ End UUserWidget Interface

protected:
	//@���� ���ε�

protected:
	//@�ܺ� ���ε�

public:
	//@�ʱ�ȭ
	virtual void InitializeAbilitySlot() override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@���ο� Ability �Ҵ�
	virtual void AssignNewAbility_Implementation(FGameplayTag Tag) override;

	//@�ٸ� Ability Slot���κ��� Ability �Ҵ�
	virtual void AssignNewAbilityFromSlot_Implementation(UAbilitySlot* FromSlot) override;

	//@�����Կ� �Ҵ�� ���� �������� �����մϴ�.
	virtual void ClearAssignedAbility_Implementation(bool bForceClear = false) override;

protected:
	//@Ability Slot�� ���� �̸�
	UPROPERTY(BlueprintReadWrite, Category = "�����Ƽ ���� | �����Ƽ ���� ��� ��", meta = (BindWidget))
		UEditableTextBox* AbilityName;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	//@GameplayTag�κ��� Ability�� TypeName�� Name�� �����ϴ� �Լ�
	bool ExtractAbilityNamesFromTag(const FGameplayTag& Tag, FString& OutTypeName, FString& OutAbilityName) const;
#pragma endregion
	
};
