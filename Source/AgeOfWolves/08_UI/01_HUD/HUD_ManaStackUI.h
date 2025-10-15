#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "HUD_ManaStackUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogHUD_ManaStack, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UImage;
class UNiagaraSystemWidget;
class UAsyncTaskAttributeChanged;
class UAbilitySystemComponent;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

/**
 *	@마나 스택 UI
 * 
 *	HUD의 우중단에 위치한 마나 스택 이미지로 각 스택 별 서로 다른 이팩트 활성화(비순환)
 */
UCLASS()
class AGEOFWOLVES_API UHUD_ManaStackUI : public UUserWidget
{
	GENERATED_BODY()
	
//@Defualt Setting
#pragma region Default Setting
public:
	UHUD_ManaStackUI(const FObjectInitializer& ObjectInitializer);

protected:
	//~ Begin UUserWidget Interfaces
	virtual void NativeOnInitialized(); // 메모리 할당 완료, 화면에 렌더되기 전에 호출됨
	virtual void NativePreConstruct();
	virtual void NativeConstruct(); // 화면에 렌더되기 직전에 호출됨
	virtual void NativeDestruct();
	//~ End UUserWidget Interface

protected:
	//@외부 바인딩
	void ExternalBindingToAttributeSet();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
	//@나이아가라 활성화
	void ActivateNiagaraForStack(int32 StackLevel);

protected:
	//@나이아가라 이펙트 - 키(기)
	UPROPERTY(BlueprintReadWrite, Category = "마나 스택 | '기' VFX", meta = (BindWidget))
	UNiagaraSystemWidget* KI_NiagaraWidget;

	//@나이아가라 이펙트 - 쿄(흡)
	UPROPERTY(BlueprintReadWrite, Category = "마나 스택 | '흡' VFX", meta = (BindWidget))
	UNiagaraSystemWidget* KYU_NiagaraWidget;

	//@나이아가라 이펙트 - 큐(혼)
	UPROPERTY(BlueprintReadWrite, Category = "마나 스택 | '혼' VFX", meta = (BindWidget))
	UNiagaraSystemWidget* KON_NiagaraWidget;

protected:
	//@능력치 속성 변화 이벤트 구독 Async Task 목록
	UPROPERTY()
	TObjectPtr<UAsyncTaskAttributeChanged> ManaStackListener;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@Attribute 변화 이벤트 구독
	UFUNCTION()
	void OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
protected:
	//@ASC 캐시
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> CachedASC;
#pragma endregion

};
