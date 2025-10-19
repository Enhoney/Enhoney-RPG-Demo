// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class RPGDemo : ModuleRules
{
	public RPGDemo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { 
			"Core", 
			"CoreUObject", 
			"Engine", 
			"InputCore", 
			"EnhancedInput", 
			
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks", 

			"AIModule",
			"MotionWarping",

            "GeometryCollectionEngine",
			"ChaosSolverEngine",
			"FieldSystemEngine",
			"NetCore"
        });

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		PublicIncludePaths.AddRange(new string[] { 
			"RPGDemo",
			"RPGDemo/Public/Character",
            "RPGDemo/Public/EnhoneyTaskSystem",
            "RPGDemo/Public/Game",
            "RPGDemo/Public/Player",
            "RPGDemo/Public/Player/InputComponent",

            "RPGDemo/Public/AbilitySystem",
            "RPGDemo/Public/AbilitySystem/AttributeSet",
            "RPGDemo/Public/AbilitySystem/ModMagCal",
            "RPGDemo/Public/AbilitySystem/Data",
            "RPGDemo/Public/AbilitySystem/Actor",
            "RPGDemo/Public/AbilitySystem/GameplayAbility/",
            "RPGDemo/Public/AbilitySystem/GameplayAbility/AbilityBase",
            "RPGDemo/Public/AbilitySystem/Components/",

            "RPGDemo/Public/Interaction",

            "RPGDemo/Public/UI",
            "RPGDemo/Public/UI/WidgetController",
            "RPGDemo/Public/UI/HUD",
            "RPGDemo/Public/UI/UserWidget",

            "RPGDemo/Public/BPLibrary",

        });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
