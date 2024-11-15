#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"

#include "StateBars.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStateBars, Log, All)
DECLARE_DELEGATE(FStateBarsInitFinished);

class UOverlay;
class UVerticalBox;
class UProgressBar;
class UAsyncTaskAttributeChanged;
class UAbilitySystemComponent;

UCLASS()
class AGEOFWOLVES_API UStateBars : public UUserWidget
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UStateBars(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    void ExternalBindingToAttributeSet();
public:
    UFUNCTION()
        void InitializeStateBars();
#pragma endregion 

#pragma region SubWidgets
protected:
    UPROPERTY(BlueprintReadWrite, Category = "State Bar", meta = (BindWidget))
        UVerticalBox* StateBarUIVerticalBox;

    UPROPERTY(BlueprintReadWrite, Category = "State Bar", meta = (BindWidget))
        UProgressBar* HP;

    UPROPERTY(BlueprintReadWrite, Category = "State Bar", meta = (BindWidget))
        UProgressBar* SP;

protected:
    TMap<FString, UProgressBar*> MStateBars;

    // AsyncTask 인스턴스들을 저장할 배열
    UPROPERTY()
        TArray<UAsyncTaskAttributeChanged*> AttributeListeners;

#pragma region Delegates
public:
    FStateBarsInitFinished StateBarsInitFinished;

#pragma endregion

#pragma region Callbacks
protected:
    UFUNCTION()
        void OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue);

    // ASC 캐시
    UPROPERTY()
        TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
#pragma endregion 
};