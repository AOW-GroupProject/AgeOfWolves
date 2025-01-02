#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GenericTeamAgentInterface.h"

#include "TeamSettingInfos.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTeamSettingInfos, Log, All)

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
/**
 *	@FTeamAttitudeSettings
 *
 *	팀 간의 관계를 정의하는 구조체
 */
USTRUCT(BlueprintType)
struct FTeamAttitudeSettings
{
    GENERATED_BODY()

public:
    //@대상 팀 ID
    UPROPERTY(EditDefaultsOnly, Category = "Team Settings")
        FGenericTeamId TargetTeamId;

    //@대상 팀에 대한 태도
    UPROPERTY(EditDefaultsOnly, Category = "Team Settings")
        TEnumAsByte<ETeamAttitude::Type> Attitude;
};

/**
 *	@FTeamSettings
 *
 *	팀 설정을 정의하는 구조체
 */
USTRUCT(BlueprintType)
struct FTeamSettings
{
    GENERATED_BODY()

public:
    // 팀 ID
    UPROPERTY(EditDefaultsOnly, Category = "Team Settings")
        FGenericTeamId TeamId;

    // 다른 팀들에 대한 태도 설정
    UPROPERTY(EditDefaultsOnly, Category = "Team Settings")
        TArray<FTeamAttitudeSettings> TeamAttitudes;
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UTeamSettingInfos
 * 
 *	캐릭터 간 팀 세팅 정보를 관리하는 Data Asset
 */
UCLASS()
class AGEOFWOLVES_API UTeamSettingInfos : public UPrimaryDataAsset
{
    GENERATED_BODY()

        //@Defualt Setting
#pragma region Default Setting
public:
    UTeamSettingInfos(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    // 팀 설정 목록
    UPROPERTY(EditDefaultsOnly, Category = "팀 설정")
        TArray<FTeamSettings> TeamSettings;
#pragma endregion
};
