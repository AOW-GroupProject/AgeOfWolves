#include "AN_PlaySoundAtSocketLocation.h"
#include "Logging/StructuredLog.h"

#include "Components/SkeletalMeshComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

DEFINE_LOG_CATEGORY(LogAN_SocketSound)

//@Default Setting
#pragma region Default Setting
UAN_PlaySoundAtSocketLocation::UAN_PlaySoundAtSocketLocation()
	: VolumeMultiplier(1.0f)
	, PitchMultiplier(1.0f)
	, bSpatialize(true)
	, bPlayWhenSoundIsMuted(false)
{
}
#pragma endregion

//@Override Functions
#pragma region Override Functions
void UAN_PlaySoundAtSocketLocation::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//@유효성 검사
	if (!Sound || !MeshComp || !MeshComp->GetOwner())
	{
		UE_LOGFMT(LogAN_SocketSound, Warning, "사운드 재생 실패: 필요한 컴포넌트가 유효하지 않음");
		return;
	}

	//@게임 일시정지 상태이거나 사운드가 음소거 상태일 때 처리
	if (!bPlayWhenSoundIsMuted)
	{
		//TODO: 음소거 처리
	}

	//@소켓 존재 여부 확인
	if (!MeshComp->DoesSocketExist(SocketName))
	{
		UE_LOGFMT(LogAN_SocketSound, Warning, "사운드 재생 위치 변경: 소켓 '{0}'이(가) 스켈레탈 메시 '{1}'에 존재하지 않음",
			*SocketName.ToString(), *MeshComp->GetName());

		//@소켓이 없으면 메시 위치에서 재생
		UGameplayStatics::PlaySoundAtLocation(
			MeshComp->GetWorld(),
			Sound,
			MeshComp->GetComponentLocation(),
			VolumeMultiplier,
			PitchMultiplier,
			0.0f,
			AttenuationSettings,
			ConcurrencySettings
		);
		return;
	}

	//@소켓 위치 및 회전 가져오기
	FVector SocketLocation = MeshComp->GetSocketLocation(SocketName);
	FRotator SocketRotation = MeshComp->GetSocketRotation(SocketName);

	//@소켓 위치에서 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(
		MeshComp->GetWorld(),
		Sound,
		SocketLocation,
		VolumeMultiplier,
		PitchMultiplier,
		0.0f,
		AttenuationSettings,
		ConcurrencySettings
	);

	UE_LOGFMT(LogAN_SocketSound, Log, "소켓 '{0}'에서 사운드 '{1}' 재생 완료",
		*SocketName.ToString(), Sound ? *Sound->GetName() : TEXT("None"));
}

FString UAN_PlaySoundAtSocketLocation::GetNotifyName_Implementation() const
{
	if (Sound)
	{
		return FString::Printf(TEXT("Sound: %s at Socket: %s"), *Sound->GetName(), *SocketName.ToString());
	}
	else
	{
		return FString::Printf(TEXT("Play Sound at Socket: %s"), *SocketName.ToString());
	}
}
#pragma endregion