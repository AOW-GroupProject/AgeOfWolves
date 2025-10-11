#include "AgeOfWolvesGameMode.h"
#include "Logging/StructuredLog.h"

#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

#include "AOWGameState.h"
#include "00_GameInstance/AOWGameInstance.h"
#include "01_Character/CharacterBase.h"
#include "03_Player/PlayerStateBase.h"
#include "16_Level/AreaQuestDataInfos.h"
#include "14_Subsystem/UIManagerSubsystem.h"
#include "14_Subsystem/LevelManagerSubsystem.h"
#include "16_Level/Area.h"

DEFINE_LOG_CATEGORY(LogAOWGameMode)

//@기본 설정
#pragma region Default Setting
AAgeOfWolvesGameMode::AAgeOfWolvesGameMode()
{
    // GameState 클래스 설정
    GameStateClass = AAOWGameState::StaticClass();
}

void AAgeOfWolvesGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    Super::InitGame(MapName, Options, ErrorMessage);

    UE_LOGFMT(LogAOWGameMode, Log, "GameMode InitGame 시작");

    // Subsystem 참조 캐싱
    CacheSubsystemReferences();
}

void AAgeOfWolvesGameMode::BeginPlay()
{
    Super::BeginPlay();

    UE_LOGFMT(LogAOWGameMode, Log, "GameMode BeginPlay 시작");

    // GameState 참조 확보
    CachedGameState = GetGameState<AAOWGameState>();
    if (!CachedGameState)
    {
        UE_LOGFMT(LogAOWGameMode, Error, "AOWGameState를 찾을 수 없음");
        return;
    }

    // UI Manager와 델리게이트 바인딩 (LoadingUI 완료 이벤트만)
    if (CachedUIManager)
    {
        CachedUIManager->LoadingUIShown.AddUObject(this, &AAgeOfWolvesGameMode::OnLoadingUIShown);
        CachedUIManager->LoadingUIHidden.AddUObject(this, &AAgeOfWolvesGameMode::OnLoadingUIHidden);
        UE_LOGFMT(LogAOWGameMode, Log, "UI Manager 이벤트 바인딩 완료");
    }

    UE_LOGFMT(LogAOWGameMode, Log, "GameMode BeginPlay 완료");
}

AActor* AAgeOfWolvesGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    if (!CachedLevelManager || !CachedGameState)
    {
        return Super::ChoosePlayerStart_Implementation(Player);
    }

    // 레벨 전환 상태에서는 목적지 레벨의 PlayerStart 사용
    if (CachedGameState->IsInLevelTransitionState())
    {
        FGameplayTag TargetLevelTag = CachedGameState->GetNextLevelTag();
        if (TargetLevelTag.IsValid())
        {
            FGameplayTag DefaultPlayerStartTag = CachedLevelManager->GetDefaultPlayerStartTagForLevel(TargetLevelTag);
            if (DefaultPlayerStartTag.IsValid())
            {
                FString TargetTag = DefaultPlayerStartTag.ToString();

                for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
                {
                    APlayerStart* PlayerStart = *It;
                    if (PlayerStart && PlayerStart->PlayerStartTag.ToString() == TargetTag)
                    {
                        UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환용 PlayerStart 발견: {0}", *TargetTag);
                        return PlayerStart;
                    }
                }
            }
        }
    }

    // 일반적인 경우 현재 레벨의 기본 PlayerStart 사용
    FGameplayTag CurrentLevelTag = CachedLevelManager->GetCurrentLevelTag();
    if (CurrentLevelTag.IsValid())
    {
        FGameplayTag DefaultPlayerStartTag = CachedLevelManager->GetDefaultPlayerStartTagForLevel(CurrentLevelTag);
        if (DefaultPlayerStartTag.IsValid())
        {
            FString TargetTag = DefaultPlayerStartTag.ToString();

            for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
            {
                APlayerStart* PlayerStart = *It;
                if (PlayerStart && PlayerStart->PlayerStartTag.ToString() == TargetTag)
                {
                    UE_LOGFMT(LogAOWGameMode, Log, "기본 PlayerStart 발견: {0}", *TargetTag);
                    return PlayerStart;
                }
            }
        }
    }

    return Super::ChoosePlayerStart_Implementation(Player);
}

void AAgeOfWolvesGameMode::CacheSubsystemReferences()
{
    UGameInstance* GameInstance = GetGameInstance();
    if (!GameInstance)
    {
        UE_LOGFMT(LogAOWGameMode, Error, "GameInstance를 찾을 수 없음");
        return;
    }

    CachedUIManager = GameInstance->GetSubsystem<UUIManagerSubsystem>();
    CachedLevelManager = GameInstance->GetSubsystem<ULevelManagerSubsystem>();

    UE_LOGFMT(LogAOWGameMode, Log, "Subsystem 참조 캐싱 완료 - UI: {0}, Level: {1}",
        CachedUIManager ? TEXT("OK") : TEXT("FAIL"),
        CachedLevelManager ? TEXT("OK") : TEXT("FAIL"));
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void AAgeOfWolvesGameMode::HandlePlayerDeath(APlayerController* PlayerController)
{
    if (!IsValid(PlayerController) || !CachedGameState)
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "플레이어 사망 처리 실패: 유효하지 않은 PlayerController 또는 GameState");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 사망 처리 시작: {0}", GetNameSafe(PlayerController));

    // 플레이어 입력 차단
    PlayerController->DisableInput(PlayerController);

    // GameState를 PlayerDeath 상태로 변경
    CachedGameState->SetGameModeState(EGameModeState::PlayerDeath, TEXT("Player Death Triggered"));

    // 사망 규칙 결정
    if (!DetermineDeathRules(PlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "사망 규칙 결정 실패");
        return;
    }

    // 리스폰 위치 미리 계산 및 GameState에 설정
    if (!PreCalculateRespawnLocation(PlayerController, true)) // true = 죽음으로 인한 리스폰
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "리스폰 위치 계산 실패 - 기본 위치 사용");
        FTransform DefaultTransform(FRotator::ZeroRotator, FVector(0, 0, 100), FVector::OneVector);
        CachedGameState->SetDeathRespawnInfo(PlayerController, DefaultTransform);
    }

    // 로딩 UI 표시
    ShowLoadingUI();

    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 사망 처리 완료");
}

void AAgeOfWolvesGameMode::HandleFirstStructureActivation(const FStructureData& StructureData)
{
    if (!CachedGameState)
    {
        UE_LOGFMT(LogAOWGameMode, Error, "구조물 활성화 처리 실패: GameState 없음");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "구조물 활성화: {0}", *StructureData.GetStructureName().ToString());

    if (!StructureData.GetNextLevelTag().IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "구조물에 다음 레벨 태그가 설정되지 않음");
        return;
    }

    // 모든 플레이어 입력 차단
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        if (APlayerController* PC = Iterator->Get())
        {
            PC->DisableInput(PC);
        }
    }

    // GameState를 LevelTransition 상태로 변경
    CachedGameState->SetGameModeState(EGameModeState::LevelTransition, TEXT("Structure Activation"));

    // 레벨 전환 정보 설정
    APlayerController* FirstPlayer = GetWorld()->GetFirstPlayerController();
    CachedGameState->SetLevelTransitionInfo(StructureData.GetNextLevelTag(), FirstPlayer);

    // 도착 레벨의 리스폰 위치 미리 계산
    if (!PreCalculateRespawnLocation(FirstPlayer, false)) // false = 레벨 전환으로 인한 리스폰
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "레벨 전환 리스폰 위치 계산 실패 - 기본 위치 사용");
        FTransform DefaultTransform(FRotator::ZeroRotator, FVector(0, 0, 100), FVector::OneVector);
        CachedGameState->SetLevelTransitionRespawnInfo(FirstPlayer, DefaultTransform);
    }

    // 로딩 UI 표시
    ShowLoadingUI();

    UE_LOGFMT(LogAOWGameMode, Log, "구조물 활성화 처리 완료");
}

