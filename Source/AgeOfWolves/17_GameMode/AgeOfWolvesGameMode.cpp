#include "AgeOfWolvesGameMode.h"
#include "Logging/StructuredLog.h"

#include "UObject/ConstructorHelpers.h"

#include "01_Character/CharacterBase.h"
#include "03_Player/PlayerStateBase.h"
#include "17_GameMode/AOWGameState.h"
#include "14_Subsystem/UIManagerSubsystem.h"
#include "14_Subsystem/LevelManagerSubsystem.h"

#include "16_Level/Area.h"
#include "EngineUtils.h" // For TActorIterator
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY(LogAOWGameMode)

//@Defualt Setting
#pragma region Default Setting
AAgeOfWolvesGameMode::AAgeOfWolvesGameMode()
{}

void AAgeOfWolvesGameMode::ExternalBindingToUIManager()
{
    UE_LOGFMT(LogAOWGameMode, Log, "UI Manager와의 외부 바인딩 시작");

    //@Game Mode는 World를 통해 Game Instance에 접근할 수 있음
    UGameInstance* GameInstance = GetGameInstance();
    if (!IsValid(GameInstance))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "Game Instance를 찾을 수 없어 UI Manager 바인딩 실패");
        return;
    }

    //@Subsystem은 게임 전체에서 하나만 존재하므로 안전하게 접근 가능
    UUIManagerSubsystem* UIManager = GameInstance->GetSubsystem<UUIManagerSubsystem>();
    if (!IsValid(UIManager))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "UI Manager Subsystem을 찾을 수 없어 바인딩 실패");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "UI Manager Subsystem 획득 성공: {0}", GetNameSafe(UIManager));


    UIManager->LoadingUIFadeInComplete.AddUFunction(this, "OnLoadingUIFadeInComplete");

    UE_LOGFMT(LogAOWGameMode, Log, "UI Manager LoadingUIFadeInComplete 이벤트 바인딩 성공");
}

void AAgeOfWolvesGameMode::ExternalBindingToLevelManager()
{
    //@Game Mode는 World를 통해 Game Instance에 접근할 수 있음
    auto* GameInstance = GetGameInstance();
    if (!IsValid(GameInstance))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "Game Instance를 찾을 수 없어 Level Manager 바인딩 실패");
        return;
    }

    //@Subsystem은 게임 전체에서 하나만 존재하므로 안전하게 접근 가능
    auto* LevelManager = GameInstance->GetSubsystem<ULevelManagerSubsystem>();
    if (!IsValid(LevelManager))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "Level Manager Subsystem을 찾을 수 없어 바인딩 실패");
        return;
    }

    //@외부 바인딩...
    LevelManager->LevelTransitionCompleted.BindUFunction(this, "OnLevelTransitionCompleted");
}

void AAgeOfWolvesGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
    //@외부 바인딩...
    ExternalBindingToUIManager();
    ExternalBindingToLevelManager();

    Super::InitGame(MapName, Options, ErrorMessage);
}

