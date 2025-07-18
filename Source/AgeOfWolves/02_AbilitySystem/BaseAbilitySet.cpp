#include "BaseAbilitySet.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "02_AbilitySystem/02_GameplayAbility/BaseGameplayAbility.h"

#include "Engine/Engine.h"
#include "HAL/PlatformFilemanager.h"
#include "Misc/OutputDeviceRedirector.h"

DEFINE_LOG_CATEGORY(LogAbilitySet)

//@Defualt Setting
#pragma region Default Setting
UBaseAbilitySet::UBaseAbilitySet(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
}

#if WITH_EDITOR
void UBaseAbilitySet::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    
    FAbilitySetValidationResult ValidationResult = ValidateAbilitySet();

    if (!ValidationResult.bIsValid)
    {
        ShowValidationError(ValidationResult);

        if (!TryAutoFixValidationError(ValidationResult))
        {
            UE_LOGFMT(LogAbilitySet, Warning,
                "Ability Set 무결성 검사 실패 - 수동 수정 필요: {0}",
                *ValidationResult.ErrorMessage);
        }
    }
}

void UBaseAbilitySet::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    FAbilitySetValidationResult ValidationResult = ValidateAbilitySet();

    if (!ValidationResult.bIsValid)
    {
        ShowValidationError(ValidationResult);
        TryAutoFixValidationError(ValidationResult);
    }
}

FAbilitySetValidationResult UBaseAbilitySet::ValidateAbilitySet() const
{

    //@기본 검증
    FAbilitySetValidationResult Result = ValidateBasicSettings();
    if (!Result.bIsValid) return Result;

    //@Attribute Sets
    Result = ValidateAttributeSets();
    if (!Result.bIsValid) return Result;

    //@GEs
    Result = ValidateGameplayEffects();
    if (!Result.bIsValid) return Result;

    //@GAs
    Result = ValidateGameplayAbilities();
    if (!Result.bIsValid) return Result;

    //@Ability Tag
    Result = ValidateAbilityTagUniqueness();
    if (!Result.bIsValid) return Result;

    //@Input Tag
    Result = ValidateInputTagUniqueness();
    if (!Result.bIsValid) return Result;

    //@일관성
    Result = ValidateAbilityLogicalConsistency();
    if (!Result.bIsValid) return Result;

    return FAbilitySetValidationResult(); 
}

FAbilitySetValidationResult UBaseAbilitySet::ValidateBasicSettings() const
{
    //@Character Tag
    if (!IsValidGameplayTag(CharacterTag))
    {
        return FAbilitySetValidationResult(
            EAbilitySetValidationError::InvalidCharacterTag,
            FString::Printf(TEXT("캐릭터 태그가 유효하지 않습니다: %s"),
                *CharacterTag.ToString())
        );
    }

    //@ATMR
    if (TagRelationship && !IsValid(TagRelationship))
    {
        UE_LOGFMT(LogAbilitySet, Warning,
            "TagRelationship이 설정되어 있지만 유효하지 않습니다. 어빌리티 상호작용에 문제가 발생할 수 있습니다.");
    }

    return FAbilitySetValidationResult();
}

FAbilitySetValidationResult UBaseAbilitySet::ValidateAttributeSets() const
{

    for (int32 SetIndex = 0; SetIndex < AttributeSets.Num(); ++SetIndex)
    {
        const FBaseAbilitySet_AttributeSet& AttributeSetInfo = AttributeSets[SetIndex];

        if (!IsValid(AttributeSetInfo.AttributeSet))
        {
            return FAbilitySetValidationResult(
                EAbilitySetValidationError::NullAttributeSetClass,
                FString::Printf(TEXT("AttributeSet 클래스가 Null입니다 (인덱스: %d)"), SetIndex),
                -1, -1, SetIndex
            );
        }

        if (!AttributeSetInfo.AttributeSet->IsChildOf<UBaseAttributeSet>())
        {
            return FAbilitySetValidationResult(
                EAbilitySetValidationError::NullAttributeSetClass,
                FString::Printf(TEXT("잘못된 AttributeSet 클래스 타입입니다 (인덱스: %d, 클래스: %s)"),
                    SetIndex, *AttributeSetInfo.AttributeSet->GetName()),
                -1, -1, SetIndex
            );
        }
    }

    return FAbilitySetValidationResult();
}