void AAgeOfWolvesGameMode::ExecutePlayerRespawn()
{
    if (!CachedGameState)
    {
        UE_LOGFMT(LogAOWGameMode, Error, "플레이어 리스폰 실행 실패: GameState 없음");
        return;
    }

    const FRespawnInfo& RespawnInfo = CachedGameState->GetCurrentRespawnInfo();
    if (!RespawnInfo.IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Error, "플레이어 리스폰 실행 실패: 유효하지 않은 리스폰 정보");
        return;
    }

    APlayerController* PlayerController = RespawnInfo.DeadPlayerController.Get();
    if (!IsValid(PlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "플레이어 리스폰 실행 실패: 유효하지 않은 PlayerController");
        return;
    }

    APawn* PlayerPawn = PlayerController->GetPawn();
    if (!IsValid(PlayerPawn))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "플레이어 리스폰 실행 실패: 유효하지 않은 Pawn");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 리스폰 실행 시작: {0} -> {1}",
        GetNameSafe(PlayerController), *RespawnInfo.RespawnTransform.GetLocation().ToString());

    // 플레이어 텔레포트
    if (!PerformPlayerTeleport(PlayerPawn, RespawnInfo.RespawnTransform))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "플레이어 텔레포트 실패 - 강제 위치 설정");
        PlayerPawn->SetActorLocation(RespawnInfo.RespawnTransform.GetLocation(), false);
        PlayerPawn->SetActorRotation(RespawnInfo.RespawnTransform.GetRotation());
    }

    // 플레이어 상태 초기화
    ResetPlayerGameplayState(PlayerController);

    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 리스폰 실행 완료");

    // 로딩 UI 숨김 (메인 스레드에서)
    AsyncTask(ENamedThreads::GameThread, [this]()
        {
            HideLoadingUI();
        });
}

void AAgeOfWolvesGameMode::HandleAreaQuestCompletion(FGameplayTag AreaTag, const FQuestDataInfo& QuestData)
{
    UE_LOGFMT(LogAOWGameMode, Log, "완료 퀘스트 정보 처리 시작");
    
    if (!QuestData.QuestTag.IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "QuestTag가 유효하지 않습니다.");
        return;
    }
    
    if (QuestData.QuestStatus == EQuestStatus::Completed)
    {
        NotifyQuestCompleteViaGameState(AreaTag, QuestData);
    }
    else
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "QuestStatus가 완료상태가 아닙니다.");
        return; 
    }
    
    UE_LOGFMT(LogAOWGameMode, Log, "완료 퀘스트 정보 처리 완료");
}

void AAgeOfWolvesGameMode::NotifyQuestCompleteViaGameState(FGameplayTag AreaTag, const FQuestDataInfo& QuestData)
{
    UE_LOGFMT(LogAOWGameMode, Log, "Game State를 통한 퀘스트 완료 알림 시작");
    
    AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>();
    if (!IsValid(CurrentGameState))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "퀘스트 완료 알림 실패: AOWGameState를 찾을 수 없습니다");
        return;
    }

    if (!QuestData.QuestTag.IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "QuestTag가 유효하지 않습니다.");
        return;
    }
    
    if (QuestData.QuestStatus == EQuestStatus::Completed)
    {
        CurrentGameState->NotifyPlayerQuestCompleted(AreaTag, QuestData);
    }
    
    UE_LOGFMT(LogAOWGameMode, Log, "Game State를 통한 퀘스트 완료 알림 호출 완료");
}

bool AAgeOfWolvesGameMode::PerformPlayerTeleport(APawn* PlayerPawn, const FTransform& TargetTransform)
{
    if (!IsValid(PlayerPawn))
    {
        return false;
    }

    bool bSuccess = PlayerPawn->SetActorTransform(TargetTransform, false, nullptr, ETeleportType::TeleportPhysics);

    if (bSuccess)
    {
        // 물리 상태 안정화
        if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(PlayerPawn->GetRootComponent()))
        {
            RootComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
            RootComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
        }
    }

    return bSuccess;
}

