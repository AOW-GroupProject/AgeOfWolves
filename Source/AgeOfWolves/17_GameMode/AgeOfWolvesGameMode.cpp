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

    //@UFunction으로 선언된 함수는 FName으로 바인딩해야 함
    LoadingUIFadeInCompleteHandle = UIManager->LoadingUIFadeInComplete.AddUFunction(
        this, FName("OnLoadingUIFadeInComplete")
    );

    //@바인딩 성공 여부 확인
    if (!LoadingUIFadeInCompleteHandle.IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Log, "UI Manager LoadingUIFadeInComplete 이벤트 바인딩 성공");
    }

    UE_LOGFMT(LogAOWGameMode, Warning, "UI Manager LoadingUIFadeInComplete 이벤트 바인딩 실패");
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

void AAgeOfWolvesGameMode::NotifyStartLevelTransition()
{
    UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 알림 시작 - 대상 레벨: {0}", *CachedNextLevelTag.ToString());

    //@Game State 가져오기
    AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>();
    if (!IsValid(CurrentGameState))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "Game State를 찾을 수 없어 레벨 전환 알림 실패");
        return;
    }

    UE_LOGFMT(LogAOWGameMode, Log, "Game State 확인 완료: {0}", GetNameSafe(CurrentGameState));

    //@Game State를 통해 레벨 전환 요청
    //@이 시점에서 실제 레벨 스트리밍이나 전환 로직이 시작됨
    CurrentGameState->NotifyRequestStartLevelTransition(CachedNextLevelTag);

    UE_LOGFMT(LogAOWGameMode, Log, "Game State에 레벨 전환 요청 완료: {0}", *CachedNextLevelTag.ToString());

    //@레벨 전환 요청 후 캐싱된 태그들을 정리
    CachedNextLevelTag = FGameplayTag();

    UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 캐시 정리 완료");
}
#pragma endregion

//@Callbacks...
#pragma region Callbacks
void AAgeOfWolvesGameMode::OnStructureInteractionActtivated(const FStructureData& StructureData)
{
    UE_LOGFMT(LogAOWGameMode, Log, "구조물 상호작용 활성화됨: {0}", *StructureData.GetStructureName().ToString());

    //@구조물 데이터 유효성 검사
    //@다음 레벨 태그가 비어있으면 레벨 전환이 불가능
    if (!StructureData.GetNextLevelTag().IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "구조물에 다음 레벨 태그가 설정되지 않음: {0}",
            *StructureData.GetStructureName().ToString());
        return;
    }

    //@현재 레벨 태그 가져오기
    //@Game State에서 현재 레벨 정보를 관리하고 있다고 가정
    AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>();
    if (!IsValid(CurrentGameState))
    {
        UE_LOGFMT(LogAOWGameMode, Error, "Game State를 찾을 수 없어 레벨 전환 처리 불가");
        return;
    }

    //@실제 구현에서는 Game State의 함수명에 맞게 수정 필요
    CachedNextLevelTag = StructureData.GetNextLevelTag();

    UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 정보 캐싱 완료 - 다음: {0}", *CachedNextLevelTag.ToString());

    //@로딩 UI 표시 요청
    CurrentGameState->NotifyRequestShowLoadingUI();
    UE_LOGFMT(LogAOWGameMode, Log, "구조물 상호작용에 의한 로딩 UI 표시 요청 완료");
}

void AAgeOfWolvesGameMode::OnLoadingUIFadeInComplete()
{
    UE_LOGFMT(LogAOWGameMode, Log, "Game Mode: 로딩 UI Fade-In 완료 이벤트 수신됨");

    // 캐싱된 레벨 태그 유효성 검사 (빠른 검사만 수행)
    if (!CachedNextLevelTag.IsValid())
    {
        UE_LOGFMT(LogAOWGameMode, Warning, "캐싱된 레벨 태그가 유효하지 않음 - 레벨 전환 중단");
        return;
    }

    // 여기서 즉시 return! UI Manager는 블로킹되지 않고 계속 진행할 수 있음
    UE_LOGFMT(LogAOWGameMode, Log, "비동기 레벨 전환 프로세스 시작 - UI 스레드는 해제됨");

    // AsyncTask로 실제 레벨 전환 작업을 백그라운드에서 처리
    AsyncTask(ENamedThreads::GameThread, [this]()
        {
            UE_LOGFMT(LogAOWGameMode, Log, "백그라운드에서 레벨 전환 처리 시작");

            //@이 부분은 별도 스레드에서 실행되므로 UI를 블로킹하지 않음
            NotifyStartLevelTransition();
        });
}

void AAgeOfWolvesGameMode::OnLevelTransitionCompleted(const FGameplayTag& CompletedLevelTag)
{
    UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 완료: {0}", *CompletedLevelTag.ToString());

    //@모든 플레이어 리스폰
    for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
    {
        if (APlayerController* PC = Iterator->Get())
        {
            UE_LOGFMT(LogAOWGameMode, Log, "레벨 전환 후 플레이어 리스폰: {0}", GetNameSafe(PC));

            RestartPlayer(PC);

            //@Game State에 리스폰 완료 알림
            if (AAOWGameState* CurrentGameState = GetGameState<AAOWGameState>())
            {
                CurrentGameState->NotifyPlayerRespawnCompleted(PC);
            }
        }
    }

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
#pragma endregion