FAbilitySetValidationResult UBaseAbilitySet::ValidateGameplayEffects() const
{
    for (int32 EffectIndex = 0; EffectIndex < GameplayEffects.Num(); ++EffectIndex)
    {
        const FBaseAbilitySet_GameplayEffect& EffectInfo = GameplayEffects[EffectIndex];

        if (!IsValid(EffectInfo.GameplayEffect))
        {
            return FAbilitySetValidationResult(
                EAbilitySetValidationError::NullGameplayEffectClass,
                FString::Printf(TEXT("GameplayEffect 클래스가 Null입니다 (인덱스: %d)"), EffectIndex),
                -1, EffectIndex, -1
            );
        }

        if (EffectInfo.EffectLevel <= 0.0f)
        {
            return FAbilitySetValidationResult(
                EAbilitySetValidationError::InvalidEffectLevel,
                FString::Printf(TEXT("GameplayEffect 레벨이 유효하지 않습니다 (인덱스: %d, 레벨: %.2f)"),
                    EffectIndex, EffectInfo.EffectLevel),
                -1, EffectIndex, -1
            );
        }

        if (EffectInfo.EffectLevel > 100.0f)
        {
            UE_LOGFMT(LogAbilitySet, Warning,
                "GameplayEffect 레벨이 매우 높습니다 (인덱스: {0}, 레벨: {1}). 게임 밸런스를 확인하세요.",
                EffectIndex, EffectInfo.EffectLevel);
        }
    }

    return FAbilitySetValidationResult();
}

FAbilitySetValidationResult UBaseAbilitySet::ValidateGameplayAbilities() const
{

    for (int32 AbilityIndex = 0; AbilityIndex < GameplayAbilities.Num(); ++AbilityIndex)
    {
        const FBaseAbilitySet_GameplayAbility& AbilityInfo = GameplayAbilities[AbilityIndex];

        if (!IsValid(AbilityInfo.Ability))
        {
            return FAbilitySetValidationResult(
                EAbilitySetValidationError::NullAbilityClass,
                FString::Printf(TEXT("GameplayAbility 클래스가 Null입니다 (인덱스: %d)"), AbilityIndex),
                AbilityIndex, -1, -1
            );
        }

        if (AbilityInfo.AbilityLevel <= 0)
        {
            return FAbilitySetValidationResult(
                EAbilitySetValidationError::InvalidAbilityLevel,
                FString::Printf(TEXT("GameplayAbility 레벨이 유효하지 않습니다 (인덱스: %d, 레벨: %d)"),
                    AbilityIndex, AbilityInfo.AbilityLevel),
                AbilityIndex, -1, -1
            );
        }

        if (!IsValidGameplayTag(AbilityInfo.AbilityTag))
        {
            return FAbilitySetValidationResult(
                EAbilitySetValidationError::InvalidAbilityTag,
                FString::Printf(TEXT("GameplayAbility 태그가 유효하지 않습니다 (인덱스: %d, 태그: %s)"),
                    AbilityIndex, *AbilityInfo.AbilityTag.ToString()),
                AbilityIndex, -1, -1
            );
        }

        if (AbilityInfo.AbilityLevel > 20)
        {
            UE_LOGFMT(LogAbilitySet, Warning,
                "GameplayAbility 레벨이 매우 높습니다 (인덱스: {0}, 레벨: {1}). 게임 밸런스를 확인하세요.",
                AbilityIndex, AbilityInfo.AbilityLevel);
        }
    }

    return FAbilitySetValidationResult(); 
}

