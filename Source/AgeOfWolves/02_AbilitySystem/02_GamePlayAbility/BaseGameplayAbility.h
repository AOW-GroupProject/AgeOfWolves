#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"

#include "BaseGameplayAbility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGA, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UGameplayEffect;
class UAbilityTagRelationshipMapping;
class UAttackGameplayAbility;
class UAnimMontage;
class UAbilityTask_PlayMontageAndWait;
#pragma endregion

//@열거형
#pragma region Enums
/*
*	@EAbilityActivationPolicy
*
*	활성화 정책
*/
UENUM(BlueprintType)
enum class EAbilityActivationPolicy : uint8
{
	/*
	* @목적: Active GA 전용 활성화 정책
	* @설명: Active GA는 사용자 입력이 Trigger 매개이며, 사용자 입력이 Pressed 상태일 때 활성화 됩니다.
	*/
	OnInputTriggered,

	/*
	* @목적: Acitve GA 전용 활성화 정책
	* @설명: Active GA는 사용자 입력이 Trigger 매개이며, 사용자 입력이 Hold 상태일 때 활성화 됩니다.
	*/
	WhileInputActive,
	/*
	* @목적: Passive GA 전용 활성화 정책
	* @설명: Passive GA 중 특정 조건 없이 등록 시 한 번 활성화 될 경우 해당 활성화 정책을 선택하세요.
	*/
	OnGranted_Instant,

	/*
	* @목적: Passive GA 전용 활성화 정책
	* @설명: Passive GA 중 특정 조건이 만족되면 주기적으로 "GE"적용 수행할 경우 해당 활성화 정책을 선택하세요.
	*/
	OnGranted_ConditionalPeriodic,

	MAX
};

/*
*	@EChainActionMode
* 
*	체인 액션 실행 모드를 정의합니다.
*/
UENUM(BlueprintType)
enum class EChainActionMode : uint8
{
	//@ ANS가 종료될 때 체인 액션 실행
	DelayedActivation UMETA(DisplayName = "Delayed Activation"),

	//@입력이 들어오면 즉시 체인 액션 실행
	ImmediateActivation UMETA(DisplayName = "Immediate Activation")
};

/*
*	@EChainSystemType
*
*	체인 시스템의 유형을 정의합니다.
*/
UENUM(BlueprintType)
enum class EChainSystemType : uint8
{
	Active      UMETA(DisplayName = "Active Chain"),    // 입력을 통한 능동적 체인
	Passive     UMETA(DisplayName = "Passive Chain"),   // 특정 조건 만족 시 자동 체인
	MAX         UMETA(DisplayName = "MAX")
};
#pragma endregion

//@구조체
#pragma region Structs
/*
*	@FDamageInformation
* 
*	데미지 정보를 담은 구조체
*/
USTRUCT(BlueprintType)
struct FDamageInformation : public FGameplayEventData
{
	GENERATED_BODY()

public:
	//@Outgoing GE Spec
	UPROPERTY(BlueprintReadWrite)
		FGameplayEffectSpecHandle OutgoingGESpecHandle;
	//@히트 위치
	UPROPERTY(BlueprintReadWrite)
		FVector ImpactLocation;
};

/*
*	@FChainActionMapping
* 
*	Chain Action 활성화를 위해 필요한 Ability Tag 정보와 Event Tag 정보
*/
USTRUCT(BlueprintType)
struct FChainActionMapping
{
	GENERATED_BODY()

public:
	//@체인 액션 실행 모드
	UPROPERTY(EditDefaultsOnly)
		EChainActionMode ChainActionMode;

	//@체인 시스템 활성화 중 활성화 요건이 될 어빌리티 태그
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag AbilityTag;

	//@체인 액션 성공 시 호출할 이벤트 태그
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag EventTag;

public:
	bool Find (const FGameplayTag& Tag) const
	{
		return Tag.MatchesTagExact(AbilityTag);
	}
};

/*
*	@FChainEventMapping
*
*	Chain Event 활성화를 위한 Event Tag 정보
*/
USTRUCT(BlueprintType)
struct FChainEventMapping
{
	GENERATED_BODY()

public:
	//@체인 액션 실행 모드
	UPROPERTY(EditDefaultsOnly)
		EChainActionMode ChainActionMode;

	//@체인 시스템 활성화 중 활성화 요건이 될 이벤트 태그
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag RequiredEventTag;

