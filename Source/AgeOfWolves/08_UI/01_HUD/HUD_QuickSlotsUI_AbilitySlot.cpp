#include "HUD_QuickSlotsUI_AbilitySlot.h"
#include "Logging/StructuredLog.h"

#include "Components/EditableTextBox.h"

DEFINE_LOG_CATEGORY(LogQuickSlotsAbilitySlot)

//@Defualt Setting
#pragma region Default Setting
UHUD_QuickSlotsUI_AbilitySlot::UHUD_QuickSlotsUI_AbilitySlot(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	AbilityName = nullptr;
}

void UHUD_QuickSlotsUI_AbilitySlot::NativeOnInitialized()
{

	Super::NativeOnInitialized();

}

void UHUD_QuickSlotsUI_AbilitySlot::InitializeAbilitySlot()
{

	Super::InitializeAbilitySlot();

}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UHUD_QuickSlotsUI_AbilitySlot::AssignNewAbility_Implementation(FGameplayTag Tag)
{
    Super::AssignNewAbility_Implementation(Tag);

    //@UEditable Text Box
    if (!AbilityName)
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : TextBox가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    //@FGameplayTag로부터 Ability Type 명과 Ability 명 추출
    FString TypeName, AbilityNameStr;
    if (!ExtractAbilityNamesFromTag(Tag, TypeName, AbilityNameStr))
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : Tag로부터 이름 추출에 실패했습니다.", __FUNCDNAME__);
        return;
    }

    //@Ability Name
    AbilityName->SetText(FText::FromString(AbilityNameStr));
}

void UHUD_QuickSlotsUI_AbilitySlot::AssignNewAbilityFromSlot_Implementation(UAbilitySlot* FromSlot)
{
    Super::AssignNewAbilityFromSlot_Implementation(FromSlot);

    if (!FromSlot)
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : 소스 슬롯이 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    if (!AbilityName )
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : TextBox가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    //@FGameplayTag로부터 Ability Type 명과 Ability 명 추출
    FString TypeName, AbilityNameStr;
    if (!ExtractAbilityNamesFromTag(FromSlot->GetAbilityTag(), TypeName, AbilityNameStr))
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : Tag로부터 이름 추출에 실패했습니다.", __FUNCDNAME__);
        return;
    }

    //@Ability Name
    AbilityName->SetText(FText::FromString(AbilityNameStr));
}

void UHUD_QuickSlotsUI_AbilitySlot::ClearAssignedAbility_Implementation(bool bForceClear)
{
    Super::ClearAssignedAbility_Implementation(bForceClear);

    if (!AbilityName)
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : TextBox가 유효하지 않습니다.", __FUNCDNAME__);
        return;
    }

    //@Ability Name
    AbilityName->SetText(FText::GetEmpty());
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
bool UHUD_QuickSlotsUI_AbilitySlot::ExtractAbilityNamesFromTag(const FGameplayTag& Tag, FString& OutTypeName, FString& OutAbilityName) const
{
    if (!Tag.IsValid())
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : 유효하지 않은 GameplayTag입니다.", __FUNCDNAME__);
        return false;
    }

    //@Tag 문자열을 '.' 기준으로 분리
    FString TagString = Tag.ToString();
    TArray<FString> TagHierarchy;
    TagString.ParseIntoArray(TagHierarchy, TEXT("."), true);

    //@최소 3계층 이상의 태그가 필요 (Ability.Type.Name)
    if (TagHierarchy.Num() < 3)
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : 어빌리티 태그의 계층이 충분하지 않습니다: {1}", __FUNCDNAME__, TagString);
        return false;
    }

    //@Type Name과 Ability Name 설정
    OutTypeName = TagHierarchy[2];  // Battoujutsu
    OutAbilityName = TagHierarchy[TagHierarchy.Num() - 1];  // 실제 기술명

    return true;
}
#pragma endregion