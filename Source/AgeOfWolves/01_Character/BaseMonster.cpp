#include "01_Character/BaseMonster.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "02_AbilitySystem/BaseAbilitySet.h"
#include "04_Component/BaseAbilitySystemComponent.h"
#include "10_Monster/BaseMonsterAIController.h"
#include "10_Monster/MonsterDataSubsystem.h"

#include "00_GameInstance/AOWGameInstance.h"
#include "14_Subsystem/AbilityManagerSubsystem.h"

#include "AIController.h"

DEFINE_LOG_CATEGORY(LogBaseMonster)

//@기본 설정
#pragma region Default Setting
ABaseMonster::ABaseMonster()
{
    PrimaryActorTick.bCanEverTick = true;

    //@Components
    AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("어빌리티 시스템 컴포넌트"));

    //@Attribute Set
    AttributeSet.Reset();
    //@Ability Manger Subsystem
    AbilityManagerSubsystemRef.Reset();

}

void ABaseMonster::PostInitializeComponents()
{
    Super::PostInitializeComponents();

}

void ABaseMonster::BeginPlay()
{
    Super::BeginPlay();
}

void ABaseMonster::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABaseMonster::PossessedBy(AController* NewController)
{
    Super::PossessedBy(NewController);

    //@ASC 초기화
    InitializeMonster();

    //@TODO: 추후에 타이밍 재기
    //@로드
    LoadGameAbilitySystem();
}

void ABaseMonster::InitializeMonster()
{

    //@Ability Manager Subsystem
    const auto& GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GameInstance)
    {
        UE_LOGFMT(LogBaseMonster, Warning, "GameInstance가 유효하지 않음");
        return;
    }

    //@캐싱
    AbilityManagerSubsystemRef = GameInstance->GetSubsystem<UAbilityManagerSubsystem>();
    if (!AbilityManagerSubsystemRef.IsValid())
    {
        UE_LOGFMT(LogBaseMonster, Warning, "AbilityManagerSubsystem 캐싱 실패");
        return;
    }
    
    //@ASC
    if (!AbilitySystemComponent)
    {
        UE_LOGFMT(LogBaseMonster, Warning, "어빌리티 시스템 컴포넌트가 유효하지 않음");
        return;
    }
    
    //@AbilityManagerSubsystem
    if (!AbilityManagerSubsystemRef.IsValid())
    {
        UE_LOGFMT(LogBaseMonster, Warning, "AbilityManagerSubsystem이 유효하지 않음");
        return;
    }

    //@BaseAbilitySet
    UBaseAbilitySet* SetToGrant = AbilityManagerSubsystemRef->GetAbilitySet(CharacterTag);
    if (!IsValid(SetToGrant))
    {
        UE_LOGFMT(LogBaseMonster, Warning, "AbilitySet이 유효하지 않음 - Tag: {0}", CharacterTag.ToString());
        return;
    }

    //@ActorInfo 초기화
    AbilitySystemComponent->InitAbilityActorInfo(this, this);

    //@기본 AttributeSet 등록
    SetToGrant->GiveStartupAttributeSetToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
    UE_LOGFMT(LogBaseMonster, Log, "기본 AttributeSet 등록 완료");

    //@Attribute 변경 콜백 함수 등록
    for (auto& AS : AbilitySystemComponent->GetSpawnedAttributes())
    {
        if (IsValid(AS))
        {
            AttributeSet = AS;
            TArray<FGameplayAttribute> Attributes = AttributeSet->GetAllAttributes();
            for (const FGameplayAttribute& Attribute : Attributes)
            {
                AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &ABaseMonster::OnAttributeValueChanged);
            }
            UE_LOGFMT(LogBaseMonster, Log, "Attribute 변경 콜백 등록 완료");
            break;
        }
    }

    //@ATMR
    auto* ATMR = SetToGrant->GetATMR();
    if (!ATMR)
    {
        UE_LOGFMT(LogBaseMonster, Warning, "ATMR이 유효하지 않음");
        return;
    }

    //@ATMR 초기화
    ATMR->InitializeCacheMaps();

    //@SetAbilityTagRelationshipMapping
    AbilitySystemComponent->SetAbilityTagRelationshipMapping(ATMR);

    UE_LOGFMT(LogBaseMonster, Log, "태그 관계 매핑 완료");
}
#pragma endregion