AActor* AAgeOfWolvesGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
    if (ULevelManagerSubsystem* LevelManager = GetGameInstance()->GetSubsystem<ULevelManagerSubsystem>())
    {
        FGameplayTag CurrentLevel = LevelManager->GetCurrentLevelTag();

        if (CurrentLevel.IsValid())
        {
            FGameplayTag DefaultPlayerStartTag = LevelManager->GetDefaultPlayerStartTagForLevel(CurrentLevel);

            if (DefaultPlayerStartTag.IsValid())
            {
                FString TargetTag = DefaultPlayerStartTag.ToString();

                //@APlayerStart로 직접 검색
                for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
                {
                    APlayerStart* PlayerStart = *It;
                    if (PlayerStart && PlayerStart->PlayerStartTag.ToString() == TargetTag)
                    {
                        UE_LOGFMT(LogAOWGameMode, Log,
                            "레벨 전환 리스폰: PlayerStart 발견 - {0}", *TargetTag);
                        return PlayerStart;
                    }
                }

                UE_LOGFMT(LogAOWGameMode, Warning,
                    "태그 '{0}'와 매칭되는 PlayerStart를 찾지 못함", *TargetTag);
            }
        }
    }

    return Super::ChoosePlayerStart_Implementation(Player);
}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void AAgeOfWolvesGameMode::PlayerRespawn()
{
    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 리스폰 시작 - 현재 상태: {0}",
        static_cast<int32>(CurrentState));

    // 기본 유효성 검사 - 가장 중요한 전제 조건들을 먼저 확인합니다
    if (!CachedDeadPlayerController.IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Error, "죽은 플레이어 정보가 유효하지 않음");
        CurrentState = EGameModeState::Normal;
        return;
    }

    APlayerController* PlayerController = CachedDeadPlayerController.Get();

    // 추가 유효성 검사 - 게임 환경이 리스폰을 수행할 수 있는 상태인지 확인합니다
    if (!IsValid(PlayerController) || !IsValid(GetWorld()))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "PlayerController 또는 World가 유효하지 않음");
        CurrentState = EGameModeState::Normal;
        CachedDeadPlayerController.Reset();
        return;
    }

    // 현재 Pawn 확인 - 리스폰할 대상이 존재하는지 검증합니다
    APawn* CurrentPawn = PlayerController->GetPawn();
    if (!IsValid(CurrentPawn))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "현재 Pawn이 유효하지 않음");
        CurrentState = EGameModeState::Normal;
        CachedDeadPlayerController.Reset();
        return;
    }

    // 상태에 따른 리스폰 위치 결정 - 이 부분이 핵심 로직입니다
    FTransform RespawnTransform;
    bool bFoundRespawnLocation = false;

    if (CurrentState == EGameModeState::PlayerDeath)
    {
        // 죽음으로 인한 리스폰: 기존 ChoosePlayerStart 로직 사용
        // 이는 레벨 디자이너가 배치한 적절한 PlayerStart를 선택하는 방식입니다
        bFoundRespawnLocation = FindDeathRespawnLocation(PlayerController, RespawnTransform);
        UE_LOGFMT(LogAOWGameMode, Log, "죽음 리스폰 위치 검색 결과: {0}",
            bFoundRespawnLocation ? TEXT("성공") : TEXT("실패"));
    }
    else if (CurrentState == EGameModeState::LevelTransition)
    {
        // 레벨 전환으로 인한 리스폰: DefaultPlayerStartTag 기반 위치 사용
        // 이는 정확한 입구 지점으로 플레이어를 배치하는 방식입니다
        bFoundRespawnLocation = FindLevelTransitionRespawnLocation(PlayerController, RespawnTransform);
        UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 리스폰 위치 검색 결과: {0}",
            bFoundRespawnLocation ? TEXT("성공") : TEXT("실패"));
    }
    else
    {
        UE_LOGFMT(LogAOWGameMode, Error, "예상하지 못한 상태에서 PlayerRespawn 호출: {0}",
            static_cast<int32>(CurrentState));
        CurrentState = EGameModeState::Normal;
        CachedDeadPlayerController.Reset();
        return;
    }

    // 리스폰 위치를 찾지 못한 경우의 폴백 처리
    if (!bFoundRespawnLocation)
    {
        UE_LOGFMT(LogAOWGameMode, Error, "리스폰 위치를 찾을 수 없음 - 기본 위치 사용");

        // 기본 위치로 폴백 (월드 원점에서 약간 위)
        RespawnTransform = FTransform(FRotator::ZeroRotator, FVector(0, 0, 100), FVector::OneVector);
    }

    // 상태를 먼저 초기화 (순환 호출 방지 및 상태 정리)
    EGameModeState PreviousState = CurrentState;
    CurrentState = EGameModeState::Normal;
    CachedDeadPlayerController.Reset();

    // 실제 위치 이동 수행 - RestartPlayer 대신 직접 텔레포트를 사용합니다
    bool bTeleportSuccess = PerformPlayerTeleport(CurrentPawn, RespawnTransform);

    if (!bTeleportSuccess)
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "플레이어 텔레포트 실패 - 강제 위치 설정 시도");

        // 강제 위치 설정 시도 - 마지막 수단으로 직접 위치를 설정합니다
        CurrentPawn->SetActorLocation(RespawnTransform.GetLocation(), false);
        CurrentPawn->SetActorRotation(RespawnTransform.GetRotation());
    }

    // 플레이어 상태 복구 (체력, 능력 등)
    ResetPlayerGameplayState(PlayerController);

    // 게임 시스템들에게 리스폰 완료 알림
    if (AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>())
    {
        CurrentGameState->NotifyRequestHideLoadingUI();
        CurrentGameState->NotifyPlayerRespawnCompleted(PlayerController);
    }

    // 입력 시스템 복구 - 리스폰이 완료되면 플레이어가 다시 조작할 수 있도록 합니다
    PlayerController->EnableInput(PlayerController);

    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 리스폰 완료 - 이전 상태: {0}, 최종 위치: {1}",
        static_cast<int32>(PreviousState), *RespawnTransform.GetLocation().ToString());
}

