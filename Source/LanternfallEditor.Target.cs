using UnrealBuildTool;
public class LanternfallEditorTarget : TargetRules
{
 public LanternfallEditorTarget(TargetInfo Target) : base(Target) { Type=TargetType.Editor; DefaultBuildSettings=BuildSettingsVersion.V5; IncludeOrderVersion=EngineIncludeOrderVersion.Unreal5_4; ExtraModuleNames.Add("Lanternfall"); }
}

