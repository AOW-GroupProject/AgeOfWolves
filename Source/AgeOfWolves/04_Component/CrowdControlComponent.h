#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "CrowdControlComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCrowdCtrl, Log, All)

//@전방 선언
#pragma region Forward Declaration
class AArea;
struct FAIGroupInfo;
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
class AGEOFWOLVES_API UCrowdControlComponent : public UActorComponent
{
//@친추 클래스
#pragma region Friend Class
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
    //@외부 바인딩
    void ExternalBindingToAI(TWeakObjectPtr<AActor> AIActor);

protected:
    //@내부 바인딩

public:
    //@초기화
    UFUNCTION()
        void InitializeCrowdControlComp(TArray<FAIGroupInfo> AIGroups);
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:

#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    UFUNCTION()
        void OnSendInfoToBelongingGroup(AActor* AI, FSharingInfoWithGroup AIGroup);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
};
