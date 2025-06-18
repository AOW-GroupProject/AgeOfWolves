#include "AgeOfWolvesGameMode.h"
#include "Logging/StructuredLog.h"

#include "UObject/ConstructorHelpers.h"

#include "01_Character/CharacterBase.h"
#include "03_Player/PlayerStateBase.h"
#include "17_GameMode/AOWGameState.h"

DEFINE_LOG_CATEGORY(LogAOWGameMode)

//@Defualt Setting
#pragma region Default Setting
AAgeOfWolvesGameMode::AAgeOfWolvesGameMode()
{}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void AAgeOfWolvesGameMode::HandlePlayerDeath(APlayerStateBase* DeadPlayerState)
{
    //@PS
    if (!IsValid(DeadPlayerState))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "플레이어 죽음 처리 - 유효하지 않은 PlayerState입니다");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 죽음 처리 시작: {0}", GetNameSafe(DeadPlayerState));

    if (DetermineDeathRules(DeadPlayerState))
    {
        //@리스폰 진행
        if (TestRespawnToPlayerStart(DeadPlayerState))
        {
            UE_LOGFMT(LogAOWGameMode, Log, "리스폰 위치로 재설정 완료!");
        }
        else UE_LOGFMT(LogAOWGameMode, Warning, "리스폰 위치로 재설정 실패!");
    }

    //@Game State의 죽음 알림 요청
    NotifyRespawnCompleteViaGameState(DeadPlayerState);
}


void AAgeOfWolvesGameMode::NotifyRespawnCompleteViaGameState(APlayerStateBase* DeadPlayerState)
{
    UE_LOGFMT(LogAOWGameMode, Log, "Game State를 통한 리스폰 완료 알림 시작");

    if (!IsValid(DeadPlayerState))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "리스폰 알림 실패: 유효하지 않은 PlayerState입니다");
        return;
    }

    APlayerController* TargetPlayerController = Cast<APlayerController>(DeadPlayerState->GetOwner());
    if (!IsValid(TargetPlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "리스폰 알림 실패: PlayerState에서 PlayerController를 찾을 수 없습니다. PlayerState: {0}",
            GetNameSafe(DeadPlayerState));
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "PlayerController 찾기 성공: {0} (PlayerState: {1})",
        GetNameSafe(TargetPlayerController), GetNameSafe(DeadPlayerState));

    AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>();
    if (!IsValid(CurrentGameState))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "리스폰 알림 실패: AOWGameState를 찾을 수 없습니다");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "AOWGameState 가져오기 성공: {0}", GetNameSafe(CurrentGameState));

    CurrentGameState->NotifyPlayerRespawnCompleted(TargetPlayerController);

    UE_LOGFMT(LogAOWGameMode, Log, "Game State 리스폰 알림 호출 완료: PlayerController {0}에 대한 알림을 전송했습니다",
        GetNameSafe(TargetPlayerController));
}

bool AAgeOfWolvesGameMode::TestRespawnToPlayerStart(APlayerStateBase* DeadPlayerState)
{
    //@기본 유효성 검증
    if (!IsValid(DeadPlayerState))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "테스트 리스폰 실패: 유효하지 않은 PlayerState");
        return false;
    }

    //@PlayerController 가져오기
    APlayerController* PlayerController = Cast<APlayerController>(DeadPlayerState->GetOwner());
    if (!IsValid(PlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "테스트 리스폰 실패: PlayerController를 찾을 수 없음");
        return false;
    }

    //@GameMode의 내장 함수로 최적의 Player Start 찾기
    //@이 함수는 Unreal Engine이 제공하는 정교한 스폰 포인트 선택 알고리즘을 사용합니다
    AActor* ChosenPlayerStart = ChoosePlayerStart(PlayerController);
    if (!IsValid(ChosenPlayerStart))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "테스트 리스폰 실패: 적절한 Player Start를 찾을 수 없음");
        return false;
    }

    //@현재 Pawn 확인 및 처리
    APawn* CurrentPawn = PlayerController->GetPawn();
    if (!IsValid(CurrentPawn))
    {
        //@Pawn이 없는 경우: GameMode의 표준 리스폰 메커니즘 사용
        UE_LOGFMT(LogAOWGameMode, Log, "Pawn이 없어서 표준 리스폰 수행");
        RestartPlayer(PlayerController);

        //@Game State를 통한 리스폰 완료 알림
        NotifyRespawnCompleteViaGameState(DeadPlayerState);

        UE_LOGFMT(LogAOWGameMode, Log, "테스트 리스폰 완료: 표준 메커니즘 사용");
        return false;
    }

    //@기존 Pawn이 있는 경우: 선택된 Player Start 위치로 이동
    FTransform SpawnTransform = ChosenPlayerStart->GetActorTransform();
    FVector SafeLocation = SpawnTransform.GetLocation();
    SafeLocation.Z += 100.0f; // 지면 겹침 방지를 위한 안전 높이 조정
    SpawnTransform.SetLocation(SafeLocation);

    //@물리 기반 텔레포트로 안전하게 이동
    //@ETeleportType::TeleportPhysics는 물리 시뮬레이션과 충돌 감지를 고려한 안전한 이동을 보장합니다
    bool bTeleportSuccess = CurrentPawn->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);

    return bTeleportSuccess;
}
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
bool AAgeOfWolvesGameMode::DetermineDeathRules(APlayerStateBase* DeadPlayerState)
{
    UE_LOGFMT(LogAOWGameMode, Log, "죽음 규칙 결정 중: {0}",
        GetNameSafe(DeadPlayerState));

    //@죽음 규칙 확인 작업...

    return true;
}

#pragma endregion