void AAgeOfWolvesGameMode::ResetPlayerGameplayState(APlayerController* PlayerController)
{
    if (!IsValid(PlayerController))
    {
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 게임플레이 상태 초기화: {0}", GetNameSafe(PlayerController));

    // 게임플레이 상태 초기화 로직 (프로젝트 특성에 맞게 구현)
    // 예: 체력 회복, 디버프 제거, 어빌리티 초기화 등
}

bool AAgeOfWolvesGameMode::PreCalculateRespawnLocation(APlayerController* PlayerController, bool bIsDeathRespawn)
{
    if (!IsValid(PlayerController) || !CachedGameState)
    {
        return false;
    }

    FTransform RespawnTransform;
    bool bSuccess = false;

    if (bIsDeathRespawn)
    {
        bSuccess = CalculateDeathRespawnLocation(PlayerController, RespawnTransform);
        if (bSuccess)
        {
            CachedGameState->SetDeathRespawnInfo(PlayerController, RespawnTransform);
        }
    }
    else
    {
        bSuccess = CalculateLevelTransitionRespawnLocation(PlayerController, RespawnTransform);
        if (bSuccess)
        {
            CachedGameState->SetLevelTransitionRespawnInfo(PlayerController, RespawnTransform);
        }
    }

    UE_LOGFMT(LogAOWGameMode, Log, "리스폰 위치 계산 완료: {0} (타입: {1})",
        bSuccess ? TEXT("성공") : TEXT("실패"),
        bIsDeathRespawn ? TEXT("Death") : TEXT("Level Transition"));

    return bSuccess;
}

void AAgeOfWolvesGameMode::StartLevelTransition(const FGameplayTag& TargetLevelTag)
{
    if (!CachedLevelManager)
    {
        UE_LOGFMT(LogAOWGameMode, Error, "레벨 전환 시작 실패: LevelManager 없음");
        return;
    }

    if (!TargetLevelTag.IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Error, "레벨 전환 시작 실패: 유효하지 않은 레벨 태그");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 시작: {0}", *TargetLevelTag.ToString());

    // 첫 번째 단계: 델리게이트 바인딩 (작업 시작 전에 미리 준비)
    // 기존 바인딩이 있다면 먼저 해제 (중복 바인딩 방지)
    if (CachedLevelManager->LevelTransitionCompleted.IsBound())
    {
        CachedLevelManager->LevelTransitionCompleted.Unbind();
    }

    // 새로운 바인딩 설정
    CachedLevelManager->LevelTransitionCompleted.BindUObject(this, &AAgeOfWolvesGameMode::OnLevelTransitionCompleted);

    UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 완료 콜백 바인딩 완료");

    // 두 번째 단계: LevelManager에 직접 레벨 전환 작업 요청
    bool bTransitionStarted = CachedLevelManager->PerformLevelStreamingOperations(TargetLevelTag);

    if (bTransitionStarted)
    {
        UE_LOGFMT(LogAOWGameMode, Log, "LevelManager에서 레벨 전환 작업 시작 성공: {0}", *TargetLevelTag.ToString());
    }
    else
    {
        UE_LOGFMT(LogAOWGameMode, Error, "LevelManager에서 레벨 전환 작업 시작 실패: {0}", *TargetLevelTag.ToString());

        // 실패한 경우 바인딩 해제
        CachedLevelManager->LevelTransitionCompleted.Unbind();

        // GameState를 정상 상태로 복구 (필요한 경우)
        if (CachedGameState && CachedGameState->IsInLevelTransitionState())
        {
            CachedGameState->ResetToNormalState();
        }
    }
}

void AAgeOfWolvesGameMode::ShowLoadingUI()
{
    if (!CachedUIManager)
    {
        UE_LOGFMT(LogAOWGameMode, Error, "로딩 UI 표시 실패: UIManager 없음");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "로딩 UI 표시 요청");

    // UIManager에 직접 로딩 UI 표시 요청
    if (!CachedUIManager->ShowSystemUI(FGameplayTag::RequestGameplayTag("UI.System.LoadingUI")))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "로딩 UI 표시 실패");
    }
}

