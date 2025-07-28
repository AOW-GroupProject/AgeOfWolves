// Fill out your copyright notice in the Description page of Project Settings.


#include "18_Structure/StructureSavePoint.h"

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
