#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "10_AI/BaseAIController.h"
#include "16_Level/Area.h"

#include "CrowdControlComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCrowdCtrl, Log, All)

//@전방 선언
#pragma region Forward Declaration
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
/*
*   @FSharingInfoQuery
*
*   그룹 내 공유 정보의 쿼리 구조체
*/
USTRUCT(BlueprintType)
struct FSharingInfoQuery
{
    GENERATED_BODY()

    AActor* SenderAI;
    FSharingInfoWithGroup Info;
    TArray<TWeakObjectPtr<AActor>> Recipients;

    FSharingInfoQuery() : SenderAI(nullptr) {}

    FSharingInfoQuery(AActor* InSender, const FSharingInfoWithGroup& InInfo)
        : SenderAI(InSender)
        , Info(InInfo)
    {
    }
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGroupInfoReceived, AActor*, AI, FSharingInfoWithGroup, InfoData);
#pragma endregion

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AGEOFWOLVES_API UCrowdControlComponent : public UActorComponent
{
//@친추 클래스
#pragma region Friend Class
    friend class AArea;
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UCrowdControlComponent();

protected:
    //~UActorComponent Interface
    virtual void OnRegister() override;
    virtual void OnUnregister() override;
    virtual void InitializeComponent() override;
    virtual void DestroyComponent(bool bPromoteChildren = false) override;
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    //~End of UActorComponent Interface

protected:
    //@외부 바인딩...
    void ExternalBindingToArea(AArea* Area);

protected:
    //@내부 바인딩...

public:
    //@초기화
    UFUNCTION()
        void InitializeCrowdControlComp(TArray<FAIGroupInfo> AIGroups);
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    //@정보 처리
    UFUNCTION()
        void ProcessInfoQueue();

    //@전달 받은 상태 정보 별 처리
    void ProcessDeadStateInfo(AActor* SenderAI, AActor* RecipientAI, const FSharingInfoWithGroup& InfoData);
    void ProcessFragileStateInfo(AActor* SenderAI, AActor* RecipientAI, const FSharingInfoWithGroup& InfoData);
    void ProcessGenericStateInfo(AActor* SenderAI, AActor* RecipientAI, const FSharingInfoWithGroup& InfoData);

protected:
    //@만료된 처리 정보 ID 제거
    void CleanupExpiredProcessedInfoIDs(float CurrentTime);

protected:
    //@정보 큐 - 그룹 내 공유 정보 저장
    TQueue<FSharingInfoQuery> GroupInfoQueue;

    //@최근 처리된 정보 ID 추적 (중복 방지)
    UPROPERTY()
        TMap<FGuid, float> ProcessedInfoIDs;

    //@정보 처리 타이머 핸들
    FTimerHandle InfoProcessingTimerHandle;

    //@한 번에 처리 가능한 최대 정보량
    const uint8 MaxInfosToProcess = 5;

    //@정보 처리 주기 (초)
    UPROPERTY(EditAnywhere, Category = "군중 제어 | 설정")
        float InfoProcessingInterval = 0.1f;

    //@중요 정보 우선순위 임계값
    UPROPERTY(EditAnywhere, Category = "군중 제어 | 설정")
        int32 HighPriorityThreshold = 5;

    //@처리된 정보 만료 시간 (초)
    UPROPERTY(EditAnywhere, Category = "군중 제어 | 설정")
        float ProcessedInfoExpiryTime = 10.0f;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@그룹 정보 수신 델리게이트
    UPROPERTY(EditAnywhere, Category = "군중 제어 | 이벤트")
        FGroupInfoReceived GroupInfoReceived;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@그룹 정보 수신 콜백
    UFUNCTION()
        void GroupToShareInfoNotified(AActor* AI, FSharingInfoWithGroup SharingInfo);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
    //@AI 액터와 소속 그룹 ID 맵핑
    TMap<FGuid, FAIGroupInfo> MAIToGroups;

protected:
    //@수신자 목록 생성 함수
    void FindRecipients(
        AActor* SenderAI,
        const FSharingInfoWithGroup& SharingInfo,
        TArray<TWeakObjectPtr<AActor>>& OutRecipients);

public:
    UFUNCTION(BlueprintCallable, Category = "군중 제어")
        const TArray<FAIGroupInfo>& GetCachedAIGroups() const;

    UFUNCTION(BlueprintCallable, Category = "군중 제어")
        EAIHierarchyType GetAIHierarchyType(AActor* AI, const FAIGroupInfo& GroupInfo) const;

protected:
    //@정보가 이미 처리되었는지 확인
    bool IsInfoAlreadyProcessed(const FGuid& InfoID, float CurrentTime);

    //@정보 우선순위 기반 처리 결정
    bool ShouldProcessInfo(const FSharingInfoWithGroup& InfoData);

public:
    //@큐 상태 정보 반환
    UFUNCTION(BlueprintCallable, Category = "군중 제어")
        int32 GetPendingInfoCount() const;
#pragma endregion
};