#include "ArmorGameplayAbility.h"
#include "Logging/StructuredLog.h"

#include "01_Character/CharacterBase.h"
#include "10_AI/BaseAIController.h"

#include "00_GameInstance/AOWGameInstance.h"
#include "14_Subsystem/AIManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogArmorGA)

//@Default Setting
#pragma region Default Setting
UArmorGameplayAbility::UArmorGameplayAbility(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    //@기본 회피 확률 설정
    BaseEvasionChance = 0.2f;

    UE_LOGFMT(LogArmorGA, Log, "ArmorGameplayAbility 생성자 초기화 완료");
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
float UArmorGameplayAbility::CalculateEvasionProbability(float AgilityValue)
{
    //@기본 계산: (기민함/100) × 기본회피확률
    float EvasionChance = (AgilityValue / 100.0f) * BaseEvasionChance;

    UE_LOGFMT(LogArmorGA, Log, "기본 회피 확률 계산 - 기민함: {0}, 확률: {1}%",
        AgilityValue, EvasionChance * 100.0f);

    //return FMath::Clamp(EvasionChance, 0.0f, 1.0f);
    return 1.f;
}

void UArmorGameplayAbility::SendRandomEvasionEvent()
{
    //@회피 이벤트 태그 목록
    TArray<FString> EvasionEventTags = {
        TEXT("EventTag.OnDashToEvade"),
        TEXT("EventTag.OnDodgeToEvade"),
        TEXT("EventTag.OnGuardParryToEvade")
    };

    //@랜덤 선택
    int32 RandomIndex = FMath::RandRange(0, EvasionEventTags.Num() - 1);
    FGameplayTag RandomEvasionTag = FGameplayTag::RequestGameplayTag(*EvasionEventTags[RandomIndex]);

    UE_LOGFMT(LogArmorGA, Log, "랜덤 회피 이벤트 선택 - 태그: {0}", RandomEvasionTag.ToString());

    //@선택된 이벤트 전송
    FGameplayEventData EmptyGameplayEvent;
    HandleEvasionGameplayEvent(RandomEvasionTag, EmptyGameplayEvent);
}

void UArmorGameplayAbility::HandleEvasionGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData)
{
    //@ASC 가져오기
    UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
    if (!ASC)
    {
        UE_LOGFMT(LogArmorGA, Error, "ASC를 찾을 수 없음 - 이벤트 전송 실패");
        return;
    }

    //@이벤트 데이터 복사 및 태그 설정
    FGameplayEventData EvasionEventData = EventData;
    EvasionEventData.EventTag = EventTag;

    //@이벤트 전송
    ASC->HandleGameplayEvent(EventTag, &EvasionEventData);

    UE_LOGFMT(LogArmorGA, Log, "회피 이벤트 전송 완료 - 태그: {0}", EventTag.ToString());
}
#pragma endregion

//@Utility
#pragma region Utility
float UArmorGameplayAbility::GetAgilityValueFromAI() const
{
    //@AI Manager 서브시스템 가져오기
    //@Ability Manager Subsystem
    const auto& GameInstance = Cast<UAOWGameInstance>(UGameplayStatics::GetGameInstance(this));
    if (!GameInstance)
    {
        UE_LOGFMT(LogBaseAIC, Warning, "Game Instance가 유효하지 않음");
        return 50.f;
    }

    auto* AIManager = GameInstance->GetSubsystem<UAIManagerSubsystem>();
    if (!AIManager)
    {
        UE_LOGFMT(LogArmorGA, Warning, "AI Manager 서브시스템을 찾을 수 없음 - 기본값 사용");
        return 50.0f;
    }

    //@AI Controller 가져오기
    ACharacterBase* Character = GetCharacterFromActorInfo();
    if (!Character)
    {
        UE_LOGFMT(LogArmorGA, Warning, "캐릭터를 찾을 수 없음 - 기본값 사용");
        return 50.0f;
    }

    ABaseAIController* AIController = Cast<ABaseAIController>(Character->GetController());
    if (!AIController)
    {
        UE_LOGFMT(LogArmorGA, Warning, "AI Controller를 찾을 수 없음 - 기본값 사용");
        return 50.0f;
    }

    //@AI 타입 가져오기
    EAIType AIType = AIController->GetAIType();
    if (AIType == EAIType::MAX)
    {
        UE_LOGFMT(LogArmorGA, Warning, "유효하지 않은 AI 타입 - 기본값 사용");
        return 50.0f;
    }

    //@AI DataSet 가져오기
    FAIDataSet AIDataSet;
    if (!AIManager->GetAIDataSet(AIType, AIDataSet))
    {
        UE_LOGFMT(LogArmorGA, Warning, "AI DataSet을 찾을 수 없음 - 기본값 사용");
        return 50.0f;
    }

    //@적응형 시스템이 활성화되어 있으면 간소화된 성향의 기민함 사용
    if (AIDataSet.AdaptiveBehaviorConfig.bEnableAdaptiveLearning)
    {
        float AgilityValue = AIDataSet.AdaptiveBehaviorConfig.SimplifiedPersonality.Agility;
        UE_LOGFMT(LogArmorGA, Log, "적응형 AI 기민함 수치: {0}", AgilityValue);
        return AgilityValue;
    }
    else
    {
        //@기존 성향 시스템의 기민함 사용
        float AgilityValue = AIDataSet.PersonalityTraits.Agility;
        UE_LOGFMT(LogArmorGA, Log, "기존 AI 기민함 수치: {0}", AgilityValue);
        return AgilityValue;
    }
}

bool UArmorGameplayAbility::ShouldEvade()
{
    //@기민함 수치 가져오기
    float AgilityValue = GetAgilityValueFromAI();

    //@기민함 기반 회피 확률 계산
    float EvasionChance = CalculateEvasionProbability(AgilityValue);

    //@랜덤 판정
    float RandomValue = FMath::FRand();
    bool bEvade = RandomValue <= EvasionChance;

    UE_LOGFMT(LogArmorGA, Log, "회피 확률 계산 - 기민함: {0}, 확률: {1}%, 랜덤값: {2}, 결과: {3}",
        AgilityValue, EvasionChance * 100.0f, RandomValue, bEvade ? TEXT("회피") : TEXT("피격"));

    //@회피 성공 시 자동으로 회피 이벤트 처리
    if (bEvade)
    {
        UE_LOGFMT(LogArmorGA, Log, "회피 성공 - 회피 이벤트 자동 처리 시작");
        SendRandomEvasionEvent();
    }

    return bEvade;
}

ACharacterBase* UArmorGameplayAbility::GetCharacterFromActorInfo() const
{
    return Cast<ACharacterBase>(GetAvatarActorFromActorInfo());
}
#pragma endregion