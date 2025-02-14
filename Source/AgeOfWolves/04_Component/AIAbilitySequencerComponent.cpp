#include "AIAbilitySequencerComponent.h"
#include "Logging/StructuredLog.h"

#include "10_AI/BaseAIController.h"
#include "10_AI/AIDataSetInfos.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "00_GameInstance/AOWGameInstance.h"
#include "14_Subsystem/AIManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogAICombatPattern);

//@Default Setting
#pragma region Default Setting
UAIAbilitySequencerComponent::UAIAbilitySequencerComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CurrentBlockIndex(0)
    , CurrentUnitIndex(0)
    , bCombatReady(true)
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UAIAbilitySequencerComponent::OnRegister()
{
    Super::OnRegister();
}

void UAIAbilitySequencerComponent::OnUnregister()
{
    Super::OnUnregister();
}

void UAIAbilitySequencerComponent::InitializeComponent()
{
    Super::InitializeComponent();
}

void UAIAbilitySequencerComponent::DestroyComponent(bool bPromoteChildren)
{
    Super::DestroyComponent(bPromoteChildren);
}

void UAIAbilitySequencerComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAIAbilitySequencerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
}

void UAIAbilitySequencerComponent::ExternalBindToASC(UBaseAbilitySystemComponent* ASC)
{
    //@ASC
    if (!ASC)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "ASC 바인딩 실패: ASC가 유효하지 않음");
        return;
    }

    //@어빌리티 활성화/종료/취소 이벤트
    ASC->AbilityActivated.AddUObject(this, &UAIAbilitySequencerComponent::OnAbilityActivated);
    ASC->AbilityEnded.AddUObject(this, &UAIAbilitySequencerComponent::OnAbilityEnded);
    ASC->AbilityCancelled.AddUObject(this, &UAIAbilitySequencerComponent::OnAbilityCancelled);

    UE_LOGFMT(LogAICombatPattern, Log, "어빌리티 시스템 이벤트 바인딩 완료");

}

void UAIAbilitySequencerComponent::ExternalBindToAIController()
{
    //@소유 컨트롤러 캐싱
    ABaseAIController* OwnerController = Cast<ABaseAIController>(GetOwner());
    if (!OwnerController)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "초기화 실패: 소유 컨트롤러가 BaseAIController가 아님");
        return;
    }

    OwnerController->RequestEndCombatPattern.BindUFunction(this, "OnRequestEndCombatPattern");
}

void UAIAbilitySequencerComponent::InitializeCombatPatternComponent()
{
    //@소유 컨트롤러 캐싱
    ABaseAIController* OwnerController = Cast<ABaseAIController>(GetOwner());
    if (!OwnerController)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "초기화 실패: 소유 컨트롤러가 BaseAIController가 아님");
        return;
    }

    //@ASC 캐싱 및 바인딩
    auto ASC = Cast<UBaseAbilitySystemComponent>(OwnerController->GetAbilitySystemComponent());
    if (!ASC)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "초기화 실패: ASC를 찾을 수 없음");
        return;
    }

    //@외부 바인딩...
    ExternalBindToAIController();
    ExternalBindToASC(ASC);

    //@Game Instance
    auto GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GameInstance)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "초기화 실패: Game Instance가 유효하지 않음");
        return;
    }

    //@AI Manager Subsystem
    auto AIManager = GameInstance->GetSubsystem<UAIManagerSubsystem>();
    if (!AIManager)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "초기화 실패: AI Manager Subsystem을 찾을 수 없음");
        return;
    }

    //@AI 데이터셋 가져오기
    FAIDataSet AIDataSet;
    if (!AIManager->GetAIDataSet(OwnerController->GetAIType(), AIDataSet))
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "AI 데이터 초기화 실패: AI 타입({0})에 대한 데이터셋이 존재하지 않음",
            StaticEnum<EAIType>()->GetNameStringByValue(static_cast<int64>(OwnerController->GetAIType())));
        return;
    }

    //@전투 시퀀스 캐싱
    CachedCombatSequence = AIDataSet.CombatSequence;

    //@초기화 작업
    bIsFirstRun = true;
    CurrentBlockIndex = -1;
    CurrentUnitIndex = 0;
    bCombatReady = true;

    UE_LOGFMT(LogAICombatPattern, Log, "전투 시퀀스 초기화 완료");
    UE_LOGFMT(LogAICombatPattern, Log, "시작 블록 Identifier: {0}",
        *CachedCombatSequence.GetStartBlock().GetBlockIdentifier().ToString());

    UE_LOGFMT(LogAICombatPattern, Log, "전투 패턴 컴포넌트 초기화 완료");
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
#pragma endregion

