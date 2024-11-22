#include "HUD_QuickSlotsUI_AbilitySlot.h"
#include "Logging/StructuredLog.h"

#include "Components/EditableTextBox.h"

DEFINE_LOG_CATEGORY(LogQuickSlotsAbilitySlot)

//@Defualt Setting
#pragma region Default Setting
UHUD_QuickSlotsUI_AbilitySlot::UHUD_QuickSlotsUI_AbilitySlot(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	AbilityTypeName = nullptr;
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
void UHUD_QuickSlotsUI_AbilitySlot::AssignNewAbility_Implementation(const FGameplayTag& Tag)
{
    Super::AssignNewAbility_Implementation(Tag);

    //@UEditable Text Box
    if (!AbilityTypeName || !AbilityName)
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "TextBox�� ��ȿ���� �ʽ��ϴ�.");
        return;
    }

    //@FGameplayTag�κ��� Ability Type ��� Ability �� ����
    FString TypeName, AbilityNameStr;
    if (!ExtractAbilityNamesFromTag(Tag, TypeName, AbilityNameStr))
    {
        return;
    }

    //@Ability Type Name
    AbilityTypeName->SetText(FText::FromString(TypeName));
    //@Ability Nmae
    AbilityName->SetText(FText::FromString(AbilityNameStr));
}

void UHUD_QuickSlotsUI_AbilitySlot::AssignNewAbilityFromSlot_Implementation(UAbilitySlot* FromSlot)
{
    Super::AssignNewAbilityFromSlot_Implementation(FromSlot);

    if (!FromSlot)
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "�ҽ� ������ ��ȿ���� �ʽ��ϴ�.");
        return;
    }

    if (!AbilityTypeName || !AbilityName)
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "TextBox�� ��ȿ���� �ʽ��ϴ�.");
        return;
    }

    //@FGameplayTag�κ��� Ability Type ��� Ability �� ����
    FString TypeName, AbilityNameStr;
    if (!ExtractAbilityNamesFromTag(FromSlot->GetAbilityTag(), TypeName, AbilityNameStr))
    {
        return;
    }

    //@Ability Type Name
    AbilityTypeName->SetText(FText::FromString(TypeName));
    //@Ability Nmae
    AbilityName->SetText(FText::FromString(AbilityNameStr));
}

void UHUD_QuickSlotsUI_AbilitySlot::ClearAssignedAbility_Implementation(bool bForceClear)
{
    Super::ClearAssignedAbility_Implementation(bForceClear);

    if (!AbilityTypeName || !AbilityName)
    {
        return;
    }

    //@Ability Type Name
    AbilityTypeName->SetText(FText::GetEmpty());
    //@Ability Nmae
    AbilityName->SetText(FText::GetEmpty());
}
#pragma endregion

//@Delegates
#pragma region Delegates
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
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "��ȿ���� ���� GameplayTag�Դϴ�.");
        return false;
    }

    //@Tag ���ڿ��� '.' �������� �и�
    FString TagString = Tag.ToString();
    TArray<FString> TagHierarchy;
    TagString.ParseIntoArray(TagHierarchy, TEXT("."), true);

    //@�ּ� 3���� �̻��� �±װ� �ʿ� (Ability.Type.Name)
    if (TagHierarchy.Num() < 3)
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "�����Ƽ �±��� ������ ������� �ʽ��ϴ�: {0}", TagString);
        return false;
    }

    //@���������� 2��° ������ TypeName����
    OutTypeName = TagHierarchy[TagHierarchy.Num() - 2];
    //@������ ������ AbilityName����
    OutAbilityName = TagHierarchy[TagHierarchy.Num() - 1];

    return true;
}
#pragma endregion