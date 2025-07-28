#include "AgeOfWolvesGameMode.h"
#include "Logging/StructuredLog.h"

#include "UObject/ConstructorHelpers.h"

#include "01_Character/CharacterBase.h"
#include "03_Player/PlayerStateBase.h"
#include "17_GameMode/AOWGameState.h"

#include "16_Level/Area.h"

DEFINE_LOG_CATEGORY(LogAOWGameMode)

//@Defualt Setting
#pragma region Default Setting
AAgeOfWolvesGameMode::AAgeOfWolvesGameMode()
{}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void AAgeOfWolvesGameMode::HandlePlayerDeath(APlayerController* PlayerController)
{
    //@PC 유효성 검증
    if (!IsValid(PlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "플레이어 죽음 처리 - 유효하지 않은 PlayerController입니다");
        return;
    }

    //@PlayerState 가져오기
    APlayerStateBase* PlayerState = PlayerController->GetPlayerState<APlayerStateBase>();
    if (!IsValid(PlayerState))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "플레이어 죽음 처리 - PlayerController에서 PlayerState를 찾을 수 없습니다: {0}",
            GetNameSafe(PlayerController));
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 죽음 처리 시작: PC={0}, PS={1}",
        GetNameSafe(PlayerController), GetNameSafe(PlayerState));

    if (DetermineDeathRules(PlayerController))
    {
        //@즉시 리스폰 대신 GameState를 통해 로딩 UI 표시 요청
        AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>();
        if (IsValid(CurrentGameState))
        {
            UE_LOGFMT(LogAOWGameMode, Log, "GameState를 통한 로딩 UI 표시 요청");
            CurrentGameState->NotifyRequestShowLoadingUI();
        }
        else
        {
            UE_LOGFMT(LogAOWGameMode, Error, "GameState를 찾을 수 없어 로딩 UI 표시 요청 실패");
        }
    }
}

void AAgeOfWolvesGameMode::ExecutePlayerRespawn(APlayerController* PlayerController)
{
    UE_LOGFMT(LogAOWGameMode, Log, "실제 리스폰 실행 시작: {0}", GetNameSafe(PlayerController));

    if (!IsValid(PlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "리스폰 실행 실패: 유효하지 않은 PlayerController");
        return;
    }

    // 실제 리스폰 처리
    if (TestRespawnToPlayerStart(PlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Log, "리스폰 위치로 재설정 완료!");

        // 리스폰 완료 후 Game State에 알림
        NotifyRespawnCompleteViaGameState(PlayerController);
    }
    else
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "리스폰 위치로 재설정 실패!");
    }
}

// 나머지 함수들은 기존과 동일하게 유지...
void AAgeOfWolvesGameMode::NotifyRespawnCompleteViaGameState(APlayerController* PlayerController)
{
    UE_LOGFMT(LogAOWGameMode, Log, "Game State를 통한 리스폰 완료 알림 시작");

    if (!IsValid(PlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "리스폰 알림 실패: 유효하지 않은 PlayerController입니다");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "PlayerController 확인 성공: {0}", GetNameSafe(PlayerController));

    AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>();
    if (!IsValid(CurrentGameState))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "리스폰 알림 실패: AOWGameState를 찾을 수 없습니다");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "AOWGameState 가져오기 성공: {0}", GetNameSafe(CurrentGameState));

    CurrentGameState->NotifyPlayerRespawnCompleted(PlayerController);

    UE_LOGFMT(LogAOWGameMode, Log, "Game State 리스폰 알림 호출 완료: PlayerController {0}에 대한 알림을 전송했습니다",
        GetNameSafe(PlayerController));
}

bool AAgeOfWolvesGameMode::TestRespawnToPlayerStart(APlayerController* PlayerController)
{
    //@기본 유효성 검증
    if (!IsValid(PlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "테스트 리스폰 실패: 유효하지 않은 PlayerController");
        return false;
    }

    //@PlayerState 가져오기 (로깅용)
    APlayerStateBase* PlayerState = PlayerController->GetPlayerState<APlayerStateBase>();
    if (!IsValid(PlayerState))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "PlayerState를 찾을 수 없음, 리스폰 계속 진행: {0}",
            GetNameSafe(PlayerController));
    }

    //@GameMode의 내장 함수로 최적의 Player Start 찾기
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
        NotifyRespawnCompleteViaGameState(PlayerController);

        UE_LOGFMT(LogAOWGameMode, Log, "테스트 리스폰 완료: 표준 메커니즘 사용");
        return true;
    }

    //@기존 Pawn이 있는 경우: 선택된 Player Start 위치로 이동
    FTransform SpawnTransform = ChosenPlayerStart->GetActorTransform();
    FVector SafeLocation = SpawnTransform.GetLocation();
    SafeLocation.Z += 100.0f; // 지면 겹침 방지를 위한 안전 높이 조정
    SpawnTransform.SetLocation(SafeLocation);

    //@물리 기반 텔레포트로 안전하게 이동
    bool bTeleportSuccess = CurrentPawn->SetActorTransform(SpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);

    if (bTeleportSuccess)
    {
        UE_LOGFMT(LogAOWGameMode, Log, "테스트 리스폰 완료: Pawn 이동 성공");
    }
    else
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "테스트 리스폰 부분 실패: Pawn 이동 실패");
    }

    return bTeleportSuccess;
}

void AAgeOfWolvesGameMode::OnStructureInteractionActtivated(const FStructureData& StructureData)
{
    //@ 재차 상호작용시 => 시스템 메뉴UI 오픈 등..
    if (StructureData.bIsActive)
    {
        
        UE_LOGFMT(LogAOWGameMode, Log, "구조물 활성화 호출 , 재회!");
    }
    else //@첫회 상호작용시 => 레벨전환.. 
    {
        
        UE_LOGFMT(LogAOWGameMode, Log, "구조물 활성화 호출, 첫회! ");
    }
}

#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility

bool AAgeOfWolvesGameMode::DetermineDeathRules(APlayerController* PlayerController)
{
    if (!IsValid(PlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "죽음 규칙 결정 실패: 유효하지 않은 PlayerController");
        return false;
    }

    //@PlayerState 가져오기
    APlayerStateBase* PlayerState = PlayerController->GetPlayerState<APlayerStateBase>();

    UE_LOGFMT(LogAOWGameMode, Log, "죽음 규칙 결정 중: PC={0}, PS={1}",
        GetNameSafe(PlayerController), GetNameSafe(PlayerState));

    //@죽음 규칙 확인 작업...

    return true;
}
#pragma endregion