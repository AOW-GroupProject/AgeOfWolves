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

//@구조체
#pragma region Structs
USTRUCT(BlueprintType)
struct FDotGaugeSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dot Gauge")
    FMargin ItemPadding = FMargin(5, 0, 0, 0);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dot Gauge")
    int32 MaxCount = 10;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dot Gauge")
    bool bRightToLeft = true;
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_DELEGATE(FHorizontalDotGaugeInitFinished);
#pragma endregion

/**
 *	@UHorizontalDotGauge (Refactored with Two Pointers)
 */
UCLASS()
class AGEOFWOLVES_API UHorizontalDotGauge : public UUserWidget
{
    GENERATED_BODY()

#pragma region Default Setting
public:
    UHorizontalDotGauge(const FObjectInitializer& ObjectInitializer);

protected:
    virtual void NativeOnInitialized();

public:
    UFUNCTION()
    virtual void InitializeHorizontalDotGauge();
#pragma endregion

#pragma region Property or Subwidgets or Infos...etc
public:
    //@게이지 유닛 생성
    void CreateDotGaugeUnits();

public:
    //@범용 게이지 업데이트
    UFUNCTION(BlueprintCallable, Category = "게이지")
    void UpdateGauge(int32 FilledCount, int32 MaxCount);

    //@최대 개수 설정
    UFUNCTION(BlueprintCallable, Category = "게이지")
    void SetMaxCount(int32 MaxCount);

    //@채워진 개수 설정
    UFUNCTION(BlueprintCallable, Category = "게이지")
    void SetFilledCount(int32 FilledCount);

private:
    //@내부 로직 (방향 무관 통합)
    void ActivateUnits(int32 Count);
    void DeactivateUnits(int32 Count);
    void FillUnits(int32 Count);
    void UnfillUnits(int32 Count);

protected:
    UPROPERTY(BlueprintReadWrite, Category = "게이지", meta = (BindWidget))
    UHorizontalBox* HorizontalDotGaugeBox;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "게이지 | 설정")
    FDotGaugeSettings GaugeSettings;

protected:
    UPROPERTY()
    TArray<UDotGaugeUnit*> DotGaugeUnits;

    UPROPERTY(EditDefaultsOnly, Category = "게이지 | 게이지 유닛 클래스")
    TSubclassOf<UDotGaugeUnit> DotGaugeUnitClass;

protected:
    //@Two Pointers (Bounds)
    int32 ActivateUnitBound;  // 활성화 경계
    int32 FillUnitBound;      // 채움 경계
#pragma endregion

#pragma region Delegates
public:
    FHorizontalDotGaugeInitFinished HorizontalDotGaugeInitFinished;
#pragma endregion

#pragma region Utility
public:
    //@현재 최대 개수 (활성화된 게이지 수)
    FORCEINLINE int32 GetMaxCount() const
    {
        return GaugeSettings.bRightToLeft
            ? (DotGaugeUnits.Num() - ActivateUnitBound)
            : ActivateUnitBound;
    }

    //@현재 채워진 개수
    FORCEINLINE int32 GetFilledCount() const
    {
        return GaugeSettings.bRightToLeft
            ? (DotGaugeUnits.Num() - FillUnitBound)
            : FillUnitBound;
    }

private:
    //@방향 추상화 헬퍼 함수들
    int32 GetInitialActivateBound() const;
    int32 GetInitialFillBound() const;
    int32 GetActivateBoundDelta(int32 Count) const;
    int32 GetFillBoundDelta(int32 Count) const;
    int32 ClampActivateBound(int32 Bound) const;
    int32 ClampFillBound(int32 Bound) const;
    void GetIterationRange(int32 OldBound, int32 NewBound, int32& OutStart, int32& OutEnd) const;

#pragma endregion

};