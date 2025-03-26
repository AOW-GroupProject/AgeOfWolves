#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "01_Character/PlayerCharacter.h"

#include "Area.generated.h"

//@전방 선언
#pragma region Forward Declaration
class UBoxComponent;
class UObjectiveDetectionComponent;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region StructsOW
/*
*   @FAreaAIInfo
* 
*   Area에 등록된 AI들의 정보를 담은 구조체
*/
USTRUCT(BlueprintType)
struct FAreaAIInfo
{
    GENERATED_BODY()

    //@AI의 약한 참조
    UPROPERTY()
        TWeakObjectPtr<AActor> AIActor;

    //@현재 상태
    UPROPERTY()
        FGameplayTag CurrentState;

    //@상태 변경 시간
    UPROPERTY()
        float StateChangeTime = 0.0f;

    //@ASC 약한 참조
    UPROPERTY()
        TWeakObjectPtr<UBaseAbilitySystemComponent> AIASC;

    FAreaAIInfo() {}

    FAreaAIInfo(AActor* InActor, const FGameplayTag& InState, UBaseAbilitySystemComponent* InASC)
        : AIActor(InActor)
        , CurrentState(InState)
        , StateChangeTime(0.0f)
        , AIASC(InASC)
    {}

};

/*
*   @FPlayerBindingInfo
* 
*   Area 내 진입한 사용자와 Area 간 바인딩에 필요한 정보를 담은 구조체 
*/
USTRUCT()
struct FPlayerBindingInfo
{
    GENERATED_BODY()

    //@Player Characetr 약한 참조
    UPROPERTY()
        TWeakObjectPtr<APlayerCharacter> PlayerCharacter;

    //@마지막 진입/이탈 시간
    float LastExitTime = 0.0f;

    //@바인딩 해제 지연 시간 (초)
    static constexpr float UnbindDelay = 5.0f;

    FPlayerBindingInfo() {}

    FPlayerBindingInfo(APlayerCharacter* InPlayer)
        : PlayerCharacter(InPlayer)
        , LastExitTime(0.0f)
    {}

    //@지연 해제 필요 여부 확인
    bool ShouldDelayUnbind(float CurrentTime) const
    {
        return (CurrentTime - LastExitTime) < UnbindDelay;
    }

};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@AI 상태 변경 델리게이트
DECLARE_MULTICAST_DELEGATE_FourParams(FAreaAIStateChanged, AActor*, const FGameplayTag&, AActor*, const FGuid&);
#pragma endregion

/*
*   @AArea
* 
*   Level 공간 분할 단위로 특정 지역을 담당하며 해당 지역과 관련된 정보를 관리하는 객체
*/
UCLASS(Blueprintable)
class AGEOFWOLVES_API AArea : public AActor
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    AArea();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
    //@내부 바인딩

protected:
    //@외부 바인딩

protected:
    //@초기화
    void InitializeArea();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    void InitializeAreaAIInfos();

protected:
    //@AI 처리 함수
    void RegisterAI(AActor* AIActor);
    void UnregisterAI(AActor* AIActor);

protected:
    //@플레이어 처리 함수
    void RegisterPlayer(APlayerCharacter* Player);
    void UnregisterPlayer(APlayerCharacter* Player);

protected:
    //@영역 경계 박스
    UPROPERTY(VisibleAnywhere, Category = "Components")
        UBoxComponent* AreaBounds;

protected:
    //@영역 식별자
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area")
        FGuid AreaID;

    //@영역 태그 (전투 지역, 휴식 지역 등 특성)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area")
        TArray<FGameplayTag> AreaTags;

    //@영역 우선순위 (중첩 처리용)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Area")
        int32 AreaPriority = 0;

protected:
    //@수동으로 등록할 AI 액터
    UPROPERTY(EditAnywhere, Category = "Area|AI")
        TArray<AActor*> RegisteredAIActors;

protected:
    //@자동으로 영역 내 AI 감지할지 여부
    UPROPERTY(EditAnywhere, Category = "Area|AI")
        bool bAutoDetectAI = true;

    //@영역 내 AI 감지 반경 (자동 감지 시)
    UPROPERTY(EditAnywhere, Category = "Area|AI", meta = (EditCondition = "bAutoDetectAI"))
        float AIDetectionRadius = 1000.0f;

protected:
    //@등록된 AI의 Map
    UPROPERTY()
        TMap<TWeakObjectPtr<AActor>, FAreaAIInfo> MAIInfos;

    //@영역 내 플레이어 정보
    UPROPERTY()
        TMap<TWeakObjectPtr<APlayerCharacter>, FPlayerBindingInfo> MPlayerBindings;

protected:
    //@정리 타이머
    FTimerHandle CleanupTimerHandle;

    //@마지막 정리 시간
    float LastCleanupTime;

    //@정리 주기 (초)
    UPROPERTY(EditAnywhere, Category = "Area|Advanced")
        float CleanupInterval = 60.0f;

protected:
    //@AI 상태 변경 처리
    UFUNCTION()
        void HandleAIStateChanged(AActor* AIActor, const FGameplayTag& StateTag);

protected:
    //@자원 정리
    void CleanupInvalidReferences();
#pragma endregion

    //@Delegates
#pragma region Delegates
public:
    //2AI 상태 변경 델리게이트
    FAreaAIStateChanged AreaAIStateChanged;
#pragma endregion

//@Callbacks
#pragma region Callbacks
public:
    //@충돌 시작 이벤트를 구독하는 콜백
    UFUNCTION()
        void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
            bool bFromSweep, const FHitResult& SweepResult);
    //@충돌 종료 이벤트를 구독하는 콜백
    UFUNCTION()
        void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

protected:
    //@AI ASC 이벤트 콜백
    UFUNCTION()
        void OnAICharacterStateEvent(AActor* Actor, const FGameplayTag& StateTag);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    //@영역 ID 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
        FGuid GetAreaID() const { return AreaID; }

    //@영역 태그 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
        const TArray<FGameplayTag>& GetAreaTags() const { return AreaTags; }

    //@영역 우선순위 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
        int32 GetAreaPriority() const { return AreaPriority; }

    //@영역 내 AI 목록 가져오기
    UFUNCTION(BlueprintCallable, Category = "Area")
        TArray<AActor*> GetAIInArea() const;
#pragma endregion
};