#include "HorizontalDotGauge.h"
#include "Logging/StructuredLog.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "08_UI/DotGaugeUnit.h"

DEFINE_LOG_CATEGORY(LogHorizontalDotGauge)

//@Default Setting
#pragma region Default Setting
UHorizontalDotGauge::UHorizontalDotGauge(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    HorizontalDotGaugeBox = nullptr;
    DotGaugeUnits.Empty();

    //@Two Pointers 초기화는 CreateDotGaugeUnits에서
    ActivateUnitBound = 0;
    FillUnitBound = 0;
}

void UHorizontalDotGauge::NativeOnInitialized()
{
    Super::NativeOnInitialized();
}

void UHorizontalDotGauge::InitializeHorizontalDotGauge()
{
    CreateDotGaugeUnits();
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

    //@풀 크기만큼 미리 생성
    for (int32 i = 0; i < GaugeSettings.MaxCount; ++i)
    {
        UDotGaugeUnit* NewUnit = CreateWidget<UDotGaugeUnit>(this, DotGaugeUnitClass);
        if (!NewUnit)
        {
            UE_LOGFMT(LogHorizontalDotGauge, Error, "DotGaugeUnit 생성 실패: {0}번째", i);
            continue;
        }

        UHorizontalBoxSlot* GaugeUnit = HorizontalDotGaugeBox->AddChildToHorizontalBox(NewUnit);
        if (GaugeUnit)
        {
            GaugeUnit->SetPadding(GaugeSettings.ItemPadding);
            GaugeUnit->SetHorizontalAlignment(HAlign_Right);
            GaugeUnit->SetVerticalAlignment(VAlign_Fill);
        }

        DotGaugeUnits.Add(NewUnit);
    }

    //@Two Pointers 초기화
    ActivateUnitBound = GetInitialActivateBound();
    FillUnitBound = GetInitialFillBound();

    UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 초기화 완료. 총 {0}개 유닛 생성 (ActivateBound={1}, FillBound={2})",
        DotGaugeUnits.Num(), ActivateUnitBound, FillUnitBound);
}

void UHorizontalDotGauge::UpdateGauge(int32 FilledCount, int32 MaxCount)
{
    SetMaxCount(MaxCount);
    SetFilledCount(FilledCount);
    UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 업데이트 완료. Filled: {0}/{1}", GetFilledCount(), GetMaxCount());
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

    int32 CurrentMaxCount = GetMaxCount();
    if (MaxCount == CurrentMaxCount)
    {
        UE_LOGFMT(LogHorizontalDotGauge, Log, "MaxCount가 이미 {0}입니다. 변경 없음.", CurrentMaxCount);
        return;
    }

    //@증가 또는 감소 처리
    int32 Difference = MaxCount - CurrentMaxCount;
    if (Difference > 0)
    {
        ActivateUnits(Difference);
    }
    else if (Difference < 0)
    {
        DeactivateUnits(FMath::Abs(Difference));
    }

    UE_LOGFMT(LogHorizontalDotGauge, Log, "MaxCount 설정 완료. MaxCount: {0}, FilledCount: {1}",
        GetMaxCount(), GetFilledCount());
}

void UHorizontalDotGauge::SetFilledCount(int32 FilledCount)
{
    if (!HorizontalDotGaugeBox)
    {
        UE_LOGFMT(LogHorizontalDotGauge, Warning, "HorizontalDotGaugeBox가 유효하지 않습니다.");
        return;
    }

    //@범위 체크
    FilledCount = FMath::Clamp(FilledCount, 0, GetMaxCount());

    int32 CurrentFilledCount = GetFilledCount();
    int32 Difference = FilledCount - CurrentFilledCount;

    if (Difference > 0)
    {
        FillUnits(Difference);
    }
    else if (Difference < 0)
    {
        UnfillUnits(FMath::Abs(Difference));
    }

    UE_LOGFMT(LogHorizontalDotGauge, Log, "FilledCount 설정 완료. FilledCount: {0}/{1}",
        GetFilledCount(), GetMaxCount());
}

void UHorizontalDotGauge::ActivateUnits(int32 Count)
{
    if (Count <= 0) return;

    //@새 Bound 계산 (방향 추상화)
    int32 OldBound = ActivateUnitBound;
    int32 Delta = GetActivateBoundDelta(Count);
    int32 NewBound = ClampActivateBound(OldBound + Delta);

    //@순회 구간 계산 (방향 무관)
    int32 Start, End;
    GetIterationRange(OldBound, NewBound, Start, End);

    //@통일된 순회
    for (int32 i = Start; i < End; ++i)
    {
        if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
        {
            Unit->ActivateDotGaugeUnit();
            UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 활성화: Index={0}", i);
        }
    }

    //@Bound 업데이트
    ActivateUnitBound = NewBound;
}

