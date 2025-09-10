#pragma once
#include "CoreMinimal.h"
#include "02_AbilitySystem/02_GamePlayAbility/BaseGameplayAbility.h"

#include "ArmorGameplayAbility.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogArmorGA, Log, All);

//@전방 선언
#pragma region Forward Declaration
class ACharacterBase;
class ABaseAIController;
class UAIManagerSubsystem;
#pragma endregion

#pragma region Enum
#pragma endregion

#pragma region Structs
#pragma endregion

#pragma region Delegates
#pragma endregion

UCLASS()
class AGEOFWOLVES_API UArmorGameplayAbility : public UBaseGameplayAbility
{
	GENERATED_BODY()

	//@Default Setting
#pragma region Default Setting
public:
	UArmorGameplayAbility(const FObjectInitializer& ObjectInitializer);
#pragma endregion

	//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@기민함 수치 기반 회피 확률 계산
	float CalculateEvasionProbability(float AgilityValue);

protected:
	//@기본 회피 확률 (0.0 ~ 1.0)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "어빌리티 | 방어 설정",
		meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float BaseEvasionChance = 0.2f;

private:
	//@랜덤 회피 이벤트 전송
	void SendRandomEvasionEvent();

	//@ASC를 통한 게임플레이 이벤트 처리
	void HandleEvasionGameplayEvent(const FGameplayTag& EventTag, const FGameplayEventData& EventData);
#pragma endregion

	//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	//@AI Manager 서브시스템에서 기민함 수치 가져오기
	float GetAgilityValueFromAI() const;

	//@회피 여부 체크 (true 반환 시 자동으로 회피 이벤트 처리)
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 방어")
	bool ShouldEvade();

public:
	//@캐릭터 가져오기
	UFUNCTION(BlueprintCallable, Category = "어빌리티 | 방어")
	ACharacterBase* GetCharacterFromActorInfo() const;
#pragma endregion
};