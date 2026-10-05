using UnrealBuildTool;
public class Lanternfall : ModuleRules
{
 public Lanternfall(ReadOnlyTargetRules Target) : base(Target)
 { PCHUsage=PCHUsageMode.UseExplicitOrSharedPCHs; PublicDependencyModuleNames.AddRange(new[]{"Core","CoreUObject","Engine","InputCore"}); }
}

