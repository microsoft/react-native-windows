// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Text.Json.Serialization;

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Core;

public enum CaptureStatus
{
    Complete,
    Incomplete,
}

public enum ChangeKind
{
    Added,
    Removed,
    Modified,
}

public enum ChangeImpact
{
    Informational,
    PotentialRegression,
}

public enum FailOn
{
    None,
    PotentialRegression,
    Any,
}

public sealed class AccessibilitySnapshot
{
    public const int CurrentSchemaVersion = 1;

    [JsonPropertyOrder(0)]
    public int SchemaVersion { get; init; } = CurrentSchemaVersion;

    [JsonPropertyOrder(1)]
    public CaptureStatus CaptureStatus { get; init; } = CaptureStatus.Complete;

    [JsonPropertyOrder(2)]
    public List<string> Warnings { get; init; } = [];

    [JsonPropertyOrder(3)]
    public DateTimeOffset CapturedAtUtc { get; init; }

    [JsonPropertyOrder(4)]
    public required SnapshotTarget Target { get; init; }

    [JsonPropertyOrder(5)]
    public required SnapshotOptions Options { get; init; }

    [JsonPropertyOrder(6)]
    public required AccessibilityNode Root { get; init; }
}

public sealed class SnapshotTarget
{
    public string? RequestedProcessName { get; init; }

    public int? RequestedProcessId { get; init; }

    public int WindowProcessId { get; init; }

    public int NativeWindowHandle { get; init; }

    public string? WindowTitle { get; init; }
}

public sealed class SnapshotOptions
{
    public string View { get; init; } = "control";

    public int MaxDepth { get; init; }

    public int MaxNodes { get; init; }
}

public sealed class AccessibilityNode
{
    public string? AutomationId { get; init; }

    public string? Name { get; init; }

    public required string ControlType { get; init; }

    public string? LocalizedControlType { get; init; }

    public string? ClassName { get; init; }

    public string? FrameworkId { get; init; }

    public bool IsEnabled { get; init; }

    public bool IsKeyboardFocusable { get; init; }

    public bool IsOffscreen { get; init; }

    public string? HelpText { get; init; }

    public string? AccessKey { get; init; }

    public bool? IsSelected { get; init; }

    public string? ToggleState { get; init; }

    public string? ExpandCollapseState { get; init; }

    public List<AccessibilityNode> Children { get; init; } = [];
}

public sealed class SnapshotDiff
{
    public required string BeforeSource { get; init; }

    public required string AfterSource { get; init; }

    public List<string> Warnings { get; init; } = [];

    public List<AccessibilityChange> Changes { get; init; } = [];

    [JsonIgnore]
    public bool HasDifferences => Changes.Count > 0;

    [JsonIgnore]
    public bool HasPotentialRegressions =>
        Changes.Any(change => change.Impact == ChangeImpact.PotentialRegression);
}

public sealed class AccessibilityChange
{
    public required ChangeKind Kind { get; init; }

    public required ChangeImpact Impact { get; init; }

    public required string Path { get; init; }

    public string? Property { get; init; }

    public string? Before { get; init; }

    public string? After { get; init; }

    public required string Description { get; init; }
}
