#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "Components/ProgressBar.h"
#include "Engine/Engine.h"
#include "Kismet/KismetMathLibrary.h"

#include "HUD_StatusUI.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogStatusUI, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UVerticalBox;
class UHorizontalBox;
class UAsyncTaskAttributeChanged;
class UAbilitySystemComponent;
class APlayerStateBase;
class UHorizontalDotGauge;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
/*
*   @FInterpolationSettings
* 
*   ProgressBar 보간 설정
*/
USTRUCT(BlueprintType)
struct FInterpolationSettings
{
    GENERATED_BODY()

    //@보간 지속 시간
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interpolation", meta = (ClampMin = "0.0", ClampMax = "5.0"))
        float Duration = 0.5f;

    //@보간 함수 타입
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interpolation")
        TEnumAsByte<EEasingFunc::Type> EasingFunction = EEasingFunc::EaseOut;

    //@보간 지수 (일부 이징 함수에서 사용)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interpolation", meta = (ClampMin = "0.0", ClampMax = "10.0"))
        float Exponent = 2.0f;

    //@보간 활성화 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interpolation")
        bool bEnabled = true;
        
    //@커스텀 지수적 이징 함수 사용 여부
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interpolation")
        bool bUseExponentialEasing = false;
};

/*
*   @FInterpolationData
* 
*   현재 진행 중인 보간 데이터
*/
USTRUCT()
struct FInterpolationData
{
    GENERATED_BODY()

    //@시작 값
    float StartValue = 0.f;
    
    //@목표 값
    float TargetValue = 0.f;
    
    //@현재 진행 시간
    float CurrentTime = 0.f;
    
    //@총 지속 시간
    float Duration = 0.f;
    
    //@보간 설정
    FInterpolationSettings Settings;
    
    //@보간 활성화 여부
    bool bIsActive = false;
    
    //@보간 완료 여부
    bool bIsCompleted = false;
};

struct FStateBarInfo
{
    UProgressBar* MainBar = nullptr;
    float CurrentValue = 0.f;
    float DamageValue = 0.f;
    FLinearColor OriginalColor;
    FTimerHandle DamageColorTimer;
    
    //@보간 관련
    FInterpolationData InterpolationData;
    FTimerHandle InterpolationTimer;

    void Reset()
    {
        CurrentValue = 0.f;
        DamageValue = 0.f;
        if (MainBar)
        {
            MainBar->SetFillColorAndOpacity(OriginalColor);
        }
        
        //@보간 데이터 리셋
        InterpolationData = FInterpolationData();
        if (InterpolationTimer.IsValid())
        {
            // 타이머는 외부에서 클리어해야 함
        }
    }
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
DECLARE_MULTICAST_DELEGATE(FRequestStartInitByStatusUI)

DECLARE_DELEGATE(FStatusUIInitFinished);
#pragma endregion

/**
 * @UHUD_StatusUI
 * 
 * HUD의 캐릭터 상태 창을 나타내는 UI입니다.
 * 
 * 2개의 Status Bar와 Memmory(Item) 관련 이미지로 구성합니다.
 */
UCLASS()
class AGEOFWOLVES_API UHUD_StatusUI : public UUserWidget
{
//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UHUD_StatusUI(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
    //~ End UUserWidget Interface

protected:
    //@내부 바인딩
    void InternalBindToManaDotGauge(UHorizontalDotGauge* ManaDotGauge);

protected:
    //@외부 바인딩
    void ExternalBindingToAttributeSet();

public:
    //@초기화
    UFUNCTION()
        void InitializeStatusUI();

public:
    bool bManaDotGaugeInitFinished = false;
    void CheckAllUIsInitFinished();
#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
protected:
    //@Memory 아이템 관련 이미지 생성 및 설정 작업
    void CreateAndSetupMemoryImage();
    //@Mana Gauge 생성 작업
    void CreateManaDotGauge();

protected:
    //@Status UI의 State Bar 업데이트
    bool UpdateStateBarAttribute(const FGameplayAttribute& Attribute, float OldValue, float NewValue, APlayerStateBase* PS);
    //@Mana Dot Gauge 업데이트
    void UpdateManaAttribute(const FGameplayAttribute& Attribute, float NewValue);
    
    //@보간 관련 메서드
    void StartInterpolation(FStateBarInfo& BarInfo, float StartValue, float TargetValue, const FInterpolationSettings& Settings);
    void UpdateInterpolation(FStateBarInfo& BarInfo, float DeltaTime);
    void CompleteInterpolation(FStateBarInfo& BarInfo);
    float CalculateInterpolatedValue(const FInterpolationData& Data) const;
    
    //@성능 최적화 메서드
    bool ShouldUpdateAttribute(const FString& AttributeName, float NewValue) const;
    void CachePlayerState();
    void OptimizeUpdateFrequency();
    
    //@리팩토링: 유틸리티 메서드
    bool ValidateAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) const;
    void LogAttributeChange(const FString& AttributeName, float OldValue, float NewValue) const;
    void CleanupInterpolationTimers();
    
    //@Blueprint 노출 메서드
public:
    UFUNCTION(BlueprintCallable, Category = "Status UI | Interpolation")
        void SetHealthInterpolationSettings(const FInterpolationSettings& NewSettings);
        
    UFUNCTION(BlueprintCallable, Category = "Status UI | Interpolation")
        void SetStaminaInterpolationSettings(const FInterpolationSettings& NewSettings);
        
    UFUNCTION(BlueprintCallable, Category = "Status UI | Interpolation")
        FInterpolationSettings GetHealthInterpolationSettings() const { return HealthInterpolationSettings; }
        
    UFUNCTION(BlueprintCallable, Category = "Status UI | Interpolation")
        FInterpolationSettings GetStaminaInterpolationSettings() const { return StaminaInterpolationSettings; }
        
    UFUNCTION(BlueprintCallable, Category = "Status UI | Performance")
        void SetMaxUpdateFrequency(float NewFrequency);
        
    UFUNCTION(BlueprintCallable, Category = "Status UI | Performance")
        float GetMaxUpdateFrequency() const { return MaxUpdateFrequency; }

protected:
    UPROPERTY(BlueprintReadWrite, Category = "Status UI", meta = (BindWidget))
        UVerticalBox* StateBarVerticalBox;

    UPROPERTY(BlueprintReadWrite, Category = "Status UI | Status Bar", meta = (BindWidget))
        UProgressBar* HP;

    UPROPERTY(BlueprintReadWrite, Category = "Status UI | Status Bar", meta = (BindWidget))
        UProgressBar* SP;

    UPROPERTY(BlueprintReadWrite, Category = "Status UI | Mana", meta = (BindWidget))
        UOverlay* ManaDotGaugeOverlay;

protected:
   //@AttributeName, FStateBarInfo
    TMap<FString, FStateBarInfo> MStateBars;

    //@데미지 표시 관련 상수
    const float DAMAGE_COLOR_DURATION = 0.3f;
    const FLinearColor DAMAGE_COLOR = FLinearColor(1.0f, 0.8f, 0.0f, 1.0f);
    
    //@보간 설정
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status UI | Interpolation")
        FInterpolationSettings HealthInterpolationSettings;
        
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status UI | Interpolation")
        FInterpolationSettings StaminaInterpolationSettings;

protected:
    //@Mana Dot Gauge 캐싱
    TObjectPtr<UHorizontalDotGauge> ManaDotGaugeRef;

    UPROPERTY(EditDefaultsOnly, category = "Status UI | Mana")
        TSubclassOf<UHorizontalDotGauge> ManaDotGaugeClass;

protected:
    //@능력치 속성 변화 이벤트 구독 Async Task 목록
    UPROPERTY()
        TArray<UAsyncTaskAttributeChanged*> AttributeListeners;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
    //@초기화 요청 이벤트
    FRequestStartInitByStatusUI RequestStartInitByStatusUI;

    //@Status UI의 초기화 완료 이벤트
    FStatusUIInitFinished StatusUIInitFinished;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
    //@Mana 게이지 초기화 완료 이벤트 구독
    UFUNCTION()
        void OnManaDotGaugeInitFinished();

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
        
    //@성능 최적화를 위한 캐시
    UPROPERTY()
        TWeakObjectPtr<APlayerStateBase> CachedPlayerState;
        
    //@마지막 업데이트된 값들 (중복 업데이트 방지)
    float LastHealthValue = -1.f;
    float LastStaminaValue = -1.f;
    float LastManaValue = -1.f;
    float LastMaxManaValue = -1.f;
    
    //@업데이트 빈도 제한 (초당 최대 업데이트 횟수)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status UI | Performance", meta = (ClampMin = "1", ClampMax = "120"))
        float MaxUpdateFrequency = 60.f;
        
    float LastUpdateTime = 0.f;
#pragma endregion
	
};
