#include "MenuUI.h"
#include "Logging/StructuredLog.h"    

#include "04_Component/UIComponent.h"

#include "08_UI/02_Menu/MenuUIToolBar.h"
#include "08_UI/02_Menu/MenuUIContent.h"

//@TODO: Menu Content UI 설꼐 이후 아래 #include 제거
#include "Components/CanvasPanel.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"

DEFINE_LOG_CATEGORY(LogMenuUI)

#pragma region Default Setting
UMenuUI::UMenuUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UMenuUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@TODO: UI Component에 외부 바인딩
    //@외부 바인딩
    ExternalBindingToUIComponent();

}

void UMenuUI::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UMenuUI::NativeConstruct()
{
    Super::NativeConstruct();
}

void UMenuUI::NativeDestruct()
{
    Super::NativeDestruct();
}

void UMenuUI::ExternalBindingToUIComponent()
{
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogMenuUI, Error, "MenuUI::ExternalBindingToUIComponent: 월드가 null입니다");
        return;
    }

    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogMenuUI, Error, "MenuUI::ExternalBindingToUIComponent: 플레이어 컨트롤러가 null입니다");
        return;
    }

    UUIComponent* UIComp = PC->FindComponentByClass<UUIComponent>();
    if (!UIComp)
    {
        UE_LOGFMT(LogMenuUI, Error, "{0}: UI 컴포넌트가 유효하지 않습니다", __FUNCTION__);
        return;
    }

    //@Visibility 변화 이벤트
    UIComp->WidgetVisibilityChanged.AddUObject(this, &UMenuUI::OnUIVisibilityChanged);
    //@Input 이벤트
    UIComp->NotifyMenuUIInputTriggered.BindUFunction(this, "MenuUIInputTriggeredNotified");
    UIComp->NotifyMenuUIInputReleased.BindUFunction(this, "MenuUIInputReleasedNotified");

    UE_LOGFMT(LogMenuUI, Log, "MenuUI가 UIComponent의 WidgetVisibilityChanged 델리게이트에 성공적으로 바인딩되었습니다.");
}

void UMenuUI::InternalBindingToToolBar(UMenuUIToolBar* ToolBar)
{
    //@Inventory UI
    if (!ToolBar)
    {
        UE_LOGFMT(LogMenuUI, Error, "ToolBar UI가 유효하지 않습니다.");
        return;
    }
    //@툴바 초기화 완료 이벤트
    ToolBar->ToolBarInitFinished.BindUFunction(this, "OnToolBarInitFinished");
    //@툴바 메뉴 카테고리 선택 이벤트
    ToolBar->MenuCategoryButtonClikced.BindUFunction(this, "OnMenuCategoryButtonClikced");

    UE_LOGFMT(LogMenuUI, Log, "MenuUI: Tool Bar UI의 OnInitFinished에 OnToolBarInitFinished 함수를 바인딩했습니다.");
}

void UMenuUI::InternalBindingToMenuUIContent(UMenuUIContent* MenuUIContent)
{
    if (!MenuUIContent)
    {
        UE_LOGFMT(LogMenuUI, Error, "MenuUIContent가 유효하지 않습니다.");
        return;
    }

    //@내부 바인딩
    MenuUIContent->MenuUIContentInitFinished.BindUFunction(this, "OnMenuUIContentInitFinished");

    UE_LOGFMT(LogMenuUI, Log, "MenuUIContent의 초기화 완료 이벤트에 콜백 함수를 바인딩했습니다.");
}

void UMenuUI::InitializeMenuUI()
{
    //@Tool Bar
    CreateToolBar();
    //@All UIs
    CreateAllCategoryUIs();
    //@초기화 요청 이벤트 호
    for (int8 MenuIdx = 0; MenuIdx < static_cast<int8>(EMenuCategory::MAX); ++MenuIdx)
    {
        RequestStartInitByMenuUI.Broadcast(static_cast<EMenuCategory>(MenuIdx));
    }
}

void UMenuUI::CheckInventoryUIInitFinished()
{
    //@초기화 완료 여부
    if (bInventoryUIInitFinished)
    {
        //@이벤트 호출
        NotifyInventoryUIInitFinished.ExecuteIfBound();
    }
}

void UMenuUI::CheckLevelUIInitFinished()
{
    if (bLevelUIInitFinished)
    {
        UE_LOGFMT(LogMenuUI, Log, "Level UI의 초기화가 완료되었습니다.");
        // NotifyLevelUIInitFinished.ExecuteIfBound(); // 필요시 추가
    }
}

void UMenuUI::CheckMapUIInitFinished()
{
    if (bMapUIInitFinished)
    {
        UE_LOGFMT(LogMenuUI, Log, "Map UI의 초기화가 완료되었습니다.");
        // NotifyMapUIInitFinished.ExecuteIfBound(); // 필요시 추가
    }
}

void UMenuUI::CheckSystemUIInitFinished()
{
    if (bSystemUIInitFinished)
    {
        UE_LOGFMT(LogMenuUI, Log, "System UI의 초기화가 완료되었습니다.");
        // NotifySystemUIInitFinished.ExecuteIfBound(); // 필요시 추가
    }
}
#pragma endregion

