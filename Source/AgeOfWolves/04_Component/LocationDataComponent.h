#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "04_Component/InteractionComponent.h"

#include "LocationDataComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLocationData, Log, All)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API ULocationDataComponent : public USceneComponent
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	ULocationDataComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@'처형' 상호작용 시 상대 위치 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction Locations")
		USceneComponent* ExecutionLocation;

	//@'암살' 상호작용 시 상대 위치
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction Locations")
		USceneComponent* AmbushLocation;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
	//@상호작용 유형에 따른 위치 가져오기
	UFUNCTION(BlueprintCallable, Category = "Interaction Locations")
		FVector GetLocationForInteractionType(EInteractionType Type) const;

	//@상호작용 유형에 따른 회전 가져오기
	UFUNCTION(BlueprintCallable, Category = "Interaction Locations")
		FRotator GetRotationForInteractionType(EInteractionType Type) const;
#pragma endregion
};
