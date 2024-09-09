#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "DropDownMenu.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDropDownMenu, Log, All)

#pragma region Forward Declaration
class UCustomButton;
#pragma endregion

/**
 * @UDropDownMenu
 * 
 * Drop Down Menu를 구현합니다.
 */
UCLASS()
class AGEOFWOLVES_API UDropDownMenu : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
    UDropDownMenu(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~ End UUserWidget Interface

protected:
    //@외부 바인딩

protected:
    // 내부 바인딩 함수
    void InternalBindToItemSlotButton(UCustomButton* InItemSlotButton);

public:
    //@초기화
    UFUNCTION()
        void InitializeDropDownMenu();
#pragma endregion
};
