// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AkAudio/Classes/AkGeometryComponent.h"
#include "AkAudio/Classes/AkGeometryData.h"
#include "Serialization/ArchiveUObjectFromStructuredArchive.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAkGeometryComponent() {}
// Cross Module References
	AKAUDIO_API UClass* Z_Construct_UClass_UAkAcousticTexture_NoRegister();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkAcousticTextureSetComponent();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkGeometryComponent();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkGeometryComponent_NoRegister();
	AKAUDIO_API UEnum* Z_Construct_UEnum_AkAudio_AkMeshType();
	AKAUDIO_API UScriptStruct* Z_Construct_UScriptStruct_FAkGeometryData();
	AKAUDIO_API UScriptStruct* Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride();
	ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UMaterialInterface_NoRegister();
	UPackage* Z_Construct_UPackage__Script_AkAudio();
// End Cross Module References
	static FEnumRegistrationInfo Z_Registration_Info_UEnum_AkMeshType;
	static UEnum* AkMeshType_StaticEnum()
	{
		if (!Z_Registration_Info_UEnum_AkMeshType.OuterSingleton)
		{
			Z_Registration_Info_UEnum_AkMeshType.OuterSingleton = GetStaticEnum(Z_Construct_UEnum_AkAudio_AkMeshType, (UObject*)Z_Construct_UPackage__Script_AkAudio(), TEXT("AkMeshType"));
		}
		return Z_Registration_Info_UEnum_AkMeshType.OuterSingleton;
	}
	template<> AKAUDIO_API UEnum* StaticEnum<AkMeshType>()
	{
		return AkMeshType_StaticEnum();
	}
	struct Z_Construct_UEnum_AkAudio_AkMeshType_Statics
	{
		static const UECodeGen_Private::FEnumeratorParam Enumerators[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Enum_MetaDataParams[];
#endif
		static const UECodeGen_Private::FEnumParams EnumParams;
	};
	const UECodeGen_Private::FEnumeratorParam Z_Construct_UEnum_AkAudio_AkMeshType_Statics::Enumerators[] = {
		{ "AkMeshType::StaticMesh", (int64)AkMeshType::StaticMesh },
		{ "AkMeshType::CollisionMesh", (int64)AkMeshType::CollisionMesh },
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UEnum_AkAudio_AkMeshType_Statics::Enum_MetaDataParams[] = {
		{ "CollisionMesh.DisplayName", "Simple Collision" },
		{ "CollisionMesh.Name", "AkMeshType::CollisionMesh" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "StaticMesh.Name", "AkMeshType::StaticMesh" },
	};
#endif
	const UECodeGen_Private::FEnumParams Z_Construct_UEnum_AkAudio_AkMeshType_Statics::EnumParams = {
		(UObject*(*)())Z_Construct_UPackage__Script_AkAudio,
		nullptr,
		"AkMeshType",
		"AkMeshType",
		Z_Construct_UEnum_AkAudio_AkMeshType_Statics::Enumerators,
		UE_ARRAY_COUNT(Z_Construct_UEnum_AkAudio_AkMeshType_Statics::Enumerators),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EEnumFlags::None,
		(uint8)UEnum::ECppForm::EnumClass,
		METADATA_PARAMS(Z_Construct_UEnum_AkAudio_AkMeshType_Statics::Enum_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UEnum_AkAudio_AkMeshType_Statics::Enum_MetaDataParams))
	};
	UEnum* Z_Construct_UEnum_AkAudio_AkMeshType()
	{
		if (!Z_Registration_Info_UEnum_AkMeshType.InnerSingleton)
		{
			UECodeGen_Private::ConstructUEnum(Z_Registration_Info_UEnum_AkMeshType.InnerSingleton, Z_Construct_UEnum_AkAudio_AkMeshType_Statics::EnumParams);
		}
		return Z_Registration_Info_UEnum_AkMeshType.InnerSingleton;
	}
	static FStructRegistrationInfo Z_Registration_Info_UScriptStruct_AkGeometrySurfaceOverride;
class UScriptStruct* FAkGeometrySurfaceOverride::StaticStruct()
{
	if (!Z_Registration_Info_UScriptStruct_AkGeometrySurfaceOverride.OuterSingleton)
	{
		Z_Registration_Info_UScriptStruct_AkGeometrySurfaceOverride.OuterSingleton = GetStaticStruct(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride, (UObject*)Z_Construct_UPackage__Script_AkAudio(), TEXT("AkGeometrySurfaceOverride"));
	}
	return Z_Registration_Info_UScriptStruct_AkGeometrySurfaceOverride.OuterSingleton;
}
template<> AKAUDIO_API UScriptStruct* StaticStruct<FAkGeometrySurfaceOverride>()
{
	return FAkGeometrySurfaceOverride::StaticStruct();
}
	struct Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Struct_MetaDataParams[];
#endif
		static void* NewStructOps();
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_AcousticTexture_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_AcousticTexture;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableOcclusionOverride_MetaData[];
#endif
		static void NewProp_bEnableOcclusionOverride_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableOcclusionOverride;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_OcclusionValue_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_OcclusionValue;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_SurfaceArea_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_SurfaceArea;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const UECodeGen_Private::FStructParams ReturnStructParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::Struct_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
	};
