// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AkAudio/Classes/AkAcousticPortal.h"
#include "Serialization/ArchiveUObjectFromStructuredArchive.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAkAcousticPortal() {}
// Cross Module References
	AKAUDIO_API UClass* Z_Construct_UClass_AAkAcousticPortal();
	AKAUDIO_API UClass* Z_Construct_UClass_AAkAcousticPortal_NoRegister();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkPortalComponent();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkPortalComponent_NoRegister();
	AKAUDIO_API UEnum* Z_Construct_UEnum_AkAudio_AkAcousticPortalState();
	AKAUDIO_API UEnum* Z_Construct_UEnum_AkAudio_EAkCollisionChannel();
	ENGINE_API UClass* Z_Construct_UClass_AVolume();
	ENGINE_API UClass* Z_Construct_UClass_UPrimitiveComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_USceneComponent();
	ENGINE_API UClass* Z_Construct_UClass_UTextRenderComponent_NoRegister();
	ENGINE_API UEnum* Z_Construct_UEnum_Engine_ECollisionChannel();
	UPackage* Z_Construct_UPackage__Script_AkAudio();
// End Cross Module References
	DEFINE_FUNCTION(UAkPortalComponent::execSetDynamic)
	{
		P_GET_UBOOL(Z_Param_bInDynamic);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->SetDynamic(Z_Param_bInDynamic);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkPortalComponent::execPortalPlacementValid)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(bool*)Z_Param__Result=P_THIS->PortalPlacementValid();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkPortalComponent::execGetPrimitiveParent)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(UPrimitiveComponent**)Z_Param__Result=P_THIS->GetPrimitiveParent();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkPortalComponent::execSetPortalOcclusion)
	{
		P_GET_PROPERTY(FFloatProperty,Z_Param_InPortalOcclusion);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->SetPortalOcclusion(Z_Param_InPortalOcclusion);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkPortalComponent::execGetPortalOcclusion)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(float*)Z_Param__Result=P_THIS->GetPortalOcclusion();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkPortalComponent::execGetCurrentState)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(AkAcousticPortalState*)Z_Param__Result=P_THIS->GetCurrentState();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkPortalComponent::execDisablePortal)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->DisablePortal();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkPortalComponent::execEnablePortal)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->EnablePortal();
		P_NATIVE_END;
	}
	void UAkPortalComponent::StaticRegisterNativesUAkPortalComponent()
	{
		UClass* Class = UAkPortalComponent::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "DisablePortal", &UAkPortalComponent::execDisablePortal },
			{ "EnablePortal", &UAkPortalComponent::execEnablePortal },
			{ "GetCurrentState", &UAkPortalComponent::execGetCurrentState },
			{ "GetPortalOcclusion", &UAkPortalComponent::execGetPortalOcclusion },
			{ "GetPrimitiveParent", &UAkPortalComponent::execGetPrimitiveParent },
			{ "PortalPlacementValid", &UAkPortalComponent::execPortalPlacementValid },
			{ "SetDynamic", &UAkPortalComponent::execSetDynamic },
			{ "SetPortalOcclusion", &UAkPortalComponent::execSetPortalOcclusion },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UAkPortalComponent_DisablePortal_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkPortalComponent_DisablePortal_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkPortalComponent" },
		{ "Comment", "/**\n\x09 * Disables the portal. Emitters positioned in the AkRoomComponent in front of and behind the portal do not emit through it.\n\x09 */" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Disables the portal. Emitters positioned in the AkRoomComponent in front of and behind the portal do not emit through it." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkPortalComponent_DisablePortal_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkPortalComponent, nullptr, "DisablePortal", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkPortalComponent_DisablePortal_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_DisablePortal_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkPortalComponent_DisablePortal()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkPortalComponent_DisablePortal_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkPortalComponent_EnablePortal_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkPortalComponent_EnablePortal_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkPortalComponent" },
		{ "Comment", "/**\n\x09 * Enables the portal. Emitters positioned in the AkRoomComponent in front of and behind the portal emit through it.\n\x09 */" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Enables the portal. Emitters positioned in the AkRoomComponent in front of and behind the portal emit through it." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkPortalComponent_EnablePortal_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkPortalComponent, nullptr, "EnablePortal", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkPortalComponent_EnablePortal_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_EnablePortal_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkPortalComponent_EnablePortal()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkPortalComponent_EnablePortal_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics
	{
		struct AkPortalComponent_eventGetCurrentState_Parms
		{
			AkAcousticPortalState ReturnValue;
		};
		static const UECodeGen_Private::FBytePropertyParams NewProp_ReturnValue_Underlying;
		static const UECodeGen_Private::FEnumPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::NewProp_ReturnValue_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 0, nullptr, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkPortalComponent_eventGetCurrentState_Parms, ReturnValue), Z_Construct_UEnum_AkAudio_AkAcousticPortalState, METADATA_PARAMS(nullptr, 0) }; // 4270424843
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::NewProp_ReturnValue_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkPortalComponent" },
		{ "Comment", "/**\n\x09 * Returns an AkAcousticPortalState, which represents current the state of the portal: Enabled or Disabled.\n\x09 */" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Returns an AkAcousticPortalState, which represents current the state of the portal: Enabled or Disabled." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkPortalComponent, nullptr, "GetCurrentState", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::AkPortalComponent_eventGetCurrentState_Parms), Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkPortalComponent_GetCurrentState()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkPortalComponent_GetCurrentState_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics
	{
		struct AkPortalComponent_eventGetPortalOcclusion_Parms
		{
			float ReturnValue;
		};
		static const UECodeGen_Private::FFloatPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkPortalComponent_eventGetPortalOcclusion_Parms, ReturnValue), METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkPortalComponent" },
		{ "Comment", "/**\n\x09 * Returns a floating point number between 0 and 1 that represents the occlusion value applied to the portal. A value of 0 indicates that the portal is not occluded and a value of 1 indicates that it is completely occluded.\n\x09 */" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Returns a floating point number between 0 and 1 that represents the occlusion value applied to the portal. A value of 0 indicates that the portal is not occluded and a value of 1 indicates that it is completely occluded." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkPortalComponent, nullptr, "GetPortalOcclusion", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics::AkPortalComponent_eventGetPortalOcclusion_Parms), Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics
	{
		struct AkPortalComponent_eventGetPrimitiveParent_Parms
		{
			UPrimitiveComponent* ReturnValue;
		};
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ReturnValue_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::NewProp_ReturnValue_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000080588, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkPortalComponent_eventGetPrimitiveParent_Parms, ReturnValue), Z_Construct_UClass_UPrimitiveComponent_NoRegister, METADATA_PARAMS(Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::NewProp_ReturnValue_MetaData, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::NewProp_ReturnValue_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkPortalComponent" },
		{ "Comment", "/**\n\x09 * Returns the UPrimitiveComponent to which this Ak Portal Component is attached.\n\x09 */" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Returns the UPrimitiveComponent to which this Ak Portal Component is attached." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkPortalComponent, nullptr, "GetPrimitiveParent", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::AkPortalComponent_eventGetPrimitiveParent_Parms), Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics
	{
		struct AkPortalComponent_eventPortalPlacementValid_Parms
		{
			bool ReturnValue;
		};
		static void NewProp_ReturnValue_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::NewProp_ReturnValue_SetBit(void* Obj)
	{
		((AkPortalComponent_eventPortalPlacementValid_Parms*)Obj)->ReturnValue = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkPortalComponent_eventPortalPlacementValid_Parms), &Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::NewProp_ReturnValue_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkPortalComponent" },
		{ "Comment", "/**\n\x09 * Returns true if the portal position and orientation are valid.\n\x09 * Portals have a front and a back room. They must have at least one connected room,\n\x09 * the front room must be different than the back room\n\x09 * and both Rooms cannot be part of the same Reverb Zone hierarchy.\n\x09 */" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Returns true if the portal position and orientation are valid.\nPortals have a front and a back room. They must have at least one connected room,\nthe front room must be different than the back room\nand both Rooms cannot be part of the same Reverb Zone hierarchy." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkPortalComponent, nullptr, "PortalPlacementValid", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::AkPortalComponent_eventPortalPlacementValid_Parms), Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics
	{
		struct AkPortalComponent_eventSetDynamic_Parms
		{
			bool bInDynamic;
		};
		static void NewProp_bInDynamic_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bInDynamic;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::NewProp_bInDynamic_SetBit(void* Obj)
	{
		((AkPortalComponent_eventSetDynamic_Parms*)Obj)->bInDynamic = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::NewProp_bInDynamic = { "bInDynamic", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkPortalComponent_eventSetDynamic_Parms), &Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::NewProp_bInDynamic_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::NewProp_bInDynamic,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::Function_MetaDataParams[] = {
		{ "BlueprintSetter", "" },
		{ "Category", "AkPortalComponent" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkPortalComponent, nullptr, "SetDynamic", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::AkPortalComponent_eventSetDynamic_Parms), Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkPortalComponent_SetDynamic()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkPortalComponent_SetDynamic_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics
	{
		struct AkPortalComponent_eventSetPortalOcclusion_Parms
		{
			float InPortalOcclusion;
		};
		static const UECodeGen_Private::FFloatPropertyParams NewProp_InPortalOcclusion;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics::NewProp_InPortalOcclusion = { "InPortalOcclusion", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkPortalComponent_eventSetPortalOcclusion_Parms, InPortalOcclusion), METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics::NewProp_InPortalOcclusion,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkPortalComponent" },
		{ "Comment", "/**\n\x09 * Sets a new portal occlusion value. A value of 0 indicates that the portal is not occluded and a value of 1 indicates that it is completely occluded.\n\x09 * The occlusion value is applied to the portal with AK::SpatialAudio::SetPortalObstructionAndOcclusion.\n\x09 * Portal occlusion can be used to modulate sound in response to a door opening or closing.\n\x09 *\n\x09 * @param InPortalOcclusion\x09The new portal occlusion value.\n\x09 */" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Sets a new portal occlusion value. A value of 0 indicates that the portal is not occluded and a value of 1 indicates that it is completely occluded.\nThe occlusion value is applied to the portal with AK::SpatialAudio::SetPortalObstructionAndOcclusion.\nPortal occlusion can be used to modulate sound in response to a door opening or closing.\n\n@param InPortalOcclusion     The new portal occlusion value." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkPortalComponent, nullptr, "SetPortalOcclusion", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics::AkPortalComponent_eventSetPortalOcclusion_Parms), Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAkPortalComponent);
	UClass* Z_Construct_UClass_UAkPortalComponent_NoRegister()
	{
		return UAkPortalComponent::StaticClass();
	}
	struct Z_Construct_UClass_UAkPortalComponent_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bDynamic_MetaData[];
#endif
		static void NewProp_bDynamic_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bDynamic;
		static const UECodeGen_Private::FBytePropertyParams NewProp_InitialState_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_InitialState_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_InitialState;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_InitialOcclusion_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_InitialOcclusion;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ObstructionRefreshInterval_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_ObstructionRefreshInterval;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ObstructionCollisionChannel_MetaData[];
#endif
		static const UECodeGen_Private::FBytePropertyParams NewProp_ObstructionCollisionChannel;
#if WITH_EDITORONLY_DATA
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_FrontRoomText_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_FrontRoomText;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_BackRoomText_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_BackRoomText;
#endif // WITH_EDITORONLY_DATA
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_EDITORONLY_DATA
#endif // WITH_EDITORONLY_DATA
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAkPortalComponent_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_USceneComponent,
		(UObject* (*)())Z_Construct_UPackage__Script_AkAudio,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UAkPortalComponent_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UAkPortalComponent_DisablePortal, "DisablePortal" }, // 1597381513
		{ &Z_Construct_UFunction_UAkPortalComponent_EnablePortal, "EnablePortal" }, // 1695185607
		{ &Z_Construct_UFunction_UAkPortalComponent_GetCurrentState, "GetCurrentState" }, // 186939637
		{ &Z_Construct_UFunction_UAkPortalComponent_GetPortalOcclusion, "GetPortalOcclusion" }, // 1163990530
		{ &Z_Construct_UFunction_UAkPortalComponent_GetPrimitiveParent, "GetPrimitiveParent" }, // 2333533975
		{ &Z_Construct_UFunction_UAkPortalComponent_PortalPlacementValid, "PortalPlacementValid" }, // 2045273326
		{ &Z_Construct_UFunction_UAkPortalComponent_SetDynamic, "SetDynamic" }, // 2457006829
		{ &Z_Construct_UFunction_UAkPortalComponent_SetPortalOcclusion, "SetPortalOcclusion" }, // 1095921169
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkPortalComponent_Statics::Class_MetaDataParams[] = {
		{ "BlueprintSpawnableComponent", "" },
		{ "BlueprintType", "true" },
		{ "ClassGroupNames", "Audiokinetic" },
		{ "HideCategories", "Advanced Attachment Volume Trigger PhysicsVolume" },
		{ "IncludePath", "AkAcousticPortal.h" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_bDynamic_MetaData[] = {
		{ "BlueprintSetter", "SetDynamic" },
		{ "Category", "AkPortalComponent" },
		{ "Comment", "/**\n\x09* If true, the room connections for this portal can change during runtime when this portal moves.\n\x09* For worlds containing many rooms, this can be expensive.\n\x09* Note that this portal's room connections may still change, even when bDynamic = false,\n\x09* when dynamic rooms are moved (i.e. when rooms move who have bDynamic = true),\n\x09* or rooms are enabled or disabled.\n\x09*/" },
		{ "DisplayName", "Is Dynamic" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "If true, the room connections for this portal can change during runtime when this portal moves.\nFor worlds containing many rooms, this can be expensive.\nNote that this portal's room connections may still change, even when bDynamic = false,\nwhen dynamic rooms are moved (i.e. when rooms move who have bDynamic = true),\nor rooms are enabled or disabled." },
	};
#endif
	void Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_bDynamic_SetBit(void* Obj)
	{
		((UAkPortalComponent*)Obj)->bDynamic = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_bDynamic = { "bDynamic", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UAkPortalComponent), &Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_bDynamic_SetBit, METADATA_PARAMS(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_bDynamic_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_bDynamic_MetaData)) };
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialState_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 0, nullptr, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialState_MetaData[] = {
		{ "Category", "AkPortalComponent" },
		{ "Comment", "/**\n\x09 * Initially enables or disables the portal. When the portal is enabled, emitters positioned in the AkRoomComponent in front of and behind the portal emit through it.\n\x09 */" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Initially enables or disables the portal. When the portal is enabled, emitters positioned in the AkRoomComponent in front of and behind the portal emit through it." },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialState = { "InitialState", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkPortalComponent, InitialState), Z_Construct_UEnum_AkAudio_AkAcousticPortalState, METADATA_PARAMS(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialState_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialState_MetaData)) }; // 4270424843
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialOcclusion_MetaData[] = {
		{ "Category", "AkPortalComponent|Obstruction Occlusion" },
		{ "ClampMax", "1.000000" },
		{ "ClampMin", "0.000000" },
		{ "Comment", "/**\n\x09* The initial occlusion value applied to the portal. When the occlusion value is set to 0, the portal is not occluded, and when it is set to 1, the portal is completely occluded.\n\x09* The occlusion value is directly applied to the portal with AK::SpatialAudio::SetPortalObstructionAndOcclusion.\n\x09* Portal occlusion can be used to modulate sound in response to a door opening or closing.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "The initial occlusion value applied to the portal. When the occlusion value is set to 0, the portal is not occluded, and when it is set to 1, the portal is completely occluded.\nThe occlusion value is directly applied to the portal with AK::SpatialAudio::SetPortalObstructionAndOcclusion.\nPortal occlusion can be used to modulate sound in response to a door opening or closing." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialOcclusion = { "InitialOcclusion", nullptr, (EPropertyFlags)0x0010000000000015, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkPortalComponent, InitialOcclusion), METADATA_PARAMS(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialOcclusion_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialOcclusion_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_ObstructionRefreshInterval_MetaData[] = {
		{ "Category", "AkPortalComponent|Obstruction Occlusion" },
		{ "ClampMin", "0.000000" },
		{ "Comment", "/** Time interval between obstruction checks; a direct line of sight between the current portal and an emitter, a listener, or another portal. Set to 0 to disable obstruction checks. Valid range [0, [.*/" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Time interval between obstruction checks; a direct line of sight between the current portal and an emitter, a listener, or another portal. Set to 0 to disable obstruction checks. Valid range [0, [." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_ObstructionRefreshInterval = { "ObstructionRefreshInterval", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkPortalComponent, ObstructionRefreshInterval), METADATA_PARAMS(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_ObstructionRefreshInterval_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_ObstructionRefreshInterval_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_ObstructionCollisionChannel_MetaData[] = {
		{ "Category", "AkPortalComponent|Obstruction Occlusion" },
		{ "Comment", "/** Collision channel for obstruction checks; a direct line of sight between the current portal and an emitter, a listener, or another portal. */" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Collision channel for obstruction checks; a direct line of sight between the current portal and an emitter, a listener, or another portal." },
	};
#endif
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_ObstructionCollisionChannel = { "ObstructionCollisionChannel", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkPortalComponent, ObstructionCollisionChannel), Z_Construct_UEnum_Engine_ECollisionChannel, METADATA_PARAMS(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_ObstructionCollisionChannel_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_ObstructionCollisionChannel_MetaData)) }; // 727872708
#if WITH_EDITORONLY_DATA
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_FrontRoomText_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_FrontRoomText = { "FrontRoomText", nullptr, (EPropertyFlags)0x00c0000c00080008, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkPortalComponent, FrontRoomText), Z_Construct_UClass_UTextRenderComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_FrontRoomText_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_FrontRoomText_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_BackRoomText_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_BackRoomText = { "BackRoomText", nullptr, (EPropertyFlags)0x00c0000c00080008, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkPortalComponent, BackRoomText), Z_Construct_UClass_UTextRenderComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_BackRoomText_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_BackRoomText_MetaData)) };
#endif // WITH_EDITORONLY_DATA
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAkPortalComponent_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_bDynamic,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialState_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialState,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_InitialOcclusion,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_ObstructionRefreshInterval,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_ObstructionCollisionChannel,
#if WITH_EDITORONLY_DATA
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_FrontRoomText,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkPortalComponent_Statics::NewProp_BackRoomText,
#endif // WITH_EDITORONLY_DATA
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAkPortalComponent_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAkPortalComponent>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UAkPortalComponent_Statics::ClassParams = {
		&UAkPortalComponent::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UAkPortalComponent_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		UE_ARRAY_COUNT(Z_Construct_UClass_UAkPortalComponent_Statics::PropPointers),
		0,
		0x00B000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UAkPortalComponent_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UAkPortalComponent_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UAkPortalComponent()
	{
		if (!Z_Registration_Info_UClass_UAkPortalComponent.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAkPortalComponent.OuterSingleton, Z_Construct_UClass_UAkPortalComponent_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UAkPortalComponent.OuterSingleton;
	}
	template<> AKAUDIO_API UClass* StaticClass<UAkPortalComponent>()
	{
		return UAkPortalComponent::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAkPortalComponent);
	UAkPortalComponent::~UAkPortalComponent() {}
	DEFINE_FUNCTION(AAkAcousticPortal::execGetCurrentState)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(AkAcousticPortalState*)Z_Param__Result=P_THIS->GetCurrentState();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(AAkAcousticPortal::execDisablePortal)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->DisablePortal();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(AAkAcousticPortal::execEnablePortal)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->EnablePortal();
		P_NATIVE_END;
	}
#if WITH_EDITOR
	DEFINE_FUNCTION(AAkAcousticPortal::execGetCollisionChannel)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(TEnumAsByte<ECollisionChannel>*)Z_Param__Result=P_THIS->GetCollisionChannel();
		P_NATIVE_END;
	}
#endif //WITH_EDITOR
	void AAkAcousticPortal::StaticRegisterNativesAAkAcousticPortal()
	{
		UClass* Class = AAkAcousticPortal::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "DisablePortal", &AAkAcousticPortal::execDisablePortal },
			{ "EnablePortal", &AAkAcousticPortal::execEnablePortal },
#if WITH_EDITOR
			{ "GetCollisionChannel", &AAkAcousticPortal::execGetCollisionChannel },
#endif // WITH_EDITOR
			{ "GetCurrentState", &AAkAcousticPortal::execGetCurrentState },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_AAkAcousticPortal_DisablePortal_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AAkAcousticPortal_DisablePortal_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkAcousticPortal" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AAkAcousticPortal_DisablePortal_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AAkAcousticPortal, nullptr, "DisablePortal", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AAkAcousticPortal_DisablePortal_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_AAkAcousticPortal_DisablePortal_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AAkAcousticPortal_DisablePortal()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AAkAcousticPortal_DisablePortal_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_AAkAcousticPortal_EnablePortal_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AAkAcousticPortal_EnablePortal_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkAcousticPortal" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AAkAcousticPortal_EnablePortal_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AAkAcousticPortal, nullptr, "EnablePortal", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AAkAcousticPortal_EnablePortal_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_AAkAcousticPortal_EnablePortal_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AAkAcousticPortal_EnablePortal()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AAkAcousticPortal_EnablePortal_Statics::FuncParams);
		}
		return ReturnFunction;
	}
#if WITH_EDITOR
	struct Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics
	{
		struct AkAcousticPortal_eventGetCollisionChannel_Parms
		{
			TEnumAsByte<ECollisionChannel> ReturnValue;
		};
#if WITH_EDITORONLY_DATA
		static const UECodeGen_Private::FBytePropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#endif // WITH_EDITORONLY_DATA
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_EDITORONLY_DATA
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000800000580, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkAcousticPortal_eventGetCollisionChannel_Parms, ReturnValue), Z_Construct_UEnum_Engine_ECollisionChannel, METADATA_PARAMS(nullptr, 0) }; // 727872708
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics::NewProp_ReturnValue,
	};
#endif // WITH_EDITORONLY_DATA
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics::Function_MetaDataParams[] = {
		{ "Category", "Fit to Geometry" },
		{ "Comment", "/**\n\x09""Converts between EAkCollisionChannel and ECollisionChannel. Returns Wwise Integration Settings default if CollisionChannel == UseIntegrationSettingsDefault. Otherwise, casts CollisionChannel to ECollisionChannel.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Converts between EAkCollisionChannel and ECollisionChannel. Returns Wwise Integration Settings default if CollisionChannel == UseIntegrationSettingsDefault. Otherwise, casts CollisionChannel to ECollisionChannel." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AAkAcousticPortal, nullptr, "GetCollisionChannel", nullptr, nullptr, sizeof(Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics::AkAcousticPortal_eventGetCollisionChannel_Parms), IF_WITH_EDITORONLY_DATA(Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics::PropPointers, nullptr), IF_WITH_EDITORONLY_DATA(UE_ARRAY_COUNT(Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics::PropPointers), 0), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x24080401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel_Statics::FuncParams);
		}
		return ReturnFunction;
	}
#endif //WITH_EDITOR
	struct Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics
	{
		struct AkAcousticPortal_eventGetCurrentState_Parms
		{
			AkAcousticPortalState ReturnValue;
		};
		static const UECodeGen_Private::FBytePropertyParams NewProp_ReturnValue_Underlying;
		static const UECodeGen_Private::FEnumPropertyParams NewProp_ReturnValue;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::NewProp_ReturnValue_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 0, nullptr, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000000580, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkAcousticPortal_eventGetCurrentState_Parms, ReturnValue), Z_Construct_UEnum_AkAudio_AkAcousticPortalState, METADATA_PARAMS(nullptr, 0) }; // 4270424843
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::NewProp_ReturnValue_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkAcousticPortal" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_AAkAcousticPortal, nullptr, "GetCurrentState", nullptr, nullptr, sizeof(Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::AkAcousticPortal_eventGetCurrentState_Parms), Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(AAkAcousticPortal);
	UClass* Z_Construct_UClass_AAkAcousticPortal_NoRegister()
	{
		return AAkAcousticPortal::StaticClass();
	}
	struct Z_Construct_UClass_AAkAcousticPortal_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Portal_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_Portal;
#if WITH_EDITORONLY_DATA
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_FitToGeometry_MetaData[];
#endif
		static void NewProp_FitToGeometry_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_FitToGeometry;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_CollisionChannel_MetaData[];
#endif
		static const UECodeGen_Private::FBytePropertyParams NewProp_CollisionChannel;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_DetectionRadius_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_DetectionRadius;
#endif // WITH_EDITORONLY_DATA
		static const UECodeGen_Private::FBytePropertyParams NewProp_InitialState_Underlying;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_InitialState_MetaData[];
#endif
		static const UECodeGen_Private::FEnumPropertyParams NewProp_InitialState;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bRequiresStateMigration_MetaData[];
#endif
		static void NewProp_bRequiresStateMigration_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bRequiresStateMigration;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_EDITORONLY_DATA
#endif // WITH_EDITORONLY_DATA
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AAkAcousticPortal_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AVolume,
		(UObject* (*)())Z_Construct_UPackage__Script_AkAudio,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_AAkAcousticPortal_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_AAkAcousticPortal_DisablePortal, "DisablePortal" }, // 1404093760
		{ &Z_Construct_UFunction_AAkAcousticPortal_EnablePortal, "EnablePortal" }, // 137094065
#if WITH_EDITOR
		{ &Z_Construct_UFunction_AAkAcousticPortal_GetCollisionChannel, "GetCollisionChannel" }, // 553643720
#endif //WITH_EDITOR
		{ &Z_Construct_UFunction_AAkAcousticPortal_GetCurrentState, "GetCurrentState" }, // 570408944
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AAkAcousticPortal_Statics::Class_MetaDataParams[] = {
		{ "BlueprintType", "true" },
		{ "ClassGroupNames", "Audiokinetic" },
		{ "HideCategories", "Advanced Attachment Volume Brush Physics Object Blueprint Display Rendering Physics Input" },
		{ "IncludePath", "AkAcousticPortal.h" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_Portal_MetaData[] = {
		{ "Category", "AcousticPortal" },
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ShowOnlyInnerProperties", "" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_Portal = { "Portal", nullptr, (EPropertyFlags)0x00100000000a001d, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AAkAcousticPortal, Portal), Z_Construct_UClass_UAkPortalComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_Portal_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_Portal_MetaData)) };
#if WITH_EDITORONLY_DATA
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_FitToGeometry_MetaData[] = {
		{ "Category", "Fit to Geometry" },
		{ "Comment", "/**\n\x09""Automatically fit the Ak Acoustic Portal to surrounding geometry. The fitting operation is performed after enabling this property, or after moving the actor to a new location.\n\x09To find portals in surrounding geometry, rays emanating spherically outwards are cast from the origin of the actor in an attempt to detect sets of parallel surfaces.\n\x09The \"best\" detected parallel surfaces are indicated with yellow outline when dragging the actor to a new location.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Automatically fit the Ak Acoustic Portal to surrounding geometry. The fitting operation is performed after enabling this property, or after moving the actor to a new location.\nTo find portals in surrounding geometry, rays emanating spherically outwards are cast from the origin of the actor in an attempt to detect sets of parallel surfaces.\nThe \"best\" detected parallel surfaces are indicated with yellow outline when dragging the actor to a new location." },
	};
#endif
	void Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_FitToGeometry_SetBit(void* Obj)
	{
		((AAkAcousticPortal*)Obj)->FitToGeometry = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_FitToGeometry = { "FitToGeometry", nullptr, (EPropertyFlags)0x0020080800000001, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AAkAcousticPortal), &Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_FitToGeometry_SetBit, METADATA_PARAMS(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_FitToGeometry_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_FitToGeometry_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_CollisionChannel_MetaData[] = {
		{ "Category", "Fit to Geometry" },
		{ "Comment", "/**\n\x09Sets the collision channel for the ray traces performed to fit the portal to the surrounding geometry. When set to 'Use Integration Settings Default', the value will be taken from the DefaultFitToGeometryCollisionChannel in the Wwise Integration Settings.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Sets the collision channel for the ray traces performed to fit the portal to the surrounding geometry. When set to 'Use Integration Settings Default', the value will be taken from the DefaultFitToGeometryCollisionChannel in the Wwise Integration Settings." },
	};
#endif
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_CollisionChannel = { "CollisionChannel", nullptr, (EPropertyFlags)0x0020080800000001, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AAkAcousticPortal, CollisionChannel), Z_Construct_UEnum_AkAudio_EAkCollisionChannel, METADATA_PARAMS(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_CollisionChannel_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_CollisionChannel_MetaData)) }; // 1224743956
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_DetectionRadius_MetaData[] = {
		{ "Category", "Fit to Geometry" },
		{ "ClampMax", "100000.000000" },
		{ "ClampMin", "1.000000" },
		{ "Comment", "/**\n\x09Limits the effective portal opening size that can be detected when fitting the portal to surrounding geometry.\n\x09Increase this value to find larger openings; decrease it if large portals are erroneously detected, for example ones that span whole rooms.\n\x09The slider range can be expanded by entering a text value into this field.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "Limits the effective portal opening size that can be detected when fitting the portal to surrounding geometry.\nIncrease this value to find larger openings; decrease it if large portals are erroneously detected, for example ones that span whole rooms.\nThe slider range can be expanded by entering a text value into this field." },
		{ "UIMax", "5000.000000" },
		{ "UIMin", "100.000000" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_DetectionRadius = { "DetectionRadius", nullptr, (EPropertyFlags)0x0020080800000001, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AAkAcousticPortal, DetectionRadius), METADATA_PARAMS(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_DetectionRadius_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_DetectionRadius_MetaData)) };
#endif // WITH_EDITORONLY_DATA
	const UECodeGen_Private::FBytePropertyParams Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_InitialState_Underlying = { "UnderlyingType", nullptr, (EPropertyFlags)0x0000000000000000, UECodeGen_Private::EPropertyGenFlags::Byte, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, 0, nullptr, METADATA_PARAMS(nullptr, 0) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_InitialState_MetaData[] = {
		{ "Comment", "/** As of Wwise 2020.1, the InitialState is contained in the AkPortalComponent */" },
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
		{ "ToolTip", "As of Wwise 2020.1, the InitialState is contained in the AkPortalComponent" },
	};
#endif
	const UECodeGen_Private::FEnumPropertyParams Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_InitialState = { "InitialState", nullptr, (EPropertyFlags)0x0040000000000000, UECodeGen_Private::EPropertyGenFlags::Enum, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AAkAcousticPortal, InitialState), Z_Construct_UEnum_AkAudio_AkAcousticPortalState, METADATA_PARAMS(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_InitialState_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_InitialState_MetaData)) }; // 4270424843
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_bRequiresStateMigration_MetaData[] = {
		{ "ModuleRelativePath", "Classes/AkAcousticPortal.h" },
	};
#endif
	void Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_bRequiresStateMigration_SetBit(void* Obj)
	{
		((AAkAcousticPortal*)Obj)->bRequiresStateMigration = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_bRequiresStateMigration = { "bRequiresStateMigration", nullptr, (EPropertyFlags)0x0040000000002000, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AAkAcousticPortal), &Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_bRequiresStateMigration_SetBit, METADATA_PARAMS(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_bRequiresStateMigration_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_bRequiresStateMigration_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_AAkAcousticPortal_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_Portal,
#if WITH_EDITORONLY_DATA
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_FitToGeometry,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_CollisionChannel,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_DetectionRadius,
#endif // WITH_EDITORONLY_DATA
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_InitialState_Underlying,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_InitialState,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_AAkAcousticPortal_Statics::NewProp_bRequiresStateMigration,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_AAkAcousticPortal_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AAkAcousticPortal>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_AAkAcousticPortal_Statics::ClassParams = {
		&AAkAcousticPortal::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_AAkAcousticPortal_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		UE_ARRAY_COUNT(Z_Construct_UClass_AAkAcousticPortal_Statics::PropPointers),
		0,
		0x009000A4u,
		METADATA_PARAMS(Z_Construct_UClass_AAkAcousticPortal_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AAkAcousticPortal_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AAkAcousticPortal()
	{
		if (!Z_Registration_Info_UClass_AAkAcousticPortal.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_AAkAcousticPortal.OuterSingleton, Z_Construct_UClass_AAkAcousticPortal_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_AAkAcousticPortal.OuterSingleton;
	}
	template<> AKAUDIO_API UClass* StaticClass<AAkAcousticPortal>()
	{
		return AAkAcousticPortal::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(AAkAcousticPortal);
	AAkAcousticPortal::~AAkAcousticPortal() {}
	IMPLEMENT_FSTRUCTUREDARCHIVE_SERIALIZER(AAkAcousticPortal)
	struct Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkAcousticPortal_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkAcousticPortal_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UAkPortalComponent, UAkPortalComponent::StaticClass, TEXT("UAkPortalComponent"), &Z_Registration_Info_UClass_UAkPortalComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAkPortalComponent), 3898791254U) },
		{ Z_Construct_UClass_AAkAcousticPortal, AAkAcousticPortal::StaticClass, TEXT("AAkAcousticPortal"), &Z_Registration_Info_UClass_AAkAcousticPortal, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(AAkAcousticPortal), 697910680U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkAcousticPortal_h_28109930(TEXT("/Script/AkAudio"),
		Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkAcousticPortal_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_AOW_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkAcousticPortal_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
