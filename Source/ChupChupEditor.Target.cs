using UnrealBuildTool;
using System.Collections.Generic;

public class ChupChupEditorTarget : TargetRules
{
	public ChupChupEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V5;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		ExtraModuleNames.Add("ChupChup");
	}
}
