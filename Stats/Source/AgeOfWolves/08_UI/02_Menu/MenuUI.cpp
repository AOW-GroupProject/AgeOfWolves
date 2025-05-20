#include "MenuUI.h"
#include "Logging/StructuredLog.h"    

#include "04_Component/BaseInputComponent.h"
#include "04_Component/UIComponent.h"

#include "08_UI/02_Menu/MenuUIToolBar.h"
#include "08_UI/02_Menu/MenuUIContent.h"
#include "Animation/WidgetAnimation.h" 

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"

DEFINE_LOG_CATEGORY(LogMenuUI)

//@Defualt Setting
#pragma region Default Setting
UMenuUI::UMenuUI(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , DefaultCategory(EMenuCategory::Inventory)
    , CurrentCategory(EMenuCategory::MAX)
    , bMenuToolBarInitFinished(false)
    , bInventoryUIInitFinished(false)
    , bLevelUIInitFinished(false)
    , bMapUIInitFinished(false)
    , bSystemUIInitFinished(false)
{
    //@ToolBar 초기화
    ToolBarRef = nullptr;

    //@MMenuContents 맵 초기화
    for (uint8 i = 0; i < static_cast<uint8>(EMenuCategory::MAX); ++i)
    {
        MMenuContents.Add(static_cast<EMenuCategory>(i), nullptr);
    }
}

void UMenuUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@외부 바인딩 아래에서 수행...
    //@외부 바인딩
    ExternalBindToUIComponent();

    //@애니메이션 완료 콜백 바인딩
    if (BlendOutAnimation)
    {
        FWidgetAnimationDynamicEvent AnimFinishedEvent;
        AnimFinishedEvent.BindDynamic(this, &UMenuUI::OnBlendOuAnimationFinished);
        BindToAnimationFinished(BlendOutAnimation, AnimFinishedEvent);
    }
}

void UMenuUI::NativePreConstruct()
{
    Super::NativePreConstruct();

    SetIsFocusable(true);
}

void UMenuUI::NativeConstruct()
{
    Super::NativeConstruct();
}

void UMenuUI::NativeDestruct()
{
    Super::NativeDestruct();
}

FNavigationReply UMenuUI::NativeOnNavigation(const FGeometry& MyGeometry, const FNavigationEvent& InNavigationEvent, const FNavigationReply& InDefaultReply)
{
    return FNavigationReply::Explicit(nullptr);
}

FReply UMenuUI::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{

    //@Set Directly(SetFocus())를 통한 포커스 시도 외에 다른 시도는 허용하지 않습니다.
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        return FReply::Handled().ClearUserFocus();
    }

    UE_LOGFMT(LogMenuUI, Log, "포커스 : 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));

    return FReply::Handled();
}

void UMenuUI::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
    //@SetDirectly(다른 UI에 대한 SetFocus() 호출)에 의한 포커스 소실 외에 다른 소실 원인은 거부합니다.
    if (InFocusEvent.GetCause() != EFocusCause::SetDirectly)
    {
        SetFocus();

        return;
    }

    Super::NativeOnFocusLost(InFocusEvent);

    UE_LOGFMT(LogMenuUI, Log, "포커스 종료: 위젯: {0}, 원인: {1}",
        *GetName(), *UEnum::GetValueAsString(InFocusEvent.GetCause()));

}

