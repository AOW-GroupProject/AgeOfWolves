#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"

#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "02_AbilitySystem/BaseAbilitySet.h"
#include "04_Component/BaseAbilitySystemComponent.h"

#include "PlayerStateBase.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPlayerStateBase, Log, All)

//@전방 선언
#pragma region Forward Declaration
class ABasePlayerController;
class UUserWidget;
class UAOWSaveGame;
class UAbilityManagerSubsystem;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttributeSetInitialized);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAnyAttributeValueChanged, FGameplayAttribute, Attribute, float, OldValue, float, NewValue);

DECLARE_MULTICAST_DELEGATE_OneParam(FNotifyPlayerDeathEvent, APlayerStateBase*);
DECLARE_MULTICAST_DELEGATE_OneParam(FNotifyPlayerRevivalEvent, APlayerStateBase*);

#pragma endregion

/**
 * Player State contaions pawn's info interacting with others
 */
UCLASS()
class AGEOFWOLVES_API APlayerStateBase : public APlayerState, public IAbilitySystemInterface
{

//@친추 클래스
#pragma region Friend Class
	friend class ABasePlayerController;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	APlayerStateBase();

protected:
	//~APlayerState Interface
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End Of APlayerState Interface

protected:
	//@내부 바인딩

protected:
	//@외부 바인딩
	void InternalBindingToASC();

public:
	UFUNCTION()
		void InitializePlayerState();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	//@로딩 작업
	UFUNCTION()
		void LoadGameAbilitySystem();
	//@캐릭터의 Ability System 정보를 Ability Manager Subsystem으로부터 Load합니다.
	void LoadDefaultAbilitySystemFromAbilityManager();
	//@캐릭터의 Ability System 정보를 Save File로부터 Load합니다.
	void LoadAbilitySystemFromSaveGame(UAOWSaveGame* SaveGame);

protected:
	FBaseAbilitySet_GrantedHandles* SetGrantedHandles;

	UPROPERTY()
		UBaseAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY()
		TSoftObjectPtr<UBaseAttributeSet> AttributeSet;

protected:
	//@캐릭터 태그
	FGameplayTag CharacterTag;

private:
	//@상태 태그 캐싱
	FGameplayTag StateTagCache;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@초기화 완료 이벤트
	FAttributeSetInitialized OnAttributeSetInitialized;

public:
	//@Attribute 수치 변화 이벤트
	FAnyAttributeValueChanged OnAnyAttributeValueChanged;

public:
	//@플레이어 죽음 알림 이벤트
	FNotifyPlayerDeathEvent NotifyPlayerDeathEvent;
	//@부활 어빌리티 종료 이벤트
	FNotifyPlayerRevivalEvent NotifyPlayerRevivalEvent;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	void OnAttributeValueChanged(const FOnAttributeChangeData& Data);

protected:
	UFUNCTION()
		void OnCharacterStateEventOnGameplay(AActor* Actor, const FGameplayTag& CharacterStateTag);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	UPROPERTY()
		TWeakObjectPtr<UAbilityManagerSubsystem> AbilityManagerSubsystemRef;

public:
	//~IAbilitySystemInterface Interface
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End Of IAbilitySystemInterface Interface
	TSoftObjectPtr<UBaseAttributeSet> GetAttributeSet() const;

public:
	template<typename T>
	T GetAttributeValue(FString FindingAttriubteName)
	{
		check(AttributeSet.Get());

		TArray<FGameplayAttribute> Attributes = AttributeSet.Get()->GetAllAttributes();
		for (const auto Attribute : Attributes)
		{
			if (Attribute.IsValid() && Attribute.AttributeName == FindingAttriubteName)
			{
				return Attribute.GetNumericValue(AttributeSet.Get());
			}
		}
		return -1;
	}
#pragma endregion
	
};
