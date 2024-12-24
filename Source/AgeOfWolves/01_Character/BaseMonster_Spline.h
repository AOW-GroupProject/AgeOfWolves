// Fill out your copyright notice in the Description page of Project Settings.
#pragma once
#include "CoreMinimal.h"
#include "01_Character/BaseMonster.h"
#include "10_Monster/MonsterInterface_Spline.h"
#include "BaseMonster_Spline.generated.h"

//@전방 선언
#pragma region Forward Declaration
class USplineComponent;
class ABaseSpline;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 * @ABaseMonster_Spline
 * @설명: Spline을 따라 이동하는 몬스터의 기본 클래스
 */
UCLASS()
class AGEOFWOLVES_API ABaseMonster_Spline : public ABaseMonster, public IMonsterInterface_Spline
{
    GENERATED_BODY()

        //@기본 설정
#pragma region Default Setting
public:
    ABaseMonster_Spline();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
#pragma endregion

    //@속성/정보
#pragma region Property or Subwidgets or Infos...etc
protected:
    //@회전 제어 오버라이드
    virtual void ControllRotation() override;

    //@상태 종료 시 호출되는 생명주기 함수 오버라이드
    virtual void WhenEndState() override;

protected:
    //@Spline 관련 변수들
    UPROPERTY()
        FVector SplineTargetVector;

    UPROPERTY()
        FVector SplineVectorBeforeDetectingPlayer;

    //@Spline 컴포넌트 및 액터
    UPROPERTY()
        USplineComponent* SplineComponent;

    UPROPERTY()
        ABaseSpline* SplineActor;

    //@Distance에 따른 Tangent 관련 변수들
    UPROPERTY(EditAnywhere, Category = "Spline|Distance")
        float SplineIndexDistance;

    UPROPERTY(EditAnywhere, Category = "Spline|Distance")
        float Distance = 0;

    UPROPERTY(EditAnywhere, Category = "Spline|Distance")
        float MaxTime = 10;

    UPROPERTY()
        float CurrentTime = 0;

    UPROPERTY()
        float Speed = 200;

    UPROPERTY()
        float BetweenDistance = 0;

    //@회전 정보
    UPROPERTY()
        FRotator Rotation;
#pragma endregion

//@인터페이스
#pragma region Interface
public:
    UFUNCTION(BlueprintCallable)
        virtual bool WhenMoveToSplinePointReturnTrue() override;
public:
    UFUNCTION(BlueprintCallable)
        virtual void MoveAlongSplinePoint(float delta) override;
#pragma endregion

    //@유틸리티
#pragma region Utility
public:
    void SetSplineActor(ABaseSpline* spline);

public:
    UFUNCTION(BlueprintCallable)
        FVector GetSplineVectorToPatrol();
    UFUNCTION(BlueprintCallable)
        void WaitToReactivateDetecting();
#pragma endregion
};