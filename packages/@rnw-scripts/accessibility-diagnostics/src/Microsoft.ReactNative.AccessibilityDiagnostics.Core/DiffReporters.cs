// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Text;

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Core;

public static class DiffReporters
{
    public static string RenderConsole(SnapshotDiff diff)
    {
        var builder = new StringBuilder();
        AppendSummary(builder, diff);

        foreach (var warning in diff.Warnings)
        {
            builder.AppendLine($"WARNING: {OutputSanitizer.Sanitize(warning)}");
        }

        foreach (var change in diff.Changes)
        {
            var marker = change.Impact == ChangeImpact.PotentialRegression ? "!" : "-";
            builder.AppendLine(
                $"{marker} [{change.Kind}] {OutputSanitizer.Sanitize(change.Path)}: " +
                $"{OutputSanitizer.Sanitize(change.Description)}");
        }

        return builder.ToString().TrimEnd();
    }

    public static string RenderMarkdown(SnapshotDiff diff)
    {
        var builder = new StringBuilder();
        builder.AppendLine("# Accessibility tree diff");
        builder.AppendLine();
        builder.AppendLine($"- Before: `{EscapeMarkdown(diff.BeforeSource)}`");
        builder.AppendLine($"- After: `{EscapeMarkdown(diff.AfterSource)}`");
        builder.AppendLine($"- Changes: {diff.Changes.Count}");
        var potentialRegressionCount =
            diff.Changes.Count(change => change.Impact == ChangeImpact.PotentialRegression);
        builder.AppendLine($"- Potential regressions: {potentialRegressionCount}");

        if (diff.Warnings.Count > 0)
        {
            builder.AppendLine();
            builder.AppendLine("## Warnings");
            builder.AppendLine();
            foreach (var warning in diff.Warnings)
            {
                builder.AppendLine($"- {EscapeMarkdown(warning)}");
            }
        }

        builder.AppendLine();
        builder.AppendLine("## Changes");
        builder.AppendLine();

        if (diff.Changes.Count == 0)
        {
            builder.AppendLine("No accessibility tree differences were found.");
            return builder.ToString().TrimEnd();
        }

        builder.AppendLine("| Impact | Kind | Path | Change |");
        builder.AppendLine("| --- | --- | --- | --- |");
        foreach (var change in diff.Changes)
        {
            builder.AppendLine(
                $"| {change.Impact} | {change.Kind} | `{EscapeMarkdown(change.Path)}` | " +
                $"{EscapeMarkdown(change.Description)} |");
        }

        return builder.ToString().TrimEnd();
    }

    private static void AppendSummary(StringBuilder builder, SnapshotDiff diff)
    {
        builder.AppendLine(
            $"Compared {OutputSanitizer.Sanitize(diff.BeforeSource)} -> " +
            $"{OutputSanitizer.Sanitize(diff.AfterSource)}");
        var potentialRegressionCount =
            diff.Changes.Count(change => change.Impact == ChangeImpact.PotentialRegression);
        builder.AppendLine(
            $"Found {diff.Changes.Count} change(s), {potentialRegressionCount} potential regression(s).");
    }

    private static string EscapeMarkdown(string value)
    {
        return OutputSanitizer.Sanitize(value)
            .Replace("|", "\\|", StringComparison.Ordinal)
            .Replace("\r", string.Empty, StringComparison.Ordinal)
            .Replace("\n", "<br>", StringComparison.Ordinal)
            .Replace("`", "\\`", StringComparison.Ordinal);
    }

}