#pragma region SubWidgets
void UMenuUI::CreateToolBar()
{
    if (!ensureMsgf(ToolBarClass && ToolBarOverlay, TEXT("ToolBarClass 또는 ToolBarOverlay가 유효하지 않습니다.")))
    {
        return;
    }

    UMenuUIToolBar* ToolBar = CreateWidget<UMenuUIToolBar>(this, ToolBarClass);
    if (!IsValid(ToolBar))
    {
        UE_LOGFMT(LogMenuUI, Error, "ToolBar 위젯 생성에 실패했습니다.");
        return;
    }

    RequestStartInitByMenuUI.AddUFunction(ToolBar, "InitializeToolBar");
    InternalBindingToToolBar(ToolBar);

    if(UOverlaySlot* OverlaySlot = ToolBarOverlay->AddChildToOverlay(ToolBar))
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
        OverlaySlot->SetVerticalAlignment(VAlign_Fill);
    }

    UE_LOGFMT(LogMenuUI, Log, "ToolBar가 성공적으로 생성되고 ToolBarOverlay에 추가되었습니다.");
}

void UMenuUI::CreateAllCategoryUIs()
{
    //@Menu UI Content Overlay
    if (!IsValid(MenuUIContentOverlay))
    {
        UE_LOGFMT(LogMenuUI, Error, "MenuUIContentOverlay가 유효하지 않습니다.");
        return;
    }
    //@TSet
    TSet<EMenuCategory> CreatedCategories;
    //@Menu Content
    for (const FMenuUIContentInfo& ContentUI : MenuContent)
    {
        //@Contains
        if (CreatedCategories.Contains(ContentUI.MenuCategory))
        {
            UE_LOGFMT(LogMenuUI, Warning, "{0} 카테고리의 MenuUIContent가 이미 생성되었습니다. 중복 생성을 건너뜁니다.", *UEnum::GetValueAsString(ContentUI.MenuCategory));
            continue;
        }
        //@MenuUIContentClass
        if (!ContentUI.MenuUIContentClass)
        {
            UE_LOGFMT(LogMenuUI, Warning, "{0} 카테고리의 MenuUIContentClass가 설정되지 않았습니다.", *UEnum::GetValueAsString(ContentUI.MenuCategory));
            continue;
        }
        //@MenuUIContent
        UMenuUIContent* NewContent = CreateWidget<UMenuUIContent>(this, ContentUI.MenuUIContentClass);
        if (!NewContent)
        {
            UE_LOGFMT(LogMenuUI, Error, "{0} 카테고리의 MenuUIContent 생성에 실패했습니다.", *UEnum::GetValueAsString(ContentUI.MenuCategory));
            continue;
        }

        //@MMenuContents
        MMenuContents.Add(ContentUI.MenuCategory, NewContent);

        //@초기화 요청 이벤트
        RequestStartInitByMenuUI.AddUFunction(NewContent, "InitializeMenuUIContent");

        //@내부 바인딩
        InternalBindingToMenuUIContent(NewContent);

        //@OverlaySlot
        if (UOverlaySlot* OverlaySlot = MenuUIContentOverlay->AddChildToOverlay(NewContent))
        {
            OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
            OverlaySlot->SetVerticalAlignment(VAlign_Fill);
        }

        //@TSet
        CreatedCategories.Add(ContentUI.MenuCategory);

        UE_LOGFMT(LogMenuUI, Log, "{0} 카테고리의 MenuUIContent가 성공적으로 생성되고 MenuUIContentOverlay에 추가되었습니다.",
            *UEnum::GetValueAsString(ContentUI.MenuCategory));
    }

    //@생성에 실패한 Category의 콘텐츠
    for (uint8 i = 0; i < static_cast<uint8>(EMenuCategory::MAX); ++i)
    {
        EMenuCategory Category = static_cast<EMenuCategory>(i);
        if (!CreatedCategories.Contains(Category))
        {
            UE_LOGFMT(LogMenuUI, Warning, "{0} 카테고리의 MenuUIContent가 생성되지 않았습니다.", *UEnum::GetValueAsString(Category));
        }
    }
}

