// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAbilitySet.h"
#include "Logging/StructuredLog.h"

#include "04_Component/BaseAbilitySystemComponent.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"
#include "02_AbilitySystem/02_GameplayAbility/BaseGameplayAbility.h"

DEFINE_LOG_CATEGORY(LogAbilitySet)


void FBaseAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FBaseAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FBaseAbilitySet_GrantedHandles::AddAttributeSet(UBaseAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}


//@Defualt Setting
#pragma region Default Setting
UBaseAbilitySet::UBaseAbilitySet(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UBaseAbilitySet::GiveStartupAttributeSetToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
    if (!ASC)
    {
        UE_LOGFMT(LogAbilitySet, Error, "AttributeSet 등록 실패 - ASC가 유효하지 않음");
        return;
    }

    UE_LOGFMT(LogAbilitySet, Log, "AttributeSet 등록 시작 - ASC Owner: {0}",
        ASC->GetOwner() ? ASC->GetOwner()->GetName() : TEXT("Invalid"));

    // Attribute Set
    for (int32 SetIndex = 0; SetIndex < AttributeSets.Num(); ++SetIndex)
    {
        const FBaseAbilitySet_AttributeSet& SetToGrant = AttributeSets[SetIndex];

        if (!IsValid(SetToGrant.AttributeSet))
        {
            UE_LOGFMT(LogAbilitySet, Error, "AttributeSet 등록 실패 - 인덱스: {0}, 사유: AttributeSet이 유효하지 않음", SetIndex);
            continue;
        }

        UBaseAttributeSet* NewSet = NewObject<UBaseAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet);
        if (!NewSet)
        {
            UE_LOGFMT(LogAbilitySet, Error, "AttributeSet 생성 실패 - 인덱스: {0}", SetIndex);
            continue;
        }

        ASC->AddAttributeSetSubobject(NewSet);
        UE_LOGFMT(LogAbilitySet, Log, "AttributeSet 등록 완료 - 타입: {0}", NewSet->GetClass()->GetName());

        if (!OutGrantedHandles)
        {
            UE_LOGFMT(LogAbilitySet, Warning, "OutGrantedHandles가 유효하지 않음 - AttributeSet Handle을 저장할 수 없음");
            continue;
        }

        OutGrantedHandles->AddAttributeSet(NewSet);
        UE_LOGFMT(LogAbilitySet, Log, "AttributeSet Handle 추가됨");
    }
}

void UBaseAbilitySet::GiveStartupGameplayEffectToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
    if (!ASC)
    {
        UE_LOGFMT(LogAbilitySet, Error, "GameplayEffect 등록 실패 - ASC가 유효하지 않음");
        return;
    }

    UE_LOGFMT(LogAbilitySet, Log, "GameplayEffect 등록 시작 - ASC Owner: {0}",
        ASC->GetOwner() ? ASC->GetOwner()->GetName() : TEXT("Invalid"));

    for (int32 EffectIndex = 0; EffectIndex < GameplayEffects.Num(); ++EffectIndex)
    {
        const FBaseAbilitySet_GameplayEffect& EffectToGrant = GameplayEffects[EffectIndex];

        if (!IsValid(EffectToGrant.GameplayEffect))
        {
            UE_LOGFMT(LogAbilitySet, Error, "GameplayEffect 등록 실패 - 인덱스: {0}, 사유: GameplayEffect가 유효하지 않음", EffectIndex);
            continue;
        }

        const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
        if (!GameplayEffect)
        {
            UE_LOGFMT(LogAbilitySet, Error, "GameplayEffect CDO 가져오기 실패 - 인덱스: {0}", EffectIndex);
            continue;
        }

        const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());
        if (!GameplayEffectHandle.WasSuccessfullyApplied())
        {
            UE_LOGFMT(LogAbilitySet, Warning, "GameplayEffect 적용 실패 - 타입: {0}", GameplayEffect->GetName());
            continue;
        }

        UE_LOGFMT(LogAbilitySet, Log, "GameplayEffect 적용 완료 - 타입: {0}, 레벨: {1}",
            GameplayEffect->GetName(), EffectToGrant.EffectLevel);

        if (!OutGrantedHandles)
        {
            UE_LOGFMT(LogAbilitySet, Warning, "OutGrantedHandles가 유효하지 않음 - GameplayEffect Handle을 저장할 수 없음");
            continue;
        }

        OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
        UE_LOGFMT(LogAbilitySet, Log, "GameplayEffect Handle 추가됨");
    }
}

void UBaseAbilitySet::GiveStartupGameplayAbilityToAbilitySystem(UBaseAbilitySystemComponent* ASC, FBaseAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
    if (!ASC)
    {
        UE_LOGFMT(LogAbilitySet, Error, "GameplayAbility 등록 실패 - ASC가 유효하지 않음");
        return;
    }

    UE_LOGFMT(LogAbilitySet, Log, "GameplayAbility 등록 시작 - ASC Owner: {0}",
        ASC->GetOwner() ? ASC->GetOwner()->GetName() : TEXT("Invalid"));

    for (int32 AbilityIndex = 0; AbilityIndex < GameplayAbilities.Num(); ++AbilityIndex)
    {
        const FBaseAbilitySet_GameplayAbility& AbilityToGrant = GameplayAbilities[AbilityIndex];

        if (!IsValid(AbilityToGrant.Ability))
        {
            UE_LOGFMT(LogAbilitySet, Error, "GameplayAbility 등록 실패 - 인덱스: {0}, 사유: Ability가 유효하지 않음", AbilityIndex);
            continue;
        }

        UBaseGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UBaseGameplayAbility>();
        if (!AbilityCDO)
        {
            UE_LOGFMT(LogAbilitySet, Error, "GameplayAbility CDO 가져오기 실패 - 인덱스: {0}", AbilityIndex);
            continue;
        }

        FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
        AbilitySpec.SourceObject = SourceObject;

        if (AbilityToGrant.bActive && AbilityToGrant.bInputBinded)
        {
            AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);
            UE_LOGFMT(LogAbilitySet, Log, "Input Tag 추가됨 - Ability: {0}, Tag: {1}",
                AbilityCDO->GetName(), AbilityToGrant.InputTag.ToString());
        }

        const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
        if (!AbilitySpecHandle.IsValid())
        {
            UE_LOGFMT(LogAbilitySet, Warning, "GameplayAbility 등록 실패 - 타입: {0}", AbilityCDO->GetName());
            continue;
        }

        UE_LOGFMT(LogAbilitySet, Log, "GameplayAbility 등록 완료 - 타입: {0}, 레벨: {1}",
            AbilityCDO->GetName(), AbilityToGrant.AbilityLevel);

        if (!OutGrantedHandles)
        {
            UE_LOGFMT(LogAbilitySet, Warning, "OutGrantedHandles가 유효하지 않음 - GameplayAbility Handle을 저장할 수 없음");
            continue;
        }

        OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
        UE_LOGFMT(LogAbilitySet, Log, "GameplayAbility Handle 추가됨");
    }
}

#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion