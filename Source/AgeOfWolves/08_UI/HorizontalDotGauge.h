#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "HorizontalDotGauge.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHorizontalDotGauge, Log, All)

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

    //@생성 가능한 최대 유닛 갯수 (풀 크기)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dot Gauge")
    int32 MaxCount = 10;

    //@채워넣기 방향 설정 (true: 오른쪽→왼쪽, false: 왼쪽→오른쪽)
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
 *	가로로 배치된 이미지 형식의 범용 게이지 UI
 *
 *  사용 예시:
 *  - 마나 게이지: SetMaxCount(10), SetFilledCount(7) → [●●●●●●●○○○]
 *  - 하트 HP: SetMaxCount(5), SetFilledCount(3) → [♥♥♥♡♡]
 *  - 화살 개수: SetMaxCount(20), SetFilledCount(15) → [➤➤➤...○○○○○]
 *  - 인벤토리 슬롯: UpdateGauge(8, 12) → [■■■■■■■■□□□□]
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
    //~ End UUserWidget Interface

protected:
    //@외부 바인딩

protected:
    //@내부 바인딩

public:
    //@초기화
    UFUNCTION()
    virtual void InitializeHorizontalDotGauge();
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    //@게이지 유닛 생성 (내부 함수)
    void CreateDotGaugeUnits();

public:
    //@범용 게이지 업데이트 (권장 - 한 번에 설정)
    // FilledCount: 채워진 유닛 개수, MaxCount: 활성화할 유닛 총 개수
    UFUNCTION(BlueprintCallable, Category = "게이지")
    void UpdateGauge(int32 FilledCount, int32 MaxCount);

    //@최대 개수 설정 (MaxCount만큼 빈 게이지 표시)
    UFUNCTION(BlueprintCallable, Category = "게이지")
    void SetMaxCount(int32 MaxCount);

    //@채워진 개수 설정 (FilledCount만큼 게이지 채우기)
    UFUNCTION(BlueprintCallable, Category = "게이지")
    void SetFilledCount(int32 FilledCount);

private:
    //@채우기/비우기 내부 로직
    void FillUnits(int32 CountToFill);
    void UnfillUnits(int32 CountToUnfill);
    void ActivateUnits(int32 Count);
    void DeactivateUnits(int32 Count);

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
    //@현재 활성화된 유닛 총 개수
    int32 MaxUnitCount;

    //@현재 채워진 유닛 개수
    int32 FilledUnitCount;
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
public:
    //@현재 최대 개수 (활성화된 게이지 수)
    FORCEINLINE int32 GetMaxCount() const { return MaxUnitCount; }

    //@현재 채워진 개수
    FORCEINLINE int32 GetFilledCount() const { return FilledUnitCount; }
#pragma endregion

};