	//@체인 시스템 조건 만족 시 호출할 이벤트 태그
	UPROPERTY(EditDefaultsOnly)
		FGameplayTag EventTagToSend;

public:
	bool Find(const FGameplayTag& Tag) const
	{
		return Tag.MatchesTagExact(RequiredEventTag);
	}
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@UBaseGameplayAbility
 * 
 *	Gameplay Ability의 기본 유형을 정의합니다.
 */
UCLASS()
class AGEOFWOLVES_API UBaseGameplayAbility : public UGameplayAbility
{

//@친추 클래스
#pragma region Friend Class
	friend class UBaseAbilitySystemComponent;
	friend class UPlayerAbilitySystemComponent;
	friend class UBaseMonsterASC;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UBaseGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	//~UGameplayAbility interface, ASC와 상호작용
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData = nullptr) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End of Interface
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//~UGameplayAbility interface, Overloading
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	//~End of Interface

protected:
	bool CheckGameplayEffectApplicationRequirements(const UGameplayEffect* GameplayEffect,
		const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const;

protected:
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	// Input Pressed 이벤트를 Blueprint에서 구현할 수 있도록 함
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
		void K2_InputPressed();
	virtual void K2_InputPressed_Implementation() { }

	// Input Released 이벤트를 Blueprint에서 구현할 수 있도록 함
	UFUNCTION(BlueprintNativeEvent, Category = "Ability")
		void K2_InputReleased();
	virtual void K2_InputReleased_Implementation() { }

protected:
	UFUNCTION(BlueprintCallable, Category = "Ability|Montage")
		UAbilityTask_PlayMontageAndWait* PlayMontageWithCallback(
			UAnimMontage* MontageToPlay,
			float Rate = 1.0f,
			FName StartSection = NAME_None,
			bool bStopWhenAbilityEnds = true);

protected:
	//@Gameplay Ability의 발동 조건입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "어빌리티 | 어빌리티 활성화 설정")
		EAbilityActivationPolicy ActivationPolicy;

protected:
	//@애님 몽타주 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "어빌리티 | 애니메이션")
		TArray<UAnimMontage*> AnimMontages;

	//@애님 몽타주 재생 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "어빌리티 | 애니메이션", meta = (EditCondition = "AnimMontages.Num() > 0"))
		float MontagePlayRate;

protected:
	//@해당 Gameplay Ability의 활성화 과정에서 Target(GA의 적용 대상)에게 전달하는 Gameplay Effect입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "어빌리티 | Gameplay Effect")
		TSubclassOf<UGameplayEffect> ApplyGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "어빌리티 | Gameplay Effect")
		FActiveGameplayEffectHandle ActiveApplyGameplayEffectHandle;

	//@해당 Gameplay Ability의 활성화 과정에서 Target(GA의 적용 대상)에게 전달하는 Sub Gameplay Effect입니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "어빌리티 | Gameplay Effect")
		TSubclassOf<UGameplayEffect> ApplySubGameplayEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "어빌리티 | Gameplay Effect")
		FActiveGameplayEffectHandle ApplySubGameplayEffectHandle;

protected:
	//@체인 시스템 활용 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "어빌리티 | 체인 시스템")
		bool bUseChainSystem;
	
	//@체인 시스템 타입
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "어빌리티 | 체인 시스템", meta = (EditCondition = "bUseChainSystem == true"))
		EChainSystemType ChainSystemType;

	//@체인 액션 가능한 어빌리티 태그와 이에 대응되는 이벤트 태그 목록
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 체인 시스템", meta = (EditCondition = "bUseChainSystem == true && ChainSystemType == EChainSystemType::Active"))
		TArray<FChainActionMapping> ChainActionMappings;

	//@체인 이벤트 매핑 목록
	UPROPERTY(EditDefaultsOnly, Category = "어빌리티 | 체인 시스템", meta = (EditCondition = "bUseChainSystem == true && ChainSystemType == EChainSystemType::Passive"))
		TArray<FChainEventMapping> ChainEventMappings;

	//@Chain Action 실행으로인한 취소 여부
	bool bIsCanceledByChainAction;
#pragma endregion

	//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
public:
	UFUNCTION(BlueprintNativeEvent, category = "체인 시스템")
		void OnChainActionActivated(FGameplayTag ChainActionEventTag);
	virtual void OnChainActionActivated_Implementation(FGameplayTag ChainActionEventTag);

	UFUNCTION(BlueprintNativeEvent, category = "체인 시스템")
		void OnChainActionFinished(FGameplayTag ChainActionEventTag);
	virtual void OnChainActionFinished_Implementation(FGameplayTag ChainActionEventTag);

