// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelUI.h"
#include "Logging/StructuredLog.h"

DEFINE_LOG_CATEGORY(LogLevelUI)


ULevelUI::ULevelUI(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{}

void ULevelUI::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //@TODO: 외부 바인딩
}

void ULevelUI::NativePreConstruct()
{
    Super::NativePreConstruct();

}

void ULevelUI::NativeConstruct()
{
    Super::NativeConstruct();

}

void ULevelUI::NativeDestruct()
{
    Super::NativeDestruct();

}

void ULevelUI::InitializeLevelUI()
{
    //@TODO: 초기화 작업 수행

    LevelUIInitFinished.ExecuteIfBound();
}
