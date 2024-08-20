#include "MenuUIContent.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogMenuUIContent)

UMenuUIContent::UMenuUIContent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}

void UMenuUIContent::NativeOnInitialized()
{
	Super::NativeOnInitialized();

}

void UMenuUIContent::NativePreConstruct()
{
	Super::NativePreConstruct();

}

void UMenuUIContent::NativeConstruct()
{
	Super::NativeConstruct();

}

void UMenuUIContent::NativeDestruct()
{
	Super::NativeDestruct();

}

void UMenuUIContent::InitializeMenuUIContent(EMenuCategory Category)
{
	//@Menu Category
	MenuCategory = Category;

	//@TODO: ���� Subwidget ����
	
	//@TODO: �ʱ�ȭ ��û �̺�Ʈ ȣ��
	
	//@TOOD: ���� ����� ���� �� ���� ���ε� �����ϸ� �����ݴϴ�.
	//CheckMenuUIContentInitFinished();
}

void UMenuUIContent::CheckMenuUIContentInitFinished() const
{
	//@TODO: Internal Binding ���� �� Callback��� ���� üũ�� �׸�� ��� ������ Ȯ��

	//@�ʱ�ȭ �Ϸ� �̺�Ʈ ȣ��
	if (MenuCategory == EMenuCategory::MAX)
	{
		UE_LOGFMT(LogMenuUIContent, Warning, "�Ҵ� ���� Menu Category�� ��ȿ���� �ʽ��ϴ�.");
		return;
	}

	//@�ʱ�ȭ �Ϸ� �̺�Ʈ ȣ��
	MenuUIContentInitFinished.ExecuteIfBound(MenuCategory);
}