//@Callbacks
#pragma region Callbacks
bool UAIAbilitySequencerComponent::OnRequestActivateAICombatLoop()
{
    //@전투 준비 상태 체크
    if (!bCombatReady)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "전투 준비가 되지 않은 상태");
        return false;
    }

    //@현재 블록의 유닛 정보 가져오기
    const FAIAbilityBlockUnit* CurrentUnit = nullptr;
    FString BlockType;

    if (IsExecutingStartBlock())
    {
        const auto& StartBlock = CachedCombatSequence.GetStartBlock();
        if (CurrentUnitIndex < StartBlock.GetAbilityBlockUnits().Num())
        {
            CurrentUnit = &StartBlock.GetAbilityBlockUnits()[CurrentUnitIndex];
            BlockType = TEXT("StartBlock");
        }
    }
    else if (IsExecutingAbilityBlocks())
    {
        const auto& AbilityBlocks = CachedCombatSequence.GetAbilityBlocks();
        if (CurrentBlockIndex < AbilityBlocks.Num() &&
            CurrentUnitIndex < AbilityBlocks[CurrentBlockIndex].GetAbilityBlockUnits().Num())
        {
            CurrentUnit = &AbilityBlocks[CurrentBlockIndex].GetAbilityBlockUnits()[CurrentUnitIndex];
            BlockType = FString::Printf(TEXT("AbilityBlock[%d]"), CurrentBlockIndex);
        }
    }
    else if (IsExecutingExitBlock())
    {
        const auto& ExitBlock = CachedCombatSequence.GetExitBlock();
        if (CurrentUnitIndex < ExitBlock.GetAbilityBlockUnits().Num())
        {
            CurrentUnit = &ExitBlock.GetAbilityBlockUnits()[CurrentUnitIndex];
            BlockType = TEXT("ExitBlock");
        }
    }

    //@유닛 유효성 검사
    if (!CurrentUnit)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "유효하지 않은 유닛");
        return false;
    }

    //@어빌리티 태그 유효성 검사
    if (!CurrentUnit->GetAbilityTag().IsValid())
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "유효하지 않은 어빌리티 태그");
        return false;
    }

    SetCombatReady(false);

    //@어빌리티 활성화 요청
    if (!RequestActivateAbilityBlockUnit.Execute(CurrentUnit->GetAbilityTag()))
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "어빌리티 활성화 요청 실패: {0} Unit[{1}] Tag[{2}]",
            *BlockType, CurrentUnitIndex, *CurrentUnit->GetAbilityTag().ToString());

        SetCombatReady(true);

        return false;
    }

    UE_LOGFMT(LogAICombatPattern, Log, "어빌리티 유닛 활성화 요청 성공: {0} Unit[{1}] Tag[{2}]",
        *BlockType, CurrentUnitIndex, *CurrentUnit->GetAbilityTag().ToString());

    return true;
}

void UAIAbilitySequencerComponent::OnRequestEndCombatPattern()
{
    //@초기화 작업
    bIsFirstRun = true;

    CurrentBlockIndex = -1;
    CurrentUnitIndex = 0;

    bCombatReady = true;

    UE_LOGFMT(LogAICombatPattern, Log, "전투 패턴 초기화 완료. 시작 블록 준비");
}
void UAIAbilitySequencerComponent::OnAbilityActivated(UGameplayAbility* Ability)
{
    if (!Ability)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "OnAbilityActivated: 유효하지 않은 어빌리티");
        return;
    }

    UE_LOGFMT(LogAICombatPattern, Log, "어빌리티 활성화: {0}", *Ability->GetName());

    //@전투 준비 상태 해제 및 다음 인덱스로 진행
    AdvanceToNextUnit();

}

