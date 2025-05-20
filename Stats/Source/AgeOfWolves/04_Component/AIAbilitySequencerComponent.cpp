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
    , CurrentActivatingUnitTag(FGameplayTag())
    , CurrentBlockIndex(-1)
    , CurrentUnitIndex(0)
    , bCombatReady(true)
    , bIsFirstRun(true)
    , bWaitingForExitBlock(false)
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

    //@외부 바인딩...
    OwnerController->RequestStartCombatPattern.BindUFunction(this, "OnRequestActivateAICombatLoop");
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
bool UAIAbilitySequencerComponent::StartExitBlock()
{
    //@Exit Block 설정
    SetExecutingExitBlock();
    CurrentUnitIndex = 0;

    //@Exit Block의 유닛들 순차 실행
    ExecuteExitBlockUnits();

    return true;
}

void UAIAbilitySequencerComponent::ExecuteExitBlockUnits()
{
    //@현재 Exit Block 유닛 가져오기
    const auto& ExitBlock = CachedCombatSequence.GetExitBlock();
    const auto& ExitBlockUnits = ExitBlock.GetAbilityBlockUnits();

    //@현재 유닛이 유효한지 확인
    if (CurrentUnitIndex >= ExitBlockUnits.Num())
    {
        //@모든 Exit Block 유닛 실행 완료, 완전 종료
        UE_LOGFMT(LogAICombatPattern, Log, "Exit Block의 모든 유닛 실행 완료, 전투 패턴 종료");

        //@컴포넌트 상태 초기화
        bIsFirstRun = true;
        CurrentBlockIndex = -1;
        CurrentUnitIndex = 0;
        bCombatReady = true;
        CurrentActivatingUnitTag = FGameplayTag();

        //@AI Controller에 완료 통지가 바인딩되지 않은 경우
        if (!NotifyCombatPatternExitComplete.IsBound())
        {
            UE_LOGFMT(LogAICombatPattern, Warning, "Exit Block 완료 통지 델리게이트가 바인딩되지 않음");
            return;
        }

        //@완료 통지 실행
        const bool bNotifySuccess = NotifyCombatPatternExitComplete.Execute();
        if (bNotifySuccess)
        {
            UE_LOGFMT(LogAICombatPattern, Log, "AI Controller에 Exit Block 완료 통지 성공");
        }
        else
        {
            UE_LOGFMT(LogAICombatPattern, Warning, "AI Controller에 Exit Block 완료 통지 실패");
        }
        return;
    }

    //@현재 유닛 가져오기
    const auto& CurrentUnit = ExitBlockUnits[CurrentUnitIndex];

    //@어빌리티 태그 유효성 검사
    if (!CurrentUnit.GetAbilityTag().IsValid())
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "Exit Block 유닛의 어빌리티 태그가 유효하지 않음");
        //@다음 유닛
        CurrentUnitIndex++;
        //@Exit Block 수행
        ExecuteExitBlockUnits();
        return;
    }

    //@현재 실행할 어빌리티 태그 저장
    CurrentActivatingUnitTag = CurrentUnit.GetAbilityTag();

    //@어빌리티 활성화 요청 실패 시
    if (!RequestActivateAbilityBlockUnit.Execute(CurrentActivatingUnitTag))
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "Exit Block 어빌리티 활성화 요청 실패: Unit[{0}] Tag[{1}]",
            CurrentUnitIndex, *CurrentActivatingUnitTag.ToString());

        //@실패 시 다음 유닛으로
        CurrentActivatingUnitTag = FGameplayTag();
        CurrentUnitIndex++;
        ExecuteExitBlockUnits();
        return;
    }

    //@요청 성공
    UE_LOGFMT(LogAICombatPattern, Log, "Exit Block 어빌리티 유닛 활성화 요청 성공: Unit[{0}] Tag[{1}]",
        CurrentUnitIndex, *CurrentActivatingUnitTag.ToString());
}
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

    //@Start Block
    if (IsExecutingStartBlock())
    {
        const auto& StartBlock = CachedCombatSequence.GetStartBlock();
        if (CurrentUnitIndex < StartBlock.GetAbilityBlockUnits().Num())
        {
            CurrentUnit = &StartBlock.GetAbilityBlockUnits()[CurrentUnitIndex];
            BlockType = TEXT("StartBlock");
        }
    }
    //@Ability Blocks
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
    //@Exit Block
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

    //@현재 실행할 어빌리티 태그 저장
    CurrentActivatingUnitTag = CurrentUnit->GetAbilityTag();

    //@어빌리티 활성화 요청
    if (!RequestActivateAbilityBlockUnit.Execute(CurrentActivatingUnitTag))
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "어빌리티 활성화 요청 실패: {0} Unit[{1}] Tag[{2}]",
            *BlockType, CurrentUnitIndex, *CurrentActivatingUnitTag.ToString());

        //@실패 시 태그 초기화
        CurrentActivatingUnitTag = FGameplayTag();
        SetCombatReady(true);

        return false;
    }

    UE_LOGFMT(LogAICombatPattern, Log, "어빌리티 유닛 활성화 요청 성공: {0} Unit[{1}] Tag[{2}]",
        *BlockType, CurrentUnitIndex, *CurrentActivatingUnitTag.ToString());

    return true;
}

