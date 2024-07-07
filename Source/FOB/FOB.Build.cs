// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class FOB : ModuleRules
{
	public FOB(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "NavigationSystem", "AIModule" });

		PrivateIncludePaths.Add("FOB");
	}
}
