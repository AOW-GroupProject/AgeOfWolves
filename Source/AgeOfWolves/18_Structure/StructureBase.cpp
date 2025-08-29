#include "18_Structure/StructureBase.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "Components/BoxComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogStructure, Log, All);
//@Defualt Setting
#pragma region Default Setting
AStructureBase::AStructureBase()
{

	USceneComponent* RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = RootSceneComponent;
	
	StructureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StructureMeshComponent"));
	StructureMesh->SetVisibility(true);
	StructureMesh->SetupAttachment(RootComponent);

}

void AStructureBase::BeginPlay()
{
	Super::BeginPlay();

	// //@콜리전 이벤트 바인딩
	// StructureBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AStructureBase::OnOverlapBegin);
	// StructureBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AStructureBase::OnOverlapEnd);
}
#pragma endregion
//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc

void AStructureBase::PerformInteraction()
{
	//이펙트및 효과 노출
	//..

	
	//상호작용 콜백 호출
	OnStructureInteractionTriggered.Broadcast(this);
}

#pragma endregion
//@Callbacks
#pragma region Callbacks


void AStructureBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// // 액터 유효성 검사
	// if (!IsValid(OtherActor))
	// {
	// 	return;
	// 	UE_LOGFMT(LogStructure, Warning, "구조물 {0} OnOverlapBegin 플레이어 감지됨 , OtherActor : {1}", *this->GetName(), *OtherActor->GetName());
	//
	// 	// if (OnStructureInteractionBegin.IsBound())
	// 	// {
	// }
	// UE_LOGFMT(LogStructure, Warning, "구조물 {0} OnOverlapBegin 발동 , OtherActor : {1}", *this->GetName(), *OtherActor->GetName());
	//
	// //@플레이어 검사
	// APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	// if (Player)
	// {
	// 	//@플레이어 확인, 감지됨
	// 	// 	OnStructureInteractionBegin.Execute(this);
	// 	// }
	// }


}

void AStructureBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// // 액터 유효성 검사
	// if (!IsValid(OtherActor))
	// {
	// 	return;
	// }
	//
	// //@플레이어 검사
	// APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	// if (Player)
	// {
	// 	//@플레이어 확인, 감지됨
	//
	// 	// if (OnStructureInteractionEnd.IsBound())
	// 	// {
	// 	// 	OnStructureInteractionEnd.Execute(this);
	// 	// }
	// }
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
void AStructureBase::SetStructureActive(bool IsActive)
{
}
#pragma endregion
