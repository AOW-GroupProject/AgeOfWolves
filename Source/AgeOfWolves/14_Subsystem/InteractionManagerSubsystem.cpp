// InteractionManagerSubsystem.cpp
#include "InteractionManagerSubsystem.h"
#include "Logging/StructuredLog.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Animation/AnimMontage.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"
#include "Animation/AnimInstance.h"

DEFINE_LOG_CATEGORY(LogInteractionManager)

//@Default Setting
#pragma region Default Setting
UInteractionManagerSubsystem::UInteractionManagerSubsystem()
	: SessionTimeoutDuration(5.0f) // 기본 5초 타임아웃
{
}

void UInteractionManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOGFMT(LogInteractionManager, Log, "InteractionManagerSubsystem 초기화 완료");

	//@타임아웃 체크 타이머 시작 (1초마다)
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TimeoutCheckTimerHandle,
			this,
			&UInteractionManagerSubsystem::CheckSessionTimeouts,
			1.0f,
			true // 반복
		);
	}
}

void UInteractionManagerSubsystem::Deinitialize()
{
	//@타이머 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TimeoutCheckTimerHandle);
	}

	//@모든 세션 정리
	ActiveSessions.Empty();

	UE_LOGFMT(LogInteractionManager, Log, "InteractionManagerSubsystem 정리 완료");

	Super::Deinitialize();
}
#pragma endregion

//@Core Functions
#pragma region Core Functions
FGuid UInteractionManagerSubsystem::CreateInteractionSession()
{
	FInteractionSession NewSession;
	NewSession.SessionID = FGuid::NewGuid();
	NewSession.CreationTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	NewSession.bActive = true;
	// NewSession.bAllReady = false; // Remove or comment out: no such member in FInteractionSession

	ActiveSessions.Add(NewSession.SessionID, NewSession);

	UE_LOGFMT(LogInteractionManager, Log, "새 상호작용 세션 생성 - SessionID: {0}",
		*NewSession.SessionID.ToString());

	return NewSession.SessionID;
}

bool UInteractionManagerSubsystem::RegisterParticipant(
	FGuid SessionID,
	ACharacterBase* Character,
	UAnimMontage* Montage,
	float PlayRate,
	FName StartSectionName,
	EInteractionRole Role,
	UBaseGameplayAbility* AssociatedAbility)
{
	//@유효성 검사
	if (!IsValid(Character))
	{
		UE_LOGFMT(LogInteractionManager, Warning, "참여자 등록 실패 - 유효하지 않은 캐릭터");
		return false;
	}

	if (!IsValid(Montage))
	{
		UE_LOGFMT(LogInteractionManager, Warning, "참여자 등록 실패 - 유효하지 않은 몽타주");
		return false;
	}

	//@세션이 없으면 자동 생성
	FInteractionSession* Session = ActiveSessions.Find(SessionID);
	if (!Session)
	{
		FInteractionSession NewSession;
		NewSession.SessionID = SessionID;
		NewSession.CreationTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
		NewSession.bActive = true;
		NewSession.MaxParticipants = 2;
		
		ActiveSessions.Add(SessionID, NewSession);
		Session = ActiveSessions.Find(SessionID);
		
		UE_LOGFMT(LogInteractionManager, Log, "새 상호작용 세션 자동 생성 - SessionID: {0}", *SessionID.ToString());
	}

	//@중복 등록 방지
	if (Session->HasParticipant(Character))
	{
		UE_LOGFMT(LogInteractionManager, Warning, "참여자 등록 실패 - 이미 등록된 캐릭터: {0}",
			*Character->GetName());
		return false;
	}

	//@최대 참여자 수 체크
	if (Session->IsMaxParticipantsReached())
	{
		UE_LOGFMT(LogInteractionManager, Warning, "참여자 등록 실패 - 최대 참여자 수 도달: {0}/{1}",
			Session->Participants.Num(), Session->MaxParticipants);
		return false;
	}

	//@참여자 추가
	FInteractionParticipant NewParticipant(Character, Montage, PlayRate, Role);
	NewParticipant.StartSectionName = StartSectionName;
	NewParticipant.AssociatedAbility = AssociatedAbility;
	Session->Participants.Add(NewParticipant);

	UE_LOGFMT(LogInteractionManager, Log, "참여자 등록 성공 - SessionID: {0} | 캐릭터: {1} | 몽타주: {2} | 참여자 수: {3}/{4}",
		*SessionID.ToString(),
		*Character->GetName(),
		*Montage->GetName(),
		Session->Participants.Num(),
		Session->MaxParticipants);

	//@최대 참여자 수에 도달하면 자동으로 동기화 재생 실행
	if (Session->IsMaxParticipantsReached())
	{
		UE_LOGFMT(LogInteractionManager, Log, "최대 참여자 수 도달 - 자동 동기화 재생 시작 - SessionID: {0}", *SessionID.ToString());
		return ExecuteSynchronizedPlay(SessionID);
	}

	return true;
}