FAbilitySetValidationResult UBaseAbilitySet::ValidateAbilityTagUniqueness() const
{

    TMap<FGameplayTag, int32> UsedAbilityTags; 

    for (int32 AbilityIndex = 0; AbilityIndex < GameplayAbilities.Num(); ++AbilityIndex)
    {
        const FBaseAbilitySet_GameplayAbility& AbilityInfo = GameplayAbilities[AbilityIndex];

        if (AbilityInfo.AbilityTag.IsValid())
        {
            if (int32* ExistingIndex = UsedAbilityTags.Find(AbilityInfo.AbilityTag))
            {
                return FAbilitySetValidationResult(
                    EAbilitySetValidationError::DuplicateAbilityTag,
                    FString::Printf(TEXT("중복된 AbilityTag 발견: %s (인덱스 %d와 %d)"),
                        *AbilityInfo.AbilityTag.ToString(), *ExistingIndex, AbilityIndex),
                    AbilityIndex, -1, -1
                );
            }

            UsedAbilityTags.Add(AbilityInfo.AbilityTag, AbilityIndex);
        }
    }

    return FAbilitySetValidationResult(); // 성공
}

FAbilitySetValidationResult UBaseAbilitySet::ValidateInputTagUniqueness() const
{

    TMap<FGameplayTag, int32> UsedInputTags;

    for (int32 AbilityIndex = 0; AbilityIndex < GameplayAbilities.Num(); ++AbilityIndex)
    {
        const FBaseAbilitySet_GameplayAbility& AbilityInfo = GameplayAbilities[AbilityIndex];

        // 입력 바인딩이 활성화된 어빌리티만 검사
        if (AbilityInfo.bActive && AbilityInfo.bInputBinded && AbilityInfo.InputTag.IsValid())
        {
            if (int32* ExistingIndex = UsedInputTags.Find(AbilityInfo.InputTag))
            {
                return FAbilitySetValidationResult(
                    EAbilitySetValidationError::DuplicateInputTag,
                    FString::Printf(TEXT("중복된 InputTag 발견: %s (인덱스 %d와 %d)"),
                        *AbilityInfo.InputTag.ToString(), *ExistingIndex, AbilityIndex),
                    AbilityIndex, -1, -1
                );
            }

            UsedInputTags.Add(AbilityInfo.InputTag, AbilityIndex);
        }
    }

    return FAbilitySetValidationResult(); 
}

FAbilitySetValidationResult UBaseAbilitySet::ValidateAbilityLogicalConsistency() const
{

    for (int32 AbilityIndex = 0; AbilityIndex < GameplayAbilities.Num(); ++AbilityIndex)
    {
        const FBaseAbilitySet_GameplayAbility& AbilityInfo = GameplayAbilities[AbilityIndex];

        // === 검증 규칙 1: bInputBinded가 true이면 bActive도 true여야 함 ===
        //@패시브 어빌리티는 입력을 받을 수 없습니다
        if (AbilityInfo.bInputBinded && !AbilityInfo.bActive)
        {
            return FAbilitySetValidationResult(
                EAbilitySetValidationError::InputBindingInconsistency,
                FString::Printf(TEXT("입력 바인딩이 활성화되었지만 어빌리티가 비활성 상태입니다 (인덱스: %d)"),
                    AbilityIndex),
                AbilityIndex, -1, -1
            );
        }

        //@입력 태그가 있는데 바인딩이 비활성화되어 있으면 모순
        if (AbilityInfo.InputTag.IsValid() && !AbilityInfo.bInputBinded)
        {
            return FAbilitySetValidationResult(
                EAbilitySetValidationError::InputBindingInconsistency,
                FString::Printf(TEXT("InputTag가 설정되어 있지만 입력 바인딩이 비활성화되어 있습니다 (인덱스: %d, 태그: %s)"),
                    AbilityIndex, *AbilityInfo.InputTag.ToString()),
                AbilityIndex, -1, -1
            );
        }

        //@입력 바인딩이 활성화되어 있는데 태그가 없으면 입력을 받을 수 없음
        if (AbilityInfo.bInputBinded && !AbilityInfo.InputTag.IsValid())
        {
            return FAbilitySetValidationResult(
                EAbilitySetValidationError::InputBindingInconsistency,
                FString::Printf(TEXT("입력 바인딩이 활성화되어 있지만 유효한 InputTag가 없습니다 (인덱스: %d)"),
                    AbilityIndex),
                AbilityIndex, -1, -1
            );
        }

        //@Input Tag
        if (AbilityInfo.InputTag.IsValid() && !IsValidInputTag(AbilityInfo.InputTag))
        {
            return FAbilitySetValidationResult(
                EAbilitySetValidationError::InvalidInputTag,
                FString::Printf(TEXT("유효하지 않은 InputTag입니다 (인덱스: %d, 태그: %s)"),
                    AbilityIndex, *AbilityInfo.InputTag.ToString()),
                AbilityIndex, -1, -1
            );
        }
    }

    return FAbilitySetValidationResult();
}

