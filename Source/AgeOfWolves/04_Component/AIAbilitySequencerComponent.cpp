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

    //@외부 바인딩
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

    //@어빌리티 블록 캐싱
    InitializeAbilityBlocks(AIDataSet.AbilityBlocks);

    UE_LOGFMT(LogAICombatPattern, Log, "전투 패턴 컴포넌트 초기화 완료");
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UAIAbilitySequencerComponent::InitializeAbilityBlocks(const TArray<FAIAbilityBlock>& InBlocks)
{
    CachedAbilityBlocks = InBlocks;
    CurrentBlockIndex = 0;
    CurrentUnitIndex = 0;
    bCombatReady = true;

    UE_LOGFMT(LogAICombatPattern, Log, "어빌리티 블록 초기화 완료 - 블록 수: {0}", CachedAbilityBlocks.Num());
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
bool UAIAbilitySequencerComponent::OnRequestActivateAICombatLoop()
{
    if (CachedAbilityBlocks.Num() == 0)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "어빌리티 블록이 없습니다");
        return false;
    }

    if (!CachedAbilityBlocks.IsValidIndex(CurrentBlockIndex))
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "유효하지 않은 블록 인덱스: {0}", CurrentBlockIndex);
        return false;
    }

    const auto& CurrentBlock = CachedAbilityBlocks[CurrentBlockIndex];
    const auto& Units = CurrentBlock.GetAbilityBlockUnits();

    if (!Units.IsValidIndex(CurrentUnitIndex))
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "유효하지 않은 유닛 인덱스: {0}", CurrentUnitIndex);
        return false;
    }

    const auto& CurrentUnit = Units[CurrentUnitIndex];

    //@어빌리티 태그 유효성 검사
    if (!CurrentUnit.GetAbilityTag().IsValid())
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "유효하지 않은 어빌리티 태그");
        return false;
    }

    //@어빌리티 활성화 요청
    if (!RequestActivateAbilityBlockUnit.Execute(CurrentUnit.GetAbilityTag()))
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "어빌리티 활성화 요청 실패: Block[{0}] Unit[{1}] Tag[{2}]",
            CurrentBlockIndex, CurrentUnitIndex, *CurrentUnit.GetAbilityTag().ToString());
        return false;
    }

    SetCombatReady(false);

    UE_LOGFMT(LogAICombatPattern, Log, "어빌리티 유닛 활성화 요청 성공: Block[{0}] Unit[{1}] Tag[{2}]",
        CurrentBlockIndex, CurrentUnitIndex, *CurrentUnit.GetAbilityTag().ToString());

    return true;

}

void UAIAbilitySequencerComponent::OnAbilityActivated(UGameplayAbility* Ability)
{
    if (!Ability)
    {
        UE_LOGFMT(LogAICombatPattern, Warning, "OnAbilityActivated: 유효하지 않은 어빌리티");
        return;
    }

    UE_LOGFMT(LogAICombatPattern, Log, "어빌리티 활성화: {0}", *Ability->GetName());
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

    const auto& CurrentBlock = CachedAbilityBlocks[CurrentBlockIndex];
    if (CurrentUnitIndex >= CurrentBlock.GetAbilityBlockUnits().Num())
    {
        AdvanceToNextBlock();
    }
}

void UAIAbilitySequencerComponent::AdvanceToNextBlock()
{
    CurrentBlockIndex++;
    CurrentUnitIndex = 0;

    if (CurrentBlockIndex >= CachedAbilityBlocks.Num())
    {
        CurrentBlockIndex = 0;
    }
}
#pragma endregion