bool UInteractionManagerSubsystem::RegisterPlayerParticipant(
	FGuid SessionID,
	ACharacterBase* PlayerCharacter,
	UAnimMontage* PlayerMontage,
	float PlayRate,
	FName StartSectionName,
	UBaseGameplayAbility* AssociatedAbility)
{
	//@기존 RegisterParticipant 함수를 Player 역할로 호출
	return RegisterParticipant(
		SessionID,
		PlayerCharacter,
		PlayerMontage,
		PlayRate,
		StartSectionName,
		EInteractionRole::Player,
		AssociatedAbility
	);
}

bool UInteractionManagerSubsystem::RegisterTargetParticipant(
	FGuid SessionID,
	ACharacterBase* TargetCharacter,
	UAnimMontage* TargetMontage,
	float PlayRate,
	FName StartSectionName,
	UBaseGameplayAbility* AssociatedAbility)
{
	//@기존 RegisterParticipant 함수를 Target 역할로 호출
	return RegisterParticipant(
		SessionID,
		TargetCharacter,
		TargetMontage,
		PlayRate,
		StartSectionName,
		EInteractionRole::Target,
		AssociatedAbility
	);
}

bool UInteractionManagerSubsystem::ExecuteSynchronizedPlay(FGuid SessionID)
{
	FInteractionSession* Session = ActiveSessions.Find(SessionID);
	if (!Session)
	{
		UE_LOGFMT(LogInteractionManager, Warning, "동기화 재생 실패 - 세션을 찾을 수 없음");
		return false;
	}

	if (!Session->bActive)
	{
		UE_LOGFMT(LogInteractionManager, Warning, "동기화 재생 실패 - 비활성 세션");
		return false;
	}

	if (!Session->IsMaxParticipantsReached())
	{
		UE_LOGFMT(LogInteractionManager, Warning, "동기화 재생 실패 - 최대 참여자 수에 도달하지 않음 (현재: {0}/{1})",
			Session->Participants.Num(),
			Session->MaxParticipants);
		return false;
	}

	UE_LOGFMT(LogInteractionManager, Log, "동기화 재생 시작 - SessionID: {0} | 참여자 수: {1}/{2}",
		*SessionID.ToString(),
		Session->Participants.Num(),
		Session->MaxParticipants);

	//@같은 프레임 내에서 모든 참여자의 몽타주를 순차적으로 재생
	int32 SuccessCount = 0;
	for (const FInteractionParticipant& Participant : Session->Participants)
	{
		if (!Participant.IsValid())
		{
			UE_LOGFMT(LogInteractionManager, Warning, "참여자 스킵 - 유효하지 않은 참여자 데이터");
			continue;
		}

		ACharacterBase* Character = Participant.Character.Get();
		UAnimMontage* Montage = Participant.Montage;

		//@역할에 따른 로그 출력
		FString RoleString = TEXT("Unknown");
		switch (Participant.Role)
		{
		case EInteractionRole::Player:
			RoleString = TEXT("Player");
			break;
		case EInteractionRole::Target:
			RoleString = TEXT("Target");
			break;
		default:
			RoleString = TEXT("None");
			break;
		}

		//@몽타주와 세션 매핑 저장
		MontageToSessionMap.Add(Montage, SessionID);

		//@몽타주 재생 전 콜백 바인딩
		if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
		{
			//@기존 델리게이트 클리어
			AnimInstance->OnMontageBlendingOut.RemoveAll(this);
			AnimInstance->OnMontageEnded.RemoveAll(this);
			AnimInstance->OnMontageInterrupted.RemoveAll(this);
			AnimInstance->OnMontageCancelled.RemoveAll(this);

			//@새로운 델리게이트 바인딩
			AnimInstance->OnMontageBlendingOut.AddDynamic(this, &UInteractionManagerSubsystem::OnMontageBlendOut);
			AnimInstance->OnMontageEnded.AddDynamic(this, &UInteractionManagerSubsystem::OnMontageCompleted);
			AnimInstance->OnMontageInterrupted.AddDynamic(this, &UInteractionManagerSubsystem::OnMontageInterrupted);
			AnimInstance->OnMontageCancelled.AddDynamic(this, &UInteractionManagerSubsystem::OnMontageCancelled);
		}

		//@몽타주 재생
		float PlayLength = Character->PlayAnimMontage(
			Montage,
			Participant.PlayRate,
			Participant.StartSectionName
		);

		if (PlayLength > 0.0f)
		{
			SuccessCount++;
			UE_LOGFMT(LogInteractionManager, Log, "몽타주 재생 성공 - 캐릭터: {0} | 역할: {1} | 몽타주: {2} | 재생 시간: {3}초",
				*Character->GetName(),
				*RoleString,
				*Montage->GetName(),
				PlayLength);
		}
		else
		{
			UE_LOGFMT(LogInteractionManager, Warning, "몽타주 재생 실패 - 캐릭터: {0} | 역할: {1} | 몽타주: {2}",
				*Character->GetName(),
				*RoleString,
				*Montage->GetName());
		}
	}

	UE_LOGFMT(LogInteractionManager, Log, "동기화 재생 완료 - SessionID: {0} | 성공: {1}/{2}",
		*SessionID.ToString(),
		SuccessCount,
		Session->Participants.Num());

	//@세션 정리
	CleanupSession(SessionID);

	return SuccessCount > 0;
}

bool UInteractionManagerSubsystem::CancelSession(FGuid SessionID)
{
	FInteractionSession* Session = ActiveSessions.Find(SessionID);
	if (!Session)
	{
		UE_LOGFMT(LogInteractionManager, Warning, "세션 취소 실패 - 세션을 찾을 수 없음");
		return false;
	}

	Session->bActive = false;

	UE_LOGFMT(LogInteractionManager, Log, "세션 취소됨 - SessionID: {0}", *SessionID.ToString());

	CleanupSession(SessionID);
	return true;
}

void UInteractionManagerSubsystem::CleanupSession(FGuid SessionID)
{
	//@해당 세션의 몽타주 매핑 정리
	FInteractionSession* Session = ActiveSessions.Find(SessionID);
	if (Session)
	{
		for (const FInteractionParticipant& Participant : Session->Participants)
		{
			if (Participant.Montage)
			{
				MontageToSessionMap.Remove(Participant.Montage);
			}
		}
	}

	if (ActiveSessions.Remove(SessionID) > 0)
	{
		UE_LOGFMT(LogInteractionManager, Log, "세션 정리 완료 - SessionID: {0} | 남은 세션: {1}",
			*SessionID.ToString(),
			ActiveSessions.Num());
	}
}

void UInteractionManagerSubsystem::CheckSessionTimeouts()
{
	if (!GetWorld())
		return;

	float CurrentTime = GetWorld()->GetTimeSeconds();
	TArray<FGuid> SessionsToRemove;

	//@타임아웃된 세션 찾기
	for (const auto& SessionPair : ActiveSessions)
	{
		const FGuid& SessionID = SessionPair.Key;
		const FInteractionSession& Session = SessionPair.Value;

		float ElapsedTime = CurrentTime - Session.CreationTime;
		if (ElapsedTime > SessionTimeoutDuration)
		{
			UE_LOGFMT(LogInteractionManager, Warning, "세션 타임아웃 - SessionID: {0} | 경과 시간: {1}초",
				*SessionID.ToString(),
				ElapsedTime);

			SessionsToRemove.Add(SessionID);
		}
	}

	//@타임아웃된 세션 정리
	for (const FGuid& SessionID : SessionsToRemove)
	{
		CleanupSession(SessionID);
	}
}

bool UInteractionManagerSubsystem::IsSessionValid(FGuid SessionID) const
{
	return ActiveSessions.Contains(SessionID);
}
#pragma endregion

//@Utility
#pragma region Utility
bool UInteractionManagerSubsystem::GetSessionInfo(FGuid SessionID, int32& OutParticipantCount, bool& OutAllReady) const
{
	const FInteractionSession* Session = ActiveSessions.Find(SessionID);
	if (!Session)
		return false;

	OutParticipantCount = Session->GetValidParticipantCount();
	OutAllReady = Session->IsMaxParticipantsReached();
	return true;
}

void UInteractionManagerSubsystem::DebugPrintActiveSessions() const
{
	UE_LOGFMT(LogInteractionManager, Log, "=== 활성 세션 목록 ({0}개) ===", ActiveSessions.Num());

	for (const auto& SessionPair : ActiveSessions)
	{
		const FGuid& SessionID = SessionPair.Key;
		const FInteractionSession& Session = SessionPair.Value;

		float ElapsedTime = GetWorld() ? (GetWorld()->GetTimeSeconds() - Session.CreationTime) : 0.0f;

		UE_LOGFMT(LogInteractionManager, Log, "SessionID: {0}", *SessionID.ToString());
		UE_LOGFMT(LogInteractionManager, Log, "  - 경과 시간: {0}초", ElapsedTime);
		UE_LOGFMT(LogInteractionManager, Log, "  - 참여자 수: {0}/{1}", Session.Participants.Num(), Session.MaxParticipants);
		UE_LOGFMT(LogInteractionManager, Log, "  - 최대 참여자 도달: {0}", Session.IsMaxParticipantsReached() ? TEXT("예") : TEXT("아니오"));
		UE_LOGFMT(LogInteractionManager, Log, "  - 활성 상태: {0}", Session.bActive ? TEXT("예") : TEXT("아니오"));

		for (int32 i = 0; i < Session.Participants.Num(); ++i)
		{
			const FInteractionParticipant& P = Session.Participants[i];
			
			FString RoleString = TEXT("Unknown");
			switch (P.Role)
			{
			case EInteractionRole::Player:
				RoleString = TEXT("Player");
				break;
			case EInteractionRole::Target:
				RoleString = TEXT("Target");
				break;
			default:
				RoleString = TEXT("None");
				break;
			}
			
			UE_LOGFMT(LogInteractionManager, Log, "    [{0}] 캐릭터: {1} | 역할: {2} | 몽타주: {3} | 유효: {4}",
				i,
				P.Character.IsValid() ? *P.Character->GetName() : TEXT("Invalid"),
				*RoleString,
				P.Montage ? *P.Montage->GetName() : TEXT("None"),
				P.IsValid() ? TEXT("예") : TEXT("아니오"));
		}
	}
}

#if WITH_EDITOR
void UInteractionManagerSubsystem::DrawDebugSession(FGuid SessionID, float Duration)
{
	const FInteractionSession* Session = ActiveSessions.Find(SessionID);
	if (!Session || !GetWorld())
		return;

	// TODO: DrawDebugString 등으로 참여자 위치에 정보 표시
}
#endif

//@Montage Callbacks
#pragma region Montage Callbacks
void UInteractionManagerSubsystem::OnMontageCompleted(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Montage)
		return;

	//@몽타주로 세션 찾기
	FGuid* SessionID = MontageToSessionMap.Find(Montage);
	if (!SessionID)
		return;

	UE_LOGFMT(LogInteractionManager, Log, "몽타주 재생 완료 - SessionID: {0} | 몽타주: {1} | 인터럽트: {2}", 
		*SessionID->ToString(), 
		*Montage->GetName(),
		bInterrupted ? TEXT("예") : TEXT("아니오"));

	//@해당 세션에서 몽타주에 연결된 어빌리티 찾기
	FInteractionSession* Session = ActiveSessions.Find(*SessionID);
	if (Session)
	{
		for (const FInteractionParticipant& Participant : Session->Participants)
		{
			if (Participant.Montage == Montage && Participant.AssociatedAbility.IsValid())
			{
				//@어빌리티의 OnMontageCompleted 콜백 호출
				Participant.AssociatedAbility->OnMontageCompleted();
				UE_LOGFMT(LogInteractionManager, Log, "어빌리티 OnMontageCompleted 호출 - 어빌리티: {0}", 
					*Participant.AssociatedAbility->GetName());
			}
		}
	}

	//@매핑에서 제거
	MontageToSessionMap.Remove(Montage);
}

void UInteractionManagerSubsystem::OnMontageBlendOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (!Montage)
		return;

	//@몽타주로 세션 찾기
	FGuid* SessionID = MontageToSessionMap.Find(Montage);
	if (!SessionID)
		return;

	UE_LOGFMT(LogInteractionManager, Log, "몽타주 블렌드 아웃 - SessionID: {0} | 몽타주: {1} | 인터럽트: {2}", 
		*SessionID->ToString(), 
		*Montage->GetName(),
		bInterrupted ? TEXT("예") : TEXT("아니오"));

	//@해당 세션에서 몽타주에 연결된 어빌리티 찾기
	FInteractionSession* Session = ActiveSessions.Find(*SessionID);
	if (Session)
	{
		for (const FInteractionParticipant& Participant : Session->Participants)
		{
			if (Participant.Montage == Montage && Participant.AssociatedAbility.IsValid())
			{
				//@어빌리티의 OnMontageBlendOut 콜백 호출
				Participant.AssociatedAbility->OnMontageBlendOut();
				UE_LOGFMT(LogInteractionManager, Log, "어빌리티 OnMontageBlendOut 호출 - 어빌리티: {0}", 
					*Participant.AssociatedAbility->GetName());
			}
		}
	}
}

void UInteractionManagerSubsystem::OnMontageInterrupted(UAnimMontage* Montage, UAnimMontage* NewMontage)
{
	if (!Montage)
		return;

	//@몽타주로 세션 찾기
	FGuid* SessionID = MontageToSessionMap.Find(Montage);
	if (!SessionID)
		return;

	UE_LOGFMT(LogInteractionManager, Warning, "몽타주 인터럽트 - SessionID: {0} | 몽타주: {1} | 새 몽타주: {2}", 
		*SessionID->ToString(), 
		*Montage->GetName(),
		NewMontage ? *NewMontage->GetName() : TEXT("None"));

	//@해당 세션에서 몽타주에 연결된 어빌리티 찾기
	FInteractionSession* Session = ActiveSessions.Find(*SessionID);
	if (Session)
	{
		for (const FInteractionParticipant& Participant : Session->Participants)
		{
			if (Participant.Montage == Montage && Participant.AssociatedAbility.IsValid())
			{
				//@어빌리티의 OnMontageInterrupted 콜백 호출
				Participant.AssociatedAbility->OnMontageInterrupted();
				UE_LOGFMT(LogInteractionManager, Log, "어빌리티 OnMontageInterrupted 호출 - 어빌리티: {0}", 
					*Participant.AssociatedAbility->GetName());
			}
		}
	}

	//@매핑에서 제거
	MontageToSessionMap.Remove(Montage);
}

void UInteractionManagerSubsystem::OnMontageCancelled(UAnimMontage* Montage)
{
	if (!Montage)
		return;

	//@몽타주로 세션 찾기
	FGuid* SessionID = MontageToSessionMap.Find(Montage);
	if (!SessionID)
		return;

	UE_LOGFMT(LogInteractionManager, Warning, "몽타주 취소 - SessionID: {0} | 몽타주: {1}", 
		*SessionID->ToString(), 
		*Montage->GetName());

	//@해당 세션에서 몽타주에 연결된 어빌리티 찾기
	FInteractionSession* Session = ActiveSessions.Find(*SessionID);
	if (Session)
	{
		for (const FInteractionParticipant& Participant : Session->Participants)
		{
			if (Participant.Montage == Montage && Participant.AssociatedAbility.IsValid())
			{
				//@어빌리티의 OnMontageCancelled 콜백 호출
				Participant.AssociatedAbility->OnMontageCancelled();
				UE_LOGFMT(LogInteractionManager, Log, "어빌리티 OnMontageCancelled 호출 - 어빌리티: {0}", 
					*Participant.AssociatedAbility->GetName());
			}
		}
	}

	//@매핑에서 제거
	MontageToSessionMap.Remove(Montage);
}
#pragma endregion
#pragma endregion