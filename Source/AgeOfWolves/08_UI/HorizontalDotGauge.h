#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"

#include "HorizontalDotGauge.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHorizontalDotGaguge, Log, All)


//@전방 선언
#pragma region Forward Declaration
class UHorizontalBox;
class UDotGaugeUnit;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
/*
*   @FDotGaugeSettings
* 
*   Dot Gauge의 설정 내용
*/
USTRUCT(BlueprintType)
struct FDotGaugeSettings
{
    GENERATED_BODY()

    //@패딩
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dot Gauge")
        FMargin ItemPadding = FMargin(5, 0, 0, 0);

    //@최대 유닛 갯수
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dot Gauge")
        int32 MaxCount = 10;

    //@채워넣기 방향 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dot Gauge")
        bool bRightToLeft = true;
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_DELEGATE(FHorizontalDotGaugeInitFinished);
#pragma endregion

/**
 *	@UHorizontalDotGauge
 * 
 *	가로로 배치된 이미지 형식의 게이지 UI
 */
UCLASS()
class AGEOFWOLVES_API UHorizontalDotGauge : public UUserWidget
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

#pragma region Default Setting
public:
	UHorizontalDotGauge(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized();
	virtual void NativePreConstruct();
	virtual void NativeConstruct();
	virtual void NativeDestruct();
	//~ End UUserWidget Interface

protected:
	//@외부 바인딩

protected:
	//@내부 바인딩

public:
	//@초기화
    UFUNCTION()
        void InitializeHorizontalDotGauge();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    void CreateDotGaugeUnits();

public:
    // 게이지 설정/업데이트 함수들
    UFUNCTION(BlueprintCallable, Category = "게이지")
        void UpdateFilledCount(int32 Count);

    UFUNCTION(BlueprintCallable, Category = "게이지")
        void UpdateMaxCount(int32 NewMaxCount);

protected:
	//@Dot Gauge를 관리하는 가로 박스
	UPROPERTY(BlueprintReadWrite, Category = "게이지", meta = (BindWidget))
		UHorizontalBox* HorizontalDotGaugeBox;

protected:
    //@Dot Gauge 세팅 관련 구조체
    UPROPERTY(EditDefaultsOnly, Category = "게이지 | 설정")
        FDotGaugeSettings GaugeSettings;

protected:
    //@Dot Gauge Unit 목록
    UPROPERTY()
        TArray<UDotGaugeUnit*> DotGaugeUnits;

    //@Dot Gauge Unit BP 클래스
    UPROPERTY(EditDefaultsOnly, Category = "게이지 | 게이지 유닛 클래스")
        TSubclassOf<UDotGaugeUnit> DotGaugeUnitClass;

protected:
    //@현재 Gauge에 추가된 Unit 개수
    int32 CurrentGaugeUnitCount;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 완료 이벤트
    FHorizontalDotGaugeInitFinished HorizontalDotGaugeInitFinished;
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
	
};
