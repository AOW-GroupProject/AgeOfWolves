#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "LevelDataInfos.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLevelDataInfos, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UWorld;
#pragma endregion

//@열거형
#pragma region Enums
/**
 * @ELevelType
 *
 * 레벨 타입을 정의하는 열거형
 */
UENUM(BlueprintType)
enum class ELevelType : uint8
{
    Title =0    UMETA(DisplayName = "Title"),
    World       UMETA(DisplayName = "World"),
    BossArena   UMETA(DisplayName = "Boss Arena"),
    Test        UMETA(DisplayName = "Test"),
    MAX
};

/**
 * @ELevelValidationError
 *
 * 레벨 데이터 검증 오류 타입
 */
UENUM(BlueprintType)
enum class ELevelValidationError : uint8
{
    None                    UMETA(DisplayName = "오류 없음"),

    //@기본 데이터 오류
    EmptyLevelData          UMETA(DisplayName = "비어있는 레벨 데이터"),
    InvalidLevelTag         UMETA(DisplayName = "유효하지 않은 레벨 태그"),
    DuplicateLevelTag       UMETA(DisplayName = "중복된 레벨 태그"),
    MismatchedTagCategory   UMETA(DisplayName = "태그와 카테고리 불일치"),

    //@레벨 정보 오류
    EmptyLevelName          UMETA(DisplayName = "비어있는 레벨 이름"),
    DuplicateLevelName      UMETA(DisplayName = "중복된 레벨 이름"),
    InvalidLevelType        UMETA(DisplayName = "유효하지 않은 레벨 타입"),

    //@에셋 참조 오류
    NullLevelAsset          UMETA(DisplayName = "Null 레벨 에셋"),
    InvalidLevelAsset       UMETA(DisplayName = "유효하지 않은 레벨 에셋"),
    MissingLevelAsset       UMETA(DisplayName = "존재하지 않는 레벨 에셋"),

    //@논리적 일관성 오류
    DisabledRequiredLevel   UMETA(DisplayName = "비활성화된 필수 레벨"),
    InconsistentLevelState  UMETA(DisplayName = "일관성 없는 레벨 상태"),
    MultipleDefaultLevels   UMETA(DisplayName = "복수 기본 레벨 지정"),

    InvalidPlayerStartTag   UMETA(DisplayName = "유효하지 않은 플레이어 시작 태그"),
    MissingPlayerStartTags  UMETA(DisplayName = "플레이어 시작 태그 누락"),
};
#pragma endregion

//@구조체
#pragma region Structs
/**
 * @FLevelData
 *
 * 개별 레벨 정보를 담는 구조체
 */
USTRUCT(BlueprintType)
struct FLevelData
{
    GENERATED_BODY()

public:
    //@레벨 분류
    UPROPERTY(EditDefaultsOnly, Category = "레벨 기본 정보")
    ELevelType LevelCategory;
    //@고유 레벨 식별자 (GameplayTag 기반)
    UPROPERTY(EditDefaultsOnly, Category = "레벨 기본 정보", meta = (Categories = "Level"))
    FGameplayTag LevelTag;

    //@표시용 레벨 이름
    UPROPERTY(EditDefaultsOnly, Category = "레벨 기본 정보")
    FText LevelName;

    //@레벨 설명
    UPROPERTY(EditDefaultsOnly, Category = "레벨 기본 정보")
    FText LevelDescription;
    //@레벨 에셋 참조
    UPROPERTY(EditDefaultsOnly, Category = "레벨 에셋")
    TSoftObjectPtr<UWorld> LevelAsset;

    //@기본 시작 레벨 여부
    UPROPERTY(EditDefaultsOnly, Category = "레벨 설정")
    bool bIsDefaultLevel = false;

    //@레벨 활성화 여부
    UPROPERTY(EditDefaultsOnly, Category = "레벨 설정")
    bool bIsEnabled = false;

    //@플레이어 시작 위치 (기본 시작점)
    UPROPERTY(EditDefaultsOnly, Category = "플레이어 시작")
    FGameplayTag DefaultPlayerStartTag;

    //@사용 가능한 모든 시작 위치 태그
    UPROPERTY(EditDefaultsOnly, Category = "플레이어 시작", meta = (Categories = "PlayerStart"))
    TArray<FGameplayTag> AvailablePlayerStartTags;

public:
    FLevelData()
        : LevelTag(FGameplayTag::EmptyTag)
        , LevelName(FText::FromString(TEXT("New Level")))
        , LevelDescription(FText::GetEmpty())
        , LevelCategory(ELevelType::World)
        , bIsDefaultLevel(false)
        , bIsEnabled(false)
        , DefaultPlayerStartTag(FGameplayTag::EmptyTag)
        , AvailablePlayerStartTags()
    {
    }

    //@레벨 태그 비교 연산자
    bool operator==(const FGameplayTag& OtherTag) const
    {
        return LevelTag.MatchesTagExact(OtherTag);
    }

    //@레벨 이름이 유효한지 확인
    bool HasValidName() const
    {
        return !LevelName.IsEmpty() && !LevelName.ToString().IsEmpty();
    }

    //@레벨 태그가 유효한지 확인
    bool HasValidTag() const
    {
        return LevelTag.IsValid();
    }

    //@레벨 에셋이 유효한지 확인
    bool HasValidAsset() const
    {
        return !LevelAsset.IsNull();
    }

    bool HasValidPlayerStartTags() const
    {
        return DefaultPlayerStartTag.IsValid() && AvailablePlayerStartTags.Num() > 0;
    }
};