protected:
	//@몽타주 재생 완료
	UFUNCTION(BlueprintNativeEvent, Category = "Ability|Montage")
		void OnMontageCompleted();
	virtual void OnMontageCompleted_Implementation();

	//@몽타주 블렌드 아웃 시작 시 호출
	UFUNCTION(BlueprintNativeEvent, Category = "Ability|Montage")
		void OnMontageBlendOut();
	virtual void OnMontageBlendOut_Implementation();

	//@다른 몽타주 재생에 의해 종료 됨
	UFUNCTION(BlueprintNativeEvent, Category = "Ability|Montage")
		void OnMontageInterrupted();
	virtual void OnMontageInterrupted_Implementation();

	//@외부 요인으로 몽타주 재생 취소 됨
	UFUNCTION(BlueprintNativeEvent, Category = "Ability|Montage")
		void OnMontageCancelled();
	virtual void OnMontageCancelled_Implementation();
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 태그")
		FGameplayTag GetAbilityTag() const;

public:
	FORCEINLINE EAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }

public:
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 애니메이션", meta = (DisplayName = "Get Anim Montages"))
		FORCEINLINE TArray<UAnimMontage*>GetAnimMontages() const { return AnimMontages; }

	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 애니메이션", meta = (DisplayName = "Get Anim Montage"))
		FORCEINLINE UAnimMontage* GetAnimMontage(int Index) const { return AnimMontages.IsValidIndex(Index) ? AnimMontages[Index] : nullptr; }

public:
	FORCEINLINE TSubclassOf<UGameplayEffect> GetApplyGameplayEffectClass() { return ApplyGameplayEffectClass; }
	FORCEINLINE FGameplayTagContainer GetRequiredTags() const { return ActivationRequiredTags; }

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay Event", meta = (DisplayName = "Convert Event Data to Damage Info"))
		static FDamageInformation ConvertEventDataToDamageInfo(const FGameplayEventData& EventData)
	{
		FDamageInformation DamageInfo;

		// FGameplayEventData의 기본 멤버들 복사
		DamageInfo.EventTag = EventData.EventTag;
		DamageInfo.Instigator = EventData.Instigator;
		DamageInfo.Target = EventData.Target;
		DamageInfo.OptionalObject = EventData.OptionalObject;
		DamageInfo.OptionalObject2 = EventData.OptionalObject2;
		DamageInfo.EventMagnitude = EventData.EventMagnitude;
		DamageInfo.ContextHandle = EventData.ContextHandle;
		DamageInfo.InstigatorTags = EventData.InstigatorTags;
		DamageInfo.TargetTags = EventData.TargetTags;

		// 추가 멤버들
		if (const FDamageInformation* SourceDamageInfo = static_cast<const FDamageInformation*>(&EventData))
		{
			DamageInfo.OutgoingGESpecHandle = SourceDamageInfo->OutgoingGESpecHandle;
			DamageInfo.ImpactLocation = SourceDamageInfo->ImpactLocation;
		}

		return DamageInfo;
	}

public:
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 체인 시스템")
	EChainSystemType GetChainSystemType() const { return ChainSystemType;}

	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 체인 시스템")
		EChainActionMode GetChainActionMode(const FGameplayTag& AbilityTag) const
	{
		for (const auto& Mapping : ChainActionMappings)
		{
			if (Mapping.AbilityTag == AbilityTag)
			{
				return Mapping.ChainActionMode;
			}
		}
		return EChainActionMode::DelayedActivation;
	}

	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 체인 시스템")
		TArray<FChainActionMapping> GetChainActionMappings() const;

	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 체인 시스템")
		FChainActionMapping GetChainActionMapping(const FGameplayTag& AbilityTag) const;

	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 체인 시스템")
		TArray<FChainEventMapping> GetChainEventMappings() const { return ChainEventMappings; }

	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 체인 시스템")
		FChainEventMapping GetChainEventMapping(const FGameplayTag& EventTag) const
	{
		for (const auto& Mapping : ChainEventMappings)
		{
			if (Mapping.RequiredEventTag == EventTag)
			{
				return Mapping;
			}
		}
		return FChainEventMapping();
	}
#pragma endregion

};