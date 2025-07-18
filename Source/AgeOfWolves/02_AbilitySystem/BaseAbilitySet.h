#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"

#include "GameplayTagContainer.h"
#include "GameplayAbilitySpecHandle.h"
#include "ActiveGameplayEffectHandle.h"

#include "BaseAbilitySet.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAbilitySet, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UBaseAttributeSet;
class UBaseGameplayAbility;
class UGameplayEffect;
class UAbilityTagRelationshipMapping;
class UBaseAbilitySystemComponent;
class APlayerStateBase;
class UInputConfig;
#pragma endregion

//@열거형
#pragma region Enums
/**
 * @EAbilitySetValidationError
 *
 * Ability Set 검증 과정에서 발생할 수 있는 오류 유형을 정의합니다.
 * 각 오류 타입은 GAS 시스템의 특정한 문제점을 나타냅니다.
 */
UENUM(BlueprintType)
enum class EAbilitySetValidationError : uint8
{
    None                        UMETA(DisplayName = "오류 없음"),

    // === 중복 관련 오류 ===
    DuplicateAbilityTag         UMETA(DisplayName = "중복된 어빌리티 태그"),
    DuplicateInputTag           UMETA(DisplayName = "중복된 입력 태그"),

    // === Null 참조 오류 ===
    NullAbilityClass            UMETA(DisplayName = "Null 어빌리티 클래스"),
    NullGameplayEffectClass     UMETA(DisplayName = "Null 게임플레이 이펙트 클래스"),
    NullAttributeSetClass       UMETA(DisplayName = "Null 어트리뷰트 셋 클래스"),

    // === 유효하지 않은 값 오류 ===
    InvalidAbilityLevel         UMETA(DisplayName = "유효하지 않은 어빌리티 레벨"),
    InvalidEffectLevel          UMETA(DisplayName = "유효하지 않은 이펙트 레벨"),
    InvalidAbilityTag           UMETA(DisplayName = "유효하지 않은 어빌리티 태그"),
    InvalidInputTag             UMETA(DisplayName = "유효하지 않은 입력 태그"),
    InvalidCharacterTag         UMETA(DisplayName = "유효하지 않은 캐릭터 태그"),

    // === 논리적 일관성 오류 ===
    InputBindingInconsistency   UMETA(DisplayName = "입력 바인딩 불일치"),
    ActiveAbilityInconsistency  UMETA(DisplayName = "액티브 어빌리티 설정 불일치")
};
#pragma endregion

//@구조체
#pragma region Structs
/**
 *	@FAbilitySet_GameplayAbility
 *
 *	Gameplay Ability 와 함께 관리할 정보들을 정의한 구조체
 */
USTRUCT(BlueprintType)
struct FBaseAbilitySet_GameplayAbility
{
    GENERATED_BODY()

public:

    //@어빌리티 태그
    UPROPERTY(EditDefaultsOnly)
    FGameplayTag AbilityTag;
    /// @어빌리티 유형의 BP 클래스
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UBaseGameplayAbility> Ability = nullptr;

    /// @어빌리티의 아이콘 이미지
    UPROPERTY(EditDefaultsOnly)
    UTexture2D* AbilityIconImage;

    /// @어빌리티 레벨
    UPROPERTY(EditDefaultsOnly)
    int32 AbilityLevel = 1;

    /// @Active/Passive 여부
    UPROPERTY(EditDefaultsOnly)
    bool bActive = false;

    /// @사용자 입력 바인딩 여부(체크를 위해 bActive가 참이여만 합니다)
    UPROPERTY(EditDefaultsOnly)
    bool bInputBinded = false;

    /// @입력 태그
    UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag", EditCondition = "bActive&&bInputBinded"))
    FGameplayTag InputTag;
};

/**
 * FAbilitySet_GameplayEffect
 *
 *	Data used by the ability set to grant gameplay effects.
 */
USTRUCT(BlueprintType)
struct FBaseAbilitySet_GameplayEffect
{
    GENERATED_BODY()

public:

    // Gameplay effect to grant.
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

    // Level of gameplay effect to grant.
    UPROPERTY(EditDefaultsOnly)
    float EffectLevel = 1.0f;
};