FReply UMenuUI::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();

    UE_LOGFMT(LogMenuUI, Log, "키 입력 감지됨: {0}", *Key.ToString());

    //@Menu UI Tool Bar
    UMenuUIToolBar* ToolBar = GetToolBarUI();
    if (!ToolBar)
    {
        UE_LOGFMT(LogMenuUI, Error, "ToolBar를 찾을 수 없습니다.");
        return FReply::Unhandled();
    }

    //@Tool Bar 카테고리 이동
    FString KeyName = Key.GetFName().ToString().ToLower();
    if (KeyName == "z")
    {
        UE_LOGFMT(LogMenuUI, Log, "Menu UI 카테고리를 왼쪽으로 이동합니다.");
        //@Menu Category를 왼쪽으로 이동 시킵니다.
        ToolBar->MoveLeft();

        return FReply::Handled();
    }
    else if (KeyName == "x")
    {
        UE_LOGFMT(LogMenuUI, Log, "Menu UI 카테고리를 오른쪽으로 이동합니다.");
        ToolBar->MoveRight();

        return FReply::Handled();
    }
    else if (KeyName == "escape")
    {
        //@Menu UI 닫기 요청 이벤트
        RequestCloseMenuUI.ExecuteIfBound();

        return FReply::Handled().ClearUserFocus();
    }
    else if (KeyName == "i")
    {
        //@TODO: Item 카테고리로 이동
        return FReply::Handled();
    }
    else if (KeyName == "m")
    {
        //@TODO: Map 카테고리로 이동
        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UMenuUI::ExternalBindToUIComponent()
{
    //@World
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogMenuUI, Error, "{0}: 월드가 null입니다", __FUNCTION__);
        return;
    }
    //@PC
    APlayerController* PC = World->GetFirstPlayerController();
    if (!PC)
    {
        UE_LOGFMT(LogMenuUI, Error, "{0}: 플레이어 컨트롤러가 null입니다", __FUNCTION__);
        return;
    }
    //@UI Comp
    UUIComponent* UIComp = PC->FindComponentByClass<UUIComponent>();
    if (!UIComp)
    {
        UE_LOGFMT(LogMenuUI, Error, "{0}: UI 컴포넌트가 유효하지 않습니다", __FUNCTION__);
        return;
    }
    //@외부 바인딩
    UIComp->WidgetVisibilityChanged.AddUObject(this, &UMenuUI::OnUIVisibilityChanged);

    UE_LOGFMT(LogMenuUI, Log, "{0}가 UIComponent의 WidgetVisibilityChanged 델리게이트에 성공적으로 바인딩되었습니다.", *GetName());
}

void UMenuUI::InternalBindingToToolBar(UMenuUIToolBar* ToolBar)
{
    //@Inventory UI
    if (!ToolBar)
    {
        UE_LOGFMT(LogMenuUI, Error, "ToolBar UI가 유효하지 않습니다.");
        return;
    }
    //@내부 바인딩 수행
    ToolBar->ToolBarInitFinished.BindUFunction(this, "OnToolBarInitFinished");
    ToolBar->MenuCategoryButtonClicked.BindUFunction(this, "OnMenuCategoryButtonClikced");

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
    RequestStartInitByMenuUI.Broadcast();
}

