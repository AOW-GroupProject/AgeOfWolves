// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AkAudio/Classes/Platforms/AkPlatform_Mac/AkMacInitializationSettings.h"
#include "AkAudio/Classes/InitializationSettings/AkInitializationSettings.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAkMacInitializationSettings() {}
// Cross Module References
	AKAUDIO_API UClass* Z_Construct_UClass_UAkMacInitializationSettings();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkMacInitializationSettings_NoRegister();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkPlatformInitialisationSettingsBase_NoRegister();
	AKAUDIO_API UScriptStruct* Z_Construct_UScriptStruct_FAkAdvancedInitializationSettingsWithMultiCoreRendering();
	AKAUDIO_API UScriptStruct* Z_Construct_UScriptStruct_FAkCommonInitializationSettingsWithSampleRate();
	AKAUDIO_API UScriptStruct* Z_Construct_UScriptStruct_FAkCommunicationSettingsWithSystemInitialization();
	AKAUDIO_API UScriptStruct* Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings();
	COREUOBJECT_API UClass* Z_Construct_UClass_UObject();
	UPackage* Z_Construct_UPackage__Script_AkAudio();
// End Cross Module References

static_assert(std::is_polymorphic<FAkMacAdvancedInitializationSettings>() == std::is_polymorphic<FAkAdvancedInitializationSettingsWithMultiCoreRendering>(), "USTRUCT FAkMacAdvancedInitializationSettings cannot be polymorphic unless super FAkAdvancedInitializationSettingsWithMultiCoreRendering is polymorphic");

	static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_AkMacAdvancedInitializationSettings;
