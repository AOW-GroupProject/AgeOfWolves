// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLevelUI, Log, All)

DECLARE_DELEGATE(FLevelUIInitFinished);

/**
 * @ULevelUI
 * 
 * 캐릭터의 Status 관련 정보를 보여주는 UI, Menu UI의 자식 UI입니다.
 */
UCLASS()
class AGEOFWOLVES_API ULevelUI : public UUserWidget
{
	GENERATED_BODY()
	
#pragma region Default Setting
public:
	ULevelUI(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface
public:
	//@초기화
	UFUNCTION()
		void InitializeLevelUI();
#pragma endregion

#pragma region Delegates
public:
	//@초기화 완료 이벤트
	FLevelUIInitFinished LevelUIInitFinished;
#pragma endregion
};
