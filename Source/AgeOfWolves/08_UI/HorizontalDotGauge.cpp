#include "HorizontalDotGauge.h"
#include "Logging/StructuredLog.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "08_UI/DotGaugeUnit.h"

DEFINE_LOG_CATEGORY(LogHorizontalDotGauge)

//@Defualt Setting
#pragma region Default Setting
UHorizontalDotGauge::UHorizontalDotGauge(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
    HorizontalDotGaugeBox = nullptr;
    DotGaugeUnits.Empty();

    MaxUnitCount = 0;
    FilledUnitCount = 0;
}

void UHorizontalDotGauge::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UHorizontalDotGauge::InitializeHorizontalDotGauge()
{
    //@Dot Gauge Unit 생성
    CreateDotGaugeUnits();

    //@초기화 완료 이벤트
    HorizontalDotGaugeInitFinished.ExecuteIfBound();
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UHorizontalDotGauge::CreateDotGaugeUnits()
{
    if (!HorizontalDotGaugeBox || !DotGaugeUnitClass)
    {
        UE_LOGFMT(LogHorizontalDotGauge, Warning, "HorizontalDotGaugeBox 또는 DotGaugeUnitClass가 유효하지 않습니다.");
        return;
    }

    //@Create Widget (풀 크기만큼 미리 생성)
    for (int32 i = 0; i < GaugeSettings.MaxCount; ++i)
    {
        UDotGaugeUnit* NewUnit = CreateWidget<UDotGaugeUnit>(this, DotGaugeUnitClass);
        if (!NewUnit)
        {
            UE_LOGFMT(LogHorizontalDotGauge, Error, "DotGaugeUnit 생성 실패: {0}번째", i);
            continue;
        }

        //@HorizontalBox에 추가
        UHorizontalBoxSlot* GaugeUnit = HorizontalDotGaugeBox->AddChildToHorizontalBox(NewUnit);
        if (GaugeUnit)
        {
            GaugeUnit->SetPadding(GaugeSettings.ItemPadding);
            GaugeUnit->SetHorizontalAlignment(HAlign_Right);
            GaugeUnit->SetVerticalAlignment(VAlign_Fill);
        }

        //@배열에 추가
        DotGaugeUnits.Add(NewUnit);
    }

    //@초기 상태: 모든 유닛 비활성화
    MaxUnitCount = 0;
    FilledUnitCount = 0;

    UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 초기화 완료. 총 {0}개의 유닛이 생성됨 (비활성 상태)", DotGaugeUnits.Num());
}

void UHorizontalDotGauge::UpdateGauge(int32 FilledCount, int32 MaxCount)
{
    //@MaxCount 먼저 설정 (게이지 슬롯 개수)
    SetMaxCount(MaxCount);

    //@FilledCount 설정 (채우기)
    SetFilledCount(FilledCount);

    UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 업데이트 완료. Filled: {0}/{1}", FilledUnitCount, MaxUnitCount);
}

void UHorizontalDotGauge::SetMaxCount(int32 MaxCount)
{
    if (!HorizontalDotGaugeBox)
    {
        UE_LOGFMT(LogHorizontalDotGauge, Warning, "HorizontalDotGaugeBox가 유효하지 않습니다.");
        return;
    }

    //@범위 체크
    MaxCount = FMath::Clamp(MaxCount, 0, GaugeSettings.MaxCount);

    if (MaxCount == MaxUnitCount)
    {
        UE_LOGFMT(LogHorizontalDotGauge, Log, "MaxCount가 이미 {0}입니다. 변경 없음.", MaxUnitCount);
        return;
    }

    //@MaxCount 증가 (게이지 슬롯 추가)
    if (MaxCount > MaxUnitCount)
    {
        int32 DifferenceCount = MaxCount - MaxUnitCount;
        ActivateUnits(DifferenceCount);
    }
    //@MaxCount 감소 (게이지 슬롯 제거)
    else if (MaxCount < MaxUnitCount)
    {
        int32 DifferenceCount = MaxUnitCount - MaxCount;
        DeactivateUnits(DifferenceCount);
    }

    //@MaxUnitCount 업데이트
    MaxUnitCount = MaxCount;

    UE_LOGFMT(LogHorizontalDotGauge, Log, "최대 개수 설정 완료. MaxCount: {0}, FilledCount: {1}", MaxUnitCount, FilledUnitCount);
}

void UHorizontalDotGauge::SetFilledCount(int32 FilledCount)
{
    if (!HorizontalDotGaugeBox)
    {
        UE_LOGFMT(LogHorizontalDotGauge, Warning, "HorizontalDotGaugeBox가 유효하지 않습니다.");
        return;
    }

    //@범위 체크 (MaxUnitCount 이하로 제한)
    FilledCount = FMath::Clamp(FilledCount, 0, MaxUnitCount);

    //@차이 계산
    int32 Difference = FilledCount - FilledUnitCount;

    if (Difference < 0)
    {
        //@감소 (Unfill)
        UnfillUnits(FMath::Abs(Difference));
    }
    else if (Difference > 0)
    {
        //@증가 (Fill)
        FillUnits(Difference);
    }

    UE_LOGFMT(LogHorizontalDotGauge, Log, "채워진 개수 설정 완료. FilledCount: {0}/{1}", FilledUnitCount, MaxUnitCount);
}

void UHorizontalDotGauge::ActivateUnits(int32 Count)
{
    if (Count <= 0) return;

    int32 ActivatedCount = 0;

    if (GaugeSettings.bRightToLeft)
    {
        //@오른쪽에서 왼쪽으로 활성화 (뒤에서부터)
        for (int32 i = 0; i < Count && (MaxUnitCount + i) < DotGaugeUnits.Num(); ++i)
        {
            int32 Index = MaxUnitCount + i;
            if (UDotGaugeUnit* Unit = DotGaugeUnits[Index])
            {
                Unit->ActivateDotGaugeUnit();
                ActivatedCount++;
                UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 활성화 (RightToLeft): {0}번째", Index);
            }
        }
    }
    else
    {
        //@왼쪽에서 오른쪽으로 활성화 (앞에서부터)
        for (int32 i = 0; i < Count && (MaxUnitCount + i) < DotGaugeUnits.Num(); ++i)
        {
            int32 Index = MaxUnitCount + i;
            if (UDotGaugeUnit* Unit = DotGaugeUnits[Index])
            {
                Unit->ActivateDotGaugeUnit();
                ActivatedCount++;
                UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 활성화 (LeftToRight): {0}번째", Index);
            }
        }
    }
}

void UHorizontalDotGauge::DeactivateUnits(int32 Count)
{
    if (Count <= 0) return;

    int32 FilledUnitsToRemove = 0;

    if (GaugeSettings.bRightToLeft)
    {
        //@오른쪽에서 왼쪽으로 비활성화 (앞에서부터 제거)
        for (int32 i = 0; i < Count && i < MaxUnitCount; ++i)
        {
            if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
            {
                if (Unit->IsFilled())
                {
                    FilledUnitsToRemove++;
                }
                Unit->DeactivateDotGaugeUnit();
                UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 비활성화 (RightToLeft): {0}번째", i);
            }
        }
    }
    else
    {
        //@왼쪽에서 오른쪽으로 비활성화 (뒤에서부터 제거)
        for (int32 i = MaxUnitCount - 1; i >= 0 && (MaxUnitCount - i) <= Count; --i)
        {
            if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
            {
                if (Unit->IsFilled())
                {
                    FilledUnitsToRemove++;
                }
                Unit->DeactivateDotGaugeUnit();
                UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 비활성화 (LeftToRight): {0}번째", i);
            }
        }
    }

    //@제거된 Filled 유닛 수만큼 FilledCount 감소
    FilledUnitCount = FMath::Max(0, FilledUnitCount - FilledUnitsToRemove);
}

void UHorizontalDotGauge::FillUnits(int32 CountToFill)
{
    if (CountToFill <= 0) return;

    int32 FilledCount = 0;

    if (GaugeSettings.bRightToLeft)
    {
        //@오른쪽에서 왼쪽으로 채우기 (역순)
        for (int32 i = MaxUnitCount - 1; i >= 0 && FilledCount < CountToFill; --i)
        {
            if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
            {
                if (Unit->IsActive() && !Unit->IsFilled())
                {
                    Unit->UpdateDotGaugeUnit(true);
                    FilledUnitCount++;
                    FilledCount++;
                    UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 Fill (RightToLeft): {0}번째", i);
                }
            }
        }
    }
    else
    {
        //@왼쪽에서 오른쪽으로 채우기 (정순)
        for (int32 i = 0; i < MaxUnitCount && FilledCount < CountToFill; ++i)
        {
            if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
            {
                if (Unit->IsActive() && !Unit->IsFilled())
                {
                    Unit->UpdateDotGaugeUnit(true);
                    FilledUnitCount++;
                    FilledCount++;
                    UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 Fill (LeftToRight): {0}번째", i);
                }
            }
        }
    }
}

void UHorizontalDotGauge::UnfillUnits(int32 CountToUnfill)
{
    if (CountToUnfill <= 0) return;

    int32 UnfilledCount = 0;

    if (GaugeSettings.bRightToLeft)
    {
        //@오른쪽에서 왼쪽으로 비우기 (정순으로 찾기)
        for (int32 i = 0; i < MaxUnitCount && UnfilledCount < CountToUnfill; ++i)
        {
            if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
            {
                if (Unit->IsActive() && Unit->IsFilled())
                {
                    Unit->UpdateDotGaugeUnit(false);
                    FilledUnitCount--;
                    UnfilledCount++;
                    UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 Unfill (RightToLeft): {0}번째", i);
                }
            }
        }
    }
    else
    {
        //@왼쪽에서 오른쪽으로 비우기 (역순으로 찾기)
        for (int32 i = MaxUnitCount - 1; i >= 0 && UnfilledCount < CountToUnfill; --i)
        {
            if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
            {
                if (Unit->IsActive() && Unit->IsFilled())
                {
                    Unit->UpdateDotGaugeUnit(false);
                    FilledUnitCount--;
                    UnfilledCount++;
                    UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 Unfill (LeftToRight): {0}번째", i);
                }
            }
        }
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion