// Fill out your copyright notice in the Description page of Project Settings.


#include "LockOnComponent.h"

#include "01_Character/PlayerCharacter.h"
#include "04_Component/BaseInputComponent.h"
#include "05_Animation/BaseAnimInstance.h"


#include "InputActionValue.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ULockOnComponent::ULockOnComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	check(PlayerCharacter);
	SpringArmComponent = PlayerCharacter->GetSpringArmComponent();
	check(SpringArmComponent);
	FollowCameraComponent = PlayerCharacter->GetCameraComponent();
	check(FollowCameraComponent);
	BaseAnimInstance = Cast<UBaseAnimInstance>(PlayerCharacter->GetMesh()->GetAnimInstance());
	check(BaseAnimInstance);
	BaseInputComponent = Cast<UBaseInputComponent>(GetOwner()->InputComponent);
	check(BaseInputComponent);
}


// Called every frame
void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AdjustCameraTransform(DeltaTime);
}

void ULockOnComponent::Input_LockOn()
{
	if (bLockOn == true) // LockOn ���� ��, ���콺 �� �Է½� LockOn �����Ѵ�.
	{
		CancelLockOn();
	}
	else // LockOn ���� �ƴ� ��, ���콺 �� �Է½� LockOn �����Ѵ�.
	{
		StartLockOn();
	}

}

void ULockOnComponent::StartLockOn()
{

	// TargetEnemy�� ã��, ����������� �ʱ�ȭ �Ѵ�.
	if (FindTargetEnemy() == true)
	{
		bLockOn = true;
		USpringArmComponent* SpringArm = GetOwner()->FindComponentByClass<USpringArmComponent>();
		// To do : ���� ���� �ҷ�����
		SpringArmComponent->CameraLagSpeed = 5;
		SpringArmComponent->CameraRotationLagSpeed = 17.5;
		BaseAnimInstance->SetbLockOn(true);
		SetControllerRotationTowardTarget();
	}
}

void ULockOnComponent::CancelLockOn()
{

	// To do : ���� ���� �ҷ�����
	SpringArmComponent->CameraLagSpeed = 10;
	SpringArmComponent->CameraRotationLagSpeed = 30;
	// BaseAnimInstance  ��� ���� �ʱ�ȭ
	BaseAnimInstance->SetbLockOn(false);

	// Component ��� ���� �ʱ�ȭ
	InputVector = FVector2D(0, 0);
	NearByEnemies.Empty();
	EnemyMap.Empty();
	TargetEnemy = nullptr;
	bLockOn = false;
}

void ULockOnComponent::Input_ChangeLockOnTarget(const FInputActionValue& Value)
{
	if (NearByEnemies.Num() == 0) return;
	FVector2D ValueVector = Value.Get<FVector2D>();
	int TargetIndex = NearByEnemies.IndexOfByKey(TargetEnemy);
	if (ValueVector.X > 0) // ���콺 �� �� �� �Է�
	{
		TargetIndex = FMath::Clamp(TargetIndex + 1, 0, NearByEnemies.Num() - 1);
		TargetEnemy = NearByEnemies[TargetIndex];
	}
	else // ���콺 �� �� �Ʒ� �Է�
	{
		TargetIndex = FMath::Clamp(TargetIndex - 1, 0, NearByEnemies.Num() - 1);
		TargetEnemy = NearByEnemies[TargetIndex];
	}
	SetControllerRotationTowardTarget();

}

void ULockOnComponent::SetControllerRotationTowardTarget()
{
	FVector Start = PlayerCharacter->GetActorLocation();
	FVector Target = TargetEnemy->GetActorLocation();
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	//PlayerCharacter->SetActorRotation(FRotator(0.f, Rotation.Yaw, 0.f));
	PlayerCharacter->GetController()->SetControlRotation(FRotator(0.f, Rotation.Yaw, 0.f));
	DrawDebugSphere(GetWorld(), TargetEnemy->GetActorLocation(), 30, 12, FColor::Red, false, 1.5f);

}

