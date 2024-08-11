// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ItemDescription.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemDescription, Log, All)

//@�ʱ�ȭ �Ϸ� �̺�Ʈ(�ʱ�ȭ �۾� �񵿱�ȭ)
DECLARE_DELEGATE(FItemDescriptionInitFinished);

/**
 * @UItemDescription
 * 
 * Inventory UI���� Item ������ �����ִ� UI
 */
UCLASS()
class AGEOFWOLVES_API UItemDescription : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
    UItemDescription(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
private:
    //@�ܺ� ���ε�

public:
    //@�ʱ�ȭ
    UFUNCTION()
        void InitializeItemDescription();
#pragma endregion

#pragma region Delegate
public:
    FItemDescriptionInitFinished ItemDescriptionInitFinished;
#pragma endregion
};
