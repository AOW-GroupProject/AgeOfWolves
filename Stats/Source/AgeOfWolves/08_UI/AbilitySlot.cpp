#include "AbilitySlot.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"

#include "Components/Image.h"
#include "Components/VerticalBox.h"

#include "14_Subsystem/AbilityManagerSubsystem.h"

DEFINE_LOG_CATEGORY(LogAbilitySlot)

//@Default Setting
#pragma region Default Setting
UAbilitySlot::UAbilitySlot(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    AbilitySlotBox = nullptr;
    SlotBGImage = nullptr;
    AbilityImage = nullptr;
    bIsFilled = false;

    AbilityTag = FGameplayTag();

    AbilityManagerCache = nullptr;
}

void UAbilitySlot::NativeOnInitialized()
{
    Super::NativeOnInitialized();

}

void UAbilitySlot::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UAbilitySlot::NativeConstruct()
{
    Super::NativeConstruct();
}

void UAbilitySlot::NativeDestruct()
{
    Super::NativeDestruct();
}

void UAbilitySlot::InitializeAbilitySlot()
{
    //@캐싱
    if (UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this))
    {
        AbilityManagerCache = GameInstance->GetSubsystem<UAbilityManagerSubsystem>();
        if (!AbilityManagerCache)
        {
            UE_LOGFMT(LogAbilitySlot, Error, "Ability Manaer Subsystem 캐싱 실패");
        }
        else
        {
            UE_LOGFMT(LogAbilitySlot, Log, "Ability Manaer Subsystem 캐싱 성공");
        }
    }

    //@초기화 완료 이벤트
    AbilitySlotInitFinished.ExecuteIfBound();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UAbilitySlot::AssignNewAbility_Implementation(FGameplayTag Tag)
{
    //@Ability Tag
    if (!Tag.IsValid())
    {
        UE_LOGFMT(LogAbilitySlot, Warning, "유효하지 않은 GameplayTag가 전달되었습니다.");
        return;
    }
    //@Ability Manager Subsystem
    if (!AbilityManagerCache)
    {
        UE_LOGFMT(LogAbilitySlot, Warning, "AbilityManagerCache가 유효하지 않습니다.");
        return;
    }
    //@Ability Image
    if (!AbilityImage)
    {
        UE_LOGFMT(LogAbilitySlot, Warning, "AbilityImage가 유효하지 않습니다.");
        return;
    }

    //@Ability Tag
    SetAbilityTag(Tag);
    //@상태 갱신
    SetIsFilled(true);

    //@아이콘 이미지 설정
    UTexture2D* IconTexture = AbilityManagerCache->GetAbilityIconTexture2D(Tag);
    if (!IconTexture->IsValidLowLevel())
    {
        AbilityImage->SetBrushFromTexture(nullptr);
        AbilityImage->SetBrushTintColor(FLinearColor::Transparent);
        UE_LOGFMT(LogAbilitySlot, Warning, "어빌리티 아이콘 이미지를 찾을 수 없습니다.");
        return;
    }

    //@Set Bursh From Texture
    AbilityImage->SetBrushFromTexture(IconTexture);
    AbilityImage->SetBrushTintColor(FLinearColor::White);

    UE_LOGFMT(LogAbilitySlot, Log, "새 어빌리티가 슬롯에 할당되었습니다. Tag: {0}", AbilityTag.ToString());
}

void UAbilitySlot::AssignNewAbilityFromSlot_Implementation(UAbilitySlot* FromSlot)
{

    if (!FromSlot)
    {
        UE_LOGFMT(LogAbilitySlot, Warning, "소스 AbilitySlot이 유효하지 않습니다.");
        return;
    }

    if (!FromSlot->AbilityTag.IsValid())
    {
        UE_LOGFMT(LogAbilitySlot, Warning, "소스 AbilitySlot의 GameplayTag가 유효하지 않습니다.");
        return;
    }

    if (!AbilityManagerCache)
    {
        UE_LOGFMT(LogAbilitySlot, Warning, "AbilityManagerCache가 유효하지 않습니다.");
        return;
    }

    if (!AbilityImage)
    {
        UE_LOGFMT(LogAbilitySlot, Warning, "AbilityImage가 유효하지 않습니다.");
        return;
    }

    //@Tag
    SetAbilityTag(FromSlot->GetAbilityTag());
    //@Filled
    SetIsFilled(FromSlot->GetIsFilled());

    //@아이콘 이미지 설정
    UTexture2D* IconTexture = AbilityManagerCache->GetAbilityIconTexture2D(AbilityTag);
    if (!IconTexture)
    {
        AbilityImage->SetBrushFromTexture(nullptr);
        AbilityImage->SetBrushTintColor(FLinearColor::Transparent);
        UE_LOGFMT(LogAbilitySlot, Warning, "어빌리티 아이콘 이미지를 찾을 수 없습니다.");
        return;
    }

    //@Set Bursh From Texture
    AbilityImage->SetBrushFromTexture(IconTexture);
    AbilityImage->SetBrushTintColor(FLinearColor::White);
    UE_LOGFMT(LogAbilitySlot, Log, "어빌리티가 다른 슬롯으로부터 복사되었습니다. Tag: {0}", AbilityTag.ToString());
}

void UAbilitySlot::ClearAssignedAbility_Implementation(bool bForceClear)
{
    //@Tag
    AbilityTag = FGameplayTag();
    //@상태 초기화
    SetIsFilled(false);

    if (!AbilityImage)
    {
        UE_LOGFMT(LogAbilitySlot, Warning, "AbilityImage가 유효하지 않습니다.");
        return;
    }

    //@Set Bursh From Texture
    AbilityImage->SetBrushFromTexture(nullptr);
    AbilityImage->SetBrushTintColor(FLinearColor::Transparent);

    UE_LOGFMT(LogAbilitySlot, Log, "어빌리티 슬롯의 어빌리티가 제거되었습니다. 강제 제거: {0}",
        bForceClear ? TEXT("Yes") : TEXT("No"));
}

void UAbilitySlot::SetImagesSize(const FVector2D& BGSize, const FVector2D& ImageSize)
{
    if (!SlotBGImage || !AbilityImage)
    {
        UE_LOG(LogAbilitySlot, Warning, TEXT("SetImagesSize - 이미지가 유효하지 않습니다."));
        return;
    }

    FSlateBrush BGBrush = SlotBGImage->Brush;
    BGBrush.ImageSize = BGSize;
    BGBrush.SetImageSize(BGSize);
    SlotBGImage->Brush = BGBrush;
    SlotBGImage->Brush.DrawAs = ESlateBrushDrawType::Image;
    SlotBGImage->SetDesiredSizeOverride(BGSize);

    FSlateBrush AbilityBrush = AbilityImage->Brush;
    AbilityBrush.ImageSize = ImageSize;
    AbilityBrush.SetImageSize(ImageSize);
    AbilityImage->Brush = AbilityBrush;
    AbilityImage->Brush.DrawAs = ESlateBrushDrawType::Image;
    AbilityImage->SetDesiredSizeOverride(ImageSize);
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion