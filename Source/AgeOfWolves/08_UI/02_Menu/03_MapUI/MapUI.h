#pragma once

#include "CoreMinimal.h"
#include "08_UI/02_Menu/MenuUIContent.h"
#include "MapUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMapUI, Log, All)

class UOverlay;
class UImage;

/**
 * @UMapUI
 *
 * Map 정보를 보여주는 UI, MenuUIContent의 자식 위젯입니다.
 */
UCLASS()
class AGEOFWOLVES_API UMapUI : public UMenuUIContent
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UMapUI(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

protected:
    virtual void InitializeMenuUIContent() override;

protected:
    virtual void CheckMenuUIContentInitFinished() override;

#pragma endregion

#pragma region SubWidgets
protected:
    // TitleImage는 이미 MenuUIContent에 있으므로 여기서 제거합니다.
    // 필요한 추가 위젯들을 여기에 선언합니다.
#pragma endregion

#pragma region Delegates
    // MenuUIContent에서 이미 델리게이트를 상속받으므로 여기서는 제거합니다.
#pragma endregion
};