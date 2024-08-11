// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "ItemDescription.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogItemDescription, Log, All)

//@초기화 완료 이벤트(초기화 작업 비동기화)
DECLARE_DELEGATE(FItemDescriptionInitFinished);

/**
 * @UItemDescription
 * 
 * Inventory UI에서 Item 정보를 보여주는 UI
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
    //@외부 바인딩

public:
    //@초기화
    UFUNCTION()
        void InitializeItemDescription();
#pragma endregion

#pragma region Delegate
public:
    FItemDescriptionInitFinished ItemDescriptionInitFinished;
#pragma endregion
};
