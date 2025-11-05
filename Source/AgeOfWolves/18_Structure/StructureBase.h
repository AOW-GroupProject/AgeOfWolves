#pragma once

#include "CoreMinimal.h"
#include "16_Level/Area.h"
#include "19_Interface/InteractionInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "GameFramework/Actor.h"

#include "StructureBase.generated.h"

//@전방 선언
#pragma region Forward Declaration
class UBoxComponent;
class UStaticMeshComponent;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_DELEGATE_OneParam(FOnInteractionBegin, AStructureBase*);
DECLARE_DELEGATE_OneParam(FOnInteractionEnd, AStructureBase*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnInteractionTriggered, AStructureBase*);
#pragma endregion

/**
 *	@AStructureBase
 *
 *	레벨에 설치될 구조물 액터입니다.
 *	구조물 의 base 클래스입니다
 */
UCLASS()
class AGEOFWOLVES_API AStructureBase : public AActor, public IInteractionInterface
{


private:
	//@친추 클래스
#pragma region Friend Class
#pragma endregion
	
	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
	
public:	
	// Sets default values for this actor's properties
	AStructureBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	virtual void SetStructureActive(bool IsActive);
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
	UPROPERTY(VisibleAnywhere, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StructureMesh;

	// Niagara 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VFX")
	UNiagaraComponent* NiagaraComp;

	// 파티클 시스템
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="VFX")
	UNiagaraSystem* NiagaraSystemAsset;
	

public:
	UPROPERTY(EditAnywhere, Category = "구조물 태그")
	FGameplayTag StructureTag;



public:

	//@ 상호작용 실행
	virtual void PerformInteraction_Implementation()override;
	virtual void SetMeshCollision_Implementation(UMeshComponent* MeshComp, ECollisionEnabled::Type NewCollision) override;

	// UFUNCTION(BlueprintCallable, BlueprintPure, Category="Interaction")
	virtual FGameplayTag GetObjectTag() const override;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	FOnInteractionTriggered OnStructureInteractionTriggered; //상호작용 발동시
#pragma endregion

//@Callbacks
#pragma region Callbacks
	

#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

};
