// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class BackontheROCKS : ModuleRules
{
	public BackontheROCKS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "ChaosVehicles", "PhysicsCore", "UMG" });
	}
}
