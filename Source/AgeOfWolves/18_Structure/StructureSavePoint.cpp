#include "StructureSavePoint.h"

AStructureSavePoint::AStructureSavePoint()
{
	StructureSavePointActivatedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SavePointActivatedMesh"));
	StructureSavePointActivatedMesh->SetupAttachment(RootComponent);
	// StructureSavePointActivatedMesh->SetVisibility(false);
	
}

void AStructureSavePoint::SetStructureActive(bool IsActive)
{
	Super::SetStructureActive(IsActive);

	if (StructureSavePointActivatedMesh)
		StructureSavePointActivatedMesh->SetVisibility(IsActive);
}