void UMenuUI::SetCategoryVisibility(EMenuCategory Category, bool bVisible)
{
    //@Widget
    if (UUserWidget* Widget = GetCategoryUI(Category))
    {
        Widget->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
}

UMenuUIToolBar* UMenuUI::GetToolBarUI() const
{
    if (!ToolBarOverlay)
    {
        UE_LOGFMT(LogMenuUI, Error, "ToolBarOverlay가 유효하지 않습니다.");
        return nullptr;
    }

    TArray<UWidget*> Children = ToolBarOverlay->GetAllChildren();
    for (UWidget* Child : Children)
    {
        UMenuUIToolBar* ToolBar = Cast<UMenuUIToolBar>(Child);
        if (ToolBar)
        {
            return ToolBar;
        }
    }

    UE_LOGFMT(LogMenuUI, Warning, "ToolBarOverlay에서 UMenuUIToolBar를 찾을 수 없습니다.");
    return nullptr;
}

UUserWidget* UMenuUI::GetCategoryUI(EMenuCategory Category) const
{
    if (auto FoundWidget = MMenuContents.Find(Category))
    {
        return *FoundWidget;
    }
    return nullptr;
}
#pragma endregion

#pragma region Callbacks
void UMenuUI::OnUIVisibilityChanged(UUserWidget* Widget, bool bVisible)
{
    if (Widget != this)
    {
        return;
    }

    if (bVisible)
    {
        //@Menu UI 내부에 표시할 Category의 Content 가시성
        for (auto& Pair : MMenuContents)
        {
            SetCategoryVisibility(Pair.Key, Pair.Key == CurrentCategory);
        }

        //@Add To Viewport
        AddToViewport();

        UE_LOGFMT(LogMenuUI, Log, "MenuUI가 뷰포트에 추가되었습니다. 현재 카테고리: {0}", *UEnum::GetValueAsString(CurrentCategory));

        // TODO: MenuUI가 표시될 때 필요한 추가 로직
        // 예: 애니메이션 재생, 초기 포커스 설정 등
    }
    else
    {
        //@Menu UI 내부 Content 가시성
        for (auto& Pair : MMenuContents)
        {
            SetCategoryVisibility(Pair.Key, false);
        }

        //@Remove From Parent
        RemoveFromParent();
        UE_LOGFMT(LogMenuUI, Log, "MenuUI가 부모로부터 제거되었습니다.");

        // TODO: MenuUI가 숨겨질 때 필요한 추가 로직
        // 예: 데이터 저장, 정리 작업 등
    }
}

void UMenuUI::MenuUIInputTriggeredNotified(const FGameplayTag& InputTag)
{
    UE_LOGFMT(LogMenuUI, Log, "Menu UI 입력이 트리거되었습니다. InputTag: {0}", *InputTag.ToString());

    //@Menu UI Tool Bar
    UMenuUIToolBar* ToolBar = GetToolBarUI();
    if (!ToolBar)
    {
        UE_LOGFMT(LogMenuUI, Error, "ToolBar를 찾을 수 없습니다.");
        return;
    }

    //@Move Menu Category To Left
    if (InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Input.UI.MenuUI.MoveMenuCategory_Left"))))
    {
        ToolBar->MoveCategoryLeft();
    }
    //@Move Menu Category To Right
    if (InputTag.MatchesTagExact(FGameplayTag::RequestGameplayTag(FName("Input.UI.MenuUI.MoveMenuCategory_Right"))))
    {
        ToolBar->MoveCategoryRight();
    }
}

void UMenuUI::MenuUIInputReleasedNotified(const FGameplayTag& InputTag)
{
    UE_LOGFMT(LogMenuUI, Log, "Menu UI 입력이 해제되었습니다. InputTag: {0}", *InputTag.ToString());

    // TODO: 여기에 입력 해제 처리 로직을 추가하세요

}

void UMenuUI::OnToolBarInitFinished()
{
    //@Delegate
    //NotifyToolBarInitFinished.ExecuteIfBound();
}

void UMenuUI::OnMenuUIContentInitFinished(EMenuCategory Category)
{
    UE_LOGFMT(LogMenuUI, Log, "{0} 카테고리의 MenuUIContent 초기화가 완료되었습니다.", *UEnum::GetValueAsString(Category));

    switch (Category)
    {
    case EMenuCategory::Inventory:
        bInventoryUIInitFinished = true;
        CheckInventoryUIInitFinished();
        break;
    case EMenuCategory::Level:
        bLevelUIInitFinished = true;
        CheckLevelUIInitFinished();
        break;
    case EMenuCategory::Map:
        bMapUIInitFinished = true;
        CheckMapUIInitFinished();
        break;
    case EMenuCategory::System:
        bSystemUIInitFinished = true;
        CheckSystemUIInitFinished();
        break;
    default:
        UE_LOGFMT(LogMenuUI, Warning, "알 수 없는 MenuUIContent 카테고리입니다.");
        break;
    }
}

void UMenuUI::OnMenuCategoryButtonClikced(EMenuCategory MenuCategory)
{
    //@Current Category 
    if (CurrentCategory == MenuCategory)
    {
        return;
    }

    //@Previous Category
    SetCategoryVisibility(CurrentCategory, false);

    //@Curretn Category Update
    CurrentCategory = MenuCategory;

    //@Visibility 
    SetCategoryVisibility(CurrentCategory, true);

    UE_LOGFMT(LogMenuUI, Log, "메뉴 카테고리가 {0}에서 {1}(으)로 변경되었습니다.",
        *UEnum::GetValueAsString(CurrentCategory),
        *UEnum::GetValueAsString(MenuCategory));

    // TODO: 필요한 경우 추가 로직 구현
    // 예: 애니메이션 재생, 포커스 설정 등

}
#pragma endregion