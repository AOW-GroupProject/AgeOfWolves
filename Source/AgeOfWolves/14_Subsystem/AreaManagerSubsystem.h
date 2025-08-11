#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "AreaManagerSubsystem.generated.h"

//@전방 선언
#pragma region Forward Declaration
struct FAreaQuestDataSet;
class AArea;
struct FAIGroupInfo;
class UAreaQuestDataInfos;
struct FAreaAIInfo;
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
 * @UAreaManagerSubsystem
 *
 * 월드에 배치된 Area들을 관리하고 필요한 정보를 제공하는 서브시스템
 */
UCLASS()
class AGEOFWOLVES_API UAreaManagerSubsystem : public UGameInstanceSubsystem
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    //@초기화
    UAreaManagerSubsystem();
    
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    //@Area 등록/해제
    UFUNCTION(BlueprintCallable, Category = "Area|Management")
    void RegisterArea(AArea* Area);

    UFUNCTION(BlueprintCallable, Category = "Area|Management")
    void UnregisterArea(AArea* Area);

private:
    //@등록된 Area 맵
    UPROPERTY()
    TMap<FGuid, AArea*> RegisteredAreas;

    //@AreaQuest 정보 참조
    UPROPERTY()
    TObjectPtr<UAreaQuestDataInfos> AreaQuestDataInfos;
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
    //@Area 검색
    UFUNCTION(BlueprintCallable, Category = "Area|Management")
    AArea* GetAreaByID(const FGuid& AreaID) const;

    //@AI 관련 정보 제공
    UFUNCTION(BlueprintCallable, Category = "Area|AI")
    FGuid GetAIGroupID(AActor* AIActor) const;

    UFUNCTION(BlueprintCallable, Category = "Area|AI")
    TArray<FAIGroupInfo> GetAllGroups() const;

    UFUNCTION(BlueprintCallable, Category = "Area|AI")
    bool GetGroupInfo(const FGuid& GroupID, FAIGroupInfo& OutGroupInfo) const;

    UFUNCTION(BlueprintCallable, Category = "Area|AI")
    AActor* GetPatrolPathForGroup(const FGuid& GroupID) const;

    //@Area의 살아있는 AI 정보만 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area|AI")
    TArray<FAreaAIInfo> GetAreaAliveAIInfos(const FGuid& AreaID) const;

    //@ Area의 QuestData 제공
    UFUNCTION(BlueprintCallable, Category = "Area|Quest")
    FAreaQuestDataSet GetAreaQuestDataSet(FGameplayTag AreaTag) const;

    //@ AreaQuestDataInfos 제공
    UFUNCTION(BlueprintCallable, Category = "Area|Quest")
    const UAreaQuestDataInfos* GetAreaQuestDataInfos() const;
#pragma endregion

};