void AAgeOfWolvesGameMode::HandlePlayerDeath(APlayerController* PlayerController)
{
    if (!IsValid(PlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "플레이어 죽음 처리 - 유효하지 않은 PlayerController입니다");
        return;
    }

    PlayerController->DisableInput(PlayerController);
    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 입력 차단됨: {0}", GetNameSafe(PlayerController));

    //@죽음 상태로 설정 및 플레이어 캐싱
    CurrentState = EGameModeState::PlayerDeath;
    CachedDeadPlayerController = PlayerController;

    // 기존 코드와 동일
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

void AAgeOfWolvesGameMode::HandleFirstStructureActivation(const FStructureData& StructureData)
{
    UE_LOGFMT(LogAOWGameMode, Log, "구조물 상호작용 활성화됨: {0}", *StructureData.GetStructureName().ToString());

    if (!StructureData.GetNextLevelTag().IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "구조물에 다음 레벨 태그가 설정되지 않음: {0}",
            *StructureData.GetStructureName().ToString());
        return;
    }

    //@모든 플레이어 입력 차단
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        if (APlayerController* PC = Iterator->Get())
        {
            PC->DisableInput(PC);
        }
    }

    //@레벨 전환 상태로 설정
    CurrentState = EGameModeState::LevelTransition;
    CachedNextLevelTag = StructureData.GetNextLevelTag();

    AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>();
    if (!IsValid(CurrentGameState))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "Game State를 찾을 수 없어 레벨 전환 처리 불가");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 정보 캐싱 완료 - 다음: {0}", *CachedNextLevelTag.ToString());

    CurrentGameState->NotifyRequestShowLoadingUI();
    UE_LOGFMT(LogAOWGameMode, Log, "구조물 상호작용에 의한 로딩 UI 표시 요청 완료");
}

bool AAgeOfWolvesGameMode::PerformPlayerTeleport(APawn* PlayerPawn, const FTransform& TargetTransform)
{
    // 안전한 텔레포트 수행 - RestartPlayer의 복잡성을 피하면서도 안정적인 이동을 보장합니다
    // ETeleportType::TeleportPhysics를 사용하여 물리 시뮬레이션도 적절히 처리합니다

    if (!IsValid(PlayerPawn))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "텔레포트할 Pawn이 유효하지 않음");
        return false;
    }

    // SetActorTransform으로 시도 - 이는 가장 안전하고 직접적인 방법입니다
    bool bSuccess = PlayerPawn->SetActorTransform(TargetTransform, false, nullptr, ETeleportType::TeleportPhysics);

    if (bSuccess)
    {
        UE_LOGFMT(LogAOWGameMode, Log, "플레이어 텔레포트 성공: {0}",
            *TargetTransform.GetLocation().ToString());

        // 텔레포트 후 물리 상태 안정화 - 이전 움직임의 관성을 제거합니다
        if (UPrimitiveComponent* RootComp = Cast<UPrimitiveComponent>(PlayerPawn->GetRootComponent()))
        {
            RootComp->SetPhysicsLinearVelocity(FVector::ZeroVector);
            RootComp->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
        }

        return true;
    }
    else
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "SetActorTransform 실패 - 개별 위치/회전 설정 시도");
        return false;
    }
}