//@속성/정보
#pragma region Property or Subwidgets or Infos...etc
void ABaseMonster::LoadGameAbilitySystem()
{
    UE_LOGFMT(LogBaseMonster, Warning, "몬스터 어빌리티 시스템의 Load 작업을 시작합니다 : {0}", __FUNCTION__);

    //@GameInstance
    if (const auto& GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(this)))
    {
        //@SaveFile
        if (GameInstance->DoesSaveGameExist())
        {
            auto* SaveGameInstance = GameInstance->GetSaveGameInstance();
            LoadAbilitySystemFromSaveGame(SaveGameInstance);
        }
        //@Ability Manager
        else
        {
            LoadDefaultAbilitySystemFromAbilityManager();
        }
    }
}

void ABaseMonster::LoadDefaultAbilitySystemFromAbilityManager()
{
    // @ASC와 AttributeSet 초기화 검증
    if (!AbilitySystemComponent || !AttributeSet.Get())
    {
        UE_LOGFMT(LogBaseMonster, Warning, "LoadGameAbilitySystem 실패: ASC 또는 AttributeSet이 초기화되지 않음");
        return;
    }

    if (!AbilityManagerSubsystemRef.IsValid())
    {
        UE_LOGFMT(LogBaseMonster, Warning, "AbilityManagerSubsystem이 유효하지 않음");
        return;
    }

    UBaseAbilitySet* SetToGrant = AbilityManagerSubsystemRef->GetAbilitySet(CharacterTag);
    if (!IsValid(SetToGrant))
    {
        UE_LOGFMT(LogBaseMonster, Warning, "AbilitySet이 유효하지 않음 - Tag: {0}", CharacterTag.ToString());
        return;
    }

    // 몬스터의 기본 Gameplay Effect를 ASC에 최초 등록/적용
    SetToGrant->GiveStartupGameplayEffectToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

    // 몬스터의 기본 Gameplay Ability를 ASC에 최초 등록/적용
    SetToGrant->GiveStartupGameplayAbilityToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

    // ASC에 Startup GA, GE, AttributeSet의 등록 완료 이벤트 호출
    // 델리게이트 추가 필요
    MonsterAttributeSetInitialized.Broadcast();

    UE_LOGFMT(LogBaseMonster, Log, "몬스터 어빌리티 시스템 로드 완료");
}

void ABaseMonster::LoadAbilitySystemFromSaveGame(UAOWSaveGame* SaveGame)
{
    // 세이브 게임으로부터 몬스터 어빌리티 시스템 로드
    // 추후 구현
}

void ABaseMonster::WhenEndState()
{
    switch (CurrentState)
    {
    case EMonsterState::Patrol:
        UE_LOG(LogBaseMonster, Log, TEXT("순찰 상태 종료"));
        break;
    case EMonsterState::Strafe:
        if (AAIController* AIController = Cast<AAIController>(GetController()))
        {
            UE_LOG(LogBaseMonster, Log, TEXT("배회 상태 종료: 포커스 해제"));
            AIController->ClearFocus(EAIFocusPriority::Gameplay);
        }
        break;
    case EMonsterState::Attacking:
        CurrentState = EMonsterState::DetectingPlayer;
        if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
        {
            UE_LOG(LogBaseMonster, Log, TEXT("공격 상태 종료: 몽타주 중지"));
            AnimInstance->Montage_Stop(0.5f);
        }
        break;
    case EMonsterState::DetectingPlayer:
        UE_LOG(LogBaseMonster, Log, TEXT("플레이어 감지 상태 종료"));
        break;
    case EMonsterState::Stunned:
        UE_LOG(LogBaseMonster, Log, TEXT("기절 상태 종료"));
        break;
    }
}
#pragma endregion

//@콜백
#pragma region Callbacks
#pragma endregion

//@유틸리티
#pragma region Utility
void ABaseMonster::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
    AnyMonsterAttributeValueChanged.Broadcast(Data.Attribute, Data.OldValue, Data.NewValue);
}
UAbilitySystemComponent* ABaseMonster::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent.Get();
}

EMonsterState ABaseMonster::GetCurrentState()
{
    return CurrentState;
}

void ABaseMonster::ChangeState(EMonsterState inValue)
{
    UE_LOG(LogBaseMonster, Log, TEXT("상태 변경: %s -> %s"),
        *UEnum::GetValueAsString(CurrentState), *UEnum::GetValueAsString(inValue));
    WhenEndState();
    CurrentState = inValue;
}
#pragma endregion