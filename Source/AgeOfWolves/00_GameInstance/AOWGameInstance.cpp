#include "AOWGameInstance.h"
#include "Logging/StructuredLog.h"
#include "Kismet/GameplayStatics.h"

#include "15_SaveGame/AOWSaveGame.h"

DEFINE_LOG_CATEGORY(LogGameInstance)
 //UE_LOGFMT(LogGameInstance, Log, "");

UAOWGameInstance::UAOWGameInstance()
{}

void UAOWGameInstance::Init()
{
    Super::Init();

    //@Save Game Instance
    if (DoesSaveGameExist())
    {
        UE_LOGFMT(LogGameInstance, Log, "기존 세이브 파일 존재 확인 - 파일명: {0}, 인덱스: {1}",
            SaveFileName, SaveIndex);

        SaveGameInstance = Cast<UAOWSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveFileName, SaveIndex));

        if (!SaveGameInstance)
        {
            UE_LOGFMT(LogGameInstance, Warning, "세이브 파일 로드 실패 - 새 인스턴스 생성 시도");
            SaveGameInstance = CreateNewSaveGameInstance();
        }
        else
        {
            UE_LOGFMT(LogGameInstance, Log, "세이브 파일 로드 성공");
        }
    }
    else
    {
        UE_LOGFMT(LogGameInstance, Log, "세이브 파일이 존재하지 않음 - 새 인스턴스 생성");
        SaveGameInstance = CreateNewSaveGameInstance();
    }

    //@Loading
    {
        FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UAOWGameInstance::PreLoadMapEvent);
        FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UAOWGameInstance::PostLoadMapEvent);
        UE_LOGFMT(LogGameInstance, Log, "로딩 이벤트 델리게이트 바인딩 완료");
    }
}

void UAOWGameInstance::Shutdown()
{
    Super::Shutdown();

    //@Save Game 정리
    if (SaveGameInstance)
    {
        UE_LOGFMT(LogGameInstance, Log, "세이브 게임 인스턴스 정리");
        SaveGameInstance = nullptr;
    }
}

UAOWSaveGame* UAOWGameInstance::CreateNewSaveGameInstance()
{
    SaveGameInstance = Cast<UAOWSaveGame>(UGameplayStatics::CreateSaveGameObject(UAOWSaveGame::StaticClass()));

    if (SaveGameInstance)
    {
        UE_LOGFMT(LogGameInstance, Log, "새로운 세이브 게임 인스턴스 생성 성공");
    }
    else
    {
        UE_LOGFMT(LogGameInstance, Error, "새로운 세이브 게임 인스턴스 생성 실패");
    }

    return SaveGameInstance;
}

bool UAOWGameInstance::DoesSaveGameExist()
{
    bool bExists = UGameplayStatics::DoesSaveGameExist(SaveFileName, SaveIndex);
    UE_LOGFMT(LogGameInstance, Log, "세이브 파일 존재 여부 확인 - 파일명: {0}, 인덱스: {1}, 결과: {2}",
        SaveFileName, SaveIndex, bExists ? TEXT("존재") : TEXT("없음"));
    return bExists;
}

UAOWSaveGame* UAOWGameInstance::GetSaveGameInstance()
{
    if (!SaveGameInstance.Get())
    {
        UE_LOGFMT(LogGameInstance, Warning, "세이브 게임 인스턴스가 없음 - 새 인스턴스 생성 시도");
        return CreateNewSaveGameInstance();
    }

    return SaveGameInstance.Get();
}

void UAOWGameInstance::PreLoadMapEvent(const FString& LevelName)
{
    UE_LOGFMT(LogGameInstance, Log, "맵 로딩 시작 - 레벨: {0}", *LevelName);
    //@TODO: Show Loading Screen Widget
}

void UAOWGameInstance::PostLoadMapEvent(UWorld* LoadedWorld)
{
    UE_LOGFMT(LogGameInstance, Log, "맵 로딩 완료 - 월드: {0}",
        LoadedWorld ? *LoadedWorld->GetName() : TEXT("Invalid"));
    //@TODO: Collapse Loading Screen Widget    
}