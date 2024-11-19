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

    Count = FMath::Clamp(Count, 0, DotGaugeUnits.Num());

    //@오른쪽 끝에서 부터 순회 시작
    for (int32 i = DotGaugeUnits.Num()-1; i >= 0; --i)
    {
        if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
        {
            if (Unit->IsActive())
            {
                if (Count <= 0) break;

                Unit->UpdateDotGaugeUnit(false);
                UE_LOGFMT(LogHorizontalDotGaguge, Log, "게이지 유닛 상태 업데이트: {0}번째", i);
                Count--;
            }
        }
    }
}

void UHorizontalDotGauge::UpdateMaxCount(int32 NewMaxCount)
{
    //@New Max Count, Horizontal Dot Gauge Box
    if (NewMaxCount <= 0 || !HorizontalDotGaugeBox)
    {
        UE_LOGFMT(LogHorizontalDotGaguge, Warning, "유효하지 않은 NewMaxCount 값이거나 HorizontalDotGaugeBox가 유효하지 않습니다.");
        return;
    }

    //@현재 활성화된 게이지와 새로운 최대값의 차이 계산
    int32 DifferenceCount = NewMaxCount - CurrentGaugeUnitCount;

    //@MaxCount
    GaugeSettings.MaxCount = NewMaxCount;

    //@Max Count 증가
    if (DifferenceCount > 0)
    {
        for (int32 i = DotGaugeUnits.Num() - 1; i >= 0; --i)
        {
            if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
            {
                if (!Unit->IsActive())
                {
                    //@Activate
                    for (int32 j = 0; j < DifferenceCount && (i - j) >= 0; ++j)
                    {
                        if (UDotGaugeUnit* UnitToActivate = DotGaugeUnits[i - j])
                        {
                            UnitToActivate->ActivateDotGaugeUnit();
                            UE_LOGFMT(LogHorizontalDotGaguge, Log, "게이지 유닛 활성화: {0}", FString::FromInt(i - j));
                        }
                    }
                    break;
                }
            }
        }
    }
    //@Max Count 감소
    else if (DifferenceCount < 0)
    {
        for (int32 i = 0; i < DotGaugeUnits.Num(); ++i)
        {
            if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
            {
                if (Unit->IsActive())
                {
                    for (int32 j = 0; j < FMath::Abs(DifferenceCount) && (i + j) < DotGaugeUnits.Num(); ++j)
                    {
                        //@Deactivate
                        if (UDotGaugeUnit* UnitToDeactivate = DotGaugeUnits[i + j])
                        {
                            UnitToDeactivate->DeactivateDotGaugeUnit();
                            UE_LOGFMT(LogHorizontalDotGaguge, Log, "게이지 유닛 비활성화: {0}", FString::FromInt(i + j));
                        }
                    }
                    break;
                }
            }
        }
    }

    // 현재 게이지 유닛 카운트 업데이트
    CurrentGaugeUnitCount = NewMaxCount;

    UE_LOGFMT(LogHorizontalDotGaguge, Log, "게이지 최대 개수 업데이트 완료. 현재 활성화된 유닛: {0}", FString::FromInt(CurrentGaugeUnitCount));
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion