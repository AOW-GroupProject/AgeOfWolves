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
	if (bLockOn == true) // LockOn 중일 때, 마우스 휠 입력시 LockOn 종료한다.
	{
		CancelLockOn();
	}
	else // LockOn 중이 아닐 때, 마우스 휠 입력시 LockOn 시작한다.
	{
		StartLockOn();
	}

}

void ULockOnComponent::StartLockOn()
{

	// TargetEnemy를 찾고, 멤버변수들을 초기화 한다.
	if (FindTargetEnemy() == true)
	{
		bLockOn = true;
		USpringArmComponent* SpringArm = GetOwner()->FindComponentByClass<USpringArmComponent>();
		// To do : 기존 값을 불러오게
		SpringArmComponent->CameraLagSpeed = 5;
		SpringArmComponent->CameraRotationLagSpeed = 17.5;
		BaseAnimInstance->SetbLockOn(true);
		SetControllerRotationTowardTarget();
	}
}

void ULockOnComponent::CancelLockOn()
{

	// To do : 기존 값을 불러오게
	SpringArmComponent->CameraLagSpeed = 10;
	SpringArmComponent->CameraRotationLagSpeed = 30;
	// BaseAnimInstance  멤버 변수 초기화
	BaseAnimInstance->SetbLockOn(false);

	// Component 멤버 변수 초기화
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
	if (ValueVector.X > 0) // 마우스 휠 축 위 입력
	{
		TargetIndex = FMath::Clamp(TargetIndex + 1, 0, NearByEnemies.Num() - 1);
		TargetEnemy = NearByEnemies[TargetIndex];
	}
	else // 마우스 휠 축 아래 입력
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

	// PlayerCharacter를 무시한 MaxDetectRadius을 반지름으로 하는 원형 트레이스를 실행함.
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
		// LineTraceSingle을 통해, LockOn이 가능한지 판단
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
		// 외적/내적을 통해 거리와 각도에 따른 값을 계산해 이를 key, Enemy를 Value로 Map을 설정함.
		for (int i = 0; i < NearByEnemies.Num(); i++)
		{
			// Option 1 : Player Character의 Forward Vector 기준으로 LockOn
			// Option 2 : Camera에 보이는 Enemy를 (Camera의 Forward Vector 기준으로) LockOn <- 현재 선택됨

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

			// FOV 안에 있는지 계산한다.
			float Cos = TempDotProductResult / (CameraToPlayer.Length() * CameraToEnemy.Length());
			float HalfFOV = FMath::Cos(FMath::DegreesToRadians(FollowCameraComponent->FieldOfView / 1.5));
			if (Cos > HalfFOV)
			{
				EnemyMap.Add(UpDotProduct, NearByEnemies[i]);
				// DrawDebugSphere(GetWorld(), NearByEnemies[i]->GetActorLocation(), 25.f, 12, FColor::Blue, false, 3.f);
				// 가장 가까운 적을 찾기 위해 min을 계산.
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

		// Target Enemy 전환을 위해 EnemyMap을 정렬함.
		TArray<float> DotProducts;
		EnemyMap.GenerateKeyArray(DotProducts);
		DotProducts.Sort();
		NearByEnemies.Empty();
		for (int i = 0; i < DotProducts.Num(); i++)
		{
			NearByEnemies.Add(*EnemyMap.Find(DotProducts[i]));
		}
		// 가장 가까운 적을 Target Enemy로 설정함.
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
	
		// MaxLockOnDistance 보다 가까우면 true
		bool bCloseToEnemy = (PlayerCharacter->GetActorLocation() - TargetEnemy->GetActorLocation()).Length() < MaxLockOnDistance;
		// 거리가 가깝거나 TargetEnemy가 존재하는 경우 true
		if (IsValid(TargetEnemy) && bCloseToEnemy)
		{
			APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
			FVector CameraStart = CameraManager->GetCameraLocation();
			FVector CharacterStart = PlayerCharacter->GetActorLocation();
			FVector TargetPosition = TargetEnemy->GetActorLocation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CharacterStart, TargetPosition);

			float SocketOffsetCoefficient = 1.0f;

			// 컨트롤러 회전에 대한 선형 보간
			FRotator TargetRotation = UKismetMathLibrary::RInterpTo(PlayerCharacter->GetController()->GetControlRotation(), LookAtRotation, DeltaTime, 10.f);
			// 결과 값을 액터 객체의 회전 값으로 설정
			PlayerCharacter->GetController()->SetControlRotation(TargetRotation);

			// GetController()->SetControlRotation(LookAtRotation);

			// 달리지 않는 동안에만 Character의 Rotation을 LockOn방향으로 설정한다.
			// 달리는 동안은 방향키 방향으로 설정된다.
			if (!(BaseAnimInstance->GetMovementState() == EMovementState::Run))
			{
				FRotator LooAtRotatoionYaw = FRotator(0.f, LookAtRotation.Yaw, 0.f);
				// 선형 보간
				FRotator TargetActorRotation = UKismetMathLibrary::RInterpTo(PlayerCharacter->GetActorRotation(), LooAtRotatoionYaw, DeltaTime, 40.f);
				PlayerCharacter->SetActorRotation(TargetActorRotation);
				SocketOffsetCoefficient = 1.5f;
			}

			// 오른쪽, 왼쪽으로 이동하는 경우 스프링암의 Y오프셋을 조절
			if (BaseInputComponent->GetInputVector().Y > 0) // 오른쪽으로 이동
			{
				if (SpringArmComponent->SocketOffset.Y > -50)
				{
					SpringArmComponent->SocketOffset.Y -= (SocketOffsetCoefficient * BaseInputComponent->GetInputVector().Y);
				}
			}
			else if (BaseInputComponent->GetInputVector().Y < 0) // 왼쪽으로 이동
			{
				if (SpringArmComponent->SocketOffset.Y < 50)
				{
					SpringArmComponent->SocketOffset.Y -= (SocketOffsetCoefficient * BaseInputComponent->GetInputVector().Y);
				}
			}
			// TargetEnemy와 거리에 따라 카메라를 위로 이동 시킴
			float DistanceFromTargetEnemy = (PlayerCharacter->GetActorLocation() - TargetPosition).Length();
			DistanceFromTargetEnemy = FMath::Clamp((6000 / DistanceFromTargetEnemy) + 20, 0, 30);
			FRotator DistanceRotation = FRotator(-DistanceFromTargetEnemy, 0, 0);

			FRotator FinalRotation = DistanceRotation + LookAtRotation;
			FRotator SpringArmRotator = UKismetMathLibrary::RInterpTo(LookAtRotation, FinalRotation, DeltaTime, 5.f);

			SpringArmComponent->SocketOffset.X = FMath::Lerp(0, -200, DistanceFromTargetEnemy / 70);
			SpringArmComponent->SetWorldRotation(FinalRotation);
		}
		else if (!bCloseToEnemy)// TargetEnemy와 너무 멀어진경우 LockOn을 취소한다.
		{
			TargetEnemy = nullptr;
			SpringArmComponent->bUsePawnControlRotation = true;
			SpringArmComponent->SocketOffset.Y = 0;
			SpringArmComponent->SocketOffset.X = 0;
			CancelLockOn();
		}
		else if (!IsValid(TargetEnemy)) // Target이 죽어 유효하지 않는 경우 LockOn을 취소하고, 다음 Target을 찾는다.
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


