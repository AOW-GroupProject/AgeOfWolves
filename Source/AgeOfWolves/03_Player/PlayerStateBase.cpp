#include "PlayerStateBase.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "00_GameInstance/AOWGameInstance.h"

#include "01_Character/CharacterBase.h"
#include "04_Component/BaseCharacterMovementComponent.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "14_Subsystem/AbilityManagerSubsystem.h"

#include "15_SaveGame/AOWSaveGame.h"

DEFINE_LOG_CATEGORY(LogPlayerStateBase)

// @목적 : 로그 메크로입니다. 복사+붙여넣기 활용
// UE_LOGFMT(LogPlayerStateBase, Log, "");

//@Defualt Setting
#pragma region Default Setting
APlayerStateBase::APlayerStateBase()
{

    //@Character Tag
    CharacterTag = FGameplayTag::RequestGameplayTag("Character.AkaOni");

    //@Ability Manger Subsystem
    AbilityManagerSubsystemRef.Reset();

    //@ASC
    AbilitySystemComponent = CreateDefaultSubobject<UBaseAbilitySystemComponent>(TEXT("Ability System Component"));

}

void APlayerStateBase::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    //@내부 바인딩
    InternalBindingToASC();

    //@Ability Manager Subsystem
    const auto& GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GameInstance)
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "GameInstance가 유효하지 않음");
        return;
    }

    //@캐싱
    AbilityManagerSubsystemRef = GameInstance->GetSubsystem<UAbilityManagerSubsystem>();
    if (!AbilityManagerSubsystemRef.IsValid())
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "AbilityManagerSubsystem 캐싱 실패");
        return;
    }

}

void APlayerStateBase::BeginPlay()
{
    Super::BeginPlay();

}

void APlayerStateBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);

    //// ASC 찾아서 이벤트 구독 해제
    //if (auto ASC = Cast<UBaseAbilitySystemComponent>(GetAbilitySystemComponent()))
    //{
    //    ASC->CharacterStateEventOnGameplay.RemoveAll(this);
    //}
}

void APlayerStateBase::InternalBindingToASC()
{
    if (!AbilitySystemComponent)
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "InternalBindingToASC: ASC가 유효하지 않습니다");
        return;
    }

    //@내부 바인딩
    AbilitySystemComponent->CharacterStateEventOnGameplay.AddUFunction(this, "OnCharacterStateEventOnGameplay");

    UE_LOGFMT(LogPlayerStateBase, Log, "캐릭터 상태 관련 이벤트 콜백이 성공적으로 바인딩되었습니다");
}

void APlayerStateBase::InitializePlayerState()
{
    //@Controller
    const auto& Controller = Cast<AController>(GetOwner());
    if (!Controller)
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "Controller가 유효하지 않음");
        return;
    }

    //@Pawn
    const auto& Pawn = Controller->GetPawn();
    if (!Pawn)
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "Pawn이 유효하지 않음");
        return;
    }

    // AbilityManagerSubsystem으로부터 AbilitySet 가져오기
    UBaseAbilitySet* SetToGrant = AbilityManagerSubsystemRef->GetAbilitySet(CharacterTag);
    if (!IsValid(SetToGrant))
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "AbilitySet이 유효하지 않음 - Tag: {0}", CharacterTag.ToString());
        return;
    }

    //@ActorInfo 초기화
    AbilitySystemComponent->InitAbilityActorInfo(Pawn, Pawn);

    //@기본 AttributeSet 등록
    SetToGrant->GiveStartupAttributeSetToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
    UE_LOGFMT(LogPlayerStateBase, Log, "기본 AttributeSet 등록 완료");

    //@Attribute 변경 콜백 함수 등록
    for (auto& AS : AbilitySystemComponent->GetSpawnedAttributes())
    {
        if (IsValid(AS))
        {
            AttributeSet = AS;
            TArray<FGameplayAttribute> Attributes = AttributeSet->GetAllAttributes();
            for (const FGameplayAttribute& Attribute : Attributes)
            {
                AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attribute).AddUObject(this, &APlayerStateBase::OnAttributeValueChanged);
            }
            UE_LOGFMT(LogPlayerStateBase, Log, "Attribute 변경 콜백 등록 완료");
            break;
        }
    }

    //@ATMR
    auto* ATMR = SetToGrant->GetATMR();
    if (!ATMR)
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "ATMR이 유효하지 않음");
        return;
    }

    //@ATMR 초기화
    ATMR->InitializeCacheMaps();

    //@SetAbilityTagRelationshipMapping
    AbilitySystemComponent->SetAbilityTagRelationshipMapping(ATMR);

    UE_LOGFMT(LogPlayerStateBase, Log, "태그 관계 매핑 완료");
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void APlayerStateBase::LoadGameAbilitySystem()
{
    UE_LOGFMT(LogPlayerStateBase, Warning, "어빌리티 시스템의 Load 작업을 시작합니다 : {0}", __FUNCTION__);

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

void APlayerStateBase::LoadDefaultAbilitySystemFromAbilityManager()
{
    // @ASC와 AttributeSet 초기화 검증
    if (!AbilitySystemComponent || !AttributeSet.Get())
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "LoadGameAbilitySystem 실패: ASC 또는 AttributeSet이 초기화되지 않음");
        return;
    }

    if (!AbilityManagerSubsystemRef.IsValid())
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "AbilityManagerSubsystem이 유효하지 않음");
        return;
    }

    UBaseAbilitySet* SetToGrant = AbilityManagerSubsystemRef->GetAbilitySet(CharacterTag);
    if (!IsValid(SetToGrant))
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "AbilitySet이 유효하지 않음 - Tag: {0}", CharacterTag.ToString());
        return;
    }

    // 캐릭터의 기본 Gameplay Effect를 ASC에 최초 등록/적용합니다.
    SetToGrant->GiveStartupGameplayEffectToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

    // 캐릭터의 기본 Gameplay Ability를 ASC에 최초 등록/적용합니다.
    SetToGrant->GiveStartupGameplayAbilityToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

    // ASC에 Startup GA, GE, AttributeSet의 등록 완료 이벤트 호출
    OnAttributeSetInitialized.Broadcast();
}

void APlayerStateBase::LoadAbilitySystemFromSaveGame(UAOWSaveGame* SaveGame)
{

}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void APlayerStateBase::OnAttributeValueChanged(const FOnAttributeChangeData& Data)
{
    OnAnyAttributeValueChanged.Broadcast(Data.Attribute, Data.OldValue, Data.NewValue);
}

void APlayerStateBase::OnCharacterStateEventOnGameplay(const FGameplayTag& CharacterStateTag)
{
    //@Player State
    if (!IsValid(this) || IsUnreachable())
    {
        return;
    }

    //@PC
    if (!GetOwner())
    {
        return;
    }

    // World 확인
    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "월드를 찾을 수 없습니다.");
        return;
    }

    //@"State.~"
    if (!CharacterStateTag.GetTagName().ToString().StartsWith("State."))
        return;


    //@Game Instance
    UAOWGameInstance* GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(World));
    if (!GameInstance)
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "GameInstance를 가져올 수 없습니다.");
        return;
    }
    
    //@SaveGame
    if (!GameInstance->DoesSaveGameExist())
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "SaveGame이 존재하지 않습니다.");
        return;
    }

    UAOWSaveGame* SaveGame = Cast<UAOWSaveGame>(GameInstance->GetSaveGameInstance());
    if (!SaveGame)
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "세이브 게임 인스턴스를 찾을 수 없습니다.");
        return;
    }

    //@Pawn
    APawn* ControlledPawn = GetPawn();
    if (!ControlledPawn)
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "폰이 유효하지 않습니다.");
        return;
    }

    //@상태 이력 추가
    SaveGame->AddCharacterStateToHistory(
        CharacterStateTag,
        ControlledPawn,
        AttributeSet.IsValid() ? AttributeSet.Get() : nullptr
    );

    // 상태 이벤트 처리 로그
    UE_LOGFMT(LogPlayerStateBase, Log,
        "캐릭터 상태 이벤트 처리 | 태그: {0}",
        CharacterStateTag.GetTagName().ToString());
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
UAbilitySystemComponent* APlayerStateBase::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

TSoftObjectPtr<UBaseAttributeSet> APlayerStateBase::GetAttributeSet() const
{
    return AttributeSet;
}
#pragma endregion