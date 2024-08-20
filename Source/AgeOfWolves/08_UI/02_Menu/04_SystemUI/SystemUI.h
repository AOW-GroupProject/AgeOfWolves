#pragma once

#include "CoreMinimal.h"
#include "08_UI/02_Menu/MenuUIContent.h"
#include "SystemUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSystemUI, Log, All)

class UOverlay;
class UImage;

/**
 * @USystemUI
 *
 * System ���� ������ �����ִ� UI, MenuUIContent�� �ڽ� �����Դϴ�.
 */
UCLASS()
class AGEOFWOLVES_API USystemUI : public UMenuUIContent
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    USystemUI(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    virtual void InitializeMenuUIContent(EMenuCategory Category) override;

protected:
    virtual void CheckMenuUIContentInitFinished() const override;

#pragma endregion

#pragma region SubWidgets
protected:
    // TitleImage�� �̹� MenuUIContent�� �����Ƿ� ���⼭ �����մϴ�.
    // �ʿ��� �߰� �������� ���⿡ �����մϴ�.
#pragma endregion

#pragma region Delegates
    // MenuUIContent���� �̹� ��������Ʈ�� ��ӹ����Ƿ� ���⼭�� �����մϴ�.
#pragma endregion
};