// Fill out your copyright notice in the Description page of Project Settings.


#include "04_Component/LockOnComponent.h"


#include "01_Character/PlayerCharacter.h"
#include "05_Animation/BaseAnimInstance.h"



#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
ULockOnComponent::ULockOnComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULockOnComponent::BeginPlay()
{
	Super::BeginPlay();
	SpringArm = GetOwner()->FindComponentByClass<USpringArmComponent>();

	// ...
	
}


// Called every frame
void ULockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AdjustCameraTransform(DeltaTime);

	// ...
}


/*



*/
void ULockOnComponent::AdjustCameraTransform(float DeltaTime)
{

	/*
	if (bLockOn == true)
	{
		SpringArm->bUsePawnControlRotation = false;
		AActor* TargetEnemy = BaseInputComponent->GetTargetEnemy();
		// MaxLockOnDistance 보다 가까우면 true
		bool bCloseToEnemy = (GetActorLocation() - TargetEnemy->GetActorLocation()).Length() < BaseInputComponent->GetMaxLockOnDistance();
		// 거리가 가깝거나 TargetEnemy가 존재하는 경우 true
		if (IsValid(TargetEnemy) && bCloseToEnemy)
		{
			APlayerCameraManager* CameraManager = GetWorld()->GetFirstPlayerController()->PlayerCameraManager;
			FVector CameraStart = CameraManager->GetCameraLocation();
			FVector CharacterStart = GetActorLocation();
			FVector TargetPosition = TargetEnemy->GetActorLocation();
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(CharacterStart, TargetPosition);

			float SocketOffsetCoefficient = 1.0f;

			GetController()->SetControlRotation(LookAtRotation);
			// 달리지 않는 동안에만 ActorRotation을 LockOn방향으로 설정
			if (!(BaseAnimInstance->GetMovementState() == EMovementState::Run))
			{
				FRotator ActorRotation = FRotator(0.f, LookAtRotation.Yaw, 0.f);
				SetActorRotation(ActorRotation);
				SocketOffsetCoefficient = 1.5f;
			}
			// 오른쪽, 왼쪽으로 이동하는 경우 스프링암의 Y오프셋을 조절
			if (BaseInputComponent->GetInputVector().Y > 0) // 오른쪽으로 이동
			{
				if (SpringArm->SocketOffset.Y > -50)
				{
					SpringArm->SocketOffset.Y -= (SocketOffsetCoefficient * BaseInputComponent->GetInputVector().Y);
				}
			}
			else if (BaseInputComponent->GetInputVector().Y < 0) // 왼쪽으로 이동
			{
				if (SpringArm->SocketOffset.Y < 50)
				{
					SpringArm->SocketOffset.Y -= (SocketOffsetCoefficient * BaseInputComponent->GetInputVector().Y);
				}
			}
			// TargetEnemy와 거리에 따라 카메라를 위로 이동 시킴
			float DistanceFromTargetEnemy = (GetActorLocation() - TargetPosition).Length();
			DistanceFromTargetEnemy = FMath::Clamp((6000 / DistanceFromTargetEnemy) + 20, 0, 70);
			FRotator DistanceRotation = FRotator(-DistanceFromTargetEnemy, 0, 0);

			FRotator FinalRotation = DistanceRotation + LookAtRotation;
			FRotator SpringArmRotator = UKismetMathLibrary::RInterpTo(LookAtRotation, FinalRotation, DeltaSeconds, 10.f);

			SpringArm->SocketOffset.X = FMath::Lerp(0, -200, DistanceFromTargetEnemy / 70);
			SpringArm->SetWorldRotation(FinalRotation);
		}
		else if (!bCloseToEnemy)// TargetEnemy와 너무 멀어진경우 LockOn을 취소한다.
		{
			TargetEnemy = nullptr;
			SpringArm->bUsePawnControlRotation = true;
			SpringArm->SocketOffset.Y = 0;
			BaseInputComponent->CancelLockOn();
		}
		else if (!IsValid(TargetEnemy)) // Target이 죽어 유효하지 않는 경우 LockOn을 취소하고, 다음 Target을 찾는다.
		{
			TargetEnemy = nullptr;
			SpringArm->bUsePawnControlRotation = true;
			SpringArm->SocketOffset.Y = 0;
			BaseInputComponent->CancelLockOn();
			BaseInputComponent->StartLockOn();
		}
	}
	else
	{
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->SocketOffset.Y = 0;
		BaseInputComponent->CancelLockOn();
	}
	*/
	




}