bool UAIAbilitySequencerComponent::OnRequestEndCombatPattern()
{
    //@현재 진행 중인 Exit Block이 있는지 체크
    if (IsExecutingExitBlock())
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "이미 Exit Block을 실행 중입니다");
        return false;
    }

    //@현재 진행 중인 유닛이 있는 경우
    if (CurrentActivatingUnitTag.IsValid())
    {
        UE_LOGFMT(LogAICombatPattern, Log, "현재 실행 중인 블록 유닛 완료 후 Exit Block 시작 대기: {0}",
            *CurrentActivatingUnitTag.ToString());

        //@Exit Block 대기 상태로 전환
        bWaitingForExitBlock = true;

        //@완료될 때까지 대기가 필요함을 알림
        return true;
    }

    //@실행 중인 유닛이 없으면 바로 Exit Block 시작
    return StartExitBlock();
}

void UAIAbilitySequencerComponent::OnAbilityActivated(UGameplayAbility* Ability)
{
    if (!Ability)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "OnAbilityActivated: 유효하지 않은 어빌리티");
        return;
    }

    //@현재 실행 중인 어빌리티가 아니면 무시
    if (!ValidateAbilityTag(Ability))
    {
        return;
    }

    UE_LOGFMT(LogAICombatPattern, Log, "시퀀서 어빌리티 활성화: {0}", *Ability->GetName());
}

void UAIAbilitySequencerComponent::OnAbilityEnded(UGameplayAbility* Ability)
{
    if (!Ability)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "OnAbilityEnded: 유효하지 않은 어빌리티");
        return;
    }

    //@현재 실행 중인 어빌리티가 아니면 무시
    if (!ValidateAbilityTag(Ability))
    {
        return;
    }

    UE_LOGFMT(LogAICombatPattern, Log, "시퀀서 어빌리티 종료: {0}", *Ability->GetName());

    //@태그 초기화
    CurrentActivatingUnitTag = FGameplayTag();

    //@Exit Block 실행 중이면 다음 Exit 유닛 실행
    if (IsExecutingExitBlock())
    {
        CurrentUnitIndex++;
        ExecuteExitBlockUnits();
    }
    //@Exit Block 대기 중이었다면 시작
    else if (bWaitingForExitBlock)
    {
        UE_LOGFMT(LogAICombatPattern, Log, "대기 중이던 Exit Block 시작");
        bWaitingForExitBlock = false;
        StartExitBlock();
    }
    //@일반 실행 중이면 다음 유닛으로
    else
    {
        AdvanceToNextUnit();
        SetCombatReady(true);
    }
}

void UAIAbilitySequencerComponent::OnAbilityCancelled(UGameplayAbility* Ability)
{
    //@OnAbilityEnded와 동일한 처리
    OnAbilityEnded(Ability);
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
    }

    //@AbilityBlocks 실행 중이었다면
    if (IsExecutingAbilityBlocks())
    {
        //@마지막 블록이었다면 처음으로 돌아감
        if (CurrentBlockIndex >= CachedCombatSequence.GetAbilityBlocks().Num() - 1)
        {
            CurrentBlockIndex = 0;
            UE_LOGFMT(LogAICombatPattern, Log, "AbilityBlocks 순환, 처음으로 돌아감");
            return;
        }
        //@다음 블록으로
        else
        {
            CurrentBlockIndex++;
            UE_LOGFMT(LogAICombatPattern, Log, "다음 Ability Block으로 이동 - Block[{0}]", CurrentBlockIndex);
            return;
        }
    }
    //@Exit Block 실행 중이었다면 완전히 종료
    else if (IsExecutingExitBlock())
    {
        //@전투 패턴 종료 처리
        bIsFirstRun = true;
        CurrentBlockIndex = -1;
        CurrentUnitIndex = 0;
        bCombatReady = true;
        CurrentActivatingUnitTag = FGameplayTag();

        UE_LOGFMT(LogAICombatPattern, Log, "Exit Block 완료, 전투 패턴 완전 종료");
        return;
    }
}

bool UAIAbilitySequencerComponent::ValidateAbilityTag(const UGameplayAbility* Ability) const
{
    //@어빌리티나 태그가 유효하지 않으면 실패
    if (!Ability || !CurrentActivatingUnitTag.IsValid())
    {
        return false;
    }

    //@어빌리티가 가진 태그들 중에 현재 활성화된 태그가 있는지 확인
    const FGameplayTagContainer& AbilityTags = Ability->AbilityTags;
    return AbilityTags.HasTag(CurrentActivatingUnitTag);
}
#pragma endregion