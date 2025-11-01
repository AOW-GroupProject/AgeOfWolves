// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "WwiseResourceLoader/Public/Wwise/CookedData/WwiseAudioDeviceShareSetCookedData.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeWwiseAudioDeviceShareSetCookedData() {}
// Cross Module References
	UPackage* Z_Construct_UPackage__Script_WwiseResourceLoader();
	WWISERESOURCELOADER_API UScriptStruct* Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData();
// End Cross Module References
	static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_WwiseAudioDeviceShareSetCookedData;
class UScriptStruct* FWwiseAudioDeviceShareSetCookedData::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_WwiseAudioDeviceShareSetCookedData.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_WwiseAudioDeviceShareSetCookedData.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData, (UObject*)Z_Construct_UPackage__Script_WwiseResourceLoader(), TEXT("WwiseAudioDeviceShareSetCookedData"));
	}
	return Z_Registration_Info_UScriptStruct_WwiseAudioDeviceShareSetCookedData.OuterSingleton;
}
template<> WWISERESOURCELOADER_API UScriptStruct* StaticStruct<FWwiseAudioDeviceShareSetCookedData>()
{
	return FWwiseAudioDeviceShareSetCookedData::StaticStruct();
}
	struct Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ShortId_MetaData[];
#endif
		static const UECodeGen_Private::FIntPropertyParams NewProp_ShortId;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_DebugName_MetaData[];
#endif
		static const UECodeGen_Private::FNamePropertyParams NewProp_DebugName;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UECodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Public/Wwise/CookedData/WwiseAudioDeviceShareSetCookedData.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FWwiseAudioDeviceShareSetCookedData>();
	}
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewProp_ShortId_MetaData[] = {
		{ "Category", "Wwise" },
		{ "ModuleRelativePath", "Public/Wwise/CookedData/WwiseAudioDeviceShareSetCookedData.h" },
	};
#endif
	const UECodeGen_Private::FIntPropertyParams Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewProp_ShortId = { "ShortId", nullptr, (EPropertyFlags)0x0010000000020815, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(FWwiseAudioDeviceShareSetCookedData, ShortId), METADATA_PARAMS(Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewProp_ShortId_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewProp_ShortId_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewProp_DebugName_MetaData[] = {
		{ "Category", "Wwise" },
		{ "Comment", "/**\n\x09 * @brief Optional debug name. Can be empty in release, contain the name, or the full path of the asset.\n\x09*/" },
		{ "ModuleRelativePath", "Public/Wwise/CookedData/WwiseAudioDeviceShareSetCookedData.h" },
		{ "ToolTip", "@brief Optional debug name. Can be empty in release, contain the name, or the full path of the asset." },
	};
#endif
	const UECodeGen_Private::FNamePropertyParams Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewProp_DebugName = { "DebugName", nullptr, (EPropertyFlags)0x0010000000020815, UECodeGen_Private::EPropertyGenFlags::Name, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(FWwiseAudioDeviceShareSetCookedData, DebugName), METADATA_PARAMS(Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewProp_DebugName_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewProp_DebugName_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewProp_ShortId,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewProp_DebugName,
	};
	const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_WwiseResourceLoader,
		nullptr,
		&NewStructOps,
		"WwiseAudioDeviceShareSetCookedData",
		sizeof(FWwiseAudioDeviceShareSetCookedData),
		alignof(FWwiseAudioDeviceShareSetCookedData),
		Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000201),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData()
	{
		if (!Z_Registration_Info_UScriptStruct_WwiseAudioDeviceShareSetCookedData.InnerSingleton)
		{
			UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_WwiseAudioDeviceShareSetCookedData.InnerSingleton, Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::ReturnStructParams);
		}
		return Z_Registration_Info_UScriptStruct_WwiseAudioDeviceShareSetCookedData.InnerSingleton;
	}
	struct Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_WwiseResourceLoader_Public_Wwise_CookedData_WwiseAudioDeviceShareSetCookedData_h_Statics
	{
		static const FStructRegisterCompiledInInfo ScriptStructInfo[];
	};
	const FStructRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_WwiseResourceLoader_Public_Wwise_CookedData_WwiseAudioDeviceShareSetCookedData_h_Statics::ScriptStructInfo[] = {
		{ FWwiseAudioDeviceShareSetCookedData::StaticStruct, Z_Construct_UScriptStruct_FWwiseAudioDeviceShareSetCookedData_Statics::NewStructOps, TEXT("WwiseAudioDeviceShareSetCookedData"), &Z_Registration_Info_UScriptStruct_WwiseAudioDeviceShareSetCookedData, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FWwiseAudioDeviceShareSetCookedData), 3705946444U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_WwiseResourceLoader_Public_Wwise_CookedData_WwiseAudioDeviceShareSetCookedData_h_2417787149(TEXT("/Script/WwiseResourceLoader"),
		nullptr, 0,
		Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_WwiseResourceLoader_Public_Wwise_CookedData_WwiseAudioDeviceShareSetCookedData_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_WwiseResourceLoader_Public_Wwise_CookedData_WwiseAudioDeviceShareSetCookedData_h_Statics::ScriptStructInfo),
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
