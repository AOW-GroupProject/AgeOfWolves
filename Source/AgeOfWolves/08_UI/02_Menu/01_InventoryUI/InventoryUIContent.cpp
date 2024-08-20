// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryUIContent.h"
#include "Logging/StructuredLog.h"

#include "08_UI/02_Menu/01_InventoryUI/InventoryToolBar.h"
#include "08_UI/ItemSlot.h"

#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"

DEFINE_LOG_CATEGORY(LogInventoryUIContent)
// UE_LOGFMT(LogInventoryUIContent, Log, "");

#pragma region Default Setting
UInventoryUIContent::UInventoryUIContent(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void UInventoryUIContent::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UInventoryUIContent::NativePreConstruct()
{
    Super::NativePreConstruct();
}

void UInventoryUIContent::NativeConstruct()
{
    Super::NativeConstruct();
}

void UInventoryUIContent::NativeDestruct()
{
    Super::NativeDestruct();
}

void UInventoryUIContent::InternalBindingToInventoryToolBar(UInventoryToolBar* ToolBar)
{
    //@Inventory Tool Bar
    if (!ToolBar)
    {
        UE_LOGFMT(LogInventoryUIContent, Error, "ToolBar UI�� ��ȿ���� �ʽ��ϴ�.");
        return;
    }

    ToolBar->InventoryToolBarInitFinished.BindUFunction(this, "OnInventoryToolBarInitFinished");
}

void UInventoryUIContent::InitializeInventoryUIContent()
{
    //@Tool Bar
    CreateToolBar();
    //@Item Slots
    CreateItemSlots();
    //@Item Description
    CreateItemDescription();
    //@�ʱ�ȭ ���� ��û �̺�Ʈ
    RequestStartInitByInventoryUIContent.Broadcast();
}

void UInventoryUIContent::CheckInventoryUIContentInitialization()
{
    if (bInventoryToolBarReady)
    {
        InventoryUIContentInitFinished.ExecuteIfBound();
    }
}
#pragma endregion

#pragma region SubWidgets
void UInventoryUIContent::CreateToolBar()
{
    //@TODO: Inventory Content�� Tool Bar ���� �۾�
}

void UInventoryUIContent::CreateItemSlots()
{
    //@TODO: Item Slots ����
    //@TODO: Item Slots Overlay�� �߰�
}

void UInventoryUIContent::CreateItemDescription()
{
    //@TODO: Item Description ����
    
    //@TODO: Item Description Overlay�� �߰�
}
#pragma endregion

#pragma region Callbacks
void UInventoryUIContent::OnInventoryToolBarInitFinished()
{
    bInventoryToolBarReady = true;

    CheckInventoryUIContentInitialization();
}
#pragma endregion
