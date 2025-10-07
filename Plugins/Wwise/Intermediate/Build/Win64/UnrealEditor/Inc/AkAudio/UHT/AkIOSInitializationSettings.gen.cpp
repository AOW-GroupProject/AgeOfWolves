// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AkAudio/Classes/Platforms/AkPlatform_iOS/AkIOSInitializationSettings.h"
#include "AkAudio/Classes/InitializationSettings/AkAudioSession.h"
#include "AkAudio/Classes/InitializationSettings/AkInitializationSettings.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAkIOSInitializationSettings() {}
// Cross Module References
	AKAUDIO_API UClass* Z_Construct_UClass_UAkIOSInitializationSettings();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkIOSInitializationSettings_NoRegister();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkPlatformInitialisationSettingsBase_NoRegister();
	AKAUDIO_API UScriptStruct* Z_Construct_UScriptStruct_FAkAdvancedInitializationSettingsWithMultiCoreRendering();
	AKAUDIO_API UScriptStruct* Z_Construct_UScriptStruct_FAkAudioSession();
	AKAUDIO_API UScriptStruct* Z_Construct_UScriptStruct_FAkCommonInitializationSettingsWithSampleRate();
	AKAUDIO_API UScriptStruct* Z_Construct_UScriptStruct_FAkCommunicationSettingsWithSystemInitialization();
	AKAUDIO_API UScriptStruct* Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_AkAudio();
// End Cross Module References

static_assert(std::is_polymorphic<FAkIOSAdvancedInitializationSettings>() == std::is_polymorphic<FAkAdvancedInitializationSettingsWithMultiCoreRendering>(), "USTRUCT FAkIOSAdvancedInitializationSettings cannot be polymorphic unless super FAkAdvancedInitializationSettingsWithMultiCoreRendering is polymorphic");

	static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_AkIOSAdvancedInitializationSettings;
class UScriptStruct* FAkIOSAdvancedInitializationSettings::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_AkIOSAdvancedInitializationSettings.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_AkIOSAdvancedInitializationSettings.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings, (UObject*)Z_Construct_UPackage__Script_AkAudio(), TEXT("AkIOSAdvancedInitializationSettings"));
	}
	return Z_Registration_Info_UScriptStruct_AkIOSAdvancedInitializationSettings.OuterSingleton;
}
template<> AKAUDIO_API UScriptStruct* StaticStruct<FAkIOSAdvancedInitializationSettings>()
{
	return FAkIOSAdvancedInitializationSettings::StaticStruct();
}
	struct Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_uNumSpatialAudioPointSources_MetaData[];
#endif
		static const UECodeGen_Private::FUInt32PropertyParams NewProp_uNumSpatialAudioPointSources;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bVerboseSystemOutput_MetaData[];