bool ULockOnComponent::FindTargetEnemy()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> NearByActors;
	TEnumAsByte<EObjectTypeQuery> PawnObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	NearByActors.Add(PawnObjectType);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerCharacter);

	// PlayerCharacter�� ������ MaxDetectRadius�� ���������� �ϴ� ���� Ʈ���̽��� ������.
	TArray<FHitResult> HitResults;
	bool SphereTraceHitResult = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(),
		PlayerCharacter->GetActorLocation(),
		PlayerCharacter->GetActorLocation() + 100.f,
		MaxDetectRadius,
		NearByActors,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		HitResults, true
	);
	if (SphereTraceHitResult == true)
	{
		// LineTraceSingle�� ����, LockOn�� �������� �Ǵ�
		FHitResult LineHitResults;
		for (const auto& Hit : HitResults)
		{
			bool LineTraceHitResult = UKismetSystemLibrary::LineTraceSingleForObjects(
				GetWorld(),
				PlayerCharacter->GetActorLocation(),
				Hit.GetActor()->GetActorLocation(),  // + 100 * (Hit.GetActor()->GetActorLocation() - PlayerCharacter->GetActorLocation()).Normalize(),
				NearByActors,
				false,
				IgnoreActors,
				EDrawDebugTrace::None,
				LineHitResults, true
			);
			if (Hit.GetActor() == LineHitResults.GetActor())
			{
				NearByEnemies.AddUnique(Hit.GetActor());
			}
		}
		float Min = 1000000.f;
		// ����/������ ���� �Ÿ��� ������ ���� ���� ����� �̸� key, Enemy�� Value�� Map�� ������.
		for (int i = 0; i < NearByEnemies.Num(); i++)
		{
			// Option 1 : Player Character�� Forward Vector �������� LockOn
			// Option 2 : Camera�� ���̴� Enemy�� (Camera�� Forward Vector ��������) LockOn <- ���� ���õ�

			// FVector PlayerForwardVector = PlayerCharacter->GetActorForwardVector();
			// FVector CameraToPlayer = PlayerCharacter->GetActorLocation() - PlayerCameraLocation;

			FVector PlayerCameraLocation = FollowCameraComponent->GetComponentTransform().GetTranslation();
			FVector CameraToPlayer = FollowCameraComponent->GetForwardVector();

			FVector CameraToEnemy = NearByEnemies[i]->GetActorLocation() - PlayerCameraLocation;
			FVector PlayerToEnemy = NearByEnemies[i]->GetActorLocation() - PlayerCharacter->GetActorLocation();

			FVector CrossProduct = FVector::CrossProduct(CameraToPlayer, CameraToEnemy);
			// FVector CrossProduct = FVector::CrossProduct(PlayerForwardVector, PlayerToEnemy);
			float UpDotProduct = FVector::DotProduct(CameraToPlayer, CrossProduct);
			// float UpDotProduct = FVector::DotProduct(PlayerForwardVector, CrossProduct);

			float TempDotProductResult = FVector::DotProduct(CameraToPlayer, CameraToEnemy);

			// FOV �ȿ� �ִ��� ����Ѵ�.
			float Cos = TempDotProductResult / (CameraToPlayer.Length() * CameraToEnemy.Length());
			float HalfFOV = FMath::Cos(FMath::DegreesToRadians(FollowCameraComponent->FieldOfView / 1.5));
			if (Cos > HalfFOV)
			{
				EnemyMap.Add(UpDotProduct, NearByEnemies[i]);
				// DrawDebugSphere(GetWorld(), NearByEnemies[i]->GetActorLocation(), 25.f, 12, FColor::Blue, false, 3.f);
				// ���� ����� ���� ã�� ���� min�� ���.
				if (FMath::Abs(Min) > FMath::Abs(UpDotProduct))
				{
					Min = UpDotProduct;
				}
			}
		}
		if (EnemyMap.IsEmpty())
		{
			return false;
		}

		// Target Enemy ��ȯ�� ���� EnemyMap�� ������.
		TArray<float> DotProducts;
		EnemyMap.GenerateKeyArray(DotProducts);
		DotProducts.Sort();
		NearByEnemies.Empty();
		for (int i = 0; i < DotProducts.Num(); i++)
		{
			NearByEnemies.Add(*EnemyMap.Find(DotProducts[i]));
		}
		// ���� ����� ���� Target Enemy�� ������.
		TargetEnemy = *EnemyMap.Find(Min);
		if (IsValid(TargetEnemy)) return true;
		else return false;
	}
	else
	{
		return false;
	}

}

