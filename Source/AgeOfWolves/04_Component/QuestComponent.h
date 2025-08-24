#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "16_Level/AreaQuestDataInfos.h"

#include "QuestComponent.generated.h"


DECLARE_LOG_CATEGORY_EXTERN(LogQuest, Log, All);

//@전방 선언
#pragma region Forward Declaration
class UAreaManagerSubsystem;
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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API UQuestComponent : public UActorComponent
{
	
//@친추 클래스
#pragma region Friend Class
#pragma endregion
	
	GENERATED_BODY()
	
//@Defualt Setting
#pragma region Default Setting
public:	
	UQuestComponent(const FObjectInitializer& ObjectInitializer);

protected:
	//~UActorComponent Interface
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void InitializeComponent() override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End of UActorComponent Interface

protected:
	//@ 외부 바인딩
	void InternalBindToArea();
	void ExternalBindToPlayerState();
	
	void UnbindFromArea();
	void UnbindFromPlayerState();
#pragma endregion

//@Property/Info...etc
#pragma region Quest
private:
	//@ Area 참조
	UPROPERTY()
	TWeakObjectPtr<AArea> AreaRef;

	//@ Area의 AreaTag에 맞는 퀘스트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FAreaQuestDataSet AreaQuest;

	//@ Subsystem Caching
	UAreaManagerSubsystem* CachedAreaManager;

private:
	//@ Quest 경과시간 업데이트
	void UpdateQuestElapsedTime(float DeltaTime);
#pragma endregion
	
//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@ 제거형 퀘스트 목표 상태 변경 통지(Area에서 호출)
	UFUNCTION()
	void OnAreaAIStateChanged(AActor* AIActor, const FGameplayTag& StateTag, AArea* SourceArea, const FGuid& AreaID);

	UFUNCTION()
	void OnPlayerDeath(APlayerStateBase* DeadPlayerState);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
};
