#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "04_Component/InventoryComponent.h"

#include "AOWSaveGame.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSaveGame, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UBaseAttributeSet;
class APlayerStateBase;
class ABaseAIController;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
/*
*	@FCharacterInformation
*
*	캐릭터 주요 정보를 담은 구조체
*/
USTRUCT(BlueprintType)
struct FCharacterInformation : public FTableRowBase
{
	GENERATED_BODY()

public:
	FCharacterInformation()
		: Actor(nullptr)
		, Location(FVector::ZeroVector)
		, AttributeSet(nullptr)
		, GameplayTimeOnEvent(0.0f)
	{}

	// Custom constructor to initialize more fields
	FCharacterInformation(
		const FGameplayTag& InStateTag,
		AActor* InActor = nullptr,
		const FVector& InLocation = FVector::ZeroVector,
		UBaseAttributeSet* InAttributeSet = nullptr,
		float InGameplayTimeOnEvent = 0.0f,
		const FString& InDescription = FString())
		: StateTag(InStateTag)
		, Actor(InActor)
		, Location(InLocation)
		, AttributeSet(InAttributeSet)
		, GameplayTimeOnEvent(InGameplayTimeOnEvent)
		, Description(InDescription)
	{}

	bool operator==(const FCharacterInformation& Other) const
	{
		return StateTag == Other.StateTag;
	}

	UPROPERTY()
		FGameplayTag StateTag;

	UPROPERTY()
		AActor* Actor;

	UPROPERTY()
		FVector Location;

	UPROPERTY()
		UBaseAttributeSet* AttributeSet;

	UPROPERTY()
		float GameplayTimeOnEvent;

	UPROPERTY()
		FString Description;
};

USTRUCT(BlueprintType)
struct FInventoryItemSaveInfo
{
	GENERATED_BODY()

public:
	FInventoryItemSaveInfo() {}
	FInventoryItemSaveInfo(const FGuid UniqueItemId, FString& ItemClasssPath, int32 ItemCount, int EnhancementCount)
	{
		this->UniqueItemId = UniqueItemId;
		this->ItemClassPath = ItemClasssPath;
		this->ItemCount = ItemCount;
		this->EnhancementCount = EnhancementCount;
	}
	//연산자 오버로딩
	const bool operator=(const FInventoryItemSaveInfo& Other)
	{
		return UniqueItemId == Other.UniqueItemId;
	}
public:
	//@FGuid
	UPROPERTY(BlueprintReadOnly, Category = "Item Save Info | Item")
		FGuid UniqueItemId;
	//@Item Class 경로
	UPROPERTY(BlueprintReadOnly, Category = "Item Save Info | Item")
		FString ItemClassPath = "";
	//@Item 갯수
	UPROPERTY(BlueprintReadOnly, Category = "Item Save Info | Item")
		int32 ItemCount = -1;
	//@강화 횟수
	UPROPERTY(BlueprintReadOnly, Category = "Item Save Info | Enhancement")
		int32 EnhancementCount = -1;

};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_MULTICAST_DELEGATE_OneParam(FCharacterStateEventToCache, FCharacterInformation)
#pragma endregion

/**
 *	@UAOWSaveGame
 * 
 *	AOW의 Save Game
 */
UCLASS()
class AGEOFWOLVES_API UAOWSaveGame : public USaveGame
{

//@친추 클래스
#pragma region Friend Class
	friend class APlayerStateBase;
	friend class ABaseAIController;
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UAOWSaveGame();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	void AddCharacterStateToHistory(
		const FGameplayTag& CharacterStateTag,
		AActor* Actor,
		UBaseAttributeSet* AttributeSet);

protected:
	//@캐싱해 둘 최대 캐릭터의 게임 플레이 이벤트 정보
	const int32 MaxStateHistorySize = 10;

	//@캐릭터 상태 변화 이벤트를 모아둔 큐
	TArray<FCharacterInformation> CharacterStateHistory;

public:
	TArray<FInventoryItemSaveInfo> InventoryItemSaveInfos;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	FCharacterStateEventToCache CharacterStateEventToCache;
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
#pragma endregion
};
