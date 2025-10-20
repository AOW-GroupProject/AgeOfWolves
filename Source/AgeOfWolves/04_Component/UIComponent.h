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
// Indicator 오프셋 설정을 위한 구조체
USTRUCT(BlueprintType)
struct FIndicatorOffsetSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	float AdditionalOffsetRightRatio = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float AdditionalOffsetUpRatio = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float InterpolationSpeed = 12.0f;

	FIndicatorOffsetSettings()
		: AdditionalOffsetRightRatio(0.0f)
		, AdditionalOffsetUpRatio(0.0f)
		, InterpolationSpeed(12.0f)
	{}
};
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
	//@마지막으로 설정한 화면 위치 (떨림 방지)
	UPROPERTY()
	TMap<FGameplayTag, FVector2D> LastIndicatorScreenPositions;

private:
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation",
		meta = (ToolTip = "보간 비활성화 (즉각 반응)"))
	bool bUseInterpolation = true;

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation",
		meta = (ToolTip = "기타 Indicator 보간 속도"))
	float GeneralInterpolationSpeed = 12.0f;  // 중간

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
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Debug",
		meta = (ToolTip = "타겟 중심점 디버그 구체 표시"))
	bool bShowIndicatorDebugSpheres = false;

	//@일반 Indicator 설정
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|General")
	float GeneralIndicatorHeightOffset = 80.0f;

	//@보간 설정 - ✅ 추가
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation")
	float IndicatorInterpolationSpeed = 5.0f;

	//@DeadZone: 이 거리(픽셀) 이하의 변화는 무시
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation", meta = (ClampMin = "0", ClampMax = "20"))
	float IndicatorDeadZone = 10.0f;

	//@SoftZone: 이 거리(픽셀) 이하는 느리게 보간
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation", meta = (ClampMin = "0", ClampMax = "50"))
	float IndicatorSoftZone = 0.30f;

	//@SoftZone 내에서의 보간 속도 감소율
	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation", meta = (ClampMin = "0.1", ClampMax = "1.0"))
	float SoftZoneSpeedMultiplier = 0.15f;

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings")
	bool bHideIndicatorWhenOffScreen = true;

protected:
	//@=== 공통 Indicator 설정 (정규화 좌표) ===

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Common",
		meta = (ToolTip = "화면 너비 대비 오른쪽 오프셋 비율 (0.0~1.0)"))
	float CommonScreenOffsetRightRatio = 0.f;  // 8% (전체 화면 기준)

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Common",
		meta = (ToolTip = "화면 높이 대비 위쪽 오프셋 비율 (0.0~1.0)"))
	float CommonScreenOffsetUpRatio = 0.0f;

protected:
	//@=== LockOn Indicator 추가 오프셋 ===

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|LockOn",
		meta = (ToolTip = "LockOn 추가 오른쪽 오프셋 비율"))
	float LockOnScreenOffsetRightRatio = 0.04f;  // 2% 추가

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|LockOn",
		meta = (ToolTip = "LockOn 추가 위쪽 오프셋 비율"))
	float LockOnScreenOffsetUpRatio = 0.0f;

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation",
		meta = (ToolTip = "LockOn Indicator 보간 속도"))
	float LockOnInterpolationSpeed = 20.0f;

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|LockOn",
		meta = (ToolTip = "타겟 중심에서 위로 올릴 높이 (cm)"))
	float LockOnHeightOffset = 100.0f;

protected:
	//@=== Structure Indicator 추가 오프셋 ===

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Structure",
		meta = (ToolTip = "Structure 추가 오른쪽 오프셋 비율"))
	float StructureScreenOffsetRightRatio = 0.0f;

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Structure",
		meta = (ToolTip = "Structure 추가 위쪽 오프셋 비율"))
	float StructureScreenOffsetUpRatio = 0.035f;  // 3.5% 위로 (1440p에서 약 50px)

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Interpolation",
		meta = (ToolTip = "Structure Indicator 보간 속도"))
	float StructureInterpolationSpeed = 0.f;

	UPROPERTY(EditAnywhere, Category = "UI|Indicator Settings|Structure",
		meta = (ToolTip = "구조물 3D 중심 높이 오프셋 (cm)"))
	float StructureWorldHeightOffset = 0.0f;
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

private:
	// Indicator 위치 업데이트 헬퍼 함수들
	FGameplayTag FindIndicatorTagByWidget(UUserWidget* Widget) const;
	FVector2D ConvertToNormalizedCoordinates(const FVector2D& ScreenPos, int32 ViewportSizeX, int32 ViewportSizeY) const;
	FIndicatorOffsetSettings GetIndicatorOffsetSettings(const FGameplayTag& IndicatorTag) const;
	FVector2D ApplyOffsetToNormalizedPosition(const FVector2D& NormalizedPos, const FIndicatorOffsetSettings& OffsetSettings) const;
	FVector2D ConvertToScreenCoordinates(const FVector2D& NormalizedPos, int32 ViewportSizeX, int32 ViewportSizeY) const;
	FVector2D ApplyInterpolation(const FGameplayTag& IndicatorTag, const FVector2D& TargetScreenPos, float DeltaTime, float InterpolationSpeed);
	void UpdateLastIndicatorPosition(const FGameplayTag& IndicatorTag, const FVector2D& NewPosition);
	
	// Indicator 업데이트 헬퍼 함수
	void UpdateIndicatorByType(const FString& TypeName, bool bShouldShow);
	
	// UI 초기화 헬퍼 함수들
	void SetupExternalBindings(APlayerController* PC);
	UUIManagerSubsystem* GetUIManagerSubsystem() const;
	void CreateWidgetsForAllCategories(APlayerController* PC, UUIManagerSubsystem* UIManagerSubsystem);
#pragma endregion

};