#endif
	void* Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewStructOps()
	{
		return (UScriptStruct::ICppStructOps*)new UScriptStruct::TCppStructOps<FAkGeometrySurfaceOverride>();
	}
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_AcousticTexture_MetaData[] = {
		{ "Category", "Geometry" },
		{ "Comment", "/** The Acoustic Texture represents the sound absorption on the surface of the geometry when a sound bounces off of it.\n\x09* If left to None, the mesh's physical material will be used to fetch an acoustic texture.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "The Acoustic Texture represents the sound absorption on the surface of the geometry when a sound bounces off of it.\nIf left to None, the mesh's physical material will be used to fetch an acoustic texture." },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_AcousticTexture = { "AcousticTexture", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(FAkGeometrySurfaceOverride, AcousticTexture), Z_Construct_UClass_UAkAcousticTexture_NoRegister, METADATA_PARAMS(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_AcousticTexture_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_AcousticTexture_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_bEnableOcclusionOverride_MetaData[] = {
		{ "Category", "Geometry" },
		{ "Comment", "/** Enable Transmission Loss Override */" },
		{ "DisplayName", "Enable Transmission Loss Override" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Enable Transmission Loss Override" },
	};
#endif
	void Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_bEnableOcclusionOverride_SetBit(void* Obj)
	{
		((FAkGeometrySurfaceOverride*)Obj)->bEnableOcclusionOverride = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_bEnableOcclusionOverride = { "bEnableOcclusionOverride", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(FAkGeometrySurfaceOverride), &Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_bEnableOcclusionOverride_SetBit, METADATA_PARAMS(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_bEnableOcclusionOverride_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_bEnableOcclusionOverride_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_OcclusionValue_MetaData[] = {
		{ "Category", "Geometry" },
		{ "ClampMax", "1.0" },
		{ "ClampMin", "0.0" },
		{ "Comment", "/** Transmission loss value to set when modeling sound transmission through geometry. Transmission is modeled only when there is no direct line of sight from the emitter to the listener.\n\x09* If there is more than one surface between the emitter and the listener, the maximum of each surface's transmission loss value is used. If the emitter and listener are in different rooms, the room's transmission loss value is taken into account.\n\x09* Valid range : (0.0, 1.0)\n\x09*/" },
		{ "DisplayName", "Transmission Loss" },
		{ "EditCondition", "bEnableOcclusionOverride" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Transmission loss value to set when modeling sound transmission through geometry. Transmission is modeled only when there is no direct line of sight from the emitter to the listener.\nIf there is more than one surface between the emitter and the listener, the maximum of each surface's transmission loss value is used. If the emitter and listener are in different rooms, the room's transmission loss value is taken into account.\nValid range : (0.0, 1.0)" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_OcclusionValue = { "OcclusionValue", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(FAkGeometrySurfaceOverride, OcclusionValue), METADATA_PARAMS(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_OcclusionValue_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_OcclusionValue_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_SurfaceArea_MetaData[] = {
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_SurfaceArea = { "SurfaceArea", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(FAkGeometrySurfaceOverride, SurfaceArea), METADATA_PARAMS(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_SurfaceArea_MetaData, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_SurfaceArea_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_AcousticTexture,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_bEnableOcclusionOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_OcclusionValue,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewProp_SurfaceArea,
	};
	const UECodeGen_Private::FStructParams Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::ReturnStructParams = {
		(UObject* (*)())Z_Construct_UPackage__Script_AkAudio,
		nullptr,
		&NewStructOps,
		"AkGeometrySurfaceOverride",
		sizeof(FAkGeometrySurfaceOverride),
		alignof(FAkGeometrySurfaceOverride),
		Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::PropPointers,
		UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::PropPointers),
		RF_Public|RF_Transient|RF_MarkAsNative,
		EStructFlags(0x00000001),
		METADATA_PARAMS(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::Struct_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::Struct_MetaDataParams))
	};
	UScriptStruct* Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride()
	{
		if (!Z_Registration_Info_UScriptStruct_AkGeometrySurfaceOverride.InnerSingleton)
		{
			UECodeGen_Private::ConstructUScriptStruct(Z_Registration_Info_UScriptStruct_AkGeometrySurfaceOverride.InnerSingleton, Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::ReturnStructParams);
		}
		return Z_Registration_Info_UScriptStruct_AkGeometrySurfaceOverride.InnerSingleton;
	}
	DEFINE_FUNCTION(UAkGeometryComponent::execSetEnableDiffraction)
	{
		P_GET_UBOOL(Z_Param_bInEnableDiffraction);
		P_GET_UBOOL(Z_Param_bInEnableDiffractionOnBoundaryEdges);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->SetEnableDiffraction(Z_Param_bInEnableDiffraction,Z_Param_bInEnableDiffractionOnBoundaryEdges);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkGeometryComponent::execSetEnableTransmissionLossOverride)
	{
		P_GET_OBJECT(UMaterialInterface,Z_Param_InMaterialInterface);
		P_GET_UBOOL(Z_Param_bInEnableTransmissionLossOverride);
		P_GET_STRUCT_REF(FAkGeometrySurfaceOverride,Z_Param_Out_OutAcousticPropertiesOverride);
		P_FINISH;
		P_NATIVE_BEGIN;
		*(bool*)Z_Param__Result=P_THIS->SetEnableTransmissionLossOverride(Z_Param_InMaterialInterface,Z_Param_bInEnableTransmissionLossOverride,Z_Param_Out_OutAcousticPropertiesOverride);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkGeometryComponent::execSetTransmissionLossOverride)
	{
		P_GET_OBJECT(UMaterialInterface,Z_Param_InMaterialInterface);
		P_GET_PROPERTY(FFloatProperty,Z_Param_InTransmissionLoss);
		P_GET_UBOOL(Z_Param_bInEnableTransmissionLossOverride);
		P_GET_STRUCT_REF(FAkGeometrySurfaceOverride,Z_Param_Out_OutAcousticPropertiesOverride);
		P_FINISH;
		P_NATIVE_BEGIN;
		*(bool*)Z_Param__Result=P_THIS->SetTransmissionLossOverride(Z_Param_InMaterialInterface,Z_Param_InTransmissionLoss,Z_Param_bInEnableTransmissionLossOverride,Z_Param_Out_OutAcousticPropertiesOverride);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkGeometryComponent::execSetAcousticTextureOverride)
	{
		P_GET_OBJECT(UMaterialInterface,Z_Param_InMaterialInterface);
		P_GET_OBJECT(UAkAcousticTexture,Z_Param_InAcousticTexture);
		P_GET_STRUCT_REF(FAkGeometrySurfaceOverride,Z_Param_Out_OutAcousticPropertiesOverride);
		P_FINISH;
		P_NATIVE_BEGIN;
		*(bool*)Z_Param__Result=P_THIS->SetAcousticTextureOverride(Z_Param_InMaterialInterface,Z_Param_InAcousticTexture,Z_Param_Out_OutAcousticPropertiesOverride);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkGeometryComponent::execSetAcousticPropertiesOverride)
	{
		P_GET_OBJECT(UMaterialInterface,Z_Param_InMaterialInterface);
		P_GET_STRUCT(FAkGeometrySurfaceOverride,Z_Param_InAcousticPropertiesOverride);
		P_GET_STRUCT_REF(FAkGeometrySurfaceOverride,Z_Param_Out_OutAcousticPropertiesOverride);
		P_FINISH;
		P_NATIVE_BEGIN;
		*(bool*)Z_Param__Result=P_THIS->SetAcousticPropertiesOverride(Z_Param_InMaterialInterface,Z_Param_InAcousticPropertiesOverride,Z_Param_Out_OutAcousticPropertiesOverride);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkGeometryComponent::execGetAcousticPropertiesOverride)
	{
		P_GET_OBJECT(UMaterialInterface,Z_Param_InMaterialInterface);
		P_GET_STRUCT_REF(FAkGeometrySurfaceOverride,Z_Param_Out_OutAcousticPropertiesOverride);
		P_FINISH;
		P_NATIVE_BEGIN;
		*(bool*)Z_Param__Result=P_THIS->GetAcousticPropertiesOverride(Z_Param_InMaterialInterface,Z_Param_Out_OutAcousticPropertiesOverride);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkGeometryComponent::execRemoveGeometry)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->RemoveGeometry();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkGeometryComponent::execUpdateGeometry)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->UpdateGeometry();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkGeometryComponent::execSendGeometry)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->SendGeometry();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkGeometryComponent::execConvertMesh)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->ConvertMesh();
		P_NATIVE_END;
	}
	void UAkGeometryComponent::StaticRegisterNativesUAkGeometryComponent()
	{
		UClass* Class = UAkGeometryComponent::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "ConvertMesh", &UAkGeometryComponent::execConvertMesh },
			{ "GetAcousticPropertiesOverride", &UAkGeometryComponent::execGetAcousticPropertiesOverride },
			{ "RemoveGeometry", &UAkGeometryComponent::execRemoveGeometry },
			{ "SendGeometry", &UAkGeometryComponent::execSendGeometry },
			{ "SetAcousticPropertiesOverride", &UAkGeometryComponent::execSetAcousticPropertiesOverride },
			{ "SetAcousticTextureOverride", &UAkGeometryComponent::execSetAcousticTextureOverride },
			{ "SetEnableDiffraction", &UAkGeometryComponent::execSetEnableDiffraction },
			{ "SetEnableTransmissionLossOverride", &UAkGeometryComponent::execSetEnableTransmissionLossOverride },
			{ "SetTransmissionLossOverride", &UAkGeometryComponent::execSetTransmissionLossOverride },
			{ "UpdateGeometry", &UAkGeometryComponent::execUpdateGeometry },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UAkGeometryComponent_ConvertMesh_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkGeometryComponent_ConvertMesh_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkGeometry" },
		{ "Comment", "/** Convert the mesh into a local representation suited for Wwise:\n\x09* a set of vertices, triangles, surfaces, acoustic textures and transmission loss values. */" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Convert the mesh into a local representation suited for Wwise:\na set of vertices, triangles, surfaces, acoustic textures and transmission loss values." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkGeometryComponent_ConvertMesh_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkGeometryComponent, nullptr, "ConvertMesh", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkGeometryComponent_ConvertMesh_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_ConvertMesh_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkGeometryComponent_ConvertMesh()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkGeometryComponent_ConvertMesh_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics
	{
		struct AkGeometryComponent_eventGetAcousticPropertiesOverride_Parms
		{
			UMaterialInterface* InMaterialInterface;
			FAkGeometrySurfaceOverride OutAcousticPropertiesOverride;
			bool ReturnValue;
		};
		static const UECodeGen_Private::FObjectPropertyParams NewProp_InMaterialInterface;
		static const UECodeGen_Private::FStructPropertyParams NewProp_OutAcousticPropertiesOverride;
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::NewProp_InMaterialInterface = { "InMaterialInterface", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventGetAcousticPropertiesOverride_Parms, InMaterialInterface), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::NewProp_OutAcousticPropertiesOverride = { "OutAcousticPropertiesOverride", nullptr, (EPropertyFlags)0x0010000000000180, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventGetAcousticPropertiesOverride_Parms, OutAcousticPropertiesOverride), Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride, METADATA_PARAMS(nullptr, 0) }; // 778562176
	void Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((AkGeometryComponent_eventGetAcousticPropertiesOverride_Parms*)Obj)->ReturnValue = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkGeometryComponent_eventGetAcousticPropertiesOverride_Parms), &Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::NewProp_InMaterialInterface,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::NewProp_OutAcousticPropertiesOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkGeometry" },
		{ "Comment", "/**\n\x09 * Get the Acoustic Properties overriding this Geometry.\n\x09 * @param InMaterialInterface - If this Geometry's Mesh Type is set to Static Mesh, provide the Material Interface that the requested Acoustic Properties override. Leave empty if the Mesh Type is set to Simple Collision.\n\x09 * @param OutAcousticPropertiesOverride - The requested Acoustic Properties Override.\n\x09 * @return True if OutAcousticPropertiesOverride is valid.\n\x09**/" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Get the Acoustic Properties overriding this Geometry.\n@param InMaterialInterface - If this Geometry's Mesh Type is set to Static Mesh, provide the Material Interface that the requested Acoustic Properties override. Leave empty if the Mesh Type is set to Simple Collision.\n@param OutAcousticPropertiesOverride - The requested Acoustic Properties Override.\n@return True if OutAcousticPropertiesOverride is valid." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkGeometryComponent, nullptr, "GetAcousticPropertiesOverride", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::AkGeometryComponent_eventGetAcousticPropertiesOverride_Parms), Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04420401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkGeometryComponent_RemoveGeometry_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkGeometryComponent_RemoveGeometry_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkGeometry" },
		{ "Comment", "/** Remove the geometry and the corresponding instance from Wwise. */" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Remove the geometry and the corresponding instance from Wwise." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkGeometryComponent_RemoveGeometry_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkGeometryComponent, nullptr, "RemoveGeometry", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkGeometryComponent_RemoveGeometry_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_RemoveGeometry_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkGeometryComponent_RemoveGeometry()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkGeometryComponent_RemoveGeometry_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkGeometryComponent_SendGeometry_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkGeometryComponent_SendGeometry_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkGeometry" },
		{ "Comment", "/** Add or update a geometry in Spatial Audio by sending the converted mesh, as well as the rest of the AkGeometryParams to Wwise.\n\x09* It is necessary to create at least one geometry instance for each geometry set that is to be used for diffraction and reflection simulation. See UpdateGeometry(). */" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Add or update a geometry in Spatial Audio by sending the converted mesh, as well as the rest of the AkGeometryParams to Wwise.\nIt is necessary to create at least one geometry instance for each geometry set that is to be used for diffraction and reflection simulation. See UpdateGeometry()." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkGeometryComponent_SendGeometry_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkGeometryComponent, nullptr, "SendGeometry", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkGeometryComponent_SendGeometry_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_SendGeometry_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkGeometryComponent_SendGeometry()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkGeometryComponent_SendGeometry_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics
	{
		struct AkGeometryComponent_eventSetAcousticPropertiesOverride_Parms
		{
			UMaterialInterface* InMaterialInterface;
			FAkGeometrySurfaceOverride InAcousticPropertiesOverride;
			FAkGeometrySurfaceOverride OutAcousticPropertiesOverride;
			bool ReturnValue;
		};
		static const UECodeGen_Private::FObjectPropertyParams NewProp_InMaterialInterface;
		static const UECodeGen_Private::FStructPropertyParams NewProp_InAcousticPropertiesOverride;
		static const UECodeGen_Private::FStructPropertyParams NewProp_OutAcousticPropertiesOverride;
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::NewProp_InMaterialInterface = { "InMaterialInterface", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventSetAcousticPropertiesOverride_Parms, InMaterialInterface), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::NewProp_InAcousticPropertiesOverride = { "InAcousticPropertiesOverride", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventSetAcousticPropertiesOverride_Parms, InAcousticPropertiesOverride), Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride, METADATA_PARAMS(nullptr, 0) }; // 778562176
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::NewProp_OutAcousticPropertiesOverride = { "OutAcousticPropertiesOverride", nullptr, (EPropertyFlags)0x0010000000000180, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventSetAcousticPropertiesOverride_Parms, OutAcousticPropertiesOverride), Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride, METADATA_PARAMS(nullptr, 0) }; // 778562176
	void Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((AkGeometryComponent_eventSetAcousticPropertiesOverride_Parms*)Obj)->ReturnValue = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkGeometryComponent_eventSetAcousticPropertiesOverride_Parms), &Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::NewProp_InMaterialInterface,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::NewProp_InAcousticPropertiesOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::NewProp_OutAcousticPropertiesOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkGeometry" },
		{ "Comment", "/**\n\x09 * Set the Acoustic Properties overriding this Geometry.\n\x09 * @param InMaterialInterface - If this Geometry's Mesh Type is set to Static Mesh, provide the Material Interface to override. Leave empty if the Mesh Type is set to Simple Collision.\n\x09 * @param InAcousticPropertiesOverride - Structure of Acoustic Properties Override to set with.\n\x09 * @param OutAcousticPropertiesOverride - Reference to the modified Acoustic Properties Override.\n\x09 * @return True if OutAcousticPropertiesOverride is valid.\n\x09**/" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Set the Acoustic Properties overriding this Geometry.\n@param InMaterialInterface - If this Geometry's Mesh Type is set to Static Mesh, provide the Material Interface to override. Leave empty if the Mesh Type is set to Simple Collision.\n@param InAcousticPropertiesOverride - Structure of Acoustic Properties Override to set with.\n@param OutAcousticPropertiesOverride - Reference to the modified Acoustic Properties Override.\n@return True if OutAcousticPropertiesOverride is valid." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkGeometryComponent, nullptr, "SetAcousticPropertiesOverride", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::AkGeometryComponent_eventSetAcousticPropertiesOverride_Parms), Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04420401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics
	{
		struct AkGeometryComponent_eventSetAcousticTextureOverride_Parms
		{
			UMaterialInterface* InMaterialInterface;
			UAkAcousticTexture* InAcousticTexture;
			FAkGeometrySurfaceOverride OutAcousticPropertiesOverride;
			bool ReturnValue;
		};
		static const UECodeGen_Private::FObjectPropertyParams NewProp_InMaterialInterface;
		static const UECodeGen_Private::FObjectPropertyParams NewProp_InAcousticTexture;
		static const UECodeGen_Private::FStructPropertyParams NewProp_OutAcousticPropertiesOverride;
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::NewProp_InMaterialInterface = { "InMaterialInterface", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventSetAcousticTextureOverride_Parms, InMaterialInterface), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::NewProp_InAcousticTexture = { "InAcousticTexture", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventSetAcousticTextureOverride_Parms, InAcousticTexture), Z_Construct_UClass_UAkAcousticTexture_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::NewProp_OutAcousticPropertiesOverride = { "OutAcousticPropertiesOverride", nullptr, (EPropertyFlags)0x0010000000000180, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventSetAcousticTextureOverride_Parms, OutAcousticPropertiesOverride), Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride, METADATA_PARAMS(nullptr, 0) }; // 778562176
	void Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((AkGeometryComponent_eventSetAcousticTextureOverride_Parms*)Obj)->ReturnValue = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkGeometryComponent_eventSetAcousticTextureOverride_Parms), &Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::NewProp_InMaterialInterface,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::NewProp_InAcousticTexture,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::NewProp_OutAcousticPropertiesOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkGeometry" },
		{ "Comment", "/**\n\x09 * Set the Acoustic Texture overriding this Geometry.\n\x09 * @param InMaterialInterface - If this Geometry's Mesh Type is set to Static Mesh, provide the Material Interface to override. Leave empty if the Mesh Type is set to Simple Collision.\n\x09 * @param InAcousticTexture - Acoustic Texture to set with.\n\x09 * @param OutAcousticPropertiesOverride - Reference to the modified Acoustic Properties Override.\n\x09 * @return True if OutAcousticPropertiesOverride is valid.\n\x09**/" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Set the Acoustic Texture overriding this Geometry.\n@param InMaterialInterface - If this Geometry's Mesh Type is set to Static Mesh, provide the Material Interface to override. Leave empty if the Mesh Type is set to Simple Collision.\n@param InAcousticTexture - Acoustic Texture to set with.\n@param OutAcousticPropertiesOverride - Reference to the modified Acoustic Properties Override.\n@return True if OutAcousticPropertiesOverride is valid." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkGeometryComponent, nullptr, "SetAcousticTextureOverride", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::AkGeometryComponent_eventSetAcousticTextureOverride_Parms), Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04420401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics
	{
		struct AkGeometryComponent_eventSetEnableDiffraction_Parms
		{
			bool bInEnableDiffraction;
			bool bInEnableDiffractionOnBoundaryEdges;
		};
		static void NewProp_bInEnableDiffraction_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bInEnableDiffraction;
		static void NewProp_bInEnableDiffractionOnBoundaryEdges_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bInEnableDiffractionOnBoundaryEdges;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::NewProp_bInEnableDiffraction_SetBit(void* Obj)
	{
		((AkGeometryComponent_eventSetEnableDiffraction_Parms*)Obj)->bInEnableDiffraction = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::NewProp_bInEnableDiffraction = { "bInEnableDiffraction", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkGeometryComponent_eventSetEnableDiffraction_Parms), &Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::NewProp_bInEnableDiffraction_SetBit, METADATA_PARAMS(nullptr, 0) };
	void Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::NewProp_bInEnableDiffractionOnBoundaryEdges_SetBit(void* Obj)
	{
		((AkGeometryComponent_eventSetEnableDiffraction_Parms*)Obj)->bInEnableDiffractionOnBoundaryEdges = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::NewProp_bInEnableDiffractionOnBoundaryEdges = { "bInEnableDiffractionOnBoundaryEdges", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkGeometryComponent_eventSetEnableDiffraction_Parms), &Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::NewProp_bInEnableDiffractionOnBoundaryEdges_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::NewProp_bInEnableDiffraction,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::NewProp_bInEnableDiffractionOnBoundaryEdges,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkGeometry" },
		{ "Comment", "/**\n\x09* Enable or disable geometric diffraction for this mesh.\n\x09* @param bInEnableDiffraction - Set to true to have Wwise Spatial Audio generate diffraction edges on the geometry.\n\x09* @param bInEnableDiffractionOnBoundaryEdges - Set to true to enable geometric diffraction on boundary edges for this Geometry. Boundary edges are edges that are connected to only one triangle.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Enable or disable geometric diffraction for this mesh.\n@param bInEnableDiffraction - Set to true to have Wwise Spatial Audio generate diffraction edges on the geometry.\n@param bInEnableDiffractionOnBoundaryEdges - Set to true to enable geometric diffraction on boundary edges for this Geometry. Boundary edges are edges that are connected to only one triangle." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkGeometryComponent, nullptr, "SetEnableDiffraction", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::AkGeometryComponent_eventSetEnableDiffraction_Parms), Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics
	{
		struct AkGeometryComponent_eventSetEnableTransmissionLossOverride_Parms
		{
			UMaterialInterface* InMaterialInterface;
			bool bInEnableTransmissionLossOverride;
			FAkGeometrySurfaceOverride OutAcousticPropertiesOverride;
			bool ReturnValue;
		};
		static const UECodeGen_Private::FObjectPropertyParams NewProp_InMaterialInterface;
		static void NewProp_bInEnableTransmissionLossOverride_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bInEnableTransmissionLossOverride;
		static const UECodeGen_Private::FStructPropertyParams NewProp_OutAcousticPropertiesOverride;
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_InMaterialInterface = { "InMaterialInterface", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventSetEnableTransmissionLossOverride_Parms, InMaterialInterface), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(nullptr, 0) };
	void Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_bInEnableTransmissionLossOverride_SetBit(void* Obj)
	{
		((AkGeometryComponent_eventSetEnableTransmissionLossOverride_Parms*)Obj)->bInEnableTransmissionLossOverride = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_bInEnableTransmissionLossOverride = { "bInEnableTransmissionLossOverride", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkGeometryComponent_eventSetEnableTransmissionLossOverride_Parms), &Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_bInEnableTransmissionLossOverride_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_OutAcousticPropertiesOverride = { "OutAcousticPropertiesOverride", nullptr, (EPropertyFlags)0x0010000000000180, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventSetEnableTransmissionLossOverride_Parms, OutAcousticPropertiesOverride), Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride, METADATA_PARAMS(nullptr, 0) }; // 778562176
	void Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((AkGeometryComponent_eventSetEnableTransmissionLossOverride_Parms*)Obj)->ReturnValue = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkGeometryComponent_eventSetEnableTransmissionLossOverride_Parms), &Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_InMaterialInterface,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_bInEnableTransmissionLossOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_OutAcousticPropertiesOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkGeometry" },
		{ "Comment", "/**\n\x09 * Enable or disable the transmission loss of this Geometry to be overriden.\n\x09 * @param InMaterialInterface - If this Geometry's Mesh Type is set to Static Mesh, provide the Material Interface to override. Leave empty if the Mesh Type is set to Simple Collision.\n\x09 * @param bInEnableTransmissionLossOverride - Set to true to enable Transmission Loss override.\n\x09 * @param OutAcousticPropertiesOverride - Reference to the modified Acoustic Properties Override.\n\x09 * @return True if OutAcousticPropertiesOverride is valid.\n\x09**/" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Enable or disable the transmission loss of this Geometry to be overriden.\n@param InMaterialInterface - If this Geometry's Mesh Type is set to Static Mesh, provide the Material Interface to override. Leave empty if the Mesh Type is set to Simple Collision.\n@param bInEnableTransmissionLossOverride - Set to true to enable Transmission Loss override.\n@param OutAcousticPropertiesOverride - Reference to the modified Acoustic Properties Override.\n@return True if OutAcousticPropertiesOverride is valid." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkGeometryComponent, nullptr, "SetEnableTransmissionLossOverride", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::AkGeometryComponent_eventSetEnableTransmissionLossOverride_Parms), Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04420401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics
	{
		struct AkGeometryComponent_eventSetTransmissionLossOverride_Parms
		{
			UMaterialInterface* InMaterialInterface;
			float InTransmissionLoss;
			bool bInEnableTransmissionLossOverride;
			FAkGeometrySurfaceOverride OutAcousticPropertiesOverride;
			bool ReturnValue;
		};
		static const UECodeGen_Private::FObjectPropertyParams NewProp_InMaterialInterface;
		static const UECodeGen_Private::FFloatPropertyParams NewProp_InTransmissionLoss;
		static void NewProp_bInEnableTransmissionLossOverride_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bInEnableTransmissionLossOverride;
		static const UECodeGen_Private::FStructPropertyParams NewProp_OutAcousticPropertiesOverride;
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_InMaterialInterface = { "InMaterialInterface", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventSetTransmissionLossOverride_Parms, InMaterialInterface), Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_InTransmissionLoss = { "InTransmissionLoss", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventSetTransmissionLossOverride_Parms, InTransmissionLoss), METADATA_PARAMS(nullptr, 0) };
	void Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_bInEnableTransmissionLossOverride_SetBit(void* Obj)
	{
		((AkGeometryComponent_eventSetTransmissionLossOverride_Parms*)Obj)->bInEnableTransmissionLossOverride = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_bInEnableTransmissionLossOverride = { "bInEnableTransmissionLossOverride", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkGeometryComponent_eventSetTransmissionLossOverride_Parms), &Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_bInEnableTransmissionLossOverride_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_OutAcousticPropertiesOverride = { "OutAcousticPropertiesOverride", nullptr, (EPropertyFlags)0x0010000000000180, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkGeometryComponent_eventSetTransmissionLossOverride_Parms, OutAcousticPropertiesOverride), Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride, METADATA_PARAMS(nullptr, 0) }; // 778562176
	void Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((AkGeometryComponent_eventSetTransmissionLossOverride_Parms*)Obj)->ReturnValue = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkGeometryComponent_eventSetTransmissionLossOverride_Parms), &Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_InMaterialInterface,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_InTransmissionLoss,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_bInEnableTransmissionLossOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_OutAcousticPropertiesOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkGeometry" },
		{ "Comment", "/**\n\x09 * Set the Transmission Loss overriding this Geometry.\n\x09 * @param InMaterialInterface - If this Geometry's Mesh Type is set to Static Mesh, provide the Material Interface to override. Leave empty if the Mesh Type is set to Simple Collision.\n\x09 * @param InTransmissionLoss - Transmission Loss value to set with.\n\x09 * @param OutAcousticPropertiesOverride - Reference to the modified Acoustic Properties Override.\n\x09 * @return True if OutAcousticPropertiesOverride is valid.\n\x09**/" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Set the Transmission Loss overriding this Geometry.\n@param InMaterialInterface - If this Geometry's Mesh Type is set to Static Mesh, provide the Material Interface to override. Leave empty if the Mesh Type is set to Simple Collision.\n@param InTransmissionLoss - Transmission Loss value to set with.\n@param OutAcousticPropertiesOverride - Reference to the modified Acoustic Properties Override.\n@return True if OutAcousticPropertiesOverride is valid." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkGeometryComponent, nullptr, "SetTransmissionLossOverride", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::AkGeometryComponent_eventSetTransmissionLossOverride_Parms), Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04420401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkGeometryComponent_UpdateGeometry_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkGeometryComponent_UpdateGeometry_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkGeometry" },
		{ "Comment", "/** Add or update an instance of the geometry by sending the transform of this component to Wwise.\n\x09* A geometry instance is a unique instance of a geometry set with a specified transform (position, rotation and scale).\n\x09* It is necessary to create at least one geometry instance for each geometry set that is to be used for diffraction and reflection simulation. */" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Add or update an instance of the geometry by sending the transform of this component to Wwise.\nA geometry instance is a unique instance of a geometry set with a specified transform (position, rotation and scale).\nIt is necessary to create at least one geometry instance for each geometry set that is to be used for diffraction and reflection simulation." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkGeometryComponent_UpdateGeometry_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkGeometryComponent, nullptr, "UpdateGeometry", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkGeometryComponent_UpdateGeometry_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkGeometryComponent_UpdateGeometry_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkGeometryComponent_UpdateGeometry()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkGeometryComponent_UpdateGeometry_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAkGeometryComponent);
	UClass* Z_Construct_UClass_UAkGeometryComponent_NoRegister()
	{
		return UAkGeometryComponent::StaticClass();
	}
	struct Z_Construct_UClass_UAkGeometryComponent_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const UECodeGen_Private::FBytePropertyParams NewProp_MeshType_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_MeshType_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_MeshType;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_LOD_MetaData[];
#endif
		static const UECodeGen_Private::FUnsizedIntPropertyParams NewProp_LOD;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_WeldingThreshold_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_WeldingThreshold;
		static const UECodeGen_Private::FStructPropertyParams NewProp_StaticMeshSurfaceOverride_ValueProp;
		static const UECodeGen_Private::FObjectPropertyParams NewProp_StaticMeshSurfaceOverride_Key_KeyProp;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_StaticMeshSurfaceOverride_MetaData[];
#endif
		static const UECodeGen_Private::FMapPropertyParams NewProp_StaticMeshSurfaceOverride;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_CollisionMeshSurfaceOverride_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_CollisionMeshSurfaceOverride;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableDiffraction_MetaData[];
#endif
		static void NewProp_bEnableDiffraction_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableDiffraction;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableDiffractionOnBoundaryEdges_MetaData[];
#endif
		static void NewProp_bEnableDiffractionOnBoundaryEdges_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableDiffractionOnBoundaryEdges;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_AssociatedRoom_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_AssociatedRoom;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_GeometryData_MetaData[];
#endif
		static const UECodeGen_Private::FStructPropertyParams NewProp_GeometryData;
		static const UECodeGen_Private::FDoublePropertyParams NewProp_SurfaceAreas_ValueProp;
		static const UECodeGen_Private::FUnsizedIntPropertyParams NewProp_SurfaceAreas_Key_KeyProp;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_SurfaceAreas_MetaData[];
#endif
		static const UECodeGen_Private::FMapPropertyParams NewProp_SurfaceAreas;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAkGeometryComponent_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UAkAcousticTextureSetComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_AkAudio,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UAkGeometryComponent_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UAkGeometryComponent_ConvertMesh, "ConvertMesh" }, // 580659881
		{ &Z_Construct_UFunction_UAkGeometryComponent_GetAcousticPropertiesOverride, "GetAcousticPropertiesOverride" }, // 16579662
		{ &Z_Construct_UFunction_UAkGeometryComponent_RemoveGeometry, "RemoveGeometry" }, // 2730140322
		{ &Z_Construct_UFunction_UAkGeometryComponent_SendGeometry, "SendGeometry" }, // 3439402521
		{ &Z_Construct_UFunction_UAkGeometryComponent_SetAcousticPropertiesOverride, "SetAcousticPropertiesOverride" }, // 4090877694
		{ &Z_Construct_UFunction_UAkGeometryComponent_SetAcousticTextureOverride, "SetAcousticTextureOverride" }, // 2816010801
		{ &Z_Construct_UFunction_UAkGeometryComponent_SetEnableDiffraction, "SetEnableDiffraction" }, // 1739931958
		{ &Z_Construct_UFunction_UAkGeometryComponent_SetEnableTransmissionLossOverride, "SetEnableTransmissionLossOverride" }, // 23609035
		{ &Z_Construct_UFunction_UAkGeometryComponent_SetTransmissionLossOverride, "SetTransmissionLossOverride" }, // 2191410476
		{ &Z_Construct_UFunction_UAkGeometryComponent_UpdateGeometry, "UpdateGeometry" }, // 1182016530
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkGeometryComponent_Statics::Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "BlueprintType", "true" },
		{ "ClassGroupNames", "Audiokinetic" },
		{ "HideCategories", "Transform Rendering Mobility LOD Component Activation Tags Trigger PhysicsVolume" },
		{ "IncludePath", "AkGeometryComponent.h" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
#endif
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_MeshType_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 0, nullptr, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_MeshType_MetaData[] = {
		{ "Category", "Geometry" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_MeshType = { "MeshType", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkGeometryComponent, MeshType), Z_Construct_UEnum_AkAudio_AkMeshType, METADATA_PARAMS(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_MeshType_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_MeshType_MetaData)) }; // 1709138623
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_LOD_MetaData[] = {
		{ "Category", "Geometry" },
		{ "ClampMin", "0.0" },
		{ "Comment", "/** The Static Mesh's LOD to use */" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "The Static Mesh's LOD to use" },
	};
#endif
	const UECodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_LOD = { "LOD", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkGeometryComponent, LOD), METADATA_PARAMS(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_LOD_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_LOD_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_WeldingThreshold_MetaData[] = {
		{ "Category", "Geometry" },
		{ "ClampMin", "0.0" },
		{ "Comment", "/** The local distance in Unreal units between two vertices to be welded together.\n\x09* Any two vertices closer than this threshold will be treated as the same unique vertex and assigned the same position.\n\x09* Increasing this threshold decreases the number of gaps between triangles, resulting in a more continuous mesh and less sound leaking though, as well as eliminating triangles that are too small to be significant.\n\x09* Increasing this threshold also helps Spatial Audio's edge-finding algorithm to find more valid diffraction edges.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "The local distance in Unreal units between two vertices to be welded together.\nAny two vertices closer than this threshold will be treated as the same unique vertex and assigned the same position.\nIncreasing this threshold decreases the number of gaps between triangles, resulting in a more continuous mesh and less sound leaking though, as well as eliminating triangles that are too small to be significant.\nIncreasing this threshold also helps Spatial Audio's edge-finding algorithm to find more valid diffraction edges." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_WeldingThreshold = { "WeldingThreshold", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkGeometryComponent, WeldingThreshold), METADATA_PARAMS(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_WeldingThreshold_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_WeldingThreshold_MetaData)) };
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_StaticMeshSurfaceOverride_ValueProp = { "StaticMeshSurfaceOverride", nullptr, (EPropertyFlags)0x0000000000020001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 1, Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride, METADATA_PARAMS(nullptr, 0) }; // 778562176
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_StaticMeshSurfaceOverride_Key_KeyProp = { "StaticMeshSurfaceOverride_Key", nullptr, (EPropertyFlags)0x0000000000020001, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 0, Z_Construct_UClass_UMaterialInterface_NoRegister, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_StaticMeshSurfaceOverride_MetaData[] = {
		{ "Category", "Geometry" },
		{ "Comment", "/** Override the acoustic properties of this mesh per material.*/" },
		{ "DisplayName", "Acoustic Properties Override" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Override the acoustic properties of this mesh per material." },
	};
#endif
	const UECodeGen_Private::FMapPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_StaticMeshSurfaceOverride = { "StaticMeshSurfaceOverride", nullptr, (EPropertyFlags)0x0010000000020001, UECodeGen_Private::EPropertyGenFlags::Map, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkGeometryComponent, StaticMeshSurfaceOverride), EMapPropertyFlags::None, METADATA_PARAMS(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_StaticMeshSurfaceOverride_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_StaticMeshSurfaceOverride_MetaData)) }; // 778562176
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_CollisionMeshSurfaceOverride_MetaData[] = {
		{ "Category", "Geometry" },
		{ "Comment", "/** Override the acoustic properties of the collision mesh.*/" },
		{ "DisplayName", "Acoustic Properties Override" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Override the acoustic properties of the collision mesh." },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_CollisionMeshSurfaceOverride = { "CollisionMeshSurfaceOverride", nullptr, (EPropertyFlags)0x0010000000020001, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkGeometryComponent, CollisionMeshSurfaceOverride), Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride, METADATA_PARAMS(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_CollisionMeshSurfaceOverride_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_CollisionMeshSurfaceOverride_MetaData)) }; // 778562176
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffraction_MetaData[] = {
		{ "Category", "Geometry" },
		{ "Comment", "/** Enable or disable geometric diffraction for this mesh. Check this box to have Wwise Spatial Audio generate diffraction edges on the geometry. The diffraction edges will be visible in the Wwise game object viewer when connected to the game. */" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Enable or disable geometric diffraction for this mesh. Check this box to have Wwise Spatial Audio generate diffraction edges on the geometry. The diffraction edges will be visible in the Wwise game object viewer when connected to the game." },
	};
#endif
	void Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffraction_SetBit(void* Obj)
	{
		((UAkGeometryComponent*)Obj)->bEnableDiffraction = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffraction = { "bEnableDiffraction", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UAkGeometryComponent), &Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffraction_SetBit, METADATA_PARAMS(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffraction_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffraction_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffractionOnBoundaryEdges_MetaData[] = {
		{ "Category", "Geometry" },
		{ "Comment", "/** Enable or disable geometric diffraction on boundary edges for this Geometry. Boundary edges are edges that are connected to only one triangle. Depending on the specific shape of the geometry, boundary edges may or may not be useful and it is beneficial to reduce the total number of diffraction edges to process.  */" },
		{ "EditCondition", "bEnableDiffraction" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "Enable or disable geometric diffraction on boundary edges for this Geometry. Boundary edges are edges that are connected to only one triangle. Depending on the specific shape of the geometry, boundary edges may or may not be useful and it is beneficial to reduce the total number of diffraction edges to process." },
	};
#endif
	void Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffractionOnBoundaryEdges_SetBit(void* Obj)
	{
		((UAkGeometryComponent*)Obj)->bEnableDiffractionOnBoundaryEdges = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffractionOnBoundaryEdges = { "bEnableDiffractionOnBoundaryEdges", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UAkGeometryComponent), &Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffractionOnBoundaryEdges_SetBit, METADATA_PARAMS(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffractionOnBoundaryEdges_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffractionOnBoundaryEdges_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_AssociatedRoom_MetaData[] = {
		{ "Category", "Geometry" },
		{ "Comment", "/** (Deprecated) Associate this Geometry component with a Room.\n\x09* This property is deprecated and will be removed in a future version. We recommend not using it by leaving it set to None.\n\x09* Associating a Geometry component with a particular Room limits the scope in which the geometry is accessible. Doing so reduces the search space for ray casting performed by reflection and diffraction calculations.\n\x09* When set to None, this geometry has a global scope.\n\x09* Note if one or more geometry sets are associated with a room, that room can no longer access geometry that is in the global scope.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
		{ "ToolTip", "(Deprecated) Associate this Geometry component with a Room.\nThis property is deprecated and will be removed in a future version. We recommend not using it by leaving it set to None.\nAssociating a Geometry component with a particular Room limits the scope in which the geometry is accessible. Doing so reduces the search space for ray casting performed by reflection and diffraction calculations.\nWhen set to None, this geometry has a global scope.\nNote if one or more geometry sets are associated with a room, that room can no longer access geometry that is in the global scope." },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_AssociatedRoom = { "AssociatedRoom", nullptr, (EPropertyFlags)0x0010040000000005, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkGeometryComponent, AssociatedRoom), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_AssociatedRoom_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_AssociatedRoom_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_GeometryData_MetaData[] = {
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
	};
#endif
	const UECodeGen_Private::FStructPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_GeometryData = { "GeometryData", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Struct, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkGeometryComponent, GeometryData), Z_Construct_UScriptStruct_FAkGeometryData, METADATA_PARAMS(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_GeometryData_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_GeometryData_MetaData)) }; // 2163801865
	const UECodeGen_Private::FDoublePropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_SurfaceAreas_ValueProp = { "SurfaceAreas", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Double, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 1, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FUnsizedIntPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_SurfaceAreas_Key_KeyProp = { "SurfaceAreas_Key", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Int, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 0, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_SurfaceAreas_MetaData[] = {
		{ "ModuleRelativePath", "Classes/AkGeometryComponent.h" },
	};
#endif
	const UECodeGen_Private::FMapPropertyParams Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_SurfaceAreas = { "SurfaceAreas", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Map, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkGeometryComponent, SurfaceAreas), EMapPropertyFlags::None, METADATA_PARAMS(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_SurfaceAreas_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_SurfaceAreas_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAkGeometryComponent_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_MeshType_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_MeshType,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_LOD,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_WeldingThreshold,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_StaticMeshSurfaceOverride_ValueProp,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_StaticMeshSurfaceOverride_Key_KeyProp,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_StaticMeshSurfaceOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_CollisionMeshSurfaceOverride,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffraction,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_bEnableDiffractionOnBoundaryEdges,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_AssociatedRoom,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_GeometryData,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_SurfaceAreas_ValueProp,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_SurfaceAreas_Key_KeyProp,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkGeometryComponent_Statics::NewProp_SurfaceAreas,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAkGeometryComponent_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAkGeometryComponent>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UAkGeometryComponent_Statics::ClassParams = {
		&UAkGeometryComponent::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UAkGeometryComponent_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::PropPointers),
		0,
		0x00B000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UAkGeometryComponent_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UAkGeometryComponent_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UAkGeometryComponent()
	{
		if (!Z_Registration_Info_UClass_UAkGeometryComponent.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAkGeometryComponent.OuterSingleton, Z_Construct_UClass_UAkGeometryComponent_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UAkGeometryComponent.OuterSingleton;
	}
	template<> AKAUDIO_API UClass* StaticClass<UAkGeometryComponent>()
	{
		return UAkGeometryComponent::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAkGeometryComponent);
	UAkGeometryComponent::~UAkGeometryComponent() {}
	IMPLEMENT_FSTRUCTUREDARCHIVE_SERIALIZER(UAkGeometryComponent)
	struct Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGeometryComponent_h_Statics
	{
		static const FEnumRegisterCompiledInInfo EnumInfo[];
		static const FStructRegisterCompiledInInfo ScriptStructInfo[];
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FEnumRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGeometryComponent_h_Statics::EnumInfo[] = {
		{ AkMeshType_StaticEnum, TEXT("AkMeshType"), &Z_Registration_Info_UEnum_AkMeshType, CONSTRUCT_RELOAD_VERSION_INFO(FEnumReloadVersionInfo, 1709138623U) },
	};
	const FStructRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGeometryComponent_h_Statics::ScriptStructInfo[] = {
		{ FAkGeometrySurfaceOverride::StaticStruct, Z_Construct_UScriptStruct_FAkGeometrySurfaceOverride_Statics::NewStructOps, TEXT("AkGeometrySurfaceOverride"), &Z_Registration_Info_UScriptStruct_AkGeometrySurfaceOverride, CONSTRUCT_RELOAD_VERSION_INFO(FStructReloadVersionInfo, sizeof(FAkGeometrySurfaceOverride), 778562176U) },
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGeometryComponent_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UAkGeometryComponent, UAkGeometryComponent::StaticClass, TEXT("UAkGeometryComponent"), &Z_Registration_Info_UClass_UAkGeometryComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAkGeometryComponent), 1659473147U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGeometryComponent_h_2233789752(TEXT("/Script/AkAudio"),
		Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGeometryComponent_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGeometryComponent_h_Statics::ClassInfo),
		Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGeometryComponent_h_Statics::ScriptStructInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGeometryComponent_h_Statics::ScriptStructInfo),
		Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGeometryComponent_h_Statics::EnumInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkGeometryComponent_h_Statics::EnumInfo));
PRAGMA_ENABLE_DEPRECATION_WARNINGS
