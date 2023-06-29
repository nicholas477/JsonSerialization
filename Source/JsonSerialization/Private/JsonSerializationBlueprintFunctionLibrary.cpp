// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonSerializationBlueprintFunctionLibrary.h"

#include "JsonSerialization.h"
#include "JsonObjectWrapper.h"

FJsonObjectWrapper UJsonSerializationBlueprintFunctionLibrary::ObjectToJson(const UObject* Object)
{
	FJsonObjectWrapper JsonWrapper;
	JsonWrapper.JsonObject = FJsonSerializationModule::SerializeUObjectToJson(Object);
	return JsonWrapper;
}