void UMenuUI::CheckAllUIsInitFinsiehd()
{
    if (bMenuToolBarInitFinished
        && bInventoryUIInitFinished && bLevelUIInitFinished
        && bMapUIInitFinished && bSystemUIInitFinished)
    {
        bMenuToolBarInitFinished = false;
        bInventoryUIInitFinished = false;
        bLevelUIInitFinished = false;
        bMapUIInitFinished = false;
        bSystemUIInitFinished = false;

        MenuUIInitFinished.ExecuteIfBound();
    }
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UMenuUI::ResetMenuUI()
{
    UE_LOGFMT(LogMenuUI, Log, "MenuUI 리셋 시작");

    //@Menu Tool Bar
    if (ToolBarOverlay)
    {
        for (auto Widget : ToolBarOverlay->GetAllChildren())
        {
            if (auto MenuToolBar = Cast<UMenuUIToolBar>(Widget))
            {
                //@Reset ToolBar
                MenuToolBar->ResetToolBar();

                UE_LOGFMT(LogMenuUI, Log, "MenuToolBar 리셋 완료");
                break;
            }
        }
    }
    else
    {
        UE_LOGFMT(LogMenuUI, Warning, "ToolBarOverlay가 유효하지 않습니다.");
    }

    //@Menu UI 내부 Content 리셋 및 가시성 설정
    for (auto& Pair : MMenuContents)
    {
        if (UMenuUIContent* Content = Cast<UMenuUIContent>(Pair.Value))
        {
            //@Reset
            Content->ResetMenuUIContent();
            
            UE_LOGFMT(LogMenuUI, Log, "{0} 카테고리의 MenuUIContent 리셋 완료", *UEnum::GetValueAsString(Pair.Key));

            //@Visibility
            SetCategoryVisibility(Pair.Key, Pair.Key == DefaultCategory);

            UE_LOGFMT(LogMenuUI, Log, "{0} 카테고리의 가시성 설정: {1}",
                *UEnum::GetValueAsString(Pair.Key),
                Pair.Key == DefaultCategory ? TEXT("표시") : TEXT("숨김"));
        }
        else
        {
            UE_LOGFMT(LogMenuUI, Warning, "{0} 카테고리의 MenuUIContent가 유효하지 않습니다.", *UEnum::GetValueAsString(Pair.Key));
        }
    }

    UE_LOGFMT(LogMenuUI, Log, "MenuUI 리셋 완료");
}

void UMenuUI::CreateToolBar()
{
    //@Tool Bar Blueprint class, Tool Bar Overlay
    if (!ensureMsgf(ToolBarClass && ToolBarOverlay, TEXT("ToolBarClass 또는 ToolBarOverlay가 유효하지 않습니다.")))
    {
        return;
    }
    //@Tool Bar
    UMenuUIToolBar* ToolBar = CreateWidget<UMenuUIToolBar>(this, ToolBarClass);
    if (!IsValid(ToolBar))
    {
        UE_LOGFMT(LogMenuUI, Error, "ToolBar 위젯 생성에 실패했습니다.");
        return;
    }
    //@초기화 요청 이벤트
    RequestStartInitByMenuUI.AddUFunction(ToolBar, "InitializeToolBar");

    //@내부 바인딩
    InternalBindingToToolBar(ToolBar);

    //@Alignment
    if(UOverlaySlot* OverlaySlot = ToolBarOverlay->AddChildToOverlay(ToolBar))
    {
        OverlaySlot->SetHorizontalAlignment(HAlign_Fill);
        OverlaySlot->SetVerticalAlignment(VAlign_Fill);
    }

    //@TWeakObjectPtr
    ToolBarRef = ToolBar;

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

    //@Menu Content
    for (const FMenuUIContentInfo& ContentUI : MenuContents)
    {

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

        UE_LOGFMT(LogMenuUI, Log, "{0} 카테고리의 MenuUIContent가 성공적으로 생성되고 MenuUIContentOverlay에 추가되었습니다.",
            *UEnum::GetValueAsString(ContentUI.MenuCategory));
    }

}

void UMenuUI::SetCategoryVisibility(EMenuCategory Category, bool bVisible)
{
    //@Menu UI Content
    UUserWidget* Widget = GetCategoryUI(Category);
    if (!Widget)
    {
        UE_LOGFMT(LogMenuUI, Error, "{0} 카테고리의 위젯을 찾을 수 없습니다.", *UEnum::GetValueAsString(Category));

        return;
    }

    //@Visibility
    Widget->SetVisibility(bVisible ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);

    //@Focus
    if (bVisible)
    {
        Widget->SetFocus();
    }

    UE_LOGFMT(LogMenuUI, Log, "{0} 카테고리의 가시성을 {1}(으)로 설정했습니다.",
        *UEnum::GetValueAsString(Category),
        bVisible ? TEXT("표시") : TEXT("숨김"));

}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void UMenuUI::OnUIVisibilityChanged_Implementation(UUserWidget* Widget, bool bVisible)
{
    if (Widget != this)
    {
        return;
    }

    //@가시성 활성화
    if (bVisible)
    {
        //@Add To Viewport
        AddToViewport();
        //@Focus
        SetIsFocusable(true);
        SetFocus();
        //@Reset Menu UI
        ResetMenuUI();

        //@애니메이션 재생
        if (BlendInAnimation)
        {
            PlayAnimation(BlendInAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward);
        }

        UE_LOGFMT(LogMenuUI, Log, "MenuUI가 뷰포트에 추가되었습니다. 현재 카테고리: {0}", *UEnum::GetValueAsString(CurrentCategory));
    }
    //@가시성 비활성화
    else
    {
        //@애니메이션 재생 및 콜백 바인딩
        if (BlendOutAnimation)
        {
            PlayAnimation(BlendOutAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward);
        }
        else
        {
            //@애니메이션이 없는 경우 즉시 제거
            RemoveFromParent();
        }

        UE_LOGFMT(LogMenuUI, Log, "MenuUI 숨김 처리를 시작합니다.");
    }
}

void UMenuUI::OnToolBarInitFinished()
{
    bMenuToolBarInitFinished = true;

    CheckAllUIsInitFinsiehd();
}

void UMenuUI::OnMenuUIContentInitFinished(EMenuCategory Category)
{

    switch (Category)
    {
    case EMenuCategory::Inventory:
        bInventoryUIInitFinished = true;
        break;
    case EMenuCategory::Level:
        bLevelUIInitFinished = true;
        break;
    case EMenuCategory::Map:
        bMapUIInitFinished = true;
        break;
    case EMenuCategory::System:
        bSystemUIInitFinished = true;
        break;
    default:
        break;
    }

    //@모든 UI의 초기화 작업 완료 체크
    CheckAllUIsInitFinsiehd();
}

void UMenuUI::OnMenuCategoryButtonClikced_Implementation(EMenuCategory MenuCategory)
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

void UMenuUI::OnBlendOuAnimationFinished()
{
    //@잠깐의 지연 후 원하는 설정으로 변경
    RemoveFromParent();

    UE_LOGFMT(LogMenuUI, Log, "MenuUI가 뷰포트에서 제거되었습니다.");
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
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