/**
 * @FLevelValidationResult
 *
 * 레벨 데이터 검증 결과
 */
USTRUCT(BlueprintType)
struct FLevelValidationResult
{
    GENERATED_BODY()

public:
    //@검증 성공 여부
    UPROPERTY(BlueprintReadOnly)
    bool bIsValid = true;

    //@오류 타입
    UPROPERTY(BlueprintReadOnly)
    ELevelValidationError ErrorType = ELevelValidationError::None;

    //@오류 메시지
    UPROPERTY(BlueprintReadOnly)
    FString ErrorMessage;

    //@문제 레벨 인덱스
    UPROPERTY(BlueprintReadOnly)
    int32 ProblemLevelIndex = -1;

public:
    FLevelValidationResult()
    {
    }

    FLevelValidationResult(ELevelValidationError InErrorType, const FString& InErrorMessage, int32 InLevelIndex = -1)
        : bIsValid(false)
        , ErrorType(InErrorType)
        , ErrorMessage(InErrorMessage)
        , ProblemLevelIndex(InLevelIndex)
    {
    }
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@ULevelDataInfos
 *
 *	Level 정보를 담은 Data Asset입니다.
 *  에디터에서 실시간 무결성 검사를 통해 레벨 설정 오류를 방지합니다.
 */
UCLASS()
class AGEOFWOLVES_API ULevelDataInfos : public UPrimaryDataAsset
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    ULevelDataInfos(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif

protected:
#if WITH_EDITOR
    //@전체 무결성 검사
    FLevelValidationResult ValidateLevelData() const;

    //@개별 검증 함수들
    FLevelValidationResult ValidateBasicSettings() const;
    FLevelValidationResult ValidateLevelTags() const;
    FLevelValidationResult ValidateLevelNames() const;
    FLevelValidationResult ValidateLevelTypes() const;
    FLevelValidationResult ValidateLevelAssets() const;
    FLevelValidationResult ValidateLogicalConsistency() const;
    FLevelValidationResult ValidatePlayerStartTags() const;

    //@오류 표시 및 자동 수정
    void ShowValidationError(const FLevelValidationResult& ValidationResult) const;
    bool TryAutoFixValidationError(const FLevelValidationResult& ValidationResult);

    //@유틸리티 함수들
    bool IsValidLevelName(const FText& LevelName) const;
    bool IsLevelAssetValid(const TSoftObjectPtr<UWorld>& LevelAsset) const;
#endif
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    //@레벨 데이터 목록
    UPROPERTY(EditDefaultsOnly, Category = "레벨 정보", meta = (TitleProperty = "LevelName"))
    TArray<FLevelData> LevelDataList;

public:
    //@레벨 검색 함수들 (C++ 전용 - 포인터 반환)
    const FLevelData* FindLevelByTag(const FGameplayTag& LevelTag) const;
    const FLevelData* FindLevelByName(const FText& LevelName) const;

    //@Blueprint 호환 검색 함수들 (값 반환)
    UFUNCTION(BlueprintCallable, Category = "Level Data")
    bool GetLevelByTag(const FGameplayTag& LevelTag, FLevelData& OutLevelData) const;

    UFUNCTION(BlueprintCallable, Category = "Level Data")
    bool GetLevelByName(const FText& LevelName, FLevelData& OutLevelData) const;

    UFUNCTION(BlueprintCallable, Category = "Level Data")
    TArray<FLevelData> GetLevelsByType(ELevelType LevelType) const;

    UFUNCTION(BlueprintCallable, Category = "Level Data")
    TArray<FLevelData> GetEnabledLevels() const;

    UFUNCTION(BlueprintCallable, Category = "Level Data")
    bool IsLevelEnabled(const FGameplayTag& LevelTag) const;

    UFUNCTION(BlueprintCallable, Category = "Level Data")
    int32 GetLevelCount() const { return LevelDataList.Num(); }

#if WITH_EDITOR
    //@에디터 검증 함수들
    UFUNCTION(CallInEditor, Category = "Level Validation")
    void ValidateLevelDataManually();

    UFUNCTION(CallInEditor, Category = "Level Validation")
    void ValidateLevelAssetsOnly();

    UFUNCTION(CallInEditor, Category = "Level Validation")
    void ValidateLevelNamesOnly();

    UFUNCTION(CallInEditor, Category = "Level Utilities")
    void SortLevelsByType();
#endif
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    //@Default 시작 레벨 가져오기
    const FLevelData* GetDefaultLevel() const;

    //@타입별 레벨 개수 반환
    int32 GetLevelCountByType(ELevelType LevelType) const;

    //@특정 타입의 첫 번째 레벨 반환 (C++ 전용)
    const FLevelData* GetFirstLevelOfType(ELevelType LevelType) const;

    //@활성화된 레벨만 반환 (GetEnabledLevels()와 동일, 편의성을 위한 별칭)
    TArray<FLevelData> GetActiveLevels() const;

    //@레벨 인덱스로 안전하게 접근 (C++ 전용)
    const FLevelData* GetLevelAtIndex(int32 Index) const;

    //@Blueprint 호환 유틸리티 함수들
    UFUNCTION(BlueprintCallable, Category = "Level Utilities")
    bool GetFirstLevelOfTypeBlueprint(ELevelType LevelType, FLevelData& OutLevelData) const;

    UFUNCTION(BlueprintCallable, Category = "Level Utilities")
    bool GetLevelAtIndexBlueprint(int32 Index, FLevelData& OutLevelData) const;
#pragma endregion

};