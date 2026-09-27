// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Globalization;
using System.Text.RegularExpressions;

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Core;

public sealed class SnapshotDiffer
{
    private const long MaximumLcsCells = 1_000_000;

    private static readonly Regex Whitespace = new(@"\s+", RegexOptions.Compiled);

    private static readonly HashSet<string> InteractiveControlTypes =
        new(StringComparer.OrdinalIgnoreCase)
        {
            "Button",
            "CheckBox",
            "ComboBox",
            "Edit",
            "Hyperlink",
            "ListItem",
            "MenuItem",
            "RadioButton",
            "Slider",
            "Spinner",
            "TabItem",
            "TreeItem",
        };

    public SnapshotDiff Compare(
        AccessibilitySnapshot before,
        AccessibilitySnapshot after,
        string beforeSource,
        string afterSource,
        bool allowIncomplete = false)
    {
        ArgumentNullException.ThrowIfNull(before);
        ArgumentNullException.ThrowIfNull(after);

        if (!allowIncomplete &&
            (before.CaptureStatus == CaptureStatus.Incomplete || after.CaptureStatus == CaptureStatus.Incomplete))
        {
            throw new SnapshotFormatException(
                "Cannot compare incomplete snapshots. Re-capture the application or pass --allow-incomplete.");
        }

        var changes = new List<AccessibilityChange>();
        CompareNode(before.Root, after.Root, "$", changes);

        var warnings = new List<string>();
        warnings.AddRange(before.Warnings.Select(warning => $"Before: {warning}"));
        warnings.AddRange(after.Warnings.Select(warning => $"After: {warning}"));

        return new SnapshotDiff
        {
            BeforeSource = beforeSource,
            AfterSource = afterSource,
            Warnings = warnings,
            Changes = changes,
        };
    }

    public static bool ShouldFail(SnapshotDiff diff, FailOn failOn)
    {
        return failOn switch
        {
            FailOn.None => false,
            FailOn.PotentialRegression => diff.HasPotentialRegressions,
            FailOn.Any => diff.HasDifferences,
            _ => throw new ArgumentOutOfRangeException(nameof(failOn), failOn, "Unknown failure policy."),
        };
    }

    private static void CompareNode(
        AccessibilityNode before,
        AccessibilityNode after,
        string path,
        List<AccessibilityChange> changes)
    {
        CompareProperty(path, "automationId", before.AutomationId, after.AutomationId, before, after, changes);
        CompareProperty(path, "name", before.Name, after.Name, before, after, changes);
        CompareProperty(path, "controlType", before.ControlType, after.ControlType, before, after, changes);
        CompareProperty(
            path,
            "localizedControlType",
            before.LocalizedControlType,
            after.LocalizedControlType,
            before,
            after,
            changes);
        CompareProperty(path, "className", before.ClassName, after.ClassName, before, after, changes);
        CompareProperty(path, "frameworkId", before.FrameworkId, after.FrameworkId, before, after, changes);
        CompareProperty(path, "isEnabled", before.IsEnabled, after.IsEnabled, before, after, changes);
        CompareProperty(
            path,
            "isKeyboardFocusable",
            before.IsKeyboardFocusable,
            after.IsKeyboardFocusable,
            before,
            after,
            changes);
        CompareProperty(path, "isOffscreen", before.IsOffscreen, after.IsOffscreen, before, after, changes);
        CompareProperty(path, "helpText", before.HelpText, after.HelpText, before, after, changes);
        CompareProperty(path, "accessKey", before.AccessKey, after.AccessKey, before, after, changes);
        CompareProperty(path, "isSelected", before.IsSelected, after.IsSelected, before, after, changes);
        CompareProperty(path, "toggleState", before.ToggleState, after.ToggleState, before, after, changes);
        CompareProperty(
            path,
            "expandCollapseState",
            before.ExpandCollapseState,
            after.ExpandCollapseState,
            before,
            after,
            changes);

        CompareChildren(before.Children, after.Children, path, changes);
    }

