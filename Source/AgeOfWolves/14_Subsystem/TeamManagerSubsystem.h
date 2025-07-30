#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GenericTeamAgentInterface.h"

#include "TeamManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogTeamManager, Log, All)

//@���� ����
#pragma region Forward Declaration
class UTeamSettingInfos;
#pragma endregion
//@������
#pragma region Enums
#pragma endregion

//@����ü
#pragma region Structs
#pragma endregion

//@�̺�Ʈ/��������Ʈ
#pragma region Delegates
#pragma endregion


/**
 *	@UTeamManagerSubsystem
 *
 *	�� ���� ������ �����ϴ� ����ý���
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
    // �� �µ� ĳ��
    TMap<uint8, TMap<uint8, TEnumAsByte<ETeamAttitude::Type>>> TeamAttitudeCache;

    // �� ���� ������
    UPROPERTY()
        TObjectPtr<UTeamSettingInfos> TeamSettingInfos;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion


//@Utility
#pragma region Utility
public:
    // �� �µ� ��ȸ
    ETeamAttitude::Type GetAttitude(uint8 SourceTeamId, uint8 TargetTeamId) const;

private:
    // �� �µ� ĳ�� �ʱ�ȭ
    void InitializeTeamAttitudeCache();
#pragma endregion
};