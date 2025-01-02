#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GenericTeamAgentInterface.h"

#include "TeamManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTeamManager, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UTeamSettingInfos;
#pragma endregion
//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion


/**
 *	@UTeamManagerSubsystem
 *
 *	팀 관련 정보를 관리하는 서브시스템
 */
UCLASS()
class AGEOFWOLVES_API UTeamManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

        //@Default Setting
#pragma region Default Setting
public:
    UTeamManagerSubsystem();

protected:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
#pragma endregion

    //@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
private:
    // 팀 태도 캐시
    TMap<uint8, TMap<uint8, TEnumAsByte<ETeamAttitude::Type>>> TeamAttitudeCache;

    // 팀 설정 데이터
    UPROPERTY()
        TObjectPtr<UTeamSettingInfos> TeamSettingInfos;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion


//@Utility
#pragma region Utility
public:
    // 팀 태도 조회
    ETeamAttitude::Type GetAttitude(uint8 SourceTeamId, uint8 TargetTeamId) const;

private:
    // 팀 태도 캐시 초기화
    void InitializeTeamAttitudeCache();
#pragma endregion
};