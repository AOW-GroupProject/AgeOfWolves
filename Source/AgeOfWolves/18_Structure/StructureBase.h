#pragma once

#include "CoreMinimal.h"
#include "16_Level/Area.h"
#include "19_Interface/InteractionInterface.h"
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

	//@영역 경계 박스
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* StructureBoxComponent;

public:
	UPROPERTY(EditAnywhere, Category = "Structure | Data")
	FStructureData StructureData;

public:
	UPROPERTY(EditAnywhere, Category = "구조물 태그")
	FGameplayTag StructureTag;



public:
	virtual FGameplayTag GetObjectTag() override
	{
		return StructureTag;
	}
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	FOnInteractionTriggered OnStructureInteractionTriggered; //상호작용 발동시

	UFUNCTION(BlueprintCallable, Category="Interaction")
	void BP_OnStructureInteractionTriggered()
	{
		OnStructureInteractionTriggered.Broadcast(this);
	}
#pragma endregion

//@Callbacks
#pragma region Callbacks
	
protected:
	//@충돌 시작 이벤트를 구독하는 콜백
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
	//@충돌 종료 이벤트를 구독하는 콜백
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion

};
