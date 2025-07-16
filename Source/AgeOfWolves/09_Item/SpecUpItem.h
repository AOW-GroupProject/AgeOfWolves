#pragma once

#include "CoreMinimal.h"
#include "09_Item/Item.h"
#include "SpecUpItem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSpecUpItem, Log, All);

//@전방 선언
#pragma region Forward Declaration
class APlayerStateBase;
class UItemManagerSubsystem;
class UGameInstance;
#pragma endregion

/**
 * ASpecUpItem
 *
 * @목적: 캐릭터 능력치/어빌리티를 영구 향상시키는 아이템
 * @특징: 인벤토리 추가 즉시 효과 발동 후 소모
 */
UCLASS()
class AGEOFWOLVES_API ASpecUpItem : public AItem
{
	GENERATED_BODY()

	//@친구 클래스
#pragma region Friend Class
	friend class UInventoryComponent;
#pragma endregion

	//@기본 설정
#pragma region Default Setting
public:
	ASpecUpItem(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
#pragma endregion

	//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@SpecUp 아이템 활성화 처리
	virtual bool TryActivateItem_Implementation(UGameInstance* GameInstance = nullptr) override;
#pragma endregion

	//@Utility
#pragma region Utility
public:
	//@SpecUp 아이템 정보 가져오기
	const FSpecUpItemInformation* GetItemInformation(UGameInstance* GameInstance);

	//@어빌리티 부여 타입인지 확인
	UFUNCTION(BlueprintPure, Category = "SpecUp Item")
	bool IsAbilityGrantType() const;

	//@스탯 증가 타입인지 확인
	UFUNCTION(BlueprintPure, Category = "SpecUp Item")
	bool IsAttributeBoostType() const;

	//@어빌리티 개수 반환
	UFUNCTION(BlueprintPure, Category = "SpecUp Item")
	int32 GetAbilityCount() const;

private:
	//@PlayerState 가져오기
	APlayerStateBase* GetPlayerState(UGameInstance* GameInstance) const;
#pragma endregion
};