    private static void CompareChildren(
        IReadOnlyList<AccessibilityNode> before,
        IReadOnlyList<AccessibilityNode> after,
        string parentPath,
        List<AccessibilityChange> changes)
    {
        var matches = MatchChildren(before, after);
        var matchedBefore = matches.Select(match => match.BeforeIndex).ToHashSet();
        var matchedAfter = matches.Select(match => match.AfterIndex).ToHashSet();

        var afterOrder = matches.OrderBy(match => match.BeforeIndex).Select(match => match.AfterIndex).ToArray();
        if (!afterOrder.SequenceEqual(afterOrder.OrderBy(index => index)))
        {
            changes.Add(new AccessibilityChange
            {
                Kind = ChangeKind.Modified,
                Impact = ChangeImpact.PotentialRegression,
                Path = parentPath,
                Property = "childrenOrder",
                Description = "The relative order of existing child elements changed.",
            });
        }

        foreach (var match in matches.OrderBy(match => match.BeforeIndex))
        {
            var segment = CreatePathSegment(before[match.BeforeIndex], match.BeforeIndex);
            CompareNode(before[match.BeforeIndex], after[match.AfterIndex], $"{parentPath}/{segment}", changes);
        }

        for (var index = 0; index < before.Count; index++)
        {
            if (matchedBefore.Contains(index))
            {
                continue;
            }

            var node = before[index];
            changes.Add(new AccessibilityChange
            {
                Kind = ChangeKind.Removed,
                Impact = ContainsInteractiveElement(node)
                    ? ChangeImpact.PotentialRegression
                    : ChangeImpact.Informational,
                Path = $"{parentPath}/{CreatePathSegment(node, index)}",
                Before = DescribeNode(node),
                Description = $"Removed {DescribeNode(node)}.",
            });
        }

        for (var index = 0; index < after.Count; index++)
        {
            if (matchedAfter.Contains(index))
            {
                continue;
            }

            var node = after[index];
            changes.Add(new AccessibilityChange
            {
                Kind = ChangeKind.Added,
                Impact = ChangeImpact.Informational,
                Path = $"{parentPath}/{CreatePathSegment(node, index)}",
                After = DescribeNode(node),
                Description = $"Added {DescribeNode(node)}.",
            });
        }
    }

    private static List<NodeMatch> MatchChildren(
        IReadOnlyList<AccessibilityNode> before,
        IReadOnlyList<AccessibilityNode> after)
    {
        var matches = new List<NodeMatch>();
        var usedBefore = new HashSet<int>();
        var usedAfter = new HashSet<int>();

        MatchUniqueAutomationIds(before, after, matches, usedBefore, usedAfter);
        MatchByStrongIdentity(before, after, matches, usedBefore, usedAfter);
        MatchByWeakIdentity(before, after, matches, usedBefore, usedAfter);

        return matches;
    }

    private static void MatchUniqueAutomationIds(
        IReadOnlyList<AccessibilityNode> before,
        IReadOnlyList<AccessibilityNode> after,
        List<NodeMatch> matches,
        HashSet<int> usedBefore,
        HashSet<int> usedAfter)
    {
        var beforeIds = GetUniqueAutomationIds(before);
        var afterIds = GetUniqueAutomationIds(after);

        foreach (var pair in beforeIds)
        {
            if (!afterIds.TryGetValue(pair.Key, out var afterIndex))
            {
                continue;
            }

            matches.Add(new NodeMatch(pair.Value, afterIndex));
            usedBefore.Add(pair.Value);
            usedAfter.Add(afterIndex);
        }
    }

    private static Dictionary<string, int> GetUniqueAutomationIds(IReadOnlyList<AccessibilityNode> nodes)
    {
        return nodes
            .Select((node, index) => new { Id = Normalize(node.AutomationId), Index = index })
            .Where(item => !string.IsNullOrEmpty(item.Id))
            .GroupBy(item => item.Id!, StringComparer.Ordinal)
            .Where(group => group.Count() == 1)
            .ToDictionary(group => group.Key, group => group.Single().Index, StringComparer.Ordinal);
    }