class UScriptStruct* FAkMacAdvancedInitializationSettings::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_AkMacAdvancedInitializationSettings.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_AkMacAdvancedInitializationSettings.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings, (UObject*)Z_Construct_UPackage__Script_AkAudio(), TEXT("AkMacAdvancedInitializationSettings"));
	}
	return Z_Registration_Info_UScriptStruct_AkMacAdvancedInitializationSettings.OuterSingleton;
}
template<> AKAUDIO_API UScriptStruct* StaticStruct<FAkMacAdvancedInitializationSettings>()
{
	return FAkMacAdvancedInitializationSettings::StaticStruct();
}
	struct Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics
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
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::Struct_MetaDataParams[] = {
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_Mac/AkMacInitializationSettings.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FAkMacAdvancedInitializationSettings>();
	}
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_uNumSpatialAudioPointSources_MetaData[] = {
		{ "Category", "Ak Initialization Settings" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_Mac/AkMacInitializationSettings.h" },
		{ "ToolTip", "Number of Apple Spatial Audio point sources to allocate for 3D audio use (each point source is a system audio object)." },
	};
#endif
	const UECodeGen_Private::FUInt32PropertyParams Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_uNumSpatialAudioPointSources = { "uNumSpatialAudioPointSources", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::UInt32, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(FAkMacAdvancedInitializationSettings, uNumSpatialAudioPointSources), METADATA_PARAMS(Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_uNumSpatialAudioPointSources_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_uNumSpatialAudioPointSources_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput_MetaData[] = {
		{ "Category", "Ak Initialization Settings" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_Mac/AkMacInitializationSettings.h" },
		{ "ToolTip", "Print detailed system output information to the system log." },
	};
#endif
	void Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput_SetBit(void* Obj)
	{
		((FAkMacAdvancedInitializationSettings*)Obj)->bVerboseSystemOutput = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput = { "bVerboseSystemOutput", nullptr, (EPropertyFlags)0x0010000000000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(FAkMacAdvancedInitializationSettings), &Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_uNumSpatialAudioPointSources,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewProp_bVerboseSystemOutput,
	};
	const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_AkAudio,
		Z_Construct_UScriptStruct_FAkAdvancedInitializationSettingsWithMultiCoreRendering,
		&NewStructOps,
		"AkMacAdvancedInitializationSettings",
		sizeof(FAkMacAdvancedInitializationSettings),
		alignof(FAkMacAdvancedInitializationSettings),
		Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings()
	{
		if (!Z_Registration_Info_UScriptStruct_AkMacAdvancedInitializationSettings.InnerSingleton)
		{
			UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_AkMacAdvancedInitializationSettings.InnerSingleton, Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::ReturnStructParams);
		}
		return Z_Registration_Info_UScriptStruct_AkMacAdvancedInitializationSettings.InnerSingleton;
	}
	DEFINE_FUNCTION(UAkMacInitializationSettings::execMigrateMultiCoreRendering)
	{
		P_GET_UBOOL(Z_Param_NewValue);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->MigrateMultiCoreRendering(Z_Param_NewValue);
		P_NATIVE_END;
	}
	void UAkMacInitializationSettings::StaticRegisterNativesUAkMacInitializationSettings()
	{
		UClass* Class = UAkMacInitializationSettings::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "MigrateMultiCoreRendering", &UAkMacInitializationSettings::execMigrateMultiCoreRendering },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics
	{
		struct AkMacInitializationSettings_eventMigrateMultiCoreRendering_Parms
		{
			bool NewValue;
		};
		static void NewProp_NewValue_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_NewValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::NewProp_NewValue_SetBit(void* Obj)
	{
		((AkMacInitializationSettings_eventMigrateMultiCoreRendering_Parms*)Obj)->NewValue = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::NewProp_NewValue = { "NewValue", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkMacInitializationSettings_eventMigrateMultiCoreRendering_Parms), &Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::NewProp_NewValue_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::NewProp_NewValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::Function_MetaDataParams[] = {
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_Mac/AkMacInitializationSettings.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkMacInitializationSettings, nullptr, "MigrateMultiCoreRendering", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::AkMacInitializationSettings_eventMigrateMultiCoreRendering_Parms), Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x00020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAkMacInitializationSettings);
	UClass* Z_Construct_UClass_UAkMacInitializationSettings_NoRegister()
	{
		return UAkMacInitializationSettings::StaticClass();
	}
	struct Z_Construct_UClass_UAkMacInitializationSettings_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_CommonSettings_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_CommonSettings;
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
	UObject* (*const Z_Construct_UClass_UAkMacInitializationSettings_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UObject,
		(UObject* (*)())Z_Construct_UPackage__Script_AkAudio,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UAkMacInitializationSettings_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UAkMacInitializationSettings_MigrateMultiCoreRendering, "MigrateMultiCoreRendering" }, // 3844815706
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkMacInitializationSettings_Statics::Class_MetaDataParams[] = {
		{ "IncludePath", "Platforms/AkPlatform_Mac/AkMacInitializationSettings.h" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_Mac/AkMacInitializationSettings.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_CommonSettings_MetaData[] = {
		{ "Category", "Initialization" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_Mac/AkMacInitializationSettings.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_CommonSettings = { "CommonSettings", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkMacInitializationSettings, CommonSettings), Z_Construct_UScriptStruct_FAkCommonInitializationSettingsWithSampleRate, METADATA_PARAMS(Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_CommonSettings_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_CommonSettings_MetaData)) }; // 1364846279
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_CommunicationSettings_MetaData[] = {
		{ "Category", "Initialization" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_Mac/AkMacInitializationSettings.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_CommunicationSettings = { "CommunicationSettings", nullptr, (EPropertyFlags)0x0010000000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkMacInitializationSettings, CommunicationSettings), Z_Construct_UScriptStruct_FAkCommunicationSettingsWithSystemInitialization, METADATA_PARAMS(Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_CommunicationSettings_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_CommunicationSettings_MetaData)) }; // 321444022
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_AdvancedSettings_MetaData[] = {
		{ "Category", "Initialization" },
		{ "ModuleRelativePath", "Classes/Platforms/AkPlatform_Mac/AkMacInitializationSettings.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_AdvancedSettings = { "AdvancedSettings", nullptr, (EPropertyFlags)0x0010040000004001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkMacInitializationSettings, AdvancedSettings), Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings, METADATA_PARAMS(Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_AdvancedSettings_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_AdvancedSettings_MetaData)) }; // 646578731
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAkMacInitializationSettings_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_CommonSettings,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_CommunicationSettings,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkMacInitializationSettings_Statics::NewProp_AdvancedSettings,
	};
		const UECodeGen_Private::FImplementedInterfaceParams Z_Construct_UClass_UAkMacInitializationSettings_Statics::InterfaceParams[] = {
			{ Z_Construct_UClass_UAkPlatformInitialisationSettingsBase_NoRegister, (int32)VTABLE_OFFSET(UAkMacInitializationSettings, IAkPlatformInitialisationSettingsBase), false },  // 3478154059
		};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAkMacInitializationSettings_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAkMacInitializationSettings>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UAkMacInitializationSettings_Statics::ClassParams = {
		&UAkMacInitializationSettings::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UAkMacInitializationSettings_Statics::PropPointers,
		InterfaceParams,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		UE_ARRAY_COUNT(Z_Construct_UClass_UAkMacInitializationSettings_Statics::PropPointers),
		UE_ARRAY_COUNT(InterfaceParams),
		0x001000A6u,
		METADATA_PARAMS(Z_Construct_UClass_UAkMacInitializationSettings_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UAkMacInitializationSettings_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UAkMacInitializationSettings()
	{
		if (!Z_Registration_Info_UClass_UAkMacInitializationSettings.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAkMacInitializationSettings.OuterSingleton, Z_Construct_UClass_UAkMacInitializationSettings_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UAkMacInitializationSettings.OuterSingleton;
	}
	template<> AKAUDIO_API UClass* StaticClass<UAkMacInitializationSettings>()
	{
		return UAkMacInitializationSettings::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAkMacInitializationSettings);
	UAkMacInitializationSettings::~UAkMacInitializationSettings() {}
	struct Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_Mac_AkMacInitializationSettings_h_Statics
	{
		static const FStructRegisterCompiledInInfo ScriptStructInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FStructRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_Mac_AkMacInitializationSettings_h_Statics::ScriptStructInfo[] = {
		{ FAkMacAdvancedInitializationSettings::StaticStruct, Z_Construct_UScriptStruct_FAkMacAdvancedInitializationSettings_Statics::NewStructOps, TEXT("AkMacAdvancedInitializationSettings"), &Z_Registration_Info_UScriptStruct_AkMacAdvancedInitializationSettings, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FAkMacAdvancedInitializationSettings), 646578731U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_Mac_AkMacInitializationSettings_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UAkMacInitializationSettings, UAkMacInitializationSettings::StaticClass, TEXT("UAkMacInitializationSettings"), &Z_Registration_Info_UClass_UAkMacInitializationSettings, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAkMacInitializationSettings), 1354736378U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_Mac_AkMacInitializationSettings_h_1178339681(TEXT("/Script/AkAudio"),
		Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_Mac_AkMacInitializationSettings_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_Mac_AkMacInitializationSettings_h_Statics::ClassInfo),
		Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_Mac_AkMacInitializationSettings_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_Platforms_AkPlatform_Mac_AkMacInitializationSettings_h_Statics::ScriptStructInfo),
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
