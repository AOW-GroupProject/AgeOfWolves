#include "18_Structure/StructureBase.h"


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

	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetupAttachment(RootComponent);
	NiagaraComp->SetAutoActivate(false); //@ 시작 시 자동 재생 방지

}

void AStructureBase::BeginPlay()
{
	Super::BeginPlay();

	if (NiagaraSystemAsset)
	{
		NiagaraComp->SetAsset(NiagaraSystemAsset);
	}
}
#pragma endregion
//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc

// void AStructureBase::PerformInteraction()
// {
// 	// //이펙트및 효과 노출
// 	// //..
// 	//
// 	//
// 	// //상호작용 콜백 호출
// 	// OnStructureInteractionTriggered.Broadcast(this);
// }

void AStructureBase::PerformInteraction_Implementation()
{
	//이펙트및 효과 노출
	//..

	
	//상호작용 콜백 호출
	OnStructureInteractionTriggered.Broadcast(this);
}

void AStructureBase::SetMeshCollision_Implementation(UMeshComponent* MeshComp,
	ECollisionEnabled::Type NewCollision)
{
	IInteractionInterface::SetMeshCollision_Implementation(MeshComp, NewCollision);

		if (!MeshComp) return;
	
		MeshComp->SetCollisionEnabled(NewCollision);
	    
		if (NewCollision == ECollisionEnabled::NoCollision)
		{
			MeshComp->SetCollisionProfileName(TEXT("NoCollision"));
		}
		else if (NewCollision == ECollisionEnabled::QueryAndPhysics)
		{
			MeshComp->SetCollisionProfileName(TEXT("BlockAll"));
		}
}

FGameplayTag AStructureBase::GetObjectTag() const
{
	return StructureTag;
}


#pragma endregion
//@Callbacks
#pragma region Callbacks

#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
void AStructureBase::SetStructureActive(bool IsActive)
{
	if (NiagaraComp != nullptr)
	{
		if (IsActive)
		{
			NiagaraComp->SetHiddenInGame(false);
			NiagaraComp->Activate(true); 
		}
		else
		{
			NiagaraComp->Deactivate();
			NiagaraComp->SetHiddenInGame(true);
		}
	}
}
#pragma endregion