void ULockOnComponent::AdjustCameraTransform(float DeltaTime)
{
	if (bLockOn == true)
	{
		SpringArmComponent->bUsePawnControlRotation = false;
	
		// MaxLockOnDistance ���� ������ true
		bool bCloseToEnemy = (PlayerCharacter->GetActorLocation() - TargetEnemy->GetActorLocation()).Length() < MaxLockOnDistance;
		// �Ÿ��� �����ų� TargetEnemy�� �����ϴ� ��� true
		if (IsValid(TargetEnemy) && bCloseToEnemy)
		{
			APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
			FVector CameraStart = CameraManager->GetCameraLocation();
			FVector CharacterStart = PlayerCharacter->GetActorLocation();
			FVector TargetPosition = TargetEnemy->GetActorLocation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CharacterStart, TargetPosition);

			float SocketOffsetCoefficient = 1.0f;

			// ��Ʈ�ѷ� ȸ���� ���� ���� ����
			FRotator TargetRotation = UKismetMathLibrary::RInterpTo(PlayerCharacter->GetController()->GetControlRotation(), LookAtRotation, DeltaTime, 10.f);
			// ��� ���� ���� ��ü�� ȸ�� ������ ����
			PlayerCharacter->GetController()->SetControlRotation(TargetRotation);

			// GetController()->SetControlRotation(LookAtRotation);

			// �޸��� �ʴ� ���ȿ��� Character�� Rotation�� LockOn�������� �����Ѵ�.
			// �޸��� ������ ����Ű �������� �����ȴ�.
			if (!(BaseAnimInstance->GetMovementState() == EMovementState::Run))
			{
				FRotator LooAtRotatoionYaw = FRotator(0.f, LookAtRotation.Yaw, 0.f);
				// ���� ����
				FRotator TargetActorRotation = UKismetMathLibrary::RInterpTo(PlayerCharacter->GetActorRotation(), LooAtRotatoionYaw, DeltaTime, 40.f);
				PlayerCharacter->SetActorRotation(TargetActorRotation);
				SocketOffsetCoefficient = 1.5f;
			}

			// ������, �������� �̵��ϴ� ��� ���������� Y�������� ����
			if (BaseInputComponent->GetInputVector().Y > 0) // ���������� �̵�
			{
				if (SpringArmComponent->SocketOffset.Y > -50)
				{
					SpringArmComponent->SocketOffset.Y -= (SocketOffsetCoefficient * BaseInputComponent->GetInputVector().Y);
				}
			}
			else if (BaseInputComponent->GetInputVector().Y < 0) // �������� �̵�
			{
				if (SpringArmComponent->SocketOffset.Y < 50)
				{
					SpringArmComponent->SocketOffset.Y -= (SocketOffsetCoefficient * BaseInputComponent->GetInputVector().Y);
				}
			}
			// TargetEnemy�� �Ÿ��� ���� ī�޶� ���� �̵� ��Ŵ
			float DistanceFromTargetEnemy = (PlayerCharacter->GetActorLocation() - TargetPosition).Length();
			DistanceFromTargetEnemy = FMath::Clamp((6000 / DistanceFromTargetEnemy) + 20, 0, 30);
			FRotator DistanceRotation = FRotator(-DistanceFromTargetEnemy, 0, 0);

			FRotator FinalRotation = DistanceRotation + LookAtRotation;
			FRotator SpringArmRotator = UKismetMathLibrary::RInterpTo(LookAtRotation, FinalRotation, DeltaTime, 5.f);

			SpringArmComponent->SocketOffset.X = FMath::Lerp(0, -200, DistanceFromTargetEnemy / 70);
			SpringArmComponent->SetWorldRotation(FinalRotation);
		}
		else if (!bCloseToEnemy)// TargetEnemy�� �ʹ� �־������ LockOn�� ����Ѵ�.
		{
			TargetEnemy = nullptr;
			SpringArmComponent->bUsePawnControlRotation = true;
			SpringArmComponent->SocketOffset.Y = 0;
			SpringArmComponent->SocketOffset.X = 0;
			CancelLockOn();
		}
		else if (!IsValid(TargetEnemy)) // Target�� �׾� ��ȿ���� �ʴ� ��� LockOn�� ����ϰ�, ���� Target�� ã�´�.
		{
			TargetEnemy = nullptr;
			SpringArmComponent->bUsePawnControlRotation = true;
			SpringArmComponent->SocketOffset.Y = 0;
			SpringArmComponent->SocketOffset.X = 0;
			CancelLockOn();
			StartLockOn();
		}
	}
	else
	{
		SpringArmComponent->bUsePawnControlRotation = true;
		SpringArmComponent->SocketOffset.Y = 0;
		SpringArmComponent->SocketOffset.X = 0;
		CancelLockOn();
	}


}


