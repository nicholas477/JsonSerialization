// Copyright Epic Games, Inc. All Rights Reserved.

#include "JsonSerialization.h"

#include "JsonObjectConverter.h"
#include "UObject/UnrealType.h"
#include "InstancedStruct.h"

#define LOCTEXT_NAMESPACE "FJsonSerializationModule"

void FJsonSerializationModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FJsonSerializationModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

static void SerializePropertyAsJsonObjectField(const void* Data, TSharedPtr<FJsonObject> OuterObject, FProperty* Property, TSet<const UObject*>& TraversedObjects);

static void SerializeStructPropertyAsJsonObjectField(const void* InnerPropData, FStructProperty* StructProperty, TSharedPtr<FJsonObject> StructObject, TSet<const UObject*>& TraversedObjects)
{
	const uint8* StructPropData = StructProperty->ContainerPtrToValuePtr<uint8>(InnerPropData);
	if (StructProperty->Struct == TBaseStructure<FInstancedStruct>::Get())
	{
		const FInstancedStruct& InstancedStruct = *(FInstancedStruct*)StructPropData;
		for (TFieldIterator<FProperty> PropertyItr(InstancedStruct.GetScriptStruct()); PropertyItr; ++PropertyItr)
		{
			SerializePropertyAsJsonObjectField(InstancedStruct.GetMemory(), StructObject, *PropertyItr, TraversedObjects);
		}
	}
	else
	{
		for (TFieldIterator<FProperty> PropertyItr(StructProperty->Struct); PropertyItr; ++PropertyItr)
		{
			SerializePropertyAsJsonObjectField((void*)StructPropData, StructObject, *PropertyItr, TraversedObjects);
		}
	}
}

static TArray<TSharedPtr<FJsonValue>> SerializePropertyAsJsonArray(const void* Data, FArrayProperty* Property, TSet<const UObject*>& TraversedObjects)
{
	const uint8* PropData = Property->ContainerPtrToValuePtr<uint8>(Data);
	FScriptArrayHelper Helper(Property, PropData);
	TArray<TSharedPtr<FJsonValue>> ValueArray;

	for (int32 i = 0, n = Helper.Num(); i < n; ++i)
	{
		const uint8* InnerPropData = Helper.GetRawPtr(i);
		if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property->Inner)) // Array
		{
			TArray<TSharedPtr<FJsonValue>> InnerArray = SerializePropertyAsJsonArray(InnerPropData, ArrayProperty, TraversedObjects);
			ValueArray.Emplace(new FJsonValueArray(InnerArray));
		}
		else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property->Inner)) // Struct
		{
			TSharedPtr<FJsonObject> StructObject = MakeShareable(new FJsonObject);
			SerializeStructPropertyAsJsonObjectField(InnerPropData, StructProperty, StructObject, TraversedObjects);
			ValueArray.Emplace(new FJsonValueObject(StructObject));
		}
		else if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property->Inner)) // Object
		{
			const UObject* SubObject = ObjectProperty->GetObjectPropertyValue_InContainer(InnerPropData);
			if (SubObject->IsValidLowLevel() && !TraversedObjects.Contains(SubObject))
			{
				TraversedObjects.Add(SubObject);
				TSharedPtr<FJsonObject> JsonSubObject = MakeShared<FJsonObject>();
				for (TFieldIterator<FProperty> PropertyItr(SubObject->GetClass()); PropertyItr; ++PropertyItr)
				{
					SerializePropertyAsJsonObjectField(SubObject, JsonSubObject, *PropertyItr, TraversedObjects);
				}
				ValueArray.Emplace(new FJsonValueObject(JsonSubObject));
			}
		}
		else
		{
			TSharedPtr<FJsonValue> JsonValue;
			const uint8* InnerInnerPropData = Property->Inner->ContainerPtrToValuePtr<uint8>(InnerPropData);
			ValueArray.Emplace(FJsonObjectConverter::UPropertyToJsonValue(Property->Inner, InnerInnerPropData));
		}
	}
	return ValueArray;
}

static void SerializePropertyAsJsonObjectField(const void* Data, TSharedPtr<FJsonObject> OuterObject, FProperty* Property, TSet<const UObject*>& TraversedObjects)
{
	if (Property->GetName() == "UberGraphFrame"
		|| Property->HasAnyPropertyFlags(CPF_Transient))
	{
		// Don't include "UberGraphFrame" or any transient properties
		return;
	}

	if (FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property)) // Array
	{
		TArray<TSharedPtr<FJsonValue>> Values = SerializePropertyAsJsonArray(Data, ArrayProperty, TraversedObjects);
		OuterObject->SetArrayField(Property->GetAuthoredName(), Values);
	}
	else if (FStructProperty* StructProperty = CastField<FStructProperty>(Property)) // Struct
	{
		TSharedPtr<FJsonObject> StructObject = MakeShareable(new FJsonObject);
		SerializeStructPropertyAsJsonObjectField(Data, StructProperty, StructObject, TraversedObjects);
		OuterObject->SetObjectField(Property->GetAuthoredName(), StructObject);
	}
	else if (FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property)) // Object
	{
		const UObject* SubObject = ObjectProperty->GetObjectPropertyValue_InContainer(Data);
		if (SubObject->IsValidLowLevel() && !TraversedObjects.Contains(SubObject))
		{
			TraversedObjects.Add(SubObject);
			TSharedPtr<FJsonObject> JsonSubObject = MakeShared<FJsonObject>();
			for (TFieldIterator<FProperty> PropertyItr(SubObject->GetClass()); PropertyItr; ++PropertyItr)
			{
				SerializePropertyAsJsonObjectField(SubObject, JsonSubObject, *PropertyItr, TraversedObjects);
			}
			OuterObject->SetObjectField(Property->GetAuthoredName(), JsonSubObject);
		}
	}
	else
	{
		TSharedPtr<FJsonValue> JsonValue;
		const uint8* PropData = Property->ContainerPtrToValuePtr<uint8>(Data);
		OuterObject->SetField(Property->GetAuthoredName(), FJsonObjectConverter::UPropertyToJsonValue(Property, PropData));
	}
}

TSharedPtr<FJsonObject> FJsonSerializationModule::SerializeUObjectToJson(const UObject* Object)
{
	TSet<const UObject*> TraversedObjects;
	TraversedObjects.Add(Object);

	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();

	for (TFieldIterator<FProperty> PropertyItr(Object->GetClass()); PropertyItr; ++PropertyItr)
	{
		SerializePropertyAsJsonObjectField(Object, JsonObject, *PropertyItr, TraversedObjects);
	}

	return JsonObject;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FJsonSerializationModule, JsonSerialization)