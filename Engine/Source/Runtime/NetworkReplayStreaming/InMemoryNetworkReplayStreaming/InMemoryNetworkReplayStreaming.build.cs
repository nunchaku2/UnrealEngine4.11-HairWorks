// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class InMemoryNetworkReplayStreaming : ModuleRules
	{
		public InMemoryNetworkReplayStreaming( TargetInfo Target )
		{
			PublicIncludePathModuleNames.Add("Engine");

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"NetworkReplayStreaming",
                    "Json",
					"Engine"
				}
			);
		}
	}
}