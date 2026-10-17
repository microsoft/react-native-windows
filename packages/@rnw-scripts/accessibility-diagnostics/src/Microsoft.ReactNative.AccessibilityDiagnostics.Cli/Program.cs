// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using Microsoft.ReactNative.AccessibilityDiagnostics.Core;
using Microsoft.ReactNative.AccessibilityDiagnostics.Cli.Windows;

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Cli;

internal static class Program
{
    private const int Success = 0;
    private const int DifferencesFound = 1;
    private const int UsageError = 2;
    private const int OperationError = 3;
    private const int IncompleteCapture = 4;

    [MTAThread]
    public static async Task<int> Main(string[] args)
    {
        Console.OutputEncoding = new UTF8Encoding(encoderShouldEmitUTF8Identifier: false);

        if (args.Length == 0 || args.Contains("--help", StringComparer.Ordinal))
        {
            PrintUsage();
            return args.Length == 0 ? UsageError : Success;
        }

        try
        {
            return args[0].ToLowerInvariant() switch
            {
                "capture" => await RunCaptureAsync(CliArguments.ParseCapture(args[1..])),
                "diff" => await RunDiffAsync(CliArguments.ParseDiff(args[1..])),
                _ => throw new ArgumentException($"Unknown command '{args[0]}'."),
            };
        }
        catch (ArgumentException exception)
        {
            Console.Error.WriteLine($"Error: {OutputSanitizer.Sanitize(exception.Message)}");
            Console.Error.WriteLine("Run 'rnw-a11y --help' for usage.");
            return UsageError;
        }
        catch (SnapshotFormatException exception)
        {
            Console.Error.WriteLine($"Snapshot error: {OutputSanitizer.Sanitize(exception.Message)}");
            return OperationError;
        }
        catch (AutomationCaptureException exception)
        {
            Console.Error.WriteLine($"Capture error: {OutputSanitizer.Sanitize(exception.Message)}");
            return OperationError;
        }
        catch (IOException exception)
        {
            Console.Error.WriteLine($"File error: {OutputSanitizer.Sanitize(exception.Message)}");
            return OperationError;
        }
        catch (UnauthorizedAccessException exception)
        {
            Console.Error.WriteLine($"Access error: {OutputSanitizer.Sanitize(exception.Message)}");
            return OperationError;
        }
        catch (COMException exception)
        {
            Console.Error.WriteLine(
                $"Windows UI Automation error: {OutputSanitizer.Sanitize(exception.Message)}");
            return OperationError;
        }
    }

    private static async Task<int> RunCaptureAsync(CaptureCommand command)
    {
        var collector = new WindowsAutomationSnapshotCollector();
        var snapshot = collector.Capture(command);
        await SnapshotSerializer.WriteAsync(command.OutputPath, snapshot);

        Console.WriteLine(
            $"Captured {CountNodes(snapshot.Root)} element(s) from " +
            $"'{OutputSanitizer.Sanitize(snapshot.Target.WindowTitle ?? "<untitled>")}' to " +
            $"{OutputSanitizer.Sanitize(command.OutputPath)}.");

        foreach (var warning in snapshot.Warnings)
        {
            Console.Error.WriteLine($"Warning: {OutputSanitizer.Sanitize(warning)}");
        }

        return snapshot.CaptureStatus == CaptureStatus.Complete ? Success : IncompleteCapture;
    }

    private static async Task<int> RunDiffAsync(DiffCommand command)
    {
        var before = await SnapshotSerializer.ReadSnapshotAsync(command.BeforePath);
        var after = await SnapshotSerializer.ReadSnapshotAsync(command.AfterPath);
        var diff = new SnapshotDiffer().Compare(
            before,
            after,
            command.BeforePath,
            command.AfterPath,
            command.AllowIncomplete);

        var report = command.Format switch
        {
            ReportFormat.Console => DiffReporters.RenderConsole(diff),
            ReportFormat.Json => SnapshotSerializer.Serialize(diff),
            ReportFormat.Markdown => DiffReporters.RenderMarkdown(diff),
            _ => throw new ArgumentOutOfRangeException(nameof(command.Format)),
        };

        if (command.OutputPath is null)
        {
            Console.WriteLine(report);
        }
        else
        {
            await WriteReportAsync(command.OutputPath, report);
            Console.WriteLine(
                $"Wrote {diff.Changes.Count} change(s) to {OutputSanitizer.Sanitize(command.OutputPath)}; " +
                $"{diff.Changes.Count(change => change.Impact == ChangeImpact.PotentialRegression)} " +
                "potential regression(s).");
        }

        return SnapshotDiffer.ShouldFail(diff, command.FailOn) ? DifferencesFound : Success;
    }

    private static async Task WriteReportAsync(string path, string report)
    {
        var directory = Path.GetDirectoryName(Path.GetFullPath(path));
        if (!string.IsNullOrEmpty(directory))
        {
            Directory.CreateDirectory(directory);
        }

        await File.WriteAllTextAsync(
            path,
            report.Replace("\r\n", "\n", StringComparison.Ordinal) + "\n",
            new UTF8Encoding(encoderShouldEmitUTF8Identifier: false));
    }

    private static int CountNodes(AccessibilityNode node)
    {
        return 1 + node.Children.Sum(CountNodes);
    }

    private static void PrintUsage()
    {
        Console.WriteLine(
            """
            RNW Accessibility Diagnostics

            Capture a Windows UI Automation tree:
              rnw-a11y capture (--pid <id> | --process <name> | --hwnd <handle> | --window-title <title>)
                               --output <snapshot.json>
                               [--max-depth <number>] [--max-nodes <number>] [--settle-ms <number>]

            Compare two snapshots:
              rnw-a11y diff <before.json> <after.json>
                            [--format console|json|markdown] [--output <path>]
                            [--fail-on none|potential-regression|any] [--allow-incomplete]

            Exit codes:
              0  Successful operation
              1  Differences matched the --fail-on policy
              2  Invalid command-line arguments
              3  Capture, snapshot, or file operation failed
              4  Capture completed with truncation or transient UIA warnings
            """);
    }
}
