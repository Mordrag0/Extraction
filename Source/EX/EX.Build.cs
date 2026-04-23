// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EX : ModuleRules
{
	public EX(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] 
            {
                "Core", 
                "CoreUObject", 
                "Engine", 
                "InputCore",
                "OnlineSubsystem",
                "OnlineSubsystemUtils",
                "AssetRegistry",
                "AdvancedSessions",
                "AdvancedSteamSessions",
                "Steamworks",
                "LowEntryExtendedStandardLibrary",
                "TcpSocketPlugin",
                "PhysicsCore",
                "NetCore",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] 
            {
                "InputCore",
                "Slate", 
                "SlateCore",
                "PakFile",
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
            new string[] 
            {
                "NetworkReplayStreaming",
                "NullNetworkReplayStreaming",
                "HttpNetworkReplayStreaming",
                "LocalFileNetworkReplayStreaming",
                "OnlineSubsystemSteam",
            }
        );


	}
}
