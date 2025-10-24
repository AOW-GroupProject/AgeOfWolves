// InteractionManagerSubsystem.h
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "01_Character/CharacterBase.h"

#include "InteractionManagerSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogInteractionManager, Log, All);

//@전방 선언
#pragma region Forward Declaration
class ACharacterBase;
class UAnimMontage;
#pragma endregion

//@구조체
#pragma region Structs
/**
 * 상호작용 참여자 역할 열거형
 */
UENUM(BlueprintType)
enum class EInteractionRole : uint8
{
	None = 0,
	Player = 1,
	Target = 2
};

/**
 * 상호작용 참여자 정보
 */
USTRUCT(BlueprintType)
struct FInteractionParticipant
{
	GENERATED_BODY()

	//@참여자 캐릭터
	UPROPERTY()
	TWeakObjectPtr<ACharacterBase> Character;

	//@재생할 애니메이션 몽타주
	UPROPERTY()
	TObjectPtr<UAnimMontage> Montage;

	//@재생 속도 (기본 1.0)
	UPROPERTY()
	float PlayRate;

	//@몽타주 시작 섹션 (선택)
	UPROPERTY()
	FName StartSectionName;

	//@참여자 역할 (Player/Target)
	UPROPERTY()
	EInteractionRole Role;

	FInteractionParticipant()
		: Character(nullptr)
		, Montage(nullptr)
		, PlayRate(1.0f)
		, StartSectionName(NAME_None)
		, Role(EInteractionRole::None)
	{
	}

	FInteractionParticipant(ACharacterBase* InCharacter, UAnimMontage* InMontage, float InPlayRate = 1.0f, EInteractionRole InRole = EInteractionRole::None)
		: Character(InCharacter)
		, Montage(InMontage)
		, PlayRate(InPlayRate)
		, StartSectionName(NAME_None)
		, Role(InRole)
	{
	}

	bool IsValid() const
	{
		return Character.IsValid() && Montage != nullptr;
	}
};

/**
 * 상호작용 세션 정보
 */
USTRUCT()
struct FInteractionSession
{
	GENERATED_BODY()

	//@세션 고유 ID
	UPROPERTY()
	FGuid SessionID;

	//@참여자 목록
	UPROPERTY()
	TArray<FInteractionParticipant> Participants;

	//@세션 생성 시간 (타임아웃 처리용)
	UPROPERTY()
	float CreationTime;

	//@세션 활성 여부
	UPROPERTY()
	bool bActive;

	//@최대 참여자 수 (기본 2명)
	UPROPERTY()
	int32 MaxParticipants;

	FInteractionSession()
		: SessionID(FGuid::NewGuid())
		, CreationTime(0.0f)
		, bActive(true)
		, MaxParticipants(2)
	{
	}

	/**
	 * 최대 참여자 수에 도달했는지 확인
	 */
	bool IsMaxParticipantsReached() const
	{
		return Participants.Num() >= MaxParticipants;
	}

	/**
	 * 특정 캐릭터가 이미 등록되어 있는지 확인
	 */
	bool HasParticipant(ACharacterBase* Character) const
	{
		for (const FInteractionParticipant& Participant : Participants)
		{
			if (Participant.Character.Get() == Character)
				return true;
		}
		return false;
	}

	/**
	 * 유효한 참여자 수 반환
	 */
	int32 GetValidParticipantCount() const
	{
		int32 Count = 0;
		for (const FInteractionParticipant& Participant : Participants)
		{
			if (Participant.IsValid())
				Count++;
		}
		return Count;
	}
};
#pragma endregion

/**
 * 상호작용 동기화 관리 서브시스템
 * 여러 캐릭터 간의 상호작용 애니메이션을 동기화하여 재생
 */
UCLASS()
class AGEOFWOLVES_API UInteractionManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

//@Default Setting
#pragma region Default Setting
public:
	UInteractionManagerSubsystem();

	//@Subsystem Override
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
#pragma endregion

//@Property
#pragma region Property
private:
	//@활성 상호작용 세션 맵
	UPROPERTY()
	TMap<FGuid, FInteractionSession> ActiveSessions;

	//@세션 타임아웃 시간 (초)
	UPROPERTY()
	float SessionTimeoutDuration;

	//@타이머 핸들
	FTimerHandle TimeoutCheckTimerHandle;
#pragma endregion

//@Core Functions
#pragma region Core Functions
public:
	/**
	 * 새로운 상호작용 세션 생성
	 * @return 생성된 세션 ID
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction Manager")
	FGuid CreateInteractionSession();

	/**
	 * 세션에 참여자 등록 (기존 통합 버전)
	 * @param SessionID 세션 ID
	 * @param Character 참여자 캐릭터
	 * @param Montage 재생할 몽타주
	 * @param PlayRate 재생 속도 (기본 1.0)
	 * @param StartSectionName 시작 섹션 이름 (선택)
	 * @param Role 참여자 역할 (Player/Target)
	 * @return 등록 성공 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction Manager")
	bool RegisterParticipant(
		FGuid SessionID,
		ACharacterBase* Character,
		UAnimMontage* Montage,
		float PlayRate = 1.0f,
		FName StartSectionName = NAME_None,
		EInteractionRole Role = EInteractionRole::None
	);

	/**
	 * 세션에 플레이어 참여자 등록 (Player 전용)
	 * @param SessionID 세션 ID
	 * @param PlayerCharacter 플레이어 캐릭터
	 * @param PlayerMontage 플레이어 몽타주
	 * @param PlayRate 재생 속도 (기본 1.0)
	 * @param StartSectionName 시작 섹션 이름 (선택)
	 * @return 등록 성공 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction Manager")
	bool RegisterPlayerParticipant(
		FGuid SessionID,
		ACharacterBase* PlayerCharacter,
		UAnimMontage* PlayerMontage,
		float PlayRate = 1.0f,
		FName StartSectionName = NAME_None
	);

	/**
	 * 세션에 타겟 참여자 등록 (Target 전용)
	 * @param SessionID 세션 ID
	 * @param TargetCharacter 타겟 캐릭터
	 * @param TargetMontage 타겟 몽타주
	 * @param PlayRate 재생 속도 (기본 1.0)
	 * @param StartSectionName 시작 섹션 이름 (선택)
	 * @return 등록 성공 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction Manager")
	bool RegisterTargetParticipant(
		FGuid SessionID,
		ACharacterBase* TargetCharacter,
		UAnimMontage* TargetMontage,
		float PlayRate = 1.0f,
		FName StartSectionName = NAME_None
	);

	/**
	 * 세션에 최대 참여자 수 도달 시 자동 재생 실행
	 * @param SessionID 세션 ID
	 * @return 재생 성공 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction Manager")
	bool ExecuteSynchronizedPlay(FGuid SessionID);

	/**
	 * 세션 취소
	 * @param SessionID 세션 ID
	 * @return 취소 성공 여부
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction Manager")
	bool CancelSession(FGuid SessionID);

	/**
	 * 세션 정리
	 * @param SessionID 세션 ID
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction Manager")
	void CleanupSession(FGuid SessionID);

private:
	/**
	 * 타임아웃된 세션 체크 및 정리
	 */
	void CheckSessionTimeouts();

	/**
	 * 세션 유효성 검사
	 */
	bool IsSessionValid(FGuid SessionID) const;
#pragma endregion

//@Utility
#pragma region Utility
public:
	/**
	 * 활성 세션 수 반환
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction Manager")
	int32 GetActiveSessionCount() const { return ActiveSessions.Num(); }

	/**
	 * 세션 정보 가져오기
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Interaction Manager")
	bool GetSessionInfo(FGuid SessionID, int32& OutParticipantCount, bool& OutAllReady) const;

	/**
	 * 디버그: 모든 활성 세션 로그 출력
	 */
	UFUNCTION(BlueprintCallable, Category = "Interaction Manager|Debug")
	void DebugPrintActiveSessions() const;

#if WITH_EDITOR
	/**
	 * 에디터 전용: 세션 시각화
	 */
	void DrawDebugSession(FGuid SessionID, float Duration = 1.0f);
#endif
#pragma endregion
};