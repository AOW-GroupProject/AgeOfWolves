#include "StateBars.h"
#include "Logging/StructuredLog.h"

#include "Components/VerticalBox.h"
#include "Components/ProgressBar.h"
#include "03_Player/PlayerStateBase.h"

DEFINE_LOG_CATEGORY(LogStateBars)

UStateBars::UStateBars(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	MStateBars.Empty();

	StateBarList = nullptr;
	HP = nullptr;
	MP = nullptr;
	SP = nullptr;
}

void UStateBars::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//@외부 바인딩
	ExternalBindingToAttributeSet();

}

void UStateBars::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UStateBars::NativeConstruct()
{
	Super::NativeConstruct();
}

void UStateBars::NativeDestruct()
{
	Super::NativeDestruct();
}

void UStateBars::ExternalBindingToAttributeSet()
{
	// Attribute 수치 변화 이벤트에 콜백 함수 등록
	if (const auto PS = GetOwningPlayerState<APlayerStateBase>())
	{
		PS->OnAnyAttributeValueChanged.AddDynamic(this, &UStateBars::OnAttributeValueChanged);
	}
}

void UStateBars::InitializeStateBars()
{

	// Attribute 이름과 이에 대응되는 State Bar를 TMap 유형의 자료구조에 저장
	MStateBars.Add({ "Health", HP });
	MStateBars.Add({ "Mana", MP });
	MStateBars.Add({ "Stamina", SP });

	StateBarsInitFinished.ExecuteIfBound();
}

/*
* @목적 : Attribute에 대응되는 ProgressBar의 현재 수치를 변경합니다.
* @설명 : 이벤트로부터 전달 받은 FGameplyaAttribute 유형의 데이터를 통해 변경된 Attribute를 찾고, 이에 대응되는 Progress Bar의 수치 값을 업데이트합니다.
*/
void UStateBars::OnAttributeValueChanged(FGameplayAttribute Attribute, float OldValue, float NewValue)
{
	if (const auto PS = GetOwningPlayerState<APlayerStateBase>())
	{
		if (Attribute.IsValid())
		{
			if (auto StateBar = MStateBars.Find(Attribute.AttributeName))
			{
				FString AttributeLookingFor = "Max" + Attribute.AttributeName;
				float MaxValue = PS->GetAttributeValue<float>(AttributeLookingFor);

				if (MaxValue != 0)
				{
					(*StateBar)->SetPercent(NewValue / MaxValue);
					UE_LOGFMT(LogStateBars, Error, "{0} : {1}", Attribute.AttributeName, FString::SanitizeFloat(NewValue));
				}
			}
		}
	}
}