#endif
		static void NewProp_bVerboseSystemOutput_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bVerboseSystemOutput;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UECodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_iOS/AkIOSInitializationSettings.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FAkIOSAdvancedInitializationSettings>();
	}
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_uNumSpatialAudioPointSources_MetaData[] = {
		{ "Category", "Ak Initialization Settings" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_iOS/AkIOSInitializationSettings.h" },
		{ "ToolTip", "Number of Apple Spatial Audio point sources to allocate for 3D audio use (each point source is a system audio object)." },
	};
#endif
	const UECodeGen_Private::FUInt32PropertyParams Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_uNumSpatialAudioPointSources = { "uNumSpatialAudioPointSources", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::UInt32, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(FAkIOSAdvancedInitializationSettings, uNumSpatialAudioPointSources), METADATA_PARAMS(Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_uNumSpatialAudioPointSources_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_uNumSpatialAudioPointSources_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput_MetaData[] = {
		{ "Category", "Ak Initialization Settings" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_iOS/AkIOSInitializationSettings.h" },
		{ "ToolTip", "Print detailed system output information to the system log." },
	};
#endif
	void Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput_SetBit(void* Obj)
	{
		((FAkIOSAdvancedInitializationSettings*)Obj)->bVerboseSystemOutput = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput = { "bVerboseSystemOutput", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(FAkIOSAdvancedInitializationSettings), &Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_uNumSpatialAudioPointSources,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput,
	};
	const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_AkAudio,
		Z_Construct_UScriptStruct_FAkAdvancedInitializationSettingsWithMultiCoreRendering,
		&NewStructOps,
		"AkIOSAdvancedInitializationSettings",
		sizeof(FAkIOSAdvancedInitializationSettings),
		alignof(FAkIOSAdvancedInitializationSettings),
		Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings()
	{
		if (!Z_Registration_Info_UScriptStruct_AkIOSAdvancedInitializationSettings.InnerSingleton)
		{
			UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_AkIOSAdvancedInitializationSettings.InnerSingleton, Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::ReturnStructParams);
		}
		return Z_Registration_Info_UScriptStruct_AkIOSAdvancedInitializationSettings.InnerSingleton;
	}
	void UAkIOSInitializationSettings::StaticRegisterNativesUAkIOSInitializationSettings()
	{
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAkIOSInitializationSettings);
	UClass* Z_Construct_UClass_UAkIOSInitializationSettings_NoRegister()
	{
		return UAkIOSInitializationSettings::StaticClass();
	}
	struct Z_Construct_UClass_UAkIOSInitializationSettings_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_CommonSettings_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_CommonSettings;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_AudioSession_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_AudioSession;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_CommunicationSettings_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_CommunicationSettings;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_AdvancedSettings_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_AdvancedSettings;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UECodeGen_Private::FImplementedInterfaceParams InterfaceParams[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAkIOSInitializationSettings_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_AkAudio,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkIOSInitializationSettings_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Platforms/AkPlatform_iOS/AkIOSInitializationSettings.h" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_iOS/AkIOSInitializationSettings.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_CommonSettings_MetaData[] = {
		{ "Category", "Initialization" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_iOS/AkIOSInitializationSettings.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_CommonSettings = { "CommonSettings", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkIOSInitializationSettings, CommonSettings), Z_Construct_UScriptStruct_FAkCommonInitializationSettingsWithSampleRate, METADATA_PARAMS(Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_CommonSettings_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_CommonSettings_MetaData)) }; // 1364846279
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_AudioSession_MetaData[] = {
		{ "Category", "Initialization" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_iOS/AkIOSInitializationSettings.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_AudioSession = { "AudioSession", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkIOSInitializationSettings, AudioSession), Z_Construct_UScriptStruct_FAkAudioSession, METADATA_PARAMS(Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_AudioSession_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_AudioSession_MetaData)) }; // 810293321
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_CommunicationSettings_MetaData[] = {
		{ "Category", "Initialization" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_iOS/AkIOSInitializationSettings.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_CommunicationSettings = { "CommunicationSettings", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkIOSInitializationSettings, CommunicationSettings), Z_Construct_UScriptStruct_FAkCommunicationSettingsWithSystemInitialization, METADATA_PARAMS(Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_CommunicationSettings_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_CommunicationSettings_MetaData)) }; // 321444022
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_AdvancedSettings_MetaData[] = {
		{ "Category", "Initialization" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_iOS/AkIOSInitializationSettings.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_AdvancedSettings = { "AdvancedSettings", nullptr, (EPropertyFlags)0x0010040000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkIOSInitializationSettings, AdvancedSettings), Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings, METADATA_PARAMS(Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_AdvancedSettings_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_AdvancedSettings_MetaData)) }; // 1675331366
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAkIOSInitializationSettings_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_CommonSettings,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_AudioSession,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_CommunicationSettings,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkIOSInitializationSettings_Statics::NewProp_AdvancedSettings,
	};
		const UECodeGen_Private::FImplementedInterfaceParams Z_Construct_UClass_UAkIOSInitializationSettings_Statics::InterfaceParams[] = {
			{ Z_Construct_UClass_UAkPlatformInitialisationSettingsBase_NoRegister, (int32)VTABLE_OFFSET(UAkIOSInitializationSettings, IAkPlatformInitialisationSettingsBase), false },  // 3478154059
		};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAkIOSInitializationSettings_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAkIOSInitializationSettings>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UAkIOSInitializationSettings_Statics::ClassParams = {
		&UAkIOSInitializationSettings::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		Z_Construct_UClass_UAkIOSInitializationSettings_Statics::PropPointers,
		InterfaceParams,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		UE_ARRAY_COUNT(Z_Construct_UClass_UAkIOSInitializationSettings_Statics::PropPointers),
		UE_ARRAY_COUNT(InterfaceParams),
		0x001000A6u,
		METADATA_PARAMS(Z_Construct_UClass_UAkIOSInitializationSettings_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UAkIOSInitializationSettings_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UAkIOSInitializationSettings()
	{
		if (!Z_Registration_Info_UClass_UAkIOSInitializationSettings.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAkIOSInitializationSettings.OuterSingleton, Z_Construct_UClass_UAkIOSInitializationSettings_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UAkIOSInitializationSettings.OuterSingleton;
	}
	template<> AKAUDIO_API UClass* StaticClass<UAkIOSInitializationSettings>()
	{
		return UAkIOSInitializationSettings::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAkIOSInitializationSettings);
	UAkIOSInitializationSettings::~UAkIOSInitializationSettings() {}
	struct Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_iOS_AkIOSInitializationSettings_h_Statics
	{
		static const FStructRegisterCompiledInInfo ScriptStructInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FStructRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_iOS_AkIOSInitializationSettings_h_Statics::ScriptStructInfo[] = {
		{ FAkIOSAdvancedInitializationSettings::StaticStruct, Z_Construct_UScriptStruct_FAkIOSAdvancedInitializationSettings_Statics::NewStructOps, TEXT("AkIOSAdvancedInitializationSettings"), &Z_Registration_Info_UScriptStruct_AkIOSAdvancedInitializationSettings, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FAkIOSAdvancedInitializationSettings), 1675331366U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_iOS_AkIOSInitializationSettings_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UAkIOSInitializationSettings, UAkIOSInitializationSettings::StaticClass, TEXT("UAkIOSInitializationSettings"), &Z_Registration_Info_UClass_UAkIOSInitializationSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAkIOSInitializationSettings), 1419071588U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_iOS_AkIOSInitializationSettings_h_2376787324(TEXT("/Script/AkAudio"),
		Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_iOS_AkIOSInitializationSettings_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_iOS_AkIOSInitializationSettings_h_Statics::ClassInfo),
		Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_iOS_AkIOSInitializationSettings_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_iOS_AkIOSInitializationSettings_h_Statics::ScriptStructInfo),
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
