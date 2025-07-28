// Fill out your copyright notice in the Description page of Project Settings.


#include "18_Structure/StructureBase.h"
#include "Logging/StructuredLog.h"

#include "01_Character/PlayerCharacter.h"
#include "Components/BoxComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogStructure, Log, All);


// Sets default values
AStructureBase::AStructureBase()
{

	StructureMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StructureMeshComponent"));
	StructureMesh->SetVisibility(true);
	RootComponent = StructureMesh;

	//@Box Component
	StructureBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	StructureBoxComponent->SetupAttachment(RootComponent);

	StructureBoxComponent->SetCollisionProfileName(TEXT("OverlapAll"));
	StructureBoxComponent->SetCollisionResponseToAllChannels(ECR_Overlap);
	StructureBoxComponent->SetBoxExtent(FVector(500.0f, 500.0f, 500.0f));
}

// Called when the game starts or when spawned
void AStructureBase::BeginPlay()
{
	Super::BeginPlay();

	//@콜리전 이벤트 바인딩
	StructureBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AStructureBase::OnOverlapBegin);
	StructureBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AStructureBase::OnOverlapEnd);
}

void AStructureBase::SetStructureActive(bool IsActive)
{
}

void AStructureBase::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 액터 유효성 검사
	if (!IsValid(OtherActor))
	{
		return;
	}
	UE_LOGFMT(LogStructure, Warning, "구조물 {0} OnOverlapBegin 발동 , OtherActor : {1}", *this->GetName(), *OtherActor->GetName());

	//@플레이어 검사
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		//@플레이어 확인, 감지됨
		UE_LOGFMT(LogStructure, Warning, "구조물 {0} OnOverlapBegin 플레이어 감지됨 , OtherActor : {1}", *this->GetName(), *OtherActor->GetName());

		if (OnStructureInteractionBegin.IsBound())
		{
			OnStructureInteractionBegin.Execute(this);
		}
	}


}

void AStructureBase::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// 액터 유효성 검사
	if (!IsValid(OtherActor))
	{
		return;
	}

	//@플레이어 검사
	APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor);
	if (Player)
	{
		//@플레이어 확인, 감지됨

		if (OnStructureInteractionEnd.IsBound())
		{
			OnStructureInteractionEnd.Execute(this);
		}
	}
}


