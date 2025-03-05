#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"

#include "AN_PlaySoundAtSocketLocation.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAN_SocketSound, Log, All)

/**
* 
*	@UAN_PlaySoundAtSocketLocation
* 
*	지정된 소켓 위치에서 사운드를 재생하는 애니메이션 노티파이
*/
UCLASS(const, hidecategories = Object, collapsecategories, meta = (DisplayName = "Play Sound At Socket"))
class AGEOFWOLVES_API UAN_PlaySoundAtSocketLocation : public UAnimNotify
{
	GENERATED_BODY()

//@Defualt Setting
#pragma region Default Setting
public:
	UAN_PlaySoundAtSocketLocation();

public:
	//@Notify 구현
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
#pragma endregion

	//@Property/Info...etc
#pragma region Property or Subwidgets or Infos...etc
public:
	//@재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", meta = (DisplayName = "Sound"))
		USoundBase* Sound;

	//@사운드를 재생할 소켓 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		FName SocketName;

	//@볼륨 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		float VolumeMultiplier;

	//@피치 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		float PitchMultiplier;

	//@사운드 감쇠 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundAttenuation* AttenuationSettings;

	//@사운드 동시 실행 설정
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		USoundConcurrency* ConcurrencySettings;

	//@공간화 사용 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
		uint32 bSpatialize : 1;

	//@사운드가 음소거되었을 때도 재생할지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound", AdvancedDisplay)
		uint32 bPlayWhenSoundIsMuted : 1;
#pragma endregion

//@Utility
#pragma region Utility
public:
	//@노티파이 이름 반환
	virtual FString GetNotifyName_Implementation() const override;
#pragma endregion
};