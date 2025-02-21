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
    // 기본 유효성 검사
    //if (!IsValid(Actor))
    //{
    //    UE_LOGFMT(LogSaveGame, Warning, "AddCharacterStateToHistory 실패: 액터가 유효하지 않습니다.");
    //    return;
    //}

    //if (!GetWorld())
    //{
    //    UE_LOGFMT(LogSaveGame, Warning, "AddCharacterStateToHistory 실패: World가 유효하지 않습니다.");
    //    return;
    //}

    //// StateInfo 초기화
    //FCharacterInformation StateInfo(CharacterStateTag);
    //StateInfo.GameplayTimeOnEvent = UGameplayStatics::GetTimeSeconds(Actor->GetWorld());
    //StateInfo.Actor = Actor;
    //StateInfo.Location = Actor->GetActorLocation();

    //// AttributeSet 복제 시 안전성 확보
    //if (IsValid(AttributeSet))
    //{
    //    UObject* TransientPackage = GetTransientPackage();
    //    if (ensure(TransientPackage))
    //    {
    //        StateInfo.AttributeSet = Cast<UBaseAttributeSet>(DuplicateObject<UBaseAttributeSet>(
    //            AttributeSet,
    //            TransientPackage
    //            ));

    //        if (!StateInfo.AttributeSet)
    //        {
    //            UE_LOGFMT(LogSaveGame, Warning, "AttributeSet 복제 실패");
    //        }
    //    }
    //}

    //// 상태 태그에 따른 처리
    //if (CharacterStateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Dead")))
    //{
    //    StateInfo.Description = FString::Printf(TEXT("캐릭터 사망 - 시간: %.2f, 위치: %s"),
    //        StateInfo.GameplayTimeOnEvent,
    //        *StateInfo.Location.ToString());
    //    UE_LOGFMT(LogSaveGame, Log,
    //        "죽음 | Actor: {0}, Time: {1}, Location: ({2}, {3}, {4})",
    //        *Actor->GetName(),
    //        StateInfo.GameplayTimeOnEvent,
    //        StateInfo.Location.X,
    //        StateInfo.Location.Y,
    //        StateInfo.Location.Z);
    //}
    //else if (CharacterStateTag.MatchesTagExact(FGameplayTag::RequestGameplayTag("State.Spawn")))
    //{
    //    StateInfo.Description = FString::Printf(TEXT("캐릭터 스폰 - 시간: %.2f, 위치: %s"),
    //        StateInfo.GameplayTimeOnEvent,
    //        *StateInfo.Location.ToString());
    //    UE_LOGFMT(LogSaveGame, Log,
    //        "스폰 | Actor: {0}, Time: {1}, Location: ({2}, {3}, {4})",
    //        *Actor->GetName(),
    //        StateInfo.GameplayTimeOnEvent,
    //        StateInfo.Location.X,
    //        StateInfo.Location.Y,
    //        StateInfo.Location.Z);
    //}
    //else
    //{
    //    UE_LOGFMT(LogSaveGame, Warning, "처리되지 않은 상태 태그: {0}", *CharacterStateTag.GetTagName().ToString());
    //    return;
    //}

    //// 배열 크기 관리
    //if (CharacterStateHistory.Num() >= MaxStateHistorySize)
    //{
    //    CharacterStateHistory.RemoveAt(0);
    //}

    //// 상태 정보 추가 및 이벤트 브로드캐스트
    //CharacterStateHistory.AddUnique(StateInfo);

    //if (CharacterStateEventToCache.IsBound())
    //{
    //    CharacterStateEventToCache.Broadcast(StateInfo);
    //}
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