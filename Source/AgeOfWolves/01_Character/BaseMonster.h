// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "02_AbilitySystem/BaseAbilitySet.h"
#include "10_Monster/MonsterData.h"

#include "BaseMonster.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogBaseMonster, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UBaseAbilitySet;
class UBaseAttributeSet;
class UBaseAbilitySystemComponent;
class UBaseGameplayAbility;
class UMotionWarpingComponent;
class UAbilityManagerSubsystem;
class UAOWSaveGame;
#pragma endregion

//@열거형
#pragma region Enums
UENUM(BlueprintType)
enum class EMonsterState : uint8
{
    Patrol UMETA(DisplayName = "순찰"),
    Strafe UMETA(DisplayName = "배회"),
    Attacking UMETA(DisplayName = "공격중"),
    DetectingPlayer UMETA(DisplayName = "플레이어감지중"),
    Stunned UMETA(DisplayName = "기절")
};
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@Monster의 Attribute Set 초기화 완료 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMonsterAttributeSetInitialized);
//@Monster의 Attribute Set 수치 값 변화 이벤트
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAnyMonsterAttributeValueChanged, FGameplayAttribute, float, float);
#pragma endregion

/*
*   @ABaseMonster
* 
*   Enemy 객체의 상위 클래스를 정의합니다.
*/
UCLASS()
class AGEOFWOLVES_API ABaseMonster : public ACharacter, public IAbilitySystemInterface
{

//@친구 클래스
#pragma region Friend Class
    friend class ABaseMonsterAIController;
#pragma endregion

    GENERATED_BODY()

//@기본 설정
#pragma region Default Setting
public:
    ABaseMonster();

protected:
    virtual void PostInitializeComponents() override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void PossessedBy(AController* NewController) override;

protected:
    //@내부 바인딩

protected:
    //@외부 바인딩

protected:
    //@어빌리티 시스템 초기화
    void InitializeMonster();
#pragma endregion

    //@속성/정보
#pragma region Property or Subwidgets or Infos...etc
protected:
    //@로딩 작업
    UFUNCTION()
        void LoadGameAbilitySystem();
    //@캐릭터의 Ability System 정보를 Ability Manager Subsystem으로부터 Load합니다.
    void LoadDefaultAbilitySystemFromAbilityManager();
    //@캐릭터의 Ability System 정보를 Save File로부터 Load합니다.
    void LoadAbilitySystemFromSaveGame(UAOWSaveGame* SaveGame);

protected:
    //@Rotation 설정
    virtual void ControllRotation(){}

protected:
    //@AI State
    UFUNCTION(BlueprintCallable)
        void ChangeState(EMonsterState inValue);

    //@상태 종료 시 호출되는 생명주기 함수
    UFUNCTION(BlueprintCallable)
        virtual void WhenEndState();

protected:
    //@ASC
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "어빌리티")
        TObjectPtr<UBaseAbilitySystemComponent> AbilitySystemComponent;

    //@Base Ability Set 부여 핸들
    FBaseAbilitySet_GrantedHandles* SetGrantedHandles;

protected:
    //@캐릭터 태그
    UPROPERTY(EditDefaultsOnly, Category = "캐릭터 태그")
        FGameplayTag CharacterTag;

public:
    //@TODO: FGmaeplayTag로 변경
    //@이름
    EMonsterName MonsterName;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
        FSingleMonsterData SingleMonsterData;

protected:
    //@AI State
    EMonsterState CurrentState;

#pragma endregion

//@델리게이트
#pragma region Delegates
public:
    //@Monster의 Attribute 초기화 완료 이벤트
    FMonsterAttributeSetInitialized MonsterAttributeSetInitialized;

public:
    //@Monster의 Attribute 수치 값 변화 이벤트
    FAnyMonsterAttributeValueChanged AnyMonsterAttributeValueChanged;
#pragma endregion

//@콜백
#pragma region Callbacks
protected:
    void OnAttributeValueChanged(const FOnAttributeChangeData& Data);
#pragma endregion

    //@유틸리티
#pragma region Utility
protected:
    //@Attribute Set 캐싱
    UPROPERTY()
        TSoftObjectPtr<UBaseAttributeSet> AttributeSet;

    //@Ability Manager Subsystem 캐싱
    UPROPERTY()
        TWeakObjectPtr<UAbilityManagerSubsystem> AbilityManagerSubsystemRef;

public:
    virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

public:
    UFUNCTION(BlueprintCallable)
        EMonsterState GetCurrentState();

    UFUNCTION(BlueprintCallable)
        FSingleMonsterData GetSingleMonsterData() const { return SingleMonsterData; }
#pragma endregion
};