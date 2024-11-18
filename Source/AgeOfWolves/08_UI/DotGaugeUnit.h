#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "DotGaugeUnit.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogDotGaugeUnit, Log, All)

//@전방 선언
#pragma region Forward Declaration
class UOverlay;
class UImage;
#pragma endregion

//@열거형
#pragma region Enums
#pragma endregion

//@구조체
#pragma region Structs
USTRUCT()
struct FDotGaugeUnitInformation
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "게이지 유닛 정보 | 배경 텍스쳐")
        UTexture2D* BackgroundImageTexture;

    UPROPERTY(EditAnywhere, Category = "게이지 유닛 정보 | 채워 넣기 텍스쳐")
        UTexture2D* FillImageTexture;
};
#pragma endregion

//@이벤트/델리게이트
#pragma region Delegates
#pragma endregion

UCLASS()
class AGEOFWOLVES_API UDotGaugeUnit : public UUserWidget
{

//@친추 클래스
#pragma region Friend Class
#pragma endregion

    GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
    UDotGaugeUnit(const FObjectInitializer& ObjectInitializer);

protected:
    //~ Begin UUserWidget Interfaces
    virtual void NativeOnInitialized() override;
    //~ End UUserWidget Interface

protected:
    //@내부 바인딩

protected:
    //@외부 바인딩

protected:
    //@초기화

#pragma endregion

//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
    //@Dot Gauge Unit의 활성화
    UFUNCTION(BlueprintNativeEvent)
        void ActivateDotGaugeUnit();
    virtual void ActivateDotGaugeUnit_Implementation();
    
    //@Dot Gauge Unit의 비활성화
    UFUNCTION(BlueprintNativeEvent)
    void DeactivateDotGaugeUnit();
        virtual void DeactivateDotGaugeUnit_Implementation();

public:
    //@Dot Gauge Unit의 업데이트
    UFUNCTION(BlueprintNativeEvent)
        void UpdateDotGaugeUnit(bool bFilled);
    virtual void UpdateDotGaugeUnit_Implementation(bool bFilled);

protected:
    UPROPERTY(BlueprintReadWrite, category = "게이지 유닛", meta = (BindWidget))
        UOverlay* DotGaugeUnitOverlay;

    UPROPERTY(BlueprintReadWrite, category = "게이지 유닛", meta = (BindWidget))
        UImage* BackgroundImage;

    UPROPERTY(BlueprintReadWrite, category = "게이지 유닛", meta = (BindWidget))
        UImage* FillImage;

protected:
    UPROPERTY(EditAnywhere, Category = "게이지 유닛 | 게이지 유닛 정보")
        FDotGaugeUnitInformation DotGaugeUnitInformation;

protected:
    bool bIsFilled;
    bool bIsActive;
#pragma endregion

//@Delegates
#pragma region Delegates
#pragma endregion

//@Callbacks
#pragma region Callbacks
#pragma endregion

//@Utility(Setter, Getter,...etc)
#pragma region Utility
public:
    FORCEINLINE bool IsActive() const { return bIsActive; }
    FORCEINLINE void SetIsActive(bool InBool) { bIsActive = InBool; }
#pragma endregion

};