#include "AOWSaveGame.h"
#include "Logging/StructuredLog.h"

#include "Kismet/GameplayStatics.h"
#include "02_AbilitySystem/01_AttributeSet/BaseAttributeSet.h"

DEFINE_LOG_CATEGORY(LogSaveGame)
// UE_LOGFMT(LogSaveGame, Log, "");

//@Defualt Setting
#pragma region Default Setting
UAOWSaveGame::UAOWSaveGame()
{}
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
void UAOWSaveGame::AddCharacterStateToHistory(
    const FGameplayTag& CharacterStateTag,
    AActor* Actor,
    UBaseAttributeSet* AttributeSet)
{
    if (!Actor)
    {
        UE_LOGFMT(LogSaveGame, Warning, "AddCharacterStateToHistory 실패: 액터가 유효하지 않습니다.");
        return;
    }

    FCharacterInformation StateInfo(CharacterStateTag);
    StateInfo.GameplayTimeOnEvent = UGameplayStatics::GetTimeSeconds(Actor->GetWorld());
    StateInfo.Actor = Actor;
    StateInfo.Location = Actor->GetActorLocation();

    if (AttributeSet)
    {
        StateInfo.AttributeSet = Cast<UBaseAttributeSet>(DuplicateObject<UBaseAttributeSet>(
            AttributeSet,
            GetTransientPackage()
            ));
    }
    
    if (CharacterStateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead")))
    {
        StateInfo.Description = FString::Printf(TEXT("캐릭터 사망 - 시간: %.2f, 위치: %s"),
            StateInfo.GameplayTimeOnEvent,
            *StateInfo.Location.ToString());

        UE_LOGFMT(LogSaveGame, Log,
            "죽음 | Actor: {0}, Time: {1}, Location: ({2}, {3}, {4})",
            Actor->GetName(),
            StateInfo.GameplayTimeOnEvent,
            StateInfo.Location.X,
            StateInfo.Location.Y,
            StateInfo.Location.Z);
    }
    else if (CharacterStateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Spawn")))
    {
        StateInfo.Description = FString::Printf(TEXT("캐릭터 스폰 - 시간: %.2f, 위치: %s"),
            StateInfo.GameplayTimeOnEvent,
            *StateInfo.Location.ToString());

        UE_LOGFMT(LogSaveGame, Log,
            "스폰 | Actor: {0}, Time: {1}, Location: ({2}, {3}, {4})",
            Actor->GetName(),
            StateInfo.GameplayTimeOnEvent,
            StateInfo.Location.X,
            StateInfo.Location.Y,
            StateInfo.Location.Z);
    }
    else
    {
        UE_LOGFMT(LogSaveGame, Warning, "처리되지 않은 상태 태그: {0}", CharacterStateTag.GetTagName().ToString());
        return;
    }

    if(CharacterStateHistory.Num() >= MaxStateHistorySize)
    {
        CharacterStateHistory.RemoveAt(0);
    }

    CharacterStateHistory.AddUnique(StateInfo);

    CharacterStateEventToCache.Broadcast(StateInfo);
}
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion