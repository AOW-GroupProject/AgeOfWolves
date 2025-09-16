// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "AkGameplayStatics.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
class AActor;
class UAkAudioEvent;
class UAkAudioType;
class UAkAuxBus;
class UAkComponent;
class UAkEffectShareSet;
class UAkPortalComponent;
class UAkRtpc;
class UAkStateValue;
class UAkSwitchValue;
class UAkTrigger;
class UObject;
class USceneComponent;
enum class AkChannelConfiguration : uint8;
enum class AkMultiPositionType : uint8;
enum class ERTPCValueType : uint8;
enum class PanningRule : uint8;
struct FAkChannelMask;
struct FAkOutdoorsRoomParameters;
struct FAkOutputDeviceID;
struct FAkOutputSettings;
struct FAkUniqueID;
struct FLatentActionInfo;
#ifdef AKAUDIO_AkGameplayStatics_generated_h
#error "AkGameplayStatics.generated.h already included, missing '#pragma once' in AkGameplayStatics.h"
#endif
#define AKAUDIO_AkGameplayStatics_generated_h

#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_37_GENERATED_BODY \
	friend struct Z_Construct_UScriptStruct_FAkOutdoorsRoomParameters_Statics; \
	AKAUDIO_API static class UScriptStruct* StaticStruct();


template<> AKAUDIO_API UScriptStruct* StaticStruct<struct FAkOutdoorsRoomParameters>();

#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_SPARSE_DATA
#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execSetDistanceProbe); \
	DECLARE_FUNCTION(execSetActorMixerEffect); \
	DECLARE_FUNCTION(execSetAuxBusEffect); \
	DECLARE_FUNCTION(execSetBusEffectByID); \
	DECLARE_FUNCTION(execSetBusEffectByName); \
	DECLARE_FUNCTION(execSetOutputDeviceEffect); \
	DECLARE_FUNCTION(execGetAkAudioTypeUserData); \
	DECLARE_FUNCTION(execSetCurrentAudioCultureAsync); \
	DECLARE_FUNCTION(execSetCurrentAudioCulture); \
	DECLARE_FUNCTION(execGetAvailableAudioCultures); \
	DECLARE_FUNCTION(execGetCurrentAudioCulture); \
	DECLARE_FUNCTION(execStopProfilerCapture); \
	DECLARE_FUNCTION(execStartProfilerCapture); \
	DECLARE_FUNCTION(execStopOutputCapture); \
	DECLARE_FUNCTION(execAddOutputCaptureMarker); \
	DECLARE_FUNCTION(execStartOutputCapture); \
	DECLARE_FUNCTION(execUnloadInitBank); \
	DECLARE_FUNCTION(execLoadInitBank); \
	DECLARE_FUNCTION(execClearSoundBanksAndMedia); \
	DECLARE_FUNCTION(execStopAllAmbientSounds); \
	DECLARE_FUNCTION(execStartAllAmbientSounds); \
	DECLARE_FUNCTION(execCancelEventCallback); \
	DECLARE_FUNCTION(execStopAll); \
	DECLARE_FUNCTION(execStopActor); \
	DECLARE_FUNCTION(execSetOcclusionRefreshInterval); \
	DECLARE_FUNCTION(execSetSpeakerAngles); \
	DECLARE_FUNCTION(execGetSpeakerAngles); \
	DECLARE_FUNCTION(execReplaceMainOutput); \
	DECLARE_FUNCTION(execRemoveOutput); \
	DECLARE_FUNCTION(execAddOutput); \
	DECLARE_FUNCTION(execSetPanningRule); \
	DECLARE_FUNCTION(execSetBusConfig); \
	DECLARE_FUNCTION(execSetOutputBusVolume); \
	DECLARE_FUNCTION(execStopOutdoors); \
	DECLARE_FUNCTION(execPostEventOutdoors); \
	DECLARE_FUNCTION(execResetOutdoorsRoomParams); \
	DECLARE_FUNCTION(execSetOutdoorsRoomParameters); \
	DECLARE_FUNCTION(execGetCurrentOutdoorsRoomParameters); \
	DECLARE_FUNCTION(execSetPortalToPortalObstruction); \
	DECLARE_FUNCTION(execSetGameObjectToPortalObstruction); \
	DECLARE_FUNCTION(execSetPortalObstructionAndOcclusion); \
	DECLARE_FUNCTION(execSetLoadBalancingSpread); \
	DECLARE_FUNCTION(execSetNumberOfPrimaryRays); \
	DECLARE_FUNCTION(execSetMaxEmitterRoomAuxSends); \
	DECLARE_FUNCTION(execSetDiffractionOrder); \
	DECLARE_FUNCTION(execSetReflectionsOrder); \
	DECLARE_FUNCTION(execUseReverbVolumes); \
	DECLARE_FUNCTION(execSetMultipleChannelMaskEmitterPositions); \
	DECLARE_FUNCTION(execSetMultipleChannelEmitterPositions); \
	DECLARE_FUNCTION(execSetMultiplePositions); \
	DECLARE_FUNCTION(execSetSwitch); \
	DECLARE_FUNCTION(execPostTrigger); \
	DECLARE_FUNCTION(execSetState); \
	DECLARE_FUNCTION(execResetRTPCValue); \
	DECLARE_FUNCTION(execGetRTPCValue); \
	DECLARE_FUNCTION(execSetRTPCValue); \
	DECLARE_FUNCTION(execSpawnAkComponentAtLocation); \
	DECLARE_FUNCTION(execPostEventAtLocation); \
	DECLARE_FUNCTION(execPostAndWaitForEndOfEvent); \
	DECLARE_FUNCTION(execPostEvent); \
	DECLARE_FUNCTION(execIsGame); \
	DECLARE_FUNCTION(execIsEditor); \
	DECLARE_FUNCTION(execGetOrCreateAkComponent); \
	DECLARE_FUNCTION(execGetAkComponent);


#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execSetDistanceProbe); \
	DECLARE_FUNCTION(execSetActorMixerEffect); \
	DECLARE_FUNCTION(execSetAuxBusEffect); \
	DECLARE_FUNCTION(execSetBusEffectByID); \
	DECLARE_FUNCTION(execSetBusEffectByName); \
	DECLARE_FUNCTION(execSetOutputDeviceEffect); \
	DECLARE_FUNCTION(execGetAkAudioTypeUserData); \
	DECLARE_FUNCTION(execSetCurrentAudioCultureAsync); \
	DECLARE_FUNCTION(execSetCurrentAudioCulture); \
	DECLARE_FUNCTION(execGetAvailableAudioCultures); \
	DECLARE_FUNCTION(execGetCurrentAudioCulture); \
	DECLARE_FUNCTION(execStopProfilerCapture); \
	DECLARE_FUNCTION(execStartProfilerCapture); \
	DECLARE_FUNCTION(execStopOutputCapture); \
	DECLARE_FUNCTION(execAddOutputCaptureMarker); \
	DECLARE_FUNCTION(execStartOutputCapture); \
	DECLARE_FUNCTION(execUnloadInitBank); \
	DECLARE_FUNCTION(execLoadInitBank); \
	DECLARE_FUNCTION(execClearSoundBanksAndMedia); \
	DECLARE_FUNCTION(execStopAllAmbientSounds); \
	DECLARE_FUNCTION(execStartAllAmbientSounds); \
	DECLARE_FUNCTION(execCancelEventCallback); \
	DECLARE_FUNCTION(execStopAll); \
	DECLARE_FUNCTION(execStopActor); \
	DECLARE_FUNCTION(execSetOcclusionRefreshInterval); \
	DECLARE_FUNCTION(execSetSpeakerAngles); \
	DECLARE_FUNCTION(execGetSpeakerAngles); \
	DECLARE_FUNCTION(execReplaceMainOutput); \
	DECLARE_FUNCTION(execRemoveOutput); \
	DECLARE_FUNCTION(execAddOutput); \
	DECLARE_FUNCTION(execSetPanningRule); \
	DECLARE_FUNCTION(execSetBusConfig); \
	DECLARE_FUNCTION(execSetOutputBusVolume); \
	DECLARE_FUNCTION(execStopOutdoors); \
	DECLARE_FUNCTION(execPostEventOutdoors); \
	DECLARE_FUNCTION(execResetOutdoorsRoomParams); \
	DECLARE_FUNCTION(execSetOutdoorsRoomParameters); \
	DECLARE_FUNCTION(execGetCurrentOutdoorsRoomParameters); \
	DECLARE_FUNCTION(execSetPortalToPortalObstruction); \
	DECLARE_FUNCTION(execSetGameObjectToPortalObstruction); \
	DECLARE_FUNCTION(execSetPortalObstructionAndOcclusion); \
	DECLARE_FUNCTION(execSetLoadBalancingSpread); \
	DECLARE_FUNCTION(execSetNumberOfPrimaryRays); \
	DECLARE_FUNCTION(execSetMaxEmitterRoomAuxSends); \
	DECLARE_FUNCTION(execSetDiffractionOrder); \
	DECLARE_FUNCTION(execSetReflectionsOrder); \
	DECLARE_FUNCTION(execUseReverbVolumes); \
	DECLARE_FUNCTION(execSetMultipleChannelMaskEmitterPositions); \
	DECLARE_FUNCTION(execSetMultipleChannelEmitterPositions); \
	DECLARE_FUNCTION(execSetMultiplePositions); \
	DECLARE_FUNCTION(execSetSwitch); \
	DECLARE_FUNCTION(execPostTrigger); \
	DECLARE_FUNCTION(execSetState); \
	DECLARE_FUNCTION(execResetRTPCValue); \
	DECLARE_FUNCTION(execGetRTPCValue); \
	DECLARE_FUNCTION(execSetRTPCValue); \
	DECLARE_FUNCTION(execSpawnAkComponentAtLocation); \
	DECLARE_FUNCTION(execPostEventAtLocation); \
	DECLARE_FUNCTION(execPostAndWaitForEndOfEvent); \
	DECLARE_FUNCTION(execPostEvent); \
	DECLARE_FUNCTION(execIsGame); \
	DECLARE_FUNCTION(execIsEditor); \
	DECLARE_FUNCTION(execGetOrCreateAkComponent); \
	DECLARE_FUNCTION(execGetAkComponent);


#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_ACCESSORS
#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAkGameplayStatics(); \
	friend struct Z_Construct_UClass_UAkGameplayStatics_Statics; \
public: \
	DECLARE_CLASS(UAkGameplayStatics, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/AkAudio"), NO_API) \
	DECLARE_SERIALIZER(UAkGameplayStatics)


#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_INCLASS \
private: \
	static void StaticRegisterNativesUAkGameplayStatics(); \
	friend struct Z_Construct_UClass_UAkGameplayStatics_Statics; \
public: \
	DECLARE_CLASS(UAkGameplayStatics, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/AkAudio"), NO_API) \
	DECLARE_SERIALIZER(UAkGameplayStatics)


#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAkGameplayStatics(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAkGameplayStatics) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAkGameplayStatics); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAkGameplayStatics); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAkGameplayStatics(UAkGameplayStatics&&); \
	NO_API UAkGameplayStatics(const UAkGameplayStatics&); \
public: \
	NO_API virtual ~UAkGameplayStatics();


#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_ENHANCED_CONSTRUCTORS \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UAkGameplayStatics(UAkGameplayStatics&&); \
	NO_API UAkGameplayStatics(const UAkGameplayStatics&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAkGameplayStatics); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAkGameplayStatics); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAkGameplayStatics) \
	NO_API virtual ~UAkGameplayStatics();


#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_66_PROLOG
#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_SPARSE_DATA \
	FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_RPC_WRAPPERS \
	FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_ACCESSORS \
	FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_INCLASS \
	FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_SPARSE_DATA \
	FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_ACCESSORS \
	FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_INCLASS_NO_PURE_DECLS \
	FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h_69_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> AKAUDIO_API UClass* StaticClass<class UAkGameplayStatics>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Unreal_Projects_Group_Project__2024_02_GP_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGameplayStatics_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