void UBaseAbilitySet::ShowValidationError(const FAbilitySetValidationResult& ValidationResult) const
{
    //@이는 로그 파일에 기록되어 나중에 분석할 수 있습니다
    UE_LOGFMT(LogAbilitySet, Error,
        "Ability Set 검증 실패 | 오류 타입: {0} | 메시지: {1} | 어빌리티 인덱스: {2} | 이펙트 인덱스: {3} | 어트리뷰트셋 인덱스: {4}",
        *UEnum::GetValueAsString(ValidationResult.ErrorType),
        *ValidationResult.ErrorMessage,
        ValidationResult.ProblemAbilityIndex,
        ValidationResult.ProblemEffectIndex,
        ValidationResult.ProblemAttributeSetIndex);

    ensureMsgf(false, TEXT("Ability Set 검증 실패: %s"), *ValidationResult.ErrorMessage);

    if (GEngine)
    {
        FString ScreenMessage = FString::Printf(
            TEXT("Ability Set 검증 실패: %s"),
            *ValidationResult.ErrorMessage
        );

        // 빨간색으로 5초간 화면에 표시
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, ScreenMessage);
    }
}

bool UBaseAbilitySet::TryAutoFixValidationError(const FAbilitySetValidationResult& ValidationResult)
{

    switch (ValidationResult.ErrorType)
    {
    case EAbilitySetValidationError::InputBindingInconsistency:
    {
        if (ValidationResult.ProblemAbilityIndex >= 0 &&
            ValidationResult.ProblemAbilityIndex < GameplayAbilities.Num())
        {
            FBaseAbilitySet_GameplayAbility& AbilityInfo =
                const_cast<FBaseAbilitySet_GameplayAbility&>(GameplayAbilities[ValidationResult.ProblemAbilityIndex]);

            // 입력 바인딩이 활성화되어 있는데 어빌리티가 비활성인 경우
            if (AbilityInfo.bInputBinded && !AbilityInfo.bActive)
            {
                AbilityInfo.bActive = true;
                UE_LOGFMT(LogAbilitySet, Log,
                    "자동 수정: 어빌리티 {0}을 Active로 변경했습니다 (입력 바인딩 일관성을 위해)",
                    ValidationResult.ProblemAbilityIndex);
                return true;
            }

            //@InputTag가 있는데 바인딩이 비활성인 경우
            if (AbilityInfo.InputTag.IsValid() && !AbilityInfo.bInputBinded)
            {
                AbilityInfo.bInputBinded = true;
                AbilityInfo.bActive = true;
                UE_LOGFMT(LogAbilitySet, Log,
                    "자동 수정: 어빌리티 {0}의 입력 바인딩을 활성화했습니다",
                    ValidationResult.ProblemAbilityIndex);
                return true;
            }
        }
        break;
    }

    case EAbilitySetValidationError::InvalidAbilityLevel:
    case EAbilitySetValidationError::InvalidEffectLevel:
    {
        if (ValidationResult.ErrorType == EAbilitySetValidationError::InvalidAbilityLevel &&
            ValidationResult.ProblemAbilityIndex >= 0 &&
            ValidationResult.ProblemAbilityIndex < GameplayAbilities.Num())
        {
            FBaseAbilitySet_GameplayAbility& AbilityInfo =
                const_cast<FBaseAbilitySet_GameplayAbility&>(GameplayAbilities[ValidationResult.ProblemAbilityIndex]);

            if (AbilityInfo.AbilityLevel <= 0)
            {
                int32 OldLevel = AbilityInfo.AbilityLevel;
                AbilityInfo.AbilityLevel = 1;
                UE_LOGFMT(LogAbilitySet, Log,
                    "자동 수정: 어빌리티 {0}의 레벨을 {1}에서 1로 변경했습니다",
                    ValidationResult.ProblemAbilityIndex, OldLevel);
                return true;
            }
        }
        else if (ValidationResult.ErrorType == EAbilitySetValidationError::InvalidEffectLevel &&
            ValidationResult.ProblemEffectIndex >= 0 &&
            ValidationResult.ProblemEffectIndex < GameplayEffects.Num())
        {
            FBaseAbilitySet_GameplayEffect& EffectInfo =
                const_cast<FBaseAbilitySet_GameplayEffect&>(GameplayEffects[ValidationResult.ProblemEffectIndex]);

            if (EffectInfo.EffectLevel <= 0.0f)
            {
                float OldLevel = EffectInfo.EffectLevel;
                EffectInfo.EffectLevel = 1.0f;
                UE_LOGFMT(LogAbilitySet, Log,
                    "자동 수정: GameplayEffect {0}의 레벨을 {1}에서 1.0으로 변경했습니다",
                    ValidationResult.ProblemEffectIndex, OldLevel);
                return true;
            }
        }
        break;
    }

    default:
    {
        UE_LOGFMT(LogAbilitySet, Log,
            "자동 수정 불가: {0} 오류는 수동 수정이 필요합니다",
            *UEnum::GetValueAsString(ValidationResult.ErrorType));

        return false;
    }
    }

    return false;
}

bool UBaseAbilitySet::IsValidGameplayTag(const FGameplayTag& Tag) const
{

    if (!Tag.IsValid())
    {
        return false;
    }

    FString TagString = Tag.ToString();
    if (TagString.IsEmpty() || TagString == TEXT("None"))
    {
        return false;
    }

    return true;
}

bool UBaseAbilitySet::IsValidInputTag(const FGameplayTag& Tag) const
{

    if (!IsValidGameplayTag(Tag))
    {
        return false;
    }

    FString TagString = Tag.ToString();

    // InputTag는 "InputTag." 접두사를 가져야 함 (프로젝트 규칙)
    // 이 부분은 프로젝트의 태그 명명 규칙에 따라 조정할 수 있습니다
    if (!TagString.StartsWith(TEXT("Input.")))
    {
        UE_LOGFMT(LogAbilitySet, Warning,
            "InputTag가 올바른 형식이 아닙니다. 'Input.'로 시작해야 합니다: {0}", *TagString);
        return false;
    }

    return true;
}

void UBaseAbilitySet::ValidateAbilitySetManually()
{
    FAbilitySetValidationResult ValidationResult = ValidateAbilitySet();

    if (ValidationResult.bIsValid)
    {
        UE_LOGFMT(LogAbilitySet, Log, "✅ Ability Set 무결성 검사 성공: 모든 설정이 유효합니다");

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ Ability Set 검증 성공! 모든 설정이 올바릅니다."));
        }
    }
    else
    {
        ShowValidationError(ValidationResult);
    }
}