void AAgeOfWolvesGameMode::ResetPlayerGameplayState(APlayerController* PlayerController)
{
    // 플레이어의 게임플레이 관련 상태를 초기화합니다
    // 이 함수는 리스폰 후 플레이어가 깨끗한 상태에서 게임을 재개할 수 있도록 도와줍니다

    if (!IsValid(PlayerController))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "PlayerController가 유효하지 않아 상태 초기화 생략");
        return;
    }

    APawn* PlayerPawn = PlayerController->GetPawn();
    if (!IsValid(PlayerPawn))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "PlayerPawn이 유효하지 않아 상태 초기화 생략");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 게임플레이 상태 초기화 시작");

    // 여기에 실제 게임플레이 상태 초기화 로직을 추가할 수 있습니다
    // 이는 프로젝트의 특성에 맞게 커스터마이징할 수 있는 확장 지점입니다

    // 예시 코드 (주석 처리):
    // if (auto CharacterBase = Cast<ACharacterBase>(PlayerPawn))
    // {
    //     CharacterBase->RestoreFullHealth();
    //     CharacterBase->ClearAllDebuffs();
    //     CharacterBase->ResetAbilities();
    // }

    UE_LOGFMT(LogAOWGameMode, Log, "플레이어 게임플레이 상태 초기화 완료");
}
#pragma endregion

//@Callbacks...
#pragma region Callbacks
void AAgeOfWolvesGameMode::OnLoadingUIFadeInComplete()
{
    // 함수 호출 시점의 GameMode 상태를 먼저 로그 출력
    UE_LOGFMT(LogAOWGameMode, Log, "OnLoadingUIFadeInComplete 호출됨 - 현재 상태: {0}",
        static_cast<int32>(CurrentState));

    if (CurrentState == EGameModeState::PlayerDeath)
    {
        UE_LOGFMT(LogAOWGameMode, Log, "PlayerDeath 상태 - AsyncTask로 리스폰 처리 시작");

        AsyncTask(ENamedThreads::GameThread, [this]()
            {
                UE_LOGFMT(LogAOWGameMode, Log, "AsyncTask 내부 - PlayerRespawn 실행");

                PlayerRespawn();
                
            });
    }
    else if (CurrentState == EGameModeState::LevelTransition)
    {
        UE_LOGFMT(LogAOWGameMode, Log, "LevelTransition 상태 - AsyncTask로 레벨 전환 처리 시작");

        AsyncTask(ENamedThreads::GameThread, [this]()
            {
                UE_LOGFMT(LogAOWGameMode, Log, "AsyncTask 내부 - 레벨 전환 요청 시작");

                if (IsValid(this) && CachedNextLevelTag.IsValid())
                {
                    if (AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>())
                    {
                        CurrentGameState->NotifyRequestStartLevelTransition(CachedNextLevelTag);
                        CachedNextLevelTag = FGameplayTag();
                    }
                }
            });
    }
    else
    {
        // 예상하지 못한 상태에서 호출된 경우를 감지
        UE_LOGFMT(LogAOWGameMode, Warning, "예상하지 못한 상태에서 OnLoadingUIFadeInComplete 호출됨: {0}",
            static_cast<int32>(CurrentState));
    }
}

