// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "AkAudio/Classes/AkRoomComponent.h"
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeAkRoomComponent() {}
// Cross Module References
	AKAUDIO_API UClass* Z_Construct_UClass_UAkAcousticTextureSetComponent_NoRegister();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkGameObject();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkRoomComponent();
	AKAUDIO_API UClass* Z_Construct_UClass_UAkRoomComponent_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_AActor_NoRegister();
	ENGINE_API UClass* Z_Construct_UClass_UPrimitiveComponent_NoRegister();
	UPackage* Z_Construct_UPackage__Script_AkAudio();
// End Cross Module References
	DEFINE_FUNCTION(UAkRoomComponent::execSetGeometryComponent)
	{
		P_GET_OBJECT(UAkAcousticTextureSetComponent,Z_Param_textureSetComponent);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->SetGeometryComponent(Z_Param_textureSetComponent);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkRoomComponent::execRemoveReverbZone)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->RemoveReverbZone();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkRoomComponent::execSetReverbZone)
	{
		P_GET_OBJECT(UAkRoomComponent,Z_Param_InParentRoom);
		P_GET_PROPERTY(FFloatProperty,Z_Param_InTransitionRegionWidth);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->SetReverbZone(Z_Param_InParentRoom,Z_Param_InTransitionRegionWidth);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkRoomComponent::execGetPrimitiveParent)
	{
		P_FINISH;
		P_NATIVE_BEGIN;
		*(UPrimitiveComponent**)Z_Param__Result=P_THIS->GetPrimitiveParent();
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkRoomComponent::execUpdateTransitionRegionWidth)
	{
		P_GET_PROPERTY(FFloatProperty,Z_Param_InTransitionRegionWidth);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->UpdateTransitionRegionWidth(Z_Param_InTransitionRegionWidth);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkRoomComponent::execUpdateParentRoomActor)
	{
		P_GET_OBJECT(AActor,Z_Param_InParentRoomActor);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->UpdateParentRoomActor(Z_Param_InParentRoomActor);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkRoomComponent::execSetEnableReverbZone)
	{
		P_GET_UBOOL(Z_Param_bInEnableReverbZone);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->SetEnableReverbZone(Z_Param_bInEnableReverbZone);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkRoomComponent::execSetAuxSendLevel)
	{
		P_GET_PROPERTY(FFloatProperty,Z_Param_InAuxSendLevel);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->SetAuxSendLevel(Z_Param_InAuxSendLevel);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkRoomComponent::execSetTransmissionLoss)
	{
		P_GET_PROPERTY(FFloatProperty,Z_Param_InTransmissionLoss);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->SetTransmissionLoss(Z_Param_InTransmissionLoss);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkRoomComponent::execSetDynamic)
	{
		P_GET_UBOOL(Z_Param_bInDynamic);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->SetDynamic(Z_Param_bInDynamic);
		P_NATIVE_END;
	}
	DEFINE_FUNCTION(UAkRoomComponent::execSetEnable)
	{
		P_GET_UBOOL(Z_Param_bInEnable);
		P_FINISH;
		P_NATIVE_BEGIN;
		P_THIS->SetEnable(Z_Param_bInEnable);
		P_NATIVE_END;
	}
	void UAkRoomComponent::StaticRegisterNativesUAkRoomComponent()
	{
		UClass* Class = UAkRoomComponent::StaticClass();
		static const FNameNativePtrPair Funcs[] = {
			{ "GetPrimitiveParent", &UAkRoomComponent::execGetPrimitiveParent },
			{ "RemoveReverbZone", &UAkRoomComponent::execRemoveReverbZone },
			{ "SetAuxSendLevel", &UAkRoomComponent::execSetAuxSendLevel },
			{ "SetDynamic", &UAkRoomComponent::execSetDynamic },
			{ "SetEnable", &UAkRoomComponent::execSetEnable },
			{ "SetEnableReverbZone", &UAkRoomComponent::execSetEnableReverbZone },
			{ "SetGeometryComponent", &UAkRoomComponent::execSetGeometryComponent },
			{ "SetReverbZone", &UAkRoomComponent::execSetReverbZone },
			{ "SetTransmissionLoss", &UAkRoomComponent::execSetTransmissionLoss },
			{ "UpdateParentRoomActor", &UAkRoomComponent::execUpdateParentRoomActor },
			{ "UpdateTransitionRegionWidth", &UAkRoomComponent::execUpdateTransitionRegionWidth },
		};
		FNativeFunctionRegistrar::RegisterFunctions(Class, Funcs, UE_ARRAY_COUNT(Funcs));
	}
	struct Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics
	{
		struct AkRoomComponent_eventGetPrimitiveParent_Parms
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
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::NewProp_ReturnValue_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::NewProp_ReturnValue = { "ReturnValue", nullptr, (EPropertyFlags)0x0010000000080588, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkRoomComponent_eventGetPrimitiveParent_Parms, ReturnValue), Z_Construct_UClass_UPrimitiveComponent_NoRegister, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::NewProp_ReturnValue_MetaData, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::NewProp_ReturnValue_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::NewProp_ReturnValue,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkRoomComponent" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkRoomComponent, nullptr, "GetPrimitiveParent", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::AkRoomComponent_eventGetPrimitiveParent_Parms), Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x54020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkRoomComponent_RemoveReverbZone_Statics
	{
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_RemoveReverbZone_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkRoomComponent" },
		{ "Comment", "/**\n\x09* Removes this Reverb Zone from its parent.\n\x09* Sound can no longer propagate between the two Rooms, unless they are explicitly connected with a Portal.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Removes this Reverb Zone from its parent.\nSound can no longer propagate between the two Rooms, unless they are explicitly connected with a Portal." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkRoomComponent_RemoveReverbZone_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkRoomComponent, nullptr, "RemoveReverbZone", nullptr, nullptr, 0, nullptr, 0, RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_RemoveReverbZone_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_RemoveReverbZone_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkRoomComponent_RemoveReverbZone()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkRoomComponent_RemoveReverbZone_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics
	{
		struct AkRoomComponent_eventSetAuxSendLevel_Parms
		{
			float InAuxSendLevel;
		};
		static const UECodeGen_Private::FFloatPropertyParams NewProp_InAuxSendLevel;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics::NewProp_InAuxSendLevel = { "InAuxSendLevel", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkRoomComponent_eventSetAuxSendLevel_Parms, InAuxSendLevel), METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics::NewProp_InAuxSendLevel,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics::Function_MetaDataParams[] = {
		{ "BlueprintSetter", "" },
		{ "Category", "Audiokinetic|AkRoomComponent" },
		{ "Comment", "/**\n\x09* Sets the Send level. A value of 0 disables the aux send.\n\x09* @param InAuxSendLevel - The new value for the Send level. Valid range 0.0f-1.0f.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Sets the Send level. A value of 0 disables the aux send.\n@param InAuxSendLevel - The new value for the Send level. Valid range 0.0f-1.0f." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkRoomComponent, nullptr, "SetAuxSendLevel", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics::AkRoomComponent_eventSetAuxSendLevel_Parms), Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics
	{
		struct AkRoomComponent_eventSetDynamic_Parms
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
	void Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::NewProp_bInDynamic_SetBit(void* Obj)
	{
		((AkRoomComponent_eventSetDynamic_Parms*)Obj)->bInDynamic = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::NewProp_bInDynamic = { "bInDynamic", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkRoomComponent_eventSetDynamic_Parms), &Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::NewProp_bInDynamic_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::NewProp_bInDynamic,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::Function_MetaDataParams[] = {
		{ "BlueprintSetter", "" },
		{ "Category", "Audiokinetic|AkRoomComponent" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkRoomComponent, nullptr, "SetDynamic", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::AkRoomComponent_eventSetDynamic_Parms), Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkRoomComponent_SetDynamic()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkRoomComponent_SetDynamic_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics
	{
		struct AkRoomComponent_eventSetEnable_Parms
		{
			bool bInEnable;
		};
		static void NewProp_bInEnable_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bInEnable;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::NewProp_bInEnable_SetBit(void* Obj)
	{
		((AkRoomComponent_eventSetEnable_Parms*)Obj)->bInEnable = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::NewProp_bInEnable = { "bInEnable", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkRoomComponent_eventSetEnable_Parms), &Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::NewProp_bInEnable_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::NewProp_bInEnable,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::Function_MetaDataParams[] = {
		{ "BlueprintSetter", "" },
		{ "Category", "Audiokinetic|AkRoomComponent" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkRoomComponent, nullptr, "SetEnable", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::AkRoomComponent_eventSetEnable_Parms), Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkRoomComponent_SetEnable()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkRoomComponent_SetEnable_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics
	{
		struct AkRoomComponent_eventSetEnableReverbZone_Parms
		{
			bool bInEnableReverbZone;
		};
		static void NewProp_bInEnableReverbZone_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bInEnableReverbZone;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	void Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::NewProp_bInEnableReverbZone_SetBit(void* Obj)
	{
		((AkRoomComponent_eventSetEnableReverbZone_Parms*)Obj)->bInEnableReverbZone = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::NewProp_bInEnableReverbZone = { "bInEnableReverbZone", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(AkRoomComponent_eventSetEnableReverbZone_Parms), &Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::NewProp_bInEnableReverbZone_SetBit, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::NewProp_bInEnableReverbZone,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::Function_MetaDataParams[] = {
		{ "BlueprintSetter", "" },
		{ "Category", "Audiokinetic|AkRoomComponent" },
		{ "Comment", "/** Set bEnableReverbZone to a new value and set or remove the Reverb Zone in Wwise. */" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Set bEnableReverbZone to a new value and set or remove the Reverb Zone in Wwise." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkRoomComponent, nullptr, "SetEnableReverbZone", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::AkRoomComponent_eventSetEnableReverbZone_Parms), Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics
	{
		struct AkRoomComponent_eventSetGeometryComponent_Parms
		{
			UAkAcousticTextureSetComponent* textureSetComponent;
		};
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_textureSetComponent_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_textureSetComponent;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::NewProp_textureSetComponent_MetaData[] = {
		{ "EditInline", "true" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::NewProp_textureSetComponent = { "textureSetComponent", nullptr, (EPropertyFlags)0x0010000000080080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkRoomComponent_eventSetGeometryComponent_Parms, textureSetComponent), Z_Construct_UClass_UAkAcousticTextureSetComponent_NoRegister, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::NewProp_textureSetComponent_MetaData, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::NewProp_textureSetComponent_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::NewProp_textureSetComponent,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkRoomComponent" },
		{ "Comment", "/** Set the geometry component used to send the geometry of the Room to Wwise. For example, in a Blueprint that has a static mesh component with an AkGeometry child component, this function can be called in BeginPlay to associate that AkGeometry component with this Room component.\n\x09 *  If this Room component has a sibling AkGeometry component (or AkSurfaceReflectorSet component), they are automatically associated and there is no need to call this function.\n\x09 */" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Set the geometry component used to send the geometry of the Room to Wwise. For example, in a Blueprint that has a static mesh component with an AkGeometry child component, this function can be called in BeginPlay to associate that AkGeometry component with this Room component.\nIf this Room component has a sibling AkGeometry component (or AkSurfaceReflectorSet component), they are automatically associated and there is no need to call this function." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkRoomComponent, nullptr, "SetGeometryComponent", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::AkRoomComponent_eventSetGeometryComponent_Parms), Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics
	{
		struct AkRoomComponent_eventSetReverbZone_Parms
		{
			const UAkRoomComponent* InParentRoom;
			float InTransitionRegionWidth;
		};
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_InParentRoom_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_InParentRoom;
		static const UECodeGen_Private::FFloatPropertyParams NewProp_InTransitionRegionWidth;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::NewProp_InParentRoom_MetaData[] = {
		{ "EditInline", "true" },
		{ "NativeConst", "" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::NewProp_InParentRoom = { "InParentRoom", nullptr, (EPropertyFlags)0x0010000000080082, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkRoomComponent_eventSetReverbZone_Parms, InParentRoom), Z_Construct_UClass_UAkRoomComponent_NoRegister, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::NewProp_InParentRoom_MetaData, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::NewProp_InParentRoom_MetaData)) };
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::NewProp_InTransitionRegionWidth = { "InTransitionRegionWidth", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkRoomComponent_eventSetReverbZone_Parms, InTransitionRegionWidth), METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::NewProp_InParentRoom,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::NewProp_InTransitionRegionWidth,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::Function_MetaDataParams[] = {
		{ "Category", "Audiokinetic|AkRoomComponent" },
		{ "Comment", "/**\n\x09* Establishes a parent-child relationship between this Room and a parent Room and allows for sound propagation between them as if they were the same Room, without the need for a connecting Portal.\n\x09* Setting a Room as a Reverb Zone is useful in situations where two or more acoustic environments are not easily modeled as closed Rooms connected by Portals.\n\x09* Possible uses for Reverb Zones include: a covered area with no walls, a forested area within an outdoor space, or any situation where multiple reverb effects are desired within a common space.\n\x09* Reverb Zones have many advantages compared to standard Game-Defined Auxiliary Sends (that is compared to the AkLateReverbComponent or the AkReverbVolume). They are part of the wet path, and form reverb chains with other Rooms;\n\x09* they are spatialized according to their 3D extent; they are also subject to other acoustic phenomena simulated in Wwise Spatial Audio, such as diffraction and transmission.\n\x09* A Reverb Zone needs to be a Room component with an associated geometry.\n\x09*\n\x09* @param InParentRoom - The parent Room component. A parent Room can have multiple Reverb Zones, but a Reverb Zone can only have a single Parent. If a Room is already assigned to a parent Room, it is first removed from the old parent (exactly as if RemoveReverbZone were called), and is then assigned to the new parent Room. A Reverb Zone can be the parent of another Reverb Zone. A Room cannot be its own parent. Defaults to the 'Outdoors' Room if left empty.\n\x09* @param InTransitionRegionWidth - Width of the transition region between the Reverb Zone and its parent. The transition region acts the same way as Portal depth, but is centered around the Reverb Zone geometry. Transition regions are only added on surfaces where transmission loss is set to 0. The value must be positive. Negative values are treated as 0.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Establishes a parent-child relationship between this Room and a parent Room and allows for sound propagation between them as if they were the same Room, without the need for a connecting Portal.\nSetting a Room as a Reverb Zone is useful in situations where two or more acoustic environments are not easily modeled as closed Rooms connected by Portals.\nPossible uses for Reverb Zones include: a covered area with no walls, a forested area within an outdoor space, or any situation where multiple reverb effects are desired within a common space.\nReverb Zones have many advantages compared to standard Game-Defined Auxiliary Sends (that is compared to the AkLateReverbComponent or the AkReverbVolume). They are part of the wet path, and form reverb chains with other Rooms;\nthey are spatialized according to their 3D extent; they are also subject to other acoustic phenomena simulated in Wwise Spatial Audio, such as diffraction and transmission.\nA Reverb Zone needs to be a Room component with an associated geometry.\n\n@param InParentRoom - The parent Room component. A parent Room can have multiple Reverb Zones, but a Reverb Zone can only have a single Parent. If a Room is already assigned to a parent Room, it is first removed from the old parent (exactly as if RemoveReverbZone were called), and is then assigned to the new parent Room. A Reverb Zone can be the parent of another Reverb Zone. A Room cannot be its own parent. Defaults to the 'Outdoors' Room if left empty.\n@param InTransitionRegionWidth - Width of the transition region between the Reverb Zone and its parent. The transition region acts the same way as Portal depth, but is centered around the Reverb Zone geometry. Transition regions are only added on surfaces where transmission loss is set to 0. The value must be positive. Negative values are treated as 0." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkRoomComponent, nullptr, "SetReverbZone", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::AkRoomComponent_eventSetReverbZone_Parms), Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkRoomComponent_SetReverbZone()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkRoomComponent_SetReverbZone_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics
	{
		struct AkRoomComponent_eventSetTransmissionLoss_Parms
		{
			float InTransmissionLoss;
		};
		static const UECodeGen_Private::FFloatPropertyParams NewProp_InTransmissionLoss;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics::NewProp_InTransmissionLoss = { "InTransmissionLoss", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkRoomComponent_eventSetTransmissionLoss_Parms, InTransmissionLoss), METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics::NewProp_InTransmissionLoss,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics::Function_MetaDataParams[] = {
		{ "BlueprintSetter", "" },
		{ "Category", "Audiokinetic|AkRoomComponent" },
		{ "Comment", "/**\n\x09* Sets the transmission loss value.\n\x09* @param InTransmissionLoss - The new value for the transmission loss. Valid range 0.0f-1.0f.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Sets the transmission loss value.\n@param InTransmissionLoss - The new value for the transmission loss. Valid range 0.0f-1.0f." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkRoomComponent, nullptr, "SetTransmissionLoss", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics::AkRoomComponent_eventSetTransmissionLoss_Parms), Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics
	{
		struct AkRoomComponent_eventUpdateParentRoomActor_Parms
		{
			AActor* InParentRoomActor;
		};
		static const UECodeGen_Private::FObjectPropertyParams NewProp_InParentRoomActor;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics::NewProp_InParentRoomActor = { "InParentRoomActor", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkRoomComponent_eventUpdateParentRoomActor_Parms, InParentRoomActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics::NewProp_InParentRoomActor,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics::Function_MetaDataParams[] = {
		{ "BlueprintSetter", "" },
		{ "Category", "Audiokinetic|AkRoomComponent" },
		{ "Comment", "/** Set ParentRoomActor with a new actor and update the Reverb Zone in Wwise asynchronously. */" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Set ParentRoomActor with a new actor and update the Reverb Zone in Wwise asynchronously." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkRoomComponent, nullptr, "UpdateParentRoomActor", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics::AkRoomComponent_eventUpdateParentRoomActor_Parms), Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	struct Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics
	{
		struct AkRoomComponent_eventUpdateTransitionRegionWidth_Parms
		{
			float InTransitionRegionWidth;
		};
		static const UECodeGen_Private::FFloatPropertyParams NewProp_InTransitionRegionWidth;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Function_MetaDataParams[];
#endif
		static const UECodeGen_Private::FFunctionParams FuncParams;
	};
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics::NewProp_InTransitionRegionWidth = { "InTransitionRegionWidth", nullptr, (EPropertyFlags)0x0010000000000080, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(AkRoomComponent_eventUpdateTransitionRegionWidth_Parms, InTransitionRegionWidth), METADATA_PARAMS(nullptr, 0) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics::NewProp_InTransitionRegionWidth,
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics::Function_MetaDataParams[] = {
		{ "BlueprintSetter", "" },
		{ "Category", "Audiokinetic|AkRoomComponent" },
		{ "Comment", "/** Set TransitionRegionWidth to a new value and updates the Reverb Zone in Wwise asynchronously. */" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Set TransitionRegionWidth to a new value and updates the Reverb Zone in Wwise asynchronously." },
	};
#endif
	const UECodeGen_Private::FFunctionParams Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics::FuncParams = { (UObject*(*)())Z_Construct_UClass_UAkRoomComponent, nullptr, "UpdateTransitionRegionWidth", nullptr, nullptr, sizeof(Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics::AkRoomComponent_eventUpdateTransitionRegionWidth_Parms), Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics::PropPointers, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics::PropPointers), RF_Public|RF_Transient|RF_MarkAsNative, (EFunctionFlags)0x04020401, 0, 0, METADATA_PARAMS(Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics::Function_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics::Function_MetaDataParams)) };
	UFunction* Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth()
	{
		static UFunction* ReturnFunction = nullptr;
		if (!ReturnFunction)
		{
			UECodeGen_Private::ConstructUFunction(&ReturnFunction, Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth_Statics::FuncParams);
		}
		return ReturnFunction;
	}
	IMPLEMENT_CLASS_NO_AUTO_REGISTRATION(UAkRoomComponent);
	UClass* Z_Construct_UClass_UAkRoomComponent_NoRegister()
	{
		return UAkRoomComponent::StaticClass();
	}
	struct Z_Construct_UClass_UAkRoomComponent_Statics
	{
		static UObject* (*const DependentSingletons[])();
		static const FClassFunctionLinkInfo FuncInfo[];
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnable_MetaData[];
#endif
		static void NewProp_bEnable_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnable;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bDynamic_MetaData[];
#endif
		static void NewProp_bDynamic_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bDynamic;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_Priority_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_Priority;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_WallOcclusion_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_WallOcclusion;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_AuxSendLevel_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_AuxSendLevel;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_AutoPost_MetaData[];
#endif
		static void NewProp_AutoPost_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_AutoPost;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_bEnableReverbZone_MetaData[];
#endif
		static void NewProp_bEnableReverbZone_SetBit(void* Obj);
		static const UECodeGen_Private::FBoolPropertyParams NewProp_bEnableReverbZone;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ParentRoomActor_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_ParentRoomActor;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ParentRoomName_MetaData[];
#endif
		static const UECodeGen_Private::FStrPropertyParams NewProp_ParentRoomName;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_TransitionRegionWidth_MetaData[];
#endif
		static const UECodeGen_Private::FFloatPropertyParams NewProp_TransitionRegionWidth;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_GeometryComponent_MetaData[];
#endif
		static const UECodeGen_Private::FObjectPropertyParams NewProp_GeometryComponent;
#if WITH_METADATA
		static const UECodeGen_Private::FMetaDataPairParam NewProp_ParentRoom_MetaData[];
#endif
		static const UECodeGen_Private::FWeakObjectPropertyParams NewProp_ParentRoom;
		static const UECodeGen_Private::FPropertyParamsBase* const PropPointers[];
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UECodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_UAkRoomComponent_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_UAkGameObject,
		(UObject* (*)())Z_Construct_UPackage__Script_AkAudio,
	};
	const FClassFunctionLinkInfo Z_Construct_UClass_UAkRoomComponent_Statics::FuncInfo[] = {
		{ &Z_Construct_UFunction_UAkRoomComponent_GetPrimitiveParent, "GetPrimitiveParent" }, // 3697461308
		{ &Z_Construct_UFunction_UAkRoomComponent_RemoveReverbZone, "RemoveReverbZone" }, // 1351085955
		{ &Z_Construct_UFunction_UAkRoomComponent_SetAuxSendLevel, "SetAuxSendLevel" }, // 1997413646
		{ &Z_Construct_UFunction_UAkRoomComponent_SetDynamic, "SetDynamic" }, // 1210241477
		{ &Z_Construct_UFunction_UAkRoomComponent_SetEnable, "SetEnable" }, // 1624358397
		{ &Z_Construct_UFunction_UAkRoomComponent_SetEnableReverbZone, "SetEnableReverbZone" }, // 1480585498
		{ &Z_Construct_UFunction_UAkRoomComponent_SetGeometryComponent, "SetGeometryComponent" }, // 1300438429
		{ &Z_Construct_UFunction_UAkRoomComponent_SetReverbZone, "SetReverbZone" }, // 2188449547
		{ &Z_Construct_UFunction_UAkRoomComponent_SetTransmissionLoss, "SetTransmissionLoss" }, // 3836389736
		{ &Z_Construct_UFunction_UAkRoomComponent_UpdateParentRoomActor, "UpdateParentRoomActor" }, // 2628144728
		{ &Z_Construct_UFunction_UAkRoomComponent_UpdateTransitionRegionWidth, "UpdateTransitionRegionWidth" }, // 405356262
	};
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::Class_MetaDataParams[] = {
		{ "AutoExpandCategories", "AkComponent" },
		{ "BlueprintSpawnableComponent", "" },
		{ "BlueprintType", "true" },
		{ "ClassGroupNames", "Audiokinetic" },
		{ "HideCategories", "Transform Rendering Mobility LOD Component Activation Tags Transform Rendering Mobility LOD Component Activation Trigger PhysicsVolume" },
		{ "IncludePath", "AkRoomComponent.h" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ObjectInitializerConstructorDeclared", "" },
	};
#endif
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnable_MetaData[] = {
		{ "BlueprintSetter", "SetEnable" },
		{ "Category", "EnableComponent" },
		{ "Comment", "/** \n\x09* Enable the Room Component to set this volume as a Spatial Audio Room. Additional properties are available in the Room and AkEvent categories.\n\x09*/" },
		{ "DisplayName", "Enable Room" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Enable the Room Component to set this volume as a Spatial Audio Room. Additional properties are available in the Room and AkEvent categories." },
	};
#endif
	void Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnable_SetBit(void* Obj)
	{
		((UAkRoomComponent*)Obj)->bEnable = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnable = { "bEnable", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UAkRoomComponent), &Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnable_SetBit, METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnable_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnable_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bDynamic_MetaData[] = {
		{ "BlueprintSetter", "SetDynamic" },
		{ "Category", "Room" },
		{ "Comment", "/** \n\x09* If true, the Portal connections for this Room can change during runtime when this Room moves.\n\x09* For worlds containing many Portals, this can be expensive. Note that this Room's Portal connections \n\x09* may still change, even when Room Is Dynamic = false, when dynamic Portals are moved (that is when Portals\n\x09* move who have bDynamic = true).\n\x09*/" },
		{ "DisplayName", "Room Is Dynamic" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "If true, the Portal connections for this Room can change during runtime when this Room moves.\nFor worlds containing many Portals, this can be expensive. Note that this Room's Portal connections\nmay still change, even when Room Is Dynamic = false, when dynamic Portals are moved (that is when Portals\nmove who have bDynamic = true)." },
	};
#endif
	void Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bDynamic_SetBit(void* Obj)
	{
		((UAkRoomComponent*)Obj)->bDynamic = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bDynamic = { "bDynamic", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UAkRoomComponent), &Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bDynamic_SetBit, METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bDynamic_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bDynamic_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_Priority_MetaData[] = {
		{ "Category", "Room" },
		{ "Comment", "/**\n\x09* The precedence in which the Rooms is applied. In the case of overlapping Rooms, only the one\n\x09* with the highest priority is chosen. If two or more overlapping Rooms have the same\n\x09* priority, the chosen Room is unpredictable.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "The precedence in which the Rooms is applied. In the case of overlapping Rooms, only the one\nwith the highest priority is chosen. If two or more overlapping Rooms have the same\npriority, the chosen Room is unpredictable." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_Priority = { "Priority", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkRoomComponent, Priority), METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_Priority_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_Priority_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_WallOcclusion_MetaData[] = {
		{ "BlueprintSetter", "SetTransmissionLoss" },
		{ "Category", "Room" },
		{ "ClampMax", "1.000000" },
		{ "ClampMin", "0.000000" },
		{ "Comment", "/**\n\x09* Used to set the transmission loss value in wwise, on emitters in the Room, when no audio paths to the \n\x09* listener are found via sound propagation in Wwise Spatial Audio. This value can be thought of as \n\x09* 'thickness', as it relates to how much sound energy is transmitted through the wall. Valid range 0.0f-1.0f.\n\x09*/" },
		{ "DisplayName", "Transmission Loss" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Used to set the transmission loss value in wwise, on emitters in the Room, when no audio paths to the\nlistener are found via sound propagation in Wwise Spatial Audio. This value can be thought of as\n'thickness', as it relates to how much sound energy is transmitted through the wall. Valid range 0.0f-1.0f." },
		{ "UIMax", "1.000000" },
		{ "UIMin", "0.000000" },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_WallOcclusion = { "WallOcclusion", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkRoomComponent, WallOcclusion), METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_WallOcclusion_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_WallOcclusion_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AuxSendLevel_MetaData[] = {
		{ "BlueprintSetter", "SetAuxSendLevel" },
		{ "Category", "AkEvent" },
		{ "ClampMax", "1.0" },
		{ "ClampMin", "0.0" },
		{ "Comment", "/**\n\x09* Send level for sounds that are posted on the room. Valid range: (0.f-1.f). A value of 0 disables the aux send.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Send level for sounds that are posted on the room. Valid range: (0.f-1.f). A value of 0 disables the aux send." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AuxSendLevel = { "AuxSendLevel", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkRoomComponent, AuxSendLevel), METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AuxSendLevel_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AuxSendLevel_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AutoPost_MetaData[] = {
		{ "Category", "AkEvent" },
		{ "Comment", "/** Automatically post the associated AkAudioEvent on BeginPlay */" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Automatically post the associated AkAudioEvent on BeginPlay" },
	};
#endif
	void Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AutoPost_SetBit(void* Obj)
	{
		((UAkRoomComponent*)Obj)->AutoPost = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AutoPost = { "AutoPost", nullptr, (EPropertyFlags)0x0010020000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UAkRoomComponent), &Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AutoPost_SetBit, METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AutoPost_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AutoPost_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnableReverbZone_MetaData[] = {
		{ "BlueprintSetter", "SetEnableReverbZone" },
		{ "Category", "ReverbZone" },
		{ "Comment", "/**\n\x09* Sets this Room as a Reverb Zone.\n\x09* A Reverb Zone models a region that has a distinct reverb effect or ambience but does not require Portals to connect to neighboring Rooms.\n\x09* Use Reverb Zones instead of standard Rooms whenever there are no obvious walls,\n\x09* or generally when there is more negative space than positive space at the interface between the two regions.\n\x09*/" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Sets this Room as a Reverb Zone.\nA Reverb Zone models a region that has a distinct reverb effect or ambience but does not require Portals to connect to neighboring Rooms.\nUse Reverb Zones instead of standard Rooms whenever there are no obvious walls,\nor generally when there is more negative space than positive space at the interface between the two regions." },
	};
#endif
	void Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnableReverbZone_SetBit(void* Obj)
	{
		((UAkRoomComponent*)Obj)->bEnableReverbZone = 1;
	}
	const UECodeGen_Private::FBoolPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnableReverbZone = { "bEnableReverbZone", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Bool | UECodeGen_Private::EPropertyGenFlags::NativeBool, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, sizeof(bool), sizeof(UAkRoomComponent), &Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnableReverbZone_SetBit, METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnableReverbZone_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnableReverbZone_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoomActor_MetaData[] = {
		{ "BlueprintSetter", "UpdateParentRoomActor" },
		{ "Category", "ReverbZone" },
		{ "Comment", "/**\n\x09* Establishes a parent-child relationship between two Rooms and allows for sound propagation between them\n\x09* as if they were the same Room, without the need for a connecting Portal.\n\x09* A parent Room can have multiple Reverb Zones, but a Reverb Zone can only have a single Parent.\n\x09* The Reverb Zone and its parent are both Rooms, and as such, must be specified using Enable Room.\n\x09* The automatically created 'outdoors' Room is commonly used as a parent Room for Reverb Zones, because they often model open spaces.\n\x09* When set to None (the default value), the Reverb Zone is automatically attached to the 'outdoors' Room.\n\x09*/" },
		{ "EditCondition", "bEnableReverbZone" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Establishes a parent-child relationship between two Rooms and allows for sound propagation between them\nas if they were the same Room, without the need for a connecting Portal.\nA parent Room can have multiple Reverb Zones, but a Reverb Zone can only have a single Parent.\nThe Reverb Zone and its parent are both Rooms, and as such, must be specified using Enable Room.\nThe automatically created 'outdoors' Room is commonly used as a parent Room for Reverb Zones, because they often model open spaces.\nWhen set to None (the default value), the Reverb Zone is automatically attached to the 'outdoors' Room." },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoomActor = { "ParentRoomActor", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkRoomComponent, ParentRoomActor), Z_Construct_UClass_AActor_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoomActor_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoomActor_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoomName_MetaData[] = {
		{ "Category", "ReverbZone" },
		{ "Comment", "/**\n\x09* The name of the Parent Room of this Reverb Zone.\n\x09* If the Parent Room Actor is None, or if the Parent Room Actor is not valid, the 'outdoors' Room is chosen and printed here.\n\x09*/" },
		{ "EditCondition", "bEnableReverbZone" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "The name of the Parent Room of this Reverb Zone.\nIf the Parent Room Actor is None, or if the Parent Room Actor is not valid, the 'outdoors' Room is chosen and printed here." },
	};
#endif
	const UECodeGen_Private::FStrPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoomName = { "ParentRoomName", nullptr, (EPropertyFlags)0x0010000000020015, UECodeGen_Private::EPropertyGenFlags::Str, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkRoomComponent, ParentRoomName), METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoomName_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoomName_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_TransitionRegionWidth_MetaData[] = {
		{ "BlueprintSetter", "UpdateTransitionRegionWidth" },
		{ "Category", "ReverbZone" },
		{ "Comment", "/**\n\x09* Width of the transition region between the Reverb Zone and its parent.\n\x09* The transition region acts the same way as Portal depth, but is centered around the Reverb Zone geometry. Transition regions are only added on surfaces where transmission loss is set to 0.\n\x09* The value must be positive. Negative values are treated as 0.\n\x09*/" },
		{ "EditCondition", "bEnableReverbZone" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "ToolTip", "Width of the transition region between the Reverb Zone and its parent.\nThe transition region acts the same way as Portal depth, but is centered around the Reverb Zone geometry. Transition regions are only added on surfaces where transmission loss is set to 0.\nThe value must be positive. Negative values are treated as 0." },
	};
#endif
	const UECodeGen_Private::FFloatPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_TransitionRegionWidth = { "TransitionRegionWidth", nullptr, (EPropertyFlags)0x0010000000000005, UECodeGen_Private::EPropertyGenFlags::Float, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkRoomComponent, TransitionRegionWidth), METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_TransitionRegionWidth_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_TransitionRegionWidth_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_GeometryComponent_MetaData[] = {
		{ "EditInline", "true" },
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
	};
#endif
	const UECodeGen_Private::FObjectPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_GeometryComponent = { "GeometryComponent", nullptr, (EPropertyFlags)0x0040000000082008, UECodeGen_Private::EPropertyGenFlags::Object, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkRoomComponent, GeometryComponent), Z_Construct_UClass_UAkAcousticTextureSetComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_GeometryComponent_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_GeometryComponent_MetaData)) };
#if WITH_METADATA
	const UECodeGen_Private::FMetaDataPairParam Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoom_MetaData[] = {
		{ "ModuleRelativePath", "Classes/AkRoomComponent.h" },
		{ "NativeConstTemplateArg", "" },
	};
#endif
	const UECodeGen_Private::FWeakObjectPropertyParams Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoom = { "ParentRoom", nullptr, (EPropertyFlags)0x0044000000080008, UECodeGen_Private::EPropertyGenFlags::WeakObject, RF_Public|RF_Transient|RF_MarkAsNative, 1, nullptr, nullptr, STRUCT_OFFSET(UAkRoomComponent, ParentRoom), Z_Construct_UClass_UAkRoomComponent_NoRegister, METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoom_MetaData, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoom_MetaData)) };
	const UECodeGen_Private::FPropertyParamsBase* const Z_Construct_UClass_UAkRoomComponent_Statics::PropPointers[] = {
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnable,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bDynamic,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_Priority,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_WallOcclusion,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AuxSendLevel,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_AutoPost,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_bEnableReverbZone,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoomActor,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoomName,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_TransitionRegionWidth,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_GeometryComponent,
		(const UECodeGen_Private::FPropertyParamsBase*)&Z_Construct_UClass_UAkRoomComponent_Statics::NewProp_ParentRoom,
	};
	const FCppClassTypeInfoStatic Z_Construct_UClass_UAkRoomComponent_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<UAkRoomComponent>::IsAbstract,
	};
	const UECodeGen_Private::FClassParams Z_Construct_UClass_UAkRoomComponent_Statics::ClassParams = {
		&UAkRoomComponent::StaticClass,
		"Engine",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		FuncInfo,
		Z_Construct_UClass_UAkRoomComponent_Statics::PropPointers,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		UE_ARRAY_COUNT(FuncInfo),
		UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::PropPointers),
		0,
		0x00B000A4u,
		METADATA_PARAMS(Z_Construct_UClass_UAkRoomComponent_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_UAkRoomComponent_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_UAkRoomComponent()
	{
		if (!Z_Registration_Info_UClass_UAkRoomComponent.OuterSingleton)
		{
			UECodeGen_Private::ConstructUClass(Z_Registration_Info_UClass_UAkRoomComponent.OuterSingleton, Z_Construct_UClass_UAkRoomComponent_Statics::ClassParams);
		}
		return Z_Registration_Info_UClass_UAkRoomComponent.OuterSingleton;
	}
	template<> AKAUDIO_API UClass* StaticClass<UAkRoomComponent>()
	{
		return UAkRoomComponent::StaticClass();
	}
	DEFINE_VTABLE_PTR_HELPER_CTOR(UAkRoomComponent);
	UAkRoomComponent::~UAkRoomComponent() {}
	struct Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkRoomComponent_h_Statics
	{
		static const FClassRegisterCompiledInInfo ClassInfo[];
	};
	const FClassRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkRoomComponent_h_Statics::ClassInfo[] = {
		{ Z_Construct_UClass_UAkRoomComponent, UAkRoomComponent::StaticClass, TEXT("UAkRoomComponent"), &Z_Registration_Info_UClass_UAkRoomComponent, CONSTRUCT_RELOAD_VERSION_INFO(FClassReloadVersionInfo, sizeof(UAkRoomComponent), 1750872073U) },
	};
	static FRegisterCompiledInInfo Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkRoomComponent_h_158863556(TEXT("/Script/AkAudio"),
		Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkRoomComponent_h_Statics::ClassInfo, UE_ARRAY_COUNT(Z_CompiledInDeferFile_FID_AgeOfWolves_Plugins_Wwise_Source_AkAudio_Classes_AkRoomComponent_h_Statics::ClassInfo),
		nullptr, 0,
		nullptr, 0);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
