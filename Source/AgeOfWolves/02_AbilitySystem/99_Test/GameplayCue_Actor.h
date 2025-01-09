// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Looping.h"
#include "GameplayTagContainer.h"
#include "GameplayCue_Actor.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;

/**
 * 
 */
UCLASS()
class AGEOFWOLVES_API AGameplayCue_Actor : public AGameplayCueNotify_Looping
{
	GENERATED_BODY()
	
public:
    // 기본 생성자
    AGameplayCue_Actor();

    void AddApplicationEffects();
    

public:
    void SpawnBurstEffects();
    void StopBurstEffects();

protected:

    FName AttachSocketName;

    // Niagara 시스템 템플릿 변수
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Niagara", meta = (AllowPrivateAccess = "true"))
    class UNiagaraSystem* NiagaraEffectTemplate;


    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Niagara", meta = (AllowPrivateAccess = "true"))
    class UNiagaraComponent* NiagaraComponent;



public:
    FORCEINLINE void SetAttachSocketName(FName InAttachSocketName) { AttachSocketName = InAttachSocketName; }
    FORCEINLINE void SetNiagaraEffectTemplate(UNiagaraSystem* InNiagaraEffectTemplate) 
    { 
        if (NiagaraEffectTemplate == InNiagaraEffectTemplate)
        {
            return;
        }

        NiagaraEffectTemplate = InNiagaraEffectTemplate; 
    }


    
};