void UBaseAbilitySet::ValidateAbilitiesOnly()
{
    FAbilitySetValidationResult Result;

    // 어빌리티 관련 검증들만 수행
    Result = ValidateGameplayAbilities();
    if (!Result.bIsValid) { ShowValidationError(Result); return; }

    Result = ValidateAbilityTagUniqueness();
    if (!Result.bIsValid) { ShowValidationError(Result); return; }

    Result = ValidateInputTagUniqueness();
    if (!Result.bIsValid) { ShowValidationError(Result); return; }

    Result = ValidateAbilityLogicalConsistency();
    if (!Result.bIsValid) { ShowValidationError(Result); return; }

    UE_LOGFMT(LogAbilitySet, Log, "✅ GameplayAbilities 검증 완료: 모든 어빌리티 설정이 유효합니다");
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
            TEXT("✅ 어빌리티 검증 성공!"));
    }
}

void UBaseAbilitySet::ValidateEffectsOnly()
{
    FAbilitySetValidationResult Result = ValidateGameplayEffects();

    if (Result.bIsValid)
    {
        UE_LOGFMT(LogAbilitySet, Log, "✅ GameplayEffects 검증 완료: 모든 이펙트 설정이 유효합니다");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ GameplayEffects 검증 성공!"));
        }
    }
    else
    {
        ShowValidationError(Result);
    }
}

void UBaseAbilitySet::ValidateAttributeSetsOnly()
{
    FAbilitySetValidationResult Result = ValidateAttributeSets();

    if (Result.bIsValid)
    {
        UE_LOGFMT(LogAbilitySet, Log, "✅ AttributeSets 검증 완료: 모든 어트리뷰트셋 설정이 유효합니다");
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green,
                TEXT("✅ AttributeSets 검증 성공!"));
        }
    }
    else
    {
        ShowValidationError(Result);
    }
}
#endif

#pragma endregion

void FBaseAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
    if (Handle.IsValid())
    {
        AbilitySpecHandles.Add(Handle);
    }
}

void FBaseAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
    if (Handle.IsValid())
    {
        GameplayEffectHandles.Add(Handle);
    }
}

void FBaseAbilitySet_GrantedHandles::AddAttributeSet(UBaseAttributeSet* Set)
{
    GrantedAttributeSets.Add(Set);
}

void UBaseAbilitySet::GiveStartupAttributeSetToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
    if (!ASC)
    {
        UE_LOGFMT(LogAbilitySet, Error, "AttributeSet 등록 실패 - ASC가 유효하지 않음");
        return;
    }

    UE_LOGFMT(LogAbilitySet, Log, "AttributeSet 등록 시작 - ASC Owner: {0}",
        ASC->GetOwner() ? ASC->GetOwner()->GetName() : TEXT("Invalid"));

    for (int32 SetIndex = 0; SetIndex < AttributeSets.Num(); ++SetIndex)
    {
        const FBaseAbilitySet_AttributeSet& SetToGrant = AttributeSets[SetIndex];

        if (!IsValid(SetToGrant.AttributeSet))
        {
            UE_LOGFMT(LogAbilitySet, Error, "AttributeSet 등록 실패 - 인덱스: {0}, 사유: AttributeSet이 유효하지 않음", SetIndex);
            continue;
        }

        UBaseAttributeSet* NewSet = NewObject<UBaseAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet);
        if (!NewSet)
        {
            UE_LOGFMT(LogAbilitySet, Error, "AttributeSet 생성 실패 - 인덱스: {0}", SetIndex);
            continue;
        }

        ASC->AddAttributeSetSubobject(NewSet);
        UE_LOGFMT(LogAbilitySet, Log, "AttributeSet 등록 완료 - 타입: {0}", NewSet->GetClass()->GetName());

        if (OutGrantedHandles)
        {
            OutGrantedHandles->AddAttributeSet(NewSet);
            UE_LOGFMT(LogAbilitySet, Log, "AttributeSet Handle 추가됨");
        }
    }
}

void UBaseAbilitySet::GiveStartupGameplayEffectToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
    if (!ASC)
    {
        UE_LOGFMT(LogAbilitySet, Error, "GameplayEffect 등록 실패 - ASC가 유효하지 않음");
        return;
    }

    UE_LOGFMT(LogAbilitySet, Log, "GameplayEffect 등록 시작 - ASC Owner: {0}",
        ASC->GetOwner() ? ASC->GetOwner()->GetName() : TEXT("Invalid"));

    for (int32 EffectIndex = 0; EffectIndex < GameplayEffects.Num(); ++EffectIndex)
    {
        const FBaseAbilitySet_GameplayEffect& EffectToGrant = GameplayEffects[EffectIndex];

        if (!IsValid(EffectToGrant.GameplayEffect))
        {
            UE_LOGFMT(LogAbilitySet, Error, "GameplayEffect 등록 실패 - 인덱스: {0}, 사유: GameplayEffect가 유효하지 않음", EffectIndex);
            continue;
        }

        const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
        if (!GameplayEffect)
        {
            UE_LOGFMT(LogAbilitySet, Error, "GameplayEffect CDO 가져오기 실패 - 인덱스: {0}", EffectIndex);
            continue;
        }

        const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());
        if (!GameplayEffectHandle.WasSuccessfullyApplied())
        {
            UE_LOGFMT(LogAbilitySet, Warning, "GameplayEffect 적용 실패 - 타입: {0}", GameplayEffect->GetName());
            continue;
        }

        UE_LOGFMT(LogAbilitySet, Log, "GameplayEffect 적용 완료 - 타입: {0}, 레벨: {1}",
            GameplayEffect->GetName(), EffectToGrant.EffectLevel);

        if (OutGrantedHandles)
        {
            OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
            UE_LOGFMT(LogAbilitySet, Log, "GameplayEffect Handle 추가됨");
        }
    }
}

void UBaseAbilitySet::GiveStartupGameplayAbilityToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
    if (!ASC)
    {
        UE_LOGFMT(LogAbilitySet, Error, "GameplayAbility 등록 실패 - ASC가 유효하지 않음");
        return;
    }

    UE_LOGFMT(LogAbilitySet, Log, "GameplayAbility 등록 시작 - ASC Owner: {0}",
        ASC->GetOwner() ? ASC->GetOwner()->GetName() : TEXT("Invalid"));

    for (int32 AbilityIndex = 0; AbilityIndex < GameplayAbilities.Num(); ++AbilityIndex)
    {
        const FBaseAbilitySet_GameplayAbility& AbilityToGrant = GameplayAbilities[AbilityIndex];

        if (!IsValid(AbilityToGrant.Ability))
        {
            UE_LOGFMT(LogAbilitySet, Error, "GameplayAbility 등록 실패 - 인덱스: {0}, 사유: Ability가 유효하지 않음", AbilityIndex);
            continue;
        }

        UBaseGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UBaseGameplayAbility>();
        if (!AbilityCDO)
        {
            UE_LOGFMT(LogAbilitySet, Error, "GameplayAbility CDO 가져오기 실패 - 인덱스: {0}", AbilityIndex);
            continue;
        }

        FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
        AbilitySpec.SourceObject = SourceObject;

        if (AbilityToGrant.bActive && AbilityToGrant.bInputBinded)
        {
            AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);
            UE_LOGFMT(LogAbilitySet, Log, "Input Tag 추가됨 - Ability: {0}, Tag: {1}",
                AbilityCDO->GetName(), AbilityToGrant.InputTag.ToString());
        }

        const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
        if (!AbilitySpecHandle.IsValid())
        {
            UE_LOGFMT(LogAbilitySet, Warning, "GameplayAbility 등록 실패 - 타입: {0}", AbilityCDO->GetName());
            continue;
        }

        UE_LOGFMT(LogAbilitySet, Log, "GameplayAbility 등록 완료 - 타입: {0}, 레벨: {1}",
            AbilityCDO->GetName(), AbilityToGrant.AbilityLevel);

        if (OutGrantedHandles)
        {
            OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
            UE_LOGFMT(LogAbilitySet, Log, "GameplayAbility Handle 추가됨");
        }
    }
}