/**
 * FBaseAbilitySet_AttributeSet
 *
 *	Data used by the ability set to grant attribute sets.
 */
USTRUCT(BlueprintType)
struct FBaseAbilitySet_AttributeSet
{
    GENERATED_BODY()

public:
    // Attribute Set
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UBaseAttributeSet> AttributeSet;

};

/**
 * @FAbilitySetValidationResult
 *
 * Ability Set 검증 결과를 담는 구조체입니다.
 * 어떤 오류가 어디서 발생했는지 상세한 정보를 제공합니다.
 */
USTRUCT(BlueprintType)
struct FAbilitySetValidationResult
{
    GENERATED_BODY()

public:
    // 검증 결과 (성공/실패)
    UPROPERTY(BlueprintReadOnly)
    bool bIsValid = true;

    // 오류 유형
    UPROPERTY(BlueprintReadOnly)
    EAbilitySetValidationError ErrorType = EAbilitySetValidationError::None;

    // 오류 메시지
    UPROPERTY(BlueprintReadOnly)
    FString ErrorMessage;

    // 문제가 발생한 어빌리티 인덱스
    UPROPERTY(BlueprintReadOnly)
    int32 ProblemAbilityIndex = -1;

    // 문제가 발생한 이펙트 인덱스
    UPROPERTY(BlueprintReadOnly)
    int32 ProblemEffectIndex = -1;

    // 문제가 발생한 어트리뷰트셋 인덱스
    UPROPERTY(BlueprintReadOnly)
    int32 ProblemAttributeSetIndex = -1;

    // 기본 생성자 (성공 케이스)
    FAbilitySetValidationResult()
    {
    }

    // 오류 케이스 생성자
    FAbilitySetValidationResult(EAbilitySetValidationError InErrorType, const FString& InErrorMessage,
        int32 InAbilityIndex = -1, int32 InEffectIndex = -1, int32 InAttributeSetIndex = -1)
        : bIsValid(false)
        , ErrorType(InErrorType)
        , ErrorMessage(InErrorMessage)
        , ProblemAbilityIndex(InAbilityIndex)
        , ProblemEffectIndex(InEffectIndex)
        , ProblemAttributeSetIndex(InAttributeSetIndex)
    {
    }
};

/**
 * FBaseAbilitySet_GrantedHandles
 *
 *	@목적 : GA, GE, 그리고 AttributeSet의 ASC 등록 이벤트 관련 핸들을 관리하는 구조체입니다.
 */
USTRUCT(BlueprintType)
struct FBaseAbilitySet_GrantedHandles
{
    GENERATED_BODY()

public:
    void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
    void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
    void AddAttributeSet(UBaseAttributeSet* Set);

    //void TakeFromAbilitySystem(ULyraAbilitySystemComponent* ASC);

protected:

    // Handles to the granted abilities.
    UPROPERTY()
    TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

    // Handles to the granted gameplay effects.
    UPROPERTY()
    TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

    // Pointers to the granted attribute sets
    UPROPERTY()
    TArray<TObjectPtr<UBaseAttributeSet>> GrantedAttributeSets;
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion


/**
*	@UBaseAbilitySet
*
*	Attribute Set, GA, GE 목록을 담는 Data Asset
*   에디터에서 실시간 무결성 검사를 통해 GAS 설정 오류를 방지합니다.
 */
UCLASS()
class AGEOFWOLVES_API UBaseAbilitySet : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    UBaseAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
    // 에디터에서 프로퍼티가 변경될 때 호출되는 함수
    // GAS 설정이 변경되는 즉시 무결성 검사를 수행합니다
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

    // 배열이나 구조체 내부의 프로퍼티가 변경될 때 호출되는 함수
    // GameplayAbilities 배열이나 GameplayEffects 배열의 변경사항을 감지합니다
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

protected:
#if WITH_EDITOR
    // === 무결성 검사 핵심 함수들 ===
    // 각 함수는 GAS의 특정 영역에 대한 검증을 담당합니다

    // 전체 무결성 검사를 수행하는 마스터 함수
    FAbilitySetValidationResult ValidateAbilitySet() const;

