// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class JsonSerialization : ModuleRules
{
	public JsonSerialization(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
                "Json",
				"JsonUtilities"
            }
		);	
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"StructUtils"
			}
		);
	}
}