    private static void MatchByStrongIdentity(
        IReadOnlyList<AccessibilityNode> before,
        IReadOnlyList<AccessibilityNode> after,
        List<NodeMatch> matches,
        HashSet<int> usedBefore,
        HashSet<int> usedAfter)
    {
        var beforeIndices = Enumerable.Range(0, before.Count).Where(index => !usedBefore.Contains(index)).ToArray();
        var afterIndices = Enumerable.Range(0, after.Count).Where(index => !usedAfter.Contains(index)).ToArray();
        var beforeIdentities = beforeIndices.Select(index => StrongIdentity(before[index])).ToArray();
        var afterIdentities = afterIndices.Select(index => StrongIdentity(after[index])).ToArray();

        if ((long)beforeIndices.Length * afterIndices.Length > MaximumLcsCells)
        {
            MatchStrongIdentityLinearly(
                beforeIndices,
                afterIndices,
                beforeIdentities,
                afterIdentities,
                matches,
                usedBefore,
                usedAfter);
            return;
        }

        var lengths = new int[beforeIndices.Length + 1, afterIndices.Length + 1];

        for (var beforeOffset = beforeIndices.Length - 1; beforeOffset >= 0; beforeOffset--)
        {
            for (var afterOffset = afterIndices.Length - 1; afterOffset >= 0; afterOffset--)
            {
                if (beforeIdentities[beforeOffset] == afterIdentities[afterOffset])
                {
                    lengths[beforeOffset, afterOffset] = lengths[beforeOffset + 1, afterOffset + 1] + 1;
                }
                else
                {
                    lengths[beforeOffset, afterOffset] = Math.Max(
                        lengths[beforeOffset + 1, afterOffset],
                        lengths[beforeOffset, afterOffset + 1]);
                }
            }
        }

        var beforePosition = 0;
        var afterPosition = 0;
        while (beforePosition < beforeIndices.Length && afterPosition < afterIndices.Length)
        {
            var beforeIndex = beforeIndices[beforePosition];
            var afterIndex = afterIndices[afterPosition];
            if (beforeIdentities[beforePosition] == afterIdentities[afterPosition])
            {
                matches.Add(new NodeMatch(beforeIndex, afterIndex));
                usedBefore.Add(beforeIndex);
                usedAfter.Add(afterIndex);
                beforePosition++;
                afterPosition++;
            }
            else if (lengths[beforePosition + 1, afterPosition] >= lengths[beforePosition, afterPosition + 1])
            {
                beforePosition++;
            }
            else
            {
                afterPosition++;
            }
        }
    }

    private static void MatchStrongIdentityLinearly(
        IReadOnlyList<int> beforeIndices,
        IReadOnlyList<int> afterIndices,
        IReadOnlyList<string> beforeIdentities,
        IReadOnlyList<string> afterIdentities,
        List<NodeMatch> matches,
        HashSet<int> usedBefore,
        HashSet<int> usedAfter)
    {
        var afterByIdentity = new Dictionary<string, Queue<int>>(StringComparer.Ordinal);
        for (var index = 0; index < afterIndices.Count; index++)
        {
            if (!afterByIdentity.TryGetValue(afterIdentities[index], out var queue))
            {
                queue = new Queue<int>();
                afterByIdentity.Add(afterIdentities[index], queue);
            }

            queue.Enqueue(afterIndices[index]);
        }

        for (var index = 0; index < beforeIndices.Count; index++)
        {
            if (!afterByIdentity.TryGetValue(beforeIdentities[index], out var queue) || queue.Count == 0)
            {
                continue;
            }

            var beforeIndex = beforeIndices[index];
            var afterIndex = queue.Dequeue();
            matches.Add(new NodeMatch(beforeIndex, afterIndex));
            usedBefore.Add(beforeIndex);
            usedAfter.Add(afterIndex);
        }
    }

    private static void MatchByWeakIdentity(
        IReadOnlyList<AccessibilityNode> before,
        IReadOnlyList<AccessibilityNode> after,
        List<NodeMatch> matches,
        HashSet<int> usedBefore,
        HashSet<int> usedAfter)
    {
        foreach (var beforeIndex in Enumerable.Range(0, before.Count).Where(index => !usedBefore.Contains(index)))
        {
            var candidates = Enumerable.Range(0, after.Count)
                .Where(index => !usedAfter.Contains(index))
                .Where(index => WeakIdentity(before[beforeIndex]) == WeakIdentity(after[index]))
                .Where(index => !WouldCrossExistingMatch(beforeIndex, index, matches))
                .OrderBy(index => Math.Abs(index - beforeIndex))
                .ToArray();

            if (candidates.Length == 0)
            {
                continue;
            }

            var afterIndex = candidates[0];
            matches.Add(new NodeMatch(beforeIndex, afterIndex));
            usedBefore.Add(beforeIndex);
            usedAfter.Add(afterIndex);
        }
    }