void ULockOnComponent::Input_LockOn(const FInputActionValue& Value)
{
	if (bLockOn == true) // LockOn 중일 때, 마우스 휠 입력시 LockOn 종료
	{
		CancelLockOn();
	}
	else // LockOn 중이 아닐 때, 마우스 휠 입력시 LockOn 시작
	{
		StartLockOn();
	}


}

void ULockOnComponent::StartLockOn()
{
	UBaseAnimInstance* BaseAnimInstance = Cast<UBaseAnimInstance>(Cast<APlayerCharacter>(GetOwner())->GetMesh()->GetAnimInstance());
	if (BaseAnimInstance)
	{
		// TargetEnemy를 찾고, 찾았다면 LockOn에 쓰이는 변수들을 설정함.
		if (FindTargetEnemy() == true)
		{
			bLockOn = true;
			BaseAnimInstance->SetbLockOn(true);
			SetControllerRotationTowardTarget();
		}
	}
}

void ULockOnComponent::CancelLockOn()
{
	UBaseAnimInstance* BaseAnimInstance = Cast<UBaseAnimInstance>(Cast<APlayerCharacter>(GetOwner())->GetMesh()->GetAnimInstance());
	check(BaseAnimInstance)

		// 클래스 멤버 변수 초기화
	BaseAnimInstance->SetbLockOn(false);
	InputVector = FVector2D(0, 0);
	NearByEnemies.Empty();
	EnemyMap.Empty();
	TargetEnemy = nullptr;
	bLockOn = false;


}

void ULockOnComponent::SetControllerRotationTowardTarget()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	FVector Start = PlayerCharacter->GetActorLocation();
	FVector Target = TargetEnemy->GetActorLocation();
	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Start, Target);
	PlayerCharacter->GetController()->SetControlRotation(FRotator(0.f, Rotation.Yaw, 0.f));
	DrawDebugSphere(GetWorld(), TargetEnemy->GetActorLocation(), 30, 12, FColor::Red, false, 1.5f);
}



bool ULockOnComponent::FindTargetEnemy()
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	check(PlayerCharacter);

	UCameraComponent* FollowCamera = PlayerCharacter->GetCameraComponent();

	TArray<TEnumAsByte<EObjectTypeQuery>> NearByActors;
	TEnumAsByte<EObjectTypeQuery> PawnObjectType = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	NearByActors.Add(PawnObjectType);

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(PlayerCharacter);

	// PlayerCharacter를 무시한 MaxDetectRadius을 반지름으로 하는 원형 트레이스를 실행함.
	TArray<FHitResult> HitResults;
	bool TraceHitResult = UKismetSystemLibrary::SphereTraceMultiForObjects(
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
	if (TraceHitResult == true)
	{
		for (const auto& Hit : HitResults)
		{
			NearByEnemies.AddUnique(Hit.GetActor());
		}
		float Min = 1000000.f;
		// 외적/내적을 통해 거리와 각도에 따른 값을 계산해 이를 key, Enemy를 Value로 Map을 설정함.
		for (int i = 0; i < NearByEnemies.Num(); i++)
		{

			FVector PlayerForwardVector = PlayerCharacter->GetActorForwardVector();
			FVector PlayerCameraLocation = FollowCamera->GetComponentTransform().GetTranslation();

			FVector CameraToPlayer = PlayerCharacter->GetActorLocation() - PlayerCameraLocation;
			FVector CameraToEnemy = NearByEnemies[i]->GetActorLocation() - PlayerCameraLocation;
			FVector PlayerToEnemy = NearByEnemies[i]->GetActorLocation() - PlayerCharacter->GetActorLocation();

			FVector CrossProduct = FVector::CrossProduct(PlayerForwardVector, PlayerToEnemy);
			float UpDotProduct = FVector::DotProduct(PlayerForwardVector, CrossProduct);

			// Option 1 : Player Character의 Forward Vector 기준으로 LockOn
			// Option 2 : Camera에 보이는 Enemy를 LockOn <- 현재 선택됨

			float TempDotProductResult = FVector::DotProduct(CameraToPlayer, CameraToEnemy);

			float Cos = TempDotProductResult / (PlayerForwardVector.Length() * PlayerToEnemy.Length());
			float HalfFOV = FMath::Cos(FMath::DegreesToRadians(FollowCamera->FieldOfView));
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