    // === 개별 검증 함수들 ===
    // 각 함수는 특정한 무결성 규칙을 담당합니다

    // 어빌리티 관련 검증
    FAbilitySetValidationResult ValidateGameplayAbilities() const;
    FAbilitySetValidationResult ValidateAbilityTagUniqueness() const;
    FAbilitySetValidationResult ValidateInputTagUniqueness() const;
    FAbilitySetValidationResult ValidateAbilityLogicalConsistency() const;

    // 게임플레이 이펙트 관련 검증
    FAbilitySetValidationResult ValidateGameplayEffects() const;

    // 어트리뷰트 셋 관련 검증
    FAbilitySetValidationResult ValidateAttributeSets() const;

    // 기본 설정 검증
    FAbilitySetValidationResult ValidateBasicSettings() const;

    // 검증 실패 시 사용자에게 알림을 표시하는 함수 (GEditor 사용 안함)
    void ShowValidationError(const FAbilitySetValidationResult& ValidationResult) const;

    // 문제가 있는 설정을 자동으로 수정하려고 시도하는 함수
    bool TryAutoFixValidationError(const FAbilitySetValidationResult& ValidationResult);

    // 유틸리티 함수들
    bool IsValidGameplayTag(const FGameplayTag& Tag) const;
    bool IsValidInputTag(const FGameplayTag& Tag) const;
#endif

public:
    /*
    * @목적 : 사용자 캐릭터의 기본 AttributeSet, Gameplay Ability, Gameplay Effect를 해당 캐릭터 객체의 ASC에 등록하는 함수들입니다.
    * @설명 : 각자의 방식으로 ASC에 등록한 후, 해당 등록 이벤트 관련한 Out Param으로 전달 받은 GrantHandle을 반환합니다.
    * @참조 : -
    */
    void GiveStartupAttributeSetToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const;
    void GiveStartupGameplayEffectToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const;
    void GiveStartupGameplayAbilityToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "캐릭터 태그")
    FGameplayTag CharacterTag;

protected:
    // Attribute sets to grant when this ability set is granted.
    UPROPERTY(EditDefaultsOnly, Category = "어트리뷰트 목록")
    TArray<FBaseAbilitySet_AttributeSet> AttributeSets;

    // Gameplay abilities to grant when this ability set is granted.
    UPROPERTY(EditDefaultsOnly, Category = "GA 목록")
    TArray<FBaseAbilitySet_GameplayAbility> GameplayAbilities;

    // Gameplay effects to grant when this ability set is granted.
    UPROPERTY(EditDefaultsOnly, Category = "GE 목록")
    TArray<FBaseAbilitySet_GameplayEffect> GameplayEffects;


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "어빌리티 관계성")
    TObjectPtr<UAbilityTagRelationshipMapping> TagRelationship;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "입력")
    TObjectPtr<UInputConfig> InputConfig;

public:
    FGameplayTag GetCharacterTag() const { return CharacterTag; }

public:
    TArray<FBaseAbilitySet_AttributeSet> GetAttributeSets() const { return AttributeSets; }
    const TArray<FBaseAbilitySet_GameplayAbility>& GetGameplayAbilities() const { return GameplayAbilities; }
    TArray<FBaseAbilitySet_GameplayEffect> GetGameplayEffects() const { return GameplayEffects; }
    UAbilityTagRelationshipMapping* GetATMR() const { return TagRelationship; }
    UInputConfig* GetInputConfig() const { return InputConfig; }

#if WITH_EDITOR
    // 에디터에서 수동으로 무결성 검사를 실행할 수 있는 함수
    UFUNCTION(CallInEditor, Category = "Ability Set Validation")
    void ValidateAbilitySetManually();

    // 개별 검증 함수들을 에디터에서 직접 호출할 수 있는 함수들
    UFUNCTION(CallInEditor, Category = "Ability Set Validation")
    void ValidateAbilitiesOnly();

    UFUNCTION(CallInEditor, Category = "Ability Set Validation")
    void ValidateEffectsOnly();

    UFUNCTION(CallInEditor, Category = "Ability Set Validation")
    void ValidateAttributeSetsOnly();
#endif

};
