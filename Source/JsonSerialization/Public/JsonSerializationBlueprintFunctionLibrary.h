// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "JsonObjectWrapper.h"
#include "JsonSerializationBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class JSONSERIALIZATION_API UJsonSerializationBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Json Serialization")
		static FJsonObjectWrapper ObjectToJson(const UObject* Object);
};