void AAgeOfWolvesGameMode::HideLoadingUI()
{
    if (!CachedUIManager)
    {
        UE_LOGFMT(LogAOWGameMode, Error, "로딩 UI 숨김 실패: UIManager 없음");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "로딩 UI 숨김 요청");

    // UIManager에 직접 로딩 UI 숨김 요청
    if (!CachedUIManager->HideSystemUI(FGameplayTag::RequestGameplayTag("UI.System.LoadingUI")))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "로딩 UI 숨김 실패");
    }
}
#pragma endregion

//@Callbacks
#pragma region Callbacks
void AAgeOfWolvesGameMode::OnLoadingUIShown()
{
    if (!CachedGameState)
    {
        UE_LOGFMT(LogAOWGameMode, Error, "로딩 UI 표시 완료 처리 실패: GameState 없음");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "로딩 UI 표시 완료 - 현재 상태: {0}",
        static_cast<int32>(CachedGameState->GetCurrentGameModeState()));

    // 현재 상태에 따른 후속 처리
    if (CachedGameState->IsInPlayerDeathState())
    {
        // 백그라운드 스레드에서 리스폰 처리
        AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
            {
                ExecutePlayerRespawn();
            });
    }
    else if (CachedGameState->IsInLevelTransitionState())
    {
        // 백그라운드 스레드에서 레벨 전환 처리
        AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
            {
                FGameplayTag TargetLevelTag = CachedGameState->GetNextLevelTag();
                if (TargetLevelTag.IsValid())
                {
                    StartLevelTransition(TargetLevelTag);
                }
            });
    }
}

void AAgeOfWolvesGameMode::OnLoadingUIHidden()
{
    if (!CachedGameState)
    {
        UE_LOGFMT(LogAOWGameMode, Error, "로딩 UI 숨김 완료 처리 실패: GameState 없음");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "로딩 UI 숨김 완료");

    // 리스폰된 플레이어 정보 가져오기
    APlayerController* RespawnedPlayer = CachedGameState->GetCurrentDeadPlayer();
    if (IsValid(RespawnedPlayer))
    {
        // GameState에 리스폰 완료 알림
        CachedGameState->CompletePlayerRespawn(RespawnedPlayer);

        // 플레이어 입력 복구
        RespawnedPlayer->EnableInput(RespawnedPlayer);
        UE_LOGFMT(LogAOWGameMode, Log, "플레이어 입력 복구: {0}", GetNameSafe(RespawnedPlayer));
    }

    // GameState를 Normal 상태로 복구
    CachedGameState->ResetToNormalState();
}

void AAgeOfWolvesGameMode::OnLevelTransitionCompleted(const FGameplayTag& CompletedLevelTag)
{
    UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 완료 콜백 수신: {0}", *CompletedLevelTag.ToString());

    if (!CachedGameState)
    {
        UE_LOGFMT(LogAOWGameMode, Error, "레벨 전환 완료 처리 실패: GameState 없음");
        return;
    }

    // 첫 번째 단계: 현재 상태 검증
    if (!CachedGameState->IsInLevelTransitionState())
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "예상하지 못한 상태에서 레벨 전환 완료 이벤트 수신: 현재 상태는 레벨 전환 상태가 아님");
        // 하지만 계속 진행 (복구 시도)
    }

    // 두 번째 단계: GameState에 레벨 전환 완료 알림
    CachedGameState->CompleteLevelTransition();

    // 세 번째 단계: 리스폰 위치 계산 및 플레이어 재배치
    // 이 작업은 PlayerStart를 찾는 등 게임 스레드에서만 가능한 작업이므로 
    // 게임 스레드에서 수행해야 합니다
    AsyncTask(ENamedThreads::GameThread, [this, CompletedLevelTag]()
        {
            UE_LOGFMT(LogAOWGameMode, Log, "게임 스레드에서 리스폰 위치 계산 시작: {0}", *CompletedLevelTag.ToString());

            // 현재 플레이어 컨트롤러 가져오기
            APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
            if (!IsValid(PlayerController))
            {
                UE_LOGFMT(LogAOWGameMode, Error, "PlayerController를 찾을 수 없어 리스폰 위치 계산 실패");
                return;
            }

            // 새로운 레벨에서의 리스폰 위치 계산
            FTransform LevelTransitionRespawnTransform;
            if (!CalculateLevelTransitionRespawnLocation(PlayerController, LevelTransitionRespawnTransform))
            {
                UE_LOGFMT(LogAOWGameMode, Warning, "레벨 전환 리스폰 위치 계산 실패 - 기본 위치 사용");
                LevelTransitionRespawnTransform = FTransform(FRotator::ZeroRotator, FVector(0, 0, 100), FVector::OneVector);
            }

            // GameState에 리스폰 정보 설정 (레벨 전환용)
            CachedGameState->SetLevelTransitionRespawnInfo(PlayerController, LevelTransitionRespawnTransform);

            UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 리스폰 정보 설정 완료, 플레이어 리스폰 실행");

            // 플레이어 리스폰 실행
            ExecutePlayerRespawn();
        });

    // 네 번째 단계: 델리게이트 바인딩 해제 (작업 완료 후 정리)
    if (CachedLevelManager && CachedLevelManager->LevelTransitionCompleted.IsBound())
    {
        CachedLevelManager->LevelTransitionCompleted.Unbind();
        UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 완료 델리게이트 바인딩 해제 완료");
    }
}
#pragma endregion

