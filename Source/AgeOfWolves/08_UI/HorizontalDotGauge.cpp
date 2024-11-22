// Fill out your copyright notice in the Description page of Project Settings.


#include "HorizontalDotGauge.h"
#include "Logging/StructuredLog.h"

#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"

#include "08_UI/DotGaugeUnit.h"

DEFINE_LOG_CATEGORY(LogHorizontalDotGaguge)

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
        UE_LOGFMT(LogHorizontalDotGaguge, Warning, "HorizontalDotGaugeBox 또는 DotGaugeUnitClass가 유효하지 않습니다.");
        return;
    }

    //@Create Widget
    for (int32 i = 0; i < GaugeSettings.MaxCount; ++i)
    {
        UDotGaugeUnit* NewUnit = CreateWidget<UDotGaugeUnit>(this, DotGaugeUnitClass);
        if (!NewUnit)
        {
            UE_LOGFMT(LogHorizontalDotGaguge, Error, "DotGaugeUnit 생성 실패: {0}번째", i);
            continue;
        }

        // 마지막 위치에 추가하여 새로운 유닛이 오른쪽에 추가되도록 함
        UHorizontalBoxSlot* GaugeUnit = HorizontalDotGaugeBox->AddChildToHorizontalBox(NewUnit);
        if (GaugeUnit)
        {
            GaugeUnit->SetPadding(GaugeSettings.ItemPadding);
            GaugeUnit->SetHorizontalAlignment(HAlign_Right);
            GaugeUnit->SetVerticalAlignment(VAlign_Fill);
        }

        // 배열에도 추가
        DotGaugeUnits.Add(NewUnit);
    }

    UE_LOGFMT(LogHorizontalDotGaguge, Log, "게이지 초기화 완료. 총 {0}개의 유닛이 생성됨", DotGaugeUnits.Num());
}

void UHorizontalDotGauge::UpdateFilledCount(int32 Count)
{
    if (!HorizontalDotGaugeBox)
    {
        UE_LOGFMT(LogHorizontalDotGaguge, Warning, "HorizontalDotGaugeBox가 유효하지 않습니다.");
        return;
    }

    //@FilledCount와 목표 Count의 차이
    int32 Difference = Count - FilledUnitCount;
    int32 RemainingDifference = FMath::Abs(Difference);

    //@Count가 현재보다 작음 (Unfill 필요)
    if (Difference < 0)
    {
        //@정순으로 순회하며 Filled Unit 찾아서 Unfill
        for (int32 i = DotGaugeUnits.Num() - 1; i >= 0 && RemainingDifference > 0; --i)
        {
            if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
            {
                if (Unit->IsActive() && Unit->IsFilled())
                {
                    Unit->UpdateDotGaugeUnit(false);
                    FilledUnitCount--;
                    RemainingDifference--;
                    UE_LOGFMT(LogHorizontalDotGaguge, Log, "게이지 유닛 Unfill: {0}번째, 남은 차이: {1}", i, RemainingDifference);
                }
            }
        }
    }
    //@Count가 현재보다 큼 (Fill 필요)
    else if (Difference > 0)
    {
        //@MaxCount 초과 체크
        if (Count > MaxUnitCount)
        {
            UpdateMaxCount(Count);
        }

        //@역순으로 순회하며 Unfilled Unit 찾아서 Fill
        for (int32 i = 0; i < DotGaugeUnits.Num() && RemainingDifference > 0; ++i)
        {
            if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
            {
                if (Unit->IsActive() && !Unit->IsFilled())
                {
                    Unit->UpdateDotGaugeUnit(true);
                    FilledUnitCount++;
                    RemainingDifference--;
                    UE_LOGFMT(LogHorizontalDotGaguge, Log, "게이지 유닛 Fill: {0}번째, 남은 차이: {1}", i, RemainingDifference);
                }
            }
        }
    }

    UE_LOGFMT(LogHorizontalDotGaguge, Log, "Filled Count 업데이트 완료. 현재 Filled Unit: {0}/{1}, 목표 Count: {2}, 처리된 변경: {3}",
        FilledUnitCount, MaxUnitCount, Count, FMath::Abs(Difference) - RemainingDifference);
}

void UHorizontalDotGauge::UpdateMaxCount(int32 NewMaxCount)
{
    //@New Max Count, Horizontal Dot Gauge Box 체크
    if (NewMaxCount <= 0 || !HorizontalDotGaugeBox)
    {
        UE_LOGFMT(LogHorizontalDotGaguge, Warning, "유효하지 않은 NewMaxCount 값이거나 HorizontalDotGaugeBox가 유효하지 않습니다.");
        return;
    }

    NewMaxCount = FMath::Clamp(NewMaxCount, 0, GaugeSettings.MaxCount);

    //@MaxCount 증가
    if (NewMaxCount > MaxUnitCount)
    {
        int32 DifferenceCount = NewMaxCount - MaxUnitCount;
        int32 ActivatedCount = 0;

        //@MaxUnitCount 인덱스부터 차례대로 활성화
        for (int32 i = 0; i < DifferenceCount && (MaxUnitCount + i) < DotGaugeUnits.Num(); ++i)
        {
            if (UDotGaugeUnit* UnitToActivate = DotGaugeUnits[MaxUnitCount + i])
            {
                UnitToActivate->ActivateDotGaugeUnit();
                UnitToActivate->UpdateDotGaugeUnit(true);
                ActivatedCount++;
                UE_LOGFMT(LogHorizontalDotGaguge, Log, "게이지 유닛 활성화 및 Fill: {0}번째", MaxUnitCount + i);
            }
        }

        //@Filled Count 업데이트
        FilledUnitCount += ActivatedCount;
    }
    //@MaxCount 감소
    else if (NewMaxCount < MaxUnitCount)
    {
        int32 DifferenceCount = MaxUnitCount - NewMaxCount;
        int32 FilledUnitsToRemove = 0;

        //@제거할 Filled Unit 수 계산
        for (int32 i = 0; i < DifferenceCount && i < DotGaugeUnits.Num(); ++i)
        {
            if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
            {
                if (Unit->IsFilled())
                {
                    FilledUnitsToRemove++;
                }
            }
        }

        //@FilledCount가 음수가 되지 않도록 보정
        FilledUnitsToRemove = FMath::Min(FilledUnitsToRemove, FilledUnitCount);
        FilledUnitCount = FMath::Max(0, FilledUnitCount - FilledUnitsToRemove);

        //@첫 번째 인덱스부터 차례대로 비활성화
        for (int32 i = 0; i < DifferenceCount && i < DotGaugeUnits.Num(); ++i)
        {
            if (UDotGaugeUnit* UnitToDeactivate = DotGaugeUnits[i])
            {
                UnitToDeactivate->DeactivateDotGaugeUnit();
                UE_LOGFMT(LogHorizontalDotGaguge, Log, "게이지 유닛 비활성화: {0}번째, Filled상태였음: {1}",
                    i, UnitToDeactivate->IsFilled());
            }
        }
    }

    //@MaxCount 업데이트
    MaxUnitCount = NewMaxCount;

    UE_LOGFMT(LogHorizontalDotGaguge, Log, "게이지 최대 개수 업데이트 완료. Active Unit: {0}, Filled Unit: {1}",
        MaxUnitCount, FilledUnitCount);
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion