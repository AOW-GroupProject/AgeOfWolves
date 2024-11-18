#include "PlayerStateBase.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "00_GameInstance/AOWGameInstance.h"

#include "01_Character/PawnData.h"
#include "01_Character/CharacterBase.h"
#include "04_Component/BaseCharacterMovementComponent.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "04_Component/PlayerAbilitySystemComponent.h"


DEFINE_LOG_CATEGORY(LogPlayerStateBase)

// @목적 : 로그 메크로입니다. 복사+붙여넣기 활용
// UE_LOGFMT(LogPlayerStateBase, Log, "");

//@Defualt Setting
#pragma region Default Setting
APlayerStateBase::APlayerStateBase()
{
    PawnData = nullptr;
    AbilitySystemComponent = CreateDefaultSubobject<UPlayerAbilitySystemComponent>(TEXT("Ability System Component"));

}

void APlayerStateBase::PostInitializeComponents()
{
    Super::PostInitializeComponents();

}

void APlayerStateBase::BeginPlay()
{
    Super::BeginPlay();

}

void APlayerStateBase::InitializePlayerState()
{
    //@Controlelr
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
    //@Pawn Data
    if (!PawnData->IsValidLowLevel())
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "PawnData가 유효하지 않음");
        return;
    }
    //@Base Ability Set
    UBaseAbilitySet* SetToGrant = PawnData->AbilitySet;
    if (!IsValid(SetToGrant))
    {
        UE_LOGFMT(LogPlayerStateBase, Warning, "AbilitySet이 유효하지 않음");
        return;
    }

    // ActorInfo 초기화
    AbilitySystemComponent->InitAbilityActorInfo(Pawn, Pawn);

    // @기본 AttributeSet 등록
    SetToGrant->GiveStartupAttributeSetToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);
    UE_LOGFMT(LogPlayerStateBase, Log, "기본 AttributeSet 등록 완료");

    // @Attribute 변경 콜백 함수 등록
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

    // @태그 관계 초기화
    if (PawnData->TagRelationship)
    {
        PawnData->TagRelationship->InitializeCacheMaps();
        AbilitySystemComponent->SetAbilityTagRelationshipMapping(PawnData->TagRelationship);
        UE_LOGFMT(LogPlayerStateBase, Log, "태그 관계 매핑 완료");
    }
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
            //UAbilityManagerSubsystem* AbilityManager = GameInstance->GetSubsystem<UAbilityManagerSubsystem>();
            LoadDefaultAbilitySystemFromAbilityManager(/*AbilityManager*/);
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

    if (const auto& Controller = Cast<AController>(GetOwner()))
    {
        if (const auto& Pawn = Controller->GetPawn())
        {
            if (PawnData->IsValidLowLevel())
            {
                UBaseAbilitySet* SetToGrant = PawnData->AbilitySet;
                if (IsValid(SetToGrant))
                {
                    // 캐릭터의 기본 Gameplay Effect를 ASC에 최초 등록/적용합니다.
                    SetToGrant->GiveStartupGameplayEffectToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

                    // 캐릭터의 기본 Gameplay Ability를 ASC에 최초 등록/적용합니다.
                    SetToGrant->GiveStartupGameplayAbilityToAbilitySystem(AbilitySystemComponent, SetGrantedHandles, this);

                    // ASC에 Startup GA, GE, AttributeSet의 등록 완료 이벤트 호출
                    OnAttributeSetInitialized.Broadcast();
                }
            }
        }
    }
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

UPawnData* APlayerStateBase::GetPawnData() const
{
    return PawnData.Get();
}

#pragma endregion