void AAgeOfWolvesGameMode::OnLevelTransitionCompleted(const FGameplayTag& CompletedLevelTag)
{
    UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 완료: {0}", *CompletedLevelTag.ToString());

    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        if (APlayerController* PC = Iterator->Get())
        {
            RestartPlayer(PC);

            if (AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>())
            {
                CurrentGameState->NotifyPlayerRespawnCompleted(PC);
            }

            //@입력 해제
            PC->EnableInput(PC);
        }
    }

    //@로딩 UI 숨기기
    if (AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>())
    {
        //GameState->NotifyHideLoadingUI()
    }

    //@상태 초기화
    CurrentState = EGameModeState::Normal;

    UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 후 처리 완료");
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

bool AAgeOfWolvesGameMode::FindDeathRespawnLocation(APlayerController* PlayerController, FTransform& OutRespawnTransform)
{
    // 죽음으로 인한 리스폰에서는 기존의 ChoosePlayerStart 로직을 활용합니다
    // 이 방식의 장점은 레벨 디자이너가 의도한 안전한 위치들 중에서 선택한다는 점입니다

    AActor* PlayerStart = ChoosePlayerStart(PlayerController);
    if (!IsValid(PlayerStart))
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "ChoosePlayerStart에서 적절한 위치를 찾지 못함");
        return false;
    }

    OutRespawnTransform = PlayerStart->GetTransform();

    UE_LOGFMT(LogAOWGameMode, Log, "죽음 리스폰 위치 확정: {0} (PlayerStart: {1})",
        *OutRespawnTransform.GetLocation().ToString(), *PlayerStart->GetName());

    return true;
}

bool AAgeOfWolvesGameMode::FindLevelTransitionRespawnLocation(APlayerController* PlayerController, FTransform& OutRespawnTransform)
{
    // 레벨 전환에서는 LevelManager에서 제공하는 DefaultPlayerStartTag를 정확히 활용합니다
    // 이는 레벨 간 이동 시 플레이어가 정확한 입구 지점에 나타나도록 보장합니다

    // LevelManager에서 현재 레벨 정보 가져오기
    ULevelManagerSubsystem* LevelManager = GetGameInstance()->GetSubsystem<ULevelManagerSubsystem>();
    if (!IsValid(LevelManager))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "LevelManager Subsystem을 찾을 수 없음");
        return false;
    }

    // 현재 레벨의 태그 가져오기
    FGameplayTag CurrentLevelTag = LevelManager->GetCurrentLevelTag();
    if (!CurrentLevelTag.IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Error, "현재 레벨 태그가 유효하지 않음");
        return false;
    }

    // 현재 레벨의 기본 PlayerStart 태그 가져오기
    FGameplayTag DefaultPlayerStartTag = LevelManager->GetDefaultPlayerStartTagForLevel(CurrentLevelTag);
    if (!DefaultPlayerStartTag.IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Error, "현재 레벨의 기본 PlayerStart 태그가 유효하지 않음: {0}",
            *CurrentLevelTag.ToString());
        return false;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 리스폰: 레벨 '{0}'의 PlayerStart 태그 '{1}' 검색 중",
        *CurrentLevelTag.ToString(), *DefaultPlayerStartTag.ToString());

    // 해당 태그를 가진 PlayerStart 찾기 - 정확한 매칭이 중요합니다
    FString TargetTagString = DefaultPlayerStartTag.ToString();

    for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
    {
        APlayerStart* PlayerStart = *It;
        if (IsValid(PlayerStart))
        {
            // PlayerStart의 태그와 정확히 비교합니다
            if (PlayerStart->PlayerStartTag.ToString() == TargetTagString)
            {
                OutRespawnTransform = PlayerStart->GetTransform();

                UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 리스폰 위치 확정: {0} (PlayerStart: {1}, 태그: {2})",
                    *OutRespawnTransform.GetLocation().ToString(),
                    *PlayerStart->GetName(),
                    *TargetTagString);

                return true;
            }
        }
    }

    UE_LOGFMT(LogAOWGameMode, Warning, "태그 '{0}'와 매칭되는 PlayerStart를 찾지 못함",
        *TargetTagString);
    return false;
}

#pragma endregion