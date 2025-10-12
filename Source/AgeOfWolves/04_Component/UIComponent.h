#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "08_UI/UICollection.h"

#include "UIComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogUI, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UUserWidget;
class UUIManagerSubsystem;
class UBaseInputComponent;
class UObjectiveDetectionComponent;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
//@초기화 요청 이벤트
DECLARE_MULTICAST_DELEGATE(FRequestInitializationByUIComp);
//@IMC 변경 요청 이벤트
DECLARE_DELEGATE_OneParam(FRequestSwapIMC, const FGameplayTag&);

//@HUD의 초기화 완료 이벤트
DECLARE_DELEGATE(FUIsForInventoryReady);
//@State Bars의 초기화 완료 이벤트
DECLARE_DELEGATE(FUIsForAttributeSetReady);

//@Menu UI관련 IA 트리거 알림 이벤트
DECLARE_DELEGATE_OneParam(FNotifyMenuUIInputTriggered, const FGameplayTag&);
//@Menu UI관련 IA 해제 알림 이벤트
DECLARE_DELEGATE_OneParam(FNotifyMenuUIInputReleased, const FGameplayTag&);

//@Widget의 Visibility 관련 알림 이벤트: Animation
DECLARE_MULTICAST_DELEGATE_TwoParams(FWidgetVisibilityChanged, UUserWidget*, bool);

//@Interaction UI Event
DECLARE_MULTICAST_DELEGATE_OneParam(FNotifyInteractionUIInputActivation, const FGameplayTag&);
#pragma endregion