//@유틸리티
#pragma region Utility
bool AAgeOfWolvesGameMode::DetermineDeathRules(APlayerController* PlayerController)
{
    if (!IsValid(PlayerController))
    {
        return false;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "사망 규칙 결정: {0}", GetNameSafe(PlayerController));

    // 사망 규칙 로직 (프로젝트 특성에 맞게 구현)
    return true;
}

bool AAgeOfWolvesGameMode::CalculateDeathRespawnLocation(APlayerController* PlayerController, FTransform& OutRespawnTransform)
{
    // 기존 ChoosePlayerStart 로직 활용
    AActor* PlayerStart = ChoosePlayerStart(PlayerController);
    if (!IsValid(PlayerStart))
    {
        return false;
    }

    OutRespawnTransform = PlayerStart->GetTransform();

    UE_LOGFMT(LogAOWGameMode, Log, "데스 리스폰 위치 계산 완료: {0}",
        *OutRespawnTransform.GetLocation().ToString());

    return true;
}

bool AAgeOfWolvesGameMode::CalculateLevelTransitionRespawnLocation(APlayerController* PlayerController, FTransform& OutRespawnTransform)
{
    if (!CachedLevelManager || !CachedGameState)
    {
        return false;
    }

    FGameplayTag TargetLevelTag = CachedGameState->GetNextLevelTag();
    if (!TargetLevelTag.IsValid())
    {
        return false;
    }

    FGameplayTag DefaultPlayerStartTag = CachedLevelManager->GetDefaultPlayerStartTagForLevel(TargetLevelTag);
    if (!DefaultPlayerStartTag.IsValid())
    {
        return false;
    }

    FString TargetTagString = DefaultPlayerStartTag.ToString();

    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* PlayerStart = *It;
        if (IsValid(PlayerStart) && PlayerStart->PlayerStartTag.ToString() == TargetTagString)
        {
            OutRespawnTransform = PlayerStart->GetTransform();

            UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 리스폰 위치 계산 완료: {0} (태그: {1})",
                *OutRespawnTransform.GetLocation().ToString(), *TargetTagString);

            return true;
        }
    }

    return false;
}

void AAgeOfWolvesGameMode::PrintGameModeDebugInfo()
{
    UE_LOGFMT(LogAOWGameMode, Log, "=== GameMode Debug Information ===");

    if (CachedGameState)
    {
        CachedGameState->PrintGameStateDebugInfo();
    }
    else
    {
        UE_LOGFMT(LogAOWGameMode, Log, "GameState: Not Available");
    }

    UE_LOGFMT(LogAOWGameMode, Log, "Cached UIManager: {0}", CachedUIManager ? TEXT("Valid") : TEXT("Invalid"));
    UE_LOGFMT(LogAOWGameMode, Log, "Cached LevelManager: {0}", CachedLevelManager ? TEXT("Valid") : TEXT("Invalid"));

    UE_LOGFMT(LogAOWGameMode, Log, "=== End Debug Information ===");
}
#pragma endregion