    private static bool WouldCrossExistingMatch(
        int beforeIndex,
        int afterIndex,
        IReadOnlyList<NodeMatch> matches)
    {
        return matches.Any(match =>
            (match.BeforeIndex < beforeIndex && match.AfterIndex > afterIndex) ||
            (match.BeforeIndex > beforeIndex && match.AfterIndex < afterIndex));
    }

    private static void CompareProperty<T>(
        string path,
        string property,
        T beforeValue,
        T afterValue,
        AccessibilityNode beforeNode,
        AccessibilityNode afterNode,
        List<AccessibilityChange> changes)
    {
        var before = FormatValue(beforeValue);
        var after = FormatValue(afterValue);
        if (before == after)
        {
            return;
        }

        changes.Add(new AccessibilityChange
        {
            Kind = ChangeKind.Modified,
            Impact = GetImpact(property, before, after, beforeNode, afterNode),
            Path = path,
            Property = property,
            Before = before,
            After = after,
            Description = $"{property} changed from {DisplayValue(before)} to {DisplayValue(after)}.",
        });
    }

    private static ChangeImpact GetImpact(
        string property,
        string? before,
        string? after,
        AccessibilityNode beforeNode,
        AccessibilityNode afterNode)
    {
        if (property == "controlType")
        {
            return ChangeImpact.PotentialRegression;
        }

        if (property == "isKeyboardFocusable" && before == "true" && after == "false")
        {
            return ChangeImpact.PotentialRegression;
        }

        if (property == "name" &&
            !string.IsNullOrEmpty(before) &&
            string.IsNullOrEmpty(after) &&
            (beforeNode.IsKeyboardFocusable || afterNode.IsKeyboardFocusable))
        {
            return ChangeImpact.PotentialRegression;
        }

        return ChangeImpact.Informational;
    }

    private static bool ContainsInteractiveElement(AccessibilityNode node)
    {
        return node.IsKeyboardFocusable ||
            InteractiveControlTypes.Contains(node.ControlType) ||
            node.Children.Any(ContainsInteractiveElement);
    }

    private static string StrongIdentity(AccessibilityNode node)
    {
        return string.Join(
            '\u001f',
            Normalize(node.ControlType),
            Normalize(node.Name),
            Normalize(node.ClassName));
    }

    private static string WeakIdentity(AccessibilityNode node)
    {
        return string.Join('\u001f', Normalize(node.ControlType), Normalize(node.ClassName));
    }

    private static string CreatePathSegment(AccessibilityNode node, int index)
    {
        var identity = !string.IsNullOrEmpty(Normalize(node.AutomationId))
            ? $"#{Normalize(node.AutomationId)}"
            : $"{Normalize(node.ControlType) ?? "Unknown"}[{index}]";
        return identity.Replace("/", "\\/", StringComparison.Ordinal);
    }

    private static string DescribeNode(AccessibilityNode node)
    {
        return string.IsNullOrEmpty(Normalize(node.Name))
            ? node.ControlType
            : $"{node.ControlType} \"{Normalize(node.Name)}\"";
    }

    private static string? FormatValue<T>(T value)
    {
        return value switch
        {
            null => null,
            string text => Normalize(text),
            bool boolean => boolean ? "true" : "false",
            IFormattable formattable => formattable.ToString(null, CultureInfo.InvariantCulture),
            _ => value.ToString(),
        };
    }

    private static string DisplayValue(string? value)
    {
        return value is null ? "<missing>" : $"\"{value}\"";
    }

    private static string? Normalize(string? value)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return null;
        }

        return Whitespace.Replace(value.Trim(), " ");
    }

    private sealed record NodeMatch(int BeforeIndex, int AfterIndex);
}