void UAIAbilitySequencerComponent::OnAbilityEnded(UGameplayAbility* Ability)
{
    if (!Ability)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "OnAbilityEnded: 유효하지 않은 어빌리티");
        return;
    }

    UE_LOGFMT(LogAICombatPattern, Log, "어빌리티 종료: {0}", *Ability->GetName());

    AdvanceToNextUnit();

    SetCombatReady(true);
}

void UAIAbilitySequencerComponent::OnAbilityCancelled(UGameplayAbility* Ability)
{
    if (!Ability)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "OnAbilityCancelled: 유효하지 않은 어빌리티");
        return;
    }

    UE_LOGFMT(LogAICombatPattern, Log, "어빌리티 취소: {0}", *Ability->GetName());

    AdvanceToNextUnit();

    SetCombatReady(true);
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
void UAIAbilitySequencerComponent::AdvanceToNextUnit()
{
    CurrentUnitIndex++;

    //@현재 블록의 유닛 정보 가져오기
    const TArray<FAIAbilityBlockUnit>* CurrentUnits = nullptr;
    if (IsExecutingStartBlock())
    {
        CurrentUnits = &CachedCombatSequence.GetStartBlock().GetAbilityBlockUnits();
    }
    else if (IsExecutingAbilityBlocks())
    {
        CurrentUnits = &CachedCombatSequence.GetAbilityBlocks()[CurrentBlockIndex].GetAbilityBlockUnits();
    }
    else if (IsExecutingExitBlock())
    {
        CurrentUnits = &CachedCombatSequence.GetExitBlock().GetAbilityBlockUnits();
    }

    //@현재 블록의 모든 유닛을 실행했는지 체크
    if (CurrentUnits && CurrentUnitIndex >= CurrentUnits->Num())
    {
        UE_LOGFMT(LogAICombatPattern, Log, "현재 블록의 모든 유닛 실행 완료");
        AdvanceToNextBlock();
    }
}
void UAIAbilitySequencerComponent::AdvanceToNextBlock()
{
    CurrentUnitIndex = 0;

    //@최초 실행 시
    if (bIsFirstRun)
    {
        //@Start Block 실행 중이었다면 AbilityBlocks로
        if (IsExecutingStartBlock())
        {
            bIsFirstRun = false;
            CurrentBlockIndex = 0;  // AbilityBlocks의 첫 번째로
            UE_LOGFMT(LogAICombatPattern, Log, "Start Block 완료, AbilityBlocks 시작");
            return;
        }
        //@AbilityBlocks 실행 중이었다면 Exit Block으로
        else if (IsExecutingAbilityBlocks())
        {
            if (CurrentBlockIndex >= CachedCombatSequence.GetAbilityBlocks().Num() - 1)
            {
                SetExecutingExitBlock();
                UE_LOGFMT(LogAICombatPattern, Log, "AbilityBlocks 완료, Exit Block 시작");
                return;
            }
            else
            {
                CurrentBlockIndex++;
            }
        }
    }
    //@최초 실행이 아닐 경우
    else
    {
        //@AbilityBlocks 실행 중이었다면
        if (IsExecutingAbilityBlocks())
        {
            //@마지막 블록이었다면 Exit Block으로
            if (CurrentBlockIndex >= CachedCombatSequence.GetAbilityBlocks().Num() - 1)
            {
                SetExecutingExitBlock();
                UE_LOGFMT(LogAICombatPattern, Log, "AbilityBlocks 완료, Exit Block 시작");
                return;
            }
            //@다음 블록으로
            else
            {
                CurrentBlockIndex++;
            }
        }
        //@Exit Block 실행 중이었다면 다시 AbilityBlocks의 처음으로
        else if (IsExecutingExitBlock())
        {
            CurrentBlockIndex = 0;  // AbilityBlocks의 첫 번째로
            UE_LOGFMT(LogAICombatPattern, Log, "Exit Block 완료, AbilityBlocks 재시작");
            return;
        }
    }

    UE_LOGFMT(LogAICombatPattern, Log, "다음 블록으로 이동 - Block[{0}]",
        IsExecutingStartBlock() ? TEXT("StartBlock") :
        IsExecutingExitBlock() ? TEXT("ExitBlock") :
        *FString::Printf(TEXT("AbilityBlock[%d]"), CurrentBlockIndex));
}
#pragma endregion