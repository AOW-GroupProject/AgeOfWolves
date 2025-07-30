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
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : TextBox�� ��ȿ���� �ʽ��ϴ�.", __FUNCDNAME__);
        return;
    }

    //@FGameplayTag�κ��� Ability Type ���� Ability �� ����
    FString TypeName, AbilityNameStr;
    if (!ExtractAbilityNamesFromTag(Tag, TypeName, AbilityNameStr))
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : Tag�κ��� �̸� ���⿡ �����߽��ϴ�.", __FUNCDNAME__);
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
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : �ҽ� ������ ��ȿ���� �ʽ��ϴ�.", __FUNCDNAME__);
        return;
    }

    if (!AbilityName )
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : TextBox�� ��ȿ���� �ʽ��ϴ�.", __FUNCDNAME__);
        return;
    }

    //@FGameplayTag�κ��� Ability Type ���� Ability �� ����
    FString TypeName, AbilityNameStr;
    if (!ExtractAbilityNamesFromTag(FromSlot->GetAbilityTag(), TypeName, AbilityNameStr))
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : Tag�κ��� �̸� ���⿡ �����߽��ϴ�.", __FUNCDNAME__);
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
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : TextBox�� ��ȿ���� �ʽ��ϴ�.", __FUNCDNAME__);
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
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : ��ȿ���� ���� GameplayTag�Դϴ�.", __FUNCDNAME__);
        return false;
    }

    //@Tag ���ڿ��� '.' �������� �и�
    FString TagString = Tag.ToString();
    TArray<FString> TagHierarchy;
    TagString.ParseIntoArray(TagHierarchy, TEXT("."), true);

    //@�ּ� 3���� �̻��� �±װ� �ʿ� (Ability.Type.Name)
    if (TagHierarchy.Num() < 3)
    {
        UE_LOGFMT(LogQuickSlotsAbilitySlot, Warning, "{0} : �����Ƽ �±��� ������ ������� �ʽ��ϴ�: {1}", __FUNCDNAME__, TagString);
        return false;
    }

    //@Type Name�� Ability Name ����
    OutTypeName = TagHierarchy[2];  // Battoujutsu
    OutAbilityName = TagHierarchy[TagHierarchy.Num() - 1];  // ���� �����

    return true;
}
#pragma endregion