/*
* UUIComponent
* 
* 사용자의 UI를 관리하는 컴포넌트입니다.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AGEOFWOLVES_API UUIComponent : public UActorComponent
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:	
	UUIComponent(const FObjectInitializer& ObjectInitializer);

protected:
	//~UActorComponent Interface
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	virtual void InitializeComponent() override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;  // ✅ 추가
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End of UActorComponent Interface

private:
	//@외부 바인딩
	void ExternalBindingToInputComponent(const APlayerController* PlayerController);
	//@OD Component 바인딩 - 동일
	void ExternalBindingToODComponent(const APlayerController* PlayerController);

private:
	//@내부 바인딩
	void InternalBindToHUDUI();
	void InternalBindToMenuUI();

public:
	//@초기화
	UFUNCTION()
		void InitializeUIComponent();

protected:
	//@Player HUD, Menu UI들의 초기화 작업 완료를 체크합니다.
	bool bHUDInitFinished = false;
	bool bMenuUIInitFinished = false;
	void CheckAllUIsForDefaultVisibilitySetting();

	//@TODO: 모든 Interaction UI 초기화 완료 체크 함수 작성...

#pragma endregion

//@Property/Info...etc
#pragma region UI
private:
	//@모든 UI 상태를 리셋합니다.
	void ResetUIs();
	//@카테고리 별 Reset 작업 수행
	void ResetCategoryUI(EUICategory UICategory, UUIManagerSubsystem* UIManagerSubsystem);

protected:
	//@Create Widget
	void CreateAndSetupWidget(APlayerController* PC, EUICategory UICategory, const FUIInformation& UIInfo, UEnum* EnumPtr);
	//@HUD UI
	void SetupHUDUI(UUserWidget* NewWidget);
	//@Menu UI
	void SetupMenuUI(UUserWidget* NewWidget);
	//@Indicator UI - 동일
	void SetupIndicatorUI(const FGameplayTag& UITag, UUserWidget* NewWidget);

protected:
	//@Widget을 화면에 나타냅니다.
	UFUNCTION(BlueprintCallable, Category = "UI")
		void ShowUI(EUICategory UICategory, const FGameplayTag& UITag);
	UFUNCTION(BlueprintCallable, Category = "UI")
		void ShowAllUI(EUICategory UICategory);
	//@Widget을 화면에서 숨깁니다.
	UFUNCTION(BlueprintCallable, Category = "UI")
		void HideUI(EUICategory UICategory, const FGameplayTag& UITag);
	UFUNCTION(BlueprintCallable, Category = "UI")
		void HideAllUI(EUICategory UICategory);

protected:
	//@활성 인디케이터 업데이트 - 동일
	void UpdateActiveIndicator();

	//@인디케이터 표시/숨김 헬퍼 - 새로 추가
	void ShowIndicatorWidget(UUserWidget* Widget);
	void HideIndicatorWidget(UUserWidget* Widget);
	void HideAllIndicators();

	//@Indicator 위치 업데이트 (Tick에서 호출)
	void UpdateIndicatorPositions(float DeltaTime);

	//@개별 Indicator 위치 업데이트
	bool UpdateSingleIndicatorPosition(UUserWidget* Indicator, AActor* Target, float DeltaTime);

protected:
	//@HUD
    UPROPERTY()
        TObjectPtr<UUserWidget> HUDUI;

protected:
	//@Menu
	UPROPERTY()
		TObjectPtr<UUserWidget> MenuUI;

protected:
	//@Indicator
	UPROPERTY()
		TMap<FGameplayTag, UUserWidget*> MIndicatorUIs;
	//@각 인디케이터의 현재 타겟 추적
	UPROPERTY()
		TMap<FGameplayTag, TWeakObjectPtr<AActor>> IndicatorTargets;

protected:
	//@Indicator 설정 - 동일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI | Indicator Settings")
	bool bEnableIndicators = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Indicator Settings",
		meta = (EditCondition = "bEnableIndicators", DisplayName = "Show LockOn Indicator"))
	bool bShowLockOnIndicator = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Indicator Settings",
		meta = (EditCondition = "bEnableIndicators", DisplayName = "Show Execution Indicator"))
	bool bShowExecutionIndicator = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Indicator Settings",
		meta = (EditCondition = "bEnableIndicators", DisplayName = "Show Ambush Indicator"))
	bool bShowAmbushIndicator = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Indicator Settings",
		meta = (EditCondition = "bEnableIndicators", DisplayName = "Show Structure Indicator"))
	bool bShowStructureIndicator = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Indicator Settings",
		meta = (EditCondition = "bEnableIndicators", DisplayName = "Debug Mode - Show All"))
	bool bDebugMode = false;

protected:
	//@LockOn Indicator 설정
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|LockOn")
	FName LockOnTargetSocketName = FName("spine_03");

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|LockOn")
	FVector LockOnSocketOffset = FVector::ZeroVector;

	//@Structure Indicator 설정 - ✅ 수정: 2D 오프셋으로 변경
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Structure")
	FVector2D StructureScreenOffset = FVector2D(80.0f, -30.0f);  // X=오른쪽, Y=위(-는 위)

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Structure")
	float StructureHeightOffset = 80.0f;  // 3D 높이만 적용

	//@일반 Indicator 설정
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|General")
	float GeneralIndicatorHeightOffset = 80.0f;

	//@보간 설정 - ✅ 추가
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation")
	float IndicatorInterpolationSpeed = 15.0f;

	//@DeadZone: 이 거리(픽셀) 이하의 변화는 무시
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation", meta = (ClampMin = "0", ClampMax = "20"))
	float IndicatorDeadZone = 5.0f;

	//@SoftZone: 이 거리(픽셀) 이하는 느리게 보간
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation", meta = (ClampMin = "0", ClampMax = "50"))
	float IndicatorSoftZone = 20.0f;

	//@SoftZone 내에서의 보간 속도 감소율
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float SoftZoneSpeedMultiplier = 0.3f;

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings")
	bool bHideIndicatorWhenOffScreen = true;
#pragma endregion

//@Delegates
#pragma region Delegates
public:
	//@초기화 요청 이벤트
	FRequestInitializationByUIComp RequestInitializationByUIComp;
	//@IMC 변경 오청 이벤트
	FRequestSwapIMC RequestSwapIMC;

public:
	//@Inventory 로딩 준비 완료 이벤트
	FUIsForInventoryReady UIsForInventoryReady;
	//@Attriburte Set 로딩 준비 완료 이벤트
	FUIsForAttributeSetReady UIsForAttributeSetReady;

public:
	//@UI 가시성 변화 이벤트
	FWidgetVisibilityChanged WidgetVisibilityChanged;

public:
	//@Menu UI 입력 이벤트
	FNotifyMenuUIInputTriggered NotifyMenuUIInputTriggered;
	FNotifyMenuUIInputReleased NotifyMenuUIInputReleased;
#pragma endregion

//@Callbacks
#pragma region Callbacks
protected:
	//@Player HUD의 초기화 완료 이벤트 구독
	UFUNCTION()
		void OnHUDInitFinished();
	UFUNCTION()
		void OnMenuUIInitFinished();

protected:
	//@UI Input Tag 활성화 이벤트 구독
	UFUNCTION()
		void OnUIInputTriggered(const FGameplayTag& InputTag);
	//@UI Input Tag 해제 이벤트 구독
	UFUNCTION()
		void OnUIInputReleased(const FGameplayTag& InputTag);

protected:
	//@Menu UI 키 입력을 통한 Menu UI 닫기 요청 이벤트 구독
	UFUNCTION()
		void OnRequestCloseMenuUI();

protected:
	//@OD Component 이벤트 콜백들
	UFUNCTION()
	void OnCurrentTargetChanged(const AActor* NewTarget);

	UFUNCTION()
	void OnExecutionTargetChanged(const AActor* NewTarget);

	UFUNCTION()
	void OnAmbushTargetChanged(const AActor* NewTarget);

	UFUNCTION()
	void OnStructureDetected(const AActor* Structure, bool bDetected);
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
private:
	//@Input Tag를 통해 UI를 찾습니다.
	FString FindUICategoryFromInputTag(const FGameplayTag& InputTag);

public:
	//@UICategory와 UI Tag를 통해 UI를 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "UI")
		UUserWidget* GetUI(EUICategory UICategory, const FGameplayTag& UITag = FGameplayTag()) const;
	//@UI Category 관련 UI들을 모두 반환합니다.
	UFUNCTION(BlueprintCallable, Category = "UI")
		TArray<UUserWidget*> GetCategoryUIs(EUICategory UICategory) const;

public:
	//@Indicator 관련 유틸리티 - 수정
	UFUNCTION(BlueprintCallable, Category = "UI|Indicator")
	UUserWidget* GetIndicatorByTag(const FGameplayTag& IndicatorTag) const;

	//@Type 이름으로 Indicator 찾기 (예: "LockOn" -> UI.Indicator.LockOn)
	UUserWidget* GetIndicatorByType(const FString& TypeName) const;

	//@특정 인디케이터의 현재 타겟 가져오기 - 새로 추가
	UFUNCTION(BlueprintCallable, Category = "UI|Indicator")
	AActor* GetIndicatorTarget(const FGameplayTag& IndicatorTag) const;

	UFUNCTION(BlueprintCallable, Category = "UI|Indicator")
	bool GetIndicatorWorldPosition(const FGameplayTag& IndicatorTag, AActor* Target, FVector& OutWorldPosition);
#pragma endregion

};
