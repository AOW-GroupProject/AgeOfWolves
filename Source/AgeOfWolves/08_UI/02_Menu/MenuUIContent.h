#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuUI.h"

#include "MenuUIContent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMenuUIContent, Log, All)

class UImage;

//@�ʱ�ȭ ��û �̺�Ʈ
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByMenuUIContent)
//@�ʱ�ȭ �Ϸ� �̺�Ʈ
DECLARE_DELEGATE_OneParam(FMenuUIContentInitFinished, EMenuCategory);

/**
 * @UMenuContentUI
 * 
 * Menu UI ���� �������� ��Ÿ���� UI�� �����մϴ�.
 */
UCLASS()
class AGEOFWOLVES_API UMenuUIContent : public UUserWidget
{
	GENERATED_BODY()
#pragma region Default Setting
public:
    UMenuUIContent(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    //~ End UUserWidget Interface

protected:
    //@�ܺ� ���ε�

protected:
    //@���� ���ε�

public:
    //@�ʱ�ȭ
    //@�������̵� �� UFUNCTION() ����
    UFUNCTION()
        virtual void InitializeMenuUIContent(EMenuCategory Category);
protected:
    //@TODO: �� ������� �ʱ�ȭ �Ϸ� ���Ҹ� Ȯ���ϴ� boolean ���� ����
    //@�ʱ�ȭ �Ϸ� Ȯ�� �Լ�
    virtual void CheckMenuUIContentInitFinished() const;
#pragma endregion

#pragma region Info
protected:
    //@Menu Content�� Category�� �����մϴ�.
    //@����: InitializeMenuUIContent()
    EMenuCategory MenuCategory = EMenuCategory::MAX;
#pragma endregion

#pragma region SubWidgets
protected:
    //@Menu Content�� Ÿ��Ʋ �̹���
    UPROPERTY(BlueprintReadWrite, Category = "Menu Content | Title Image", meta = (BindWidget))
        UImage* TitleImage;
#pragma endregion

#pragma region Delegates
public:
    //@�ʱ�ȭ ��û �̺�Ʈ
    FRequestStartInitByMenuUIContent RequestStartInitByMenuUIContent;
    //@�ʱ�ȭ �Ϸ� �̺�Ʈ
    FMenuUIContentInitFinished MenuUIContentInitFinished;
#pragma endregion

#pragma region Callbacks
protected:
    //@TODO: ������� �ʱ�ȭ �Ϸῡ ��ϵǴ� �ݹ� �Լ��� CheckMenuUIContentInitFinished �Լ� ȣ��
#pragma endregion
	
};