void UHorizontalDotGauge::DeactivateUnits(int32 Count)
{
    if (Count <= 0) return;

    //@새 Bound 계산
    int32 OldBound = ActivateUnitBound;
    int32 Delta = -GetActivateBoundDelta(Count);  // 반대 방향
    int32 NewBound = ClampActivateBound(OldBound + Delta);

    //@순회 구간 계산
    int32 Start, End;
    GetIterationRange(OldBound, NewBound, Start, End);

    //@통일된 순회
    for (int32 i = Start; i < End; ++i)
    {
        if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
        {
            Unit->DeactivateDotGaugeUnit();
            UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 비활성화: Index={0}", i);
        }
    }

    //@Bound 업데이트
    ActivateUnitBound = NewBound;

    //@Fill Bound 조정 (비활성화된 구간에 채워진 유닛 방지)
    FillUnitBound = ClampFillBound(FillUnitBound);
}

void UHorizontalDotGauge::FillUnits(int32 Count)
{
    if (Count <= 0) return;

    //@새 Bound 계산
    int32 OldBound = FillUnitBound;
    int32 Delta = GetFillBoundDelta(Count);
    int32 NewBound = ClampFillBound(OldBound + Delta);

    //@순회 구간 계산
    int32 Start, End;
    GetIterationRange(OldBound, NewBound, Start, End);

    //@통일된 순회
    for (int32 i = Start; i < End; ++i)
    {
        if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
        {
            Unit->UpdateDotGaugeUnit(true);
            UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 Fill: Index={0}", i);
        }
    }

    //@Bound 업데이트
    FillUnitBound = NewBound;
}

void UHorizontalDotGauge::UnfillUnits(int32 Count)
{
    if (Count <= 0) return;

    //@새 Bound 계산
    int32 OldBound = FillUnitBound;
    int32 Delta = -GetFillBoundDelta(Count);  // 반대 방향
    int32 NewBound = ClampFillBound(OldBound + Delta);

    //@순회 구간 계산
    int32 Start, End;
    GetIterationRange(OldBound, NewBound, Start, End);

    //@통일된 순회
    for (int32 i = Start; i < End; ++i)
    {
        if (UDotGaugeUnit* Unit = DotGaugeUnits[i])
        {
            Unit->UpdateDotGaugeUnit(false);
            UE_LOGFMT(LogHorizontalDotGauge, Log, "게이지 유닛 Unfill: Index={0}", i);
        }
    }

    //@Bound 업데이트
    FillUnitBound = NewBound;
}
#pragma endregion

//@방향 추상화 헬퍼 함수들
#pragma region Helper Functions
int32 UHorizontalDotGauge::GetInitialActivateBound() const
{
    // RightToLeft: 오른쪽 끝의 다음 (Num)
    // LeftToRight: 왼쪽 끝 (0)
    return GaugeSettings.bRightToLeft ? DotGaugeUnits.Num() : 0;
}

int32 UHorizontalDotGauge::GetInitialFillBound() const
{
    // 초기에는 Activate와 동일
    return GetInitialActivateBound();
}

int32 UHorizontalDotGauge::GetActivateBoundDelta(int32 Count) const
{
    // RightToLeft: 왼쪽으로 이동 (감소)
    // LeftToRight: 오른쪽으로 이동 (증가)
    return GaugeSettings.bRightToLeft ? -Count : Count;
}

int32 UHorizontalDotGauge::GetFillBoundDelta(int32 Count) const
{
    // Activate와 동일한 방향
    return GetActivateBoundDelta(Count);
}

int32 UHorizontalDotGauge::ClampActivateBound(int32 Bound) const
{
    // [0, Num] 범위로 제한
    return FMath::Clamp(Bound, 0, DotGaugeUnits.Num());
}

int32 UHorizontalDotGauge::ClampFillBound(int32 Bound) const
{
    // Fill은 Activate 구간 내에만 존재
    if (GaugeSettings.bRightToLeft)
    {
        // RightToLeft: FillBound >= ActivateBound
        int32 MinBound = ActivateUnitBound;
        int32 MaxBound = DotGaugeUnits.Num();
        return FMath::Clamp(Bound, MinBound, MaxBound);
    }
    else
    {
        // LeftToRight: FillBound <= ActivateBound
        int32 MinBound = 0;
        int32 MaxBound = ActivateUnitBound;
        return FMath::Clamp(Bound, MinBound, MaxBound);
    }
}

void UHorizontalDotGauge::GetIterationRange(int32 OldBound, int32 NewBound, int32& OutStart, int32& OutEnd) const
{
    // 항상 작은 값 → 큰 값 순회
    OutStart = FMath::Min(OldBound, NewBound);
    OutEnd = FMath::Max(OldBound, NewBound);
}
#pragma endregion