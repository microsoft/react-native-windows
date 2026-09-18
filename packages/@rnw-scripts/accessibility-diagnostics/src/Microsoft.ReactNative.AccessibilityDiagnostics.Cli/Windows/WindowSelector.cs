// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Diagnostics;
using System.IO;
using System.Windows.Automation;

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Cli.Windows;

internal sealed record WindowSelection(
    AutomationElement Root,
    string? RequestedProcessName,
    int? RequestedProcessId);

internal static class WindowSelector
{
    public static WindowSelection Select(CaptureCommand command)
    {
        return command.SelectorKind switch
        {
            TargetSelectorKind.ProcessId => SelectByProcessIds(
                [CliArguments.ParseProcessId(command.SelectorValue)],
                requestedProcessName: null),
            TargetSelectorKind.ProcessName => SelectByProcessName(command.SelectorValue),
            TargetSelectorKind.WindowHandle => SelectByWindowHandle(command.SelectorValue),
            TargetSelectorKind.WindowTitle => SelectByWindowTitle(command.SelectorValue),
            _ => throw new ArgumentOutOfRangeException(nameof(command.SelectorKind)),
        };
    }

    private static WindowSelection SelectByWindowHandle(string value)
    {
        var handle = CliArguments.ParseWindowHandle(value);
        try
        {
            return new WindowSelection(AutomationElement.FromHandle(handle), null, null);
        }
        catch (ElementNotAvailableException exception)
        {
            throw new AutomationCaptureException(
                $"No accessible window exists for handle 0x{handle:X}.",
                exception);
        }
    }

    private static WindowSelection SelectByProcessName(string processName)
    {
        var fileName = Path.GetFileName(processName);
        var normalizedName = fileName.EndsWith(".exe", StringComparison.OrdinalIgnoreCase)
            ? fileName[..^4]
            : fileName;
        var processes = Process.GetProcessesByName(normalizedName);
        if (processes.Length == 0)
        {
            throw new AutomationCaptureException($"No running process named '{normalizedName}' was found.");
        }

        try
        {
            return SelectByProcessIds(processes.Select(process => process.Id).ToArray(), normalizedName);
        }
        finally
        {
            foreach (var process in processes)
            {
                process.Dispose();
            }
        }
    }

    private static WindowSelection SelectByProcessIds(IReadOnlyCollection<int> processIds, string? requestedProcessName)
    {
        var windows = GetTopLevelWindows();
        var directMatches = windows
            .Where(window => TryGetProcessId(window, out var processId) && processIds.Contains(processId))
            .ToArray();

        var matches = directMatches.Length > 0
            ? directMatches
            : windows.Where(window => ContainsProcess(window, processIds)).ToArray();

        if (matches.Length == 0)
        {
            throw new AutomationCaptureException(
                $"No top-level window was found for process ID(s) {string.Join(", ", processIds)}. " +
                "Use --hwnd when the application is hosted by another process.");
        }

        if (matches.Length > 1)
        {
            throw new AutomationCaptureException(
                $"Multiple top-level windows matched process ID(s) {string.Join(", ", processIds)}: " +
                $"{string.Join(", ", matches.Select(DescribeWindow))}. Use --hwnd to select one window.");
        }

        return new WindowSelection(
            matches[0],
            requestedProcessName,
            processIds.Count == 1 ? processIds.Single() : null);
    }

    private static WindowSelection SelectByWindowTitle(string title)
    {
        var matches = GetTopLevelWindows()
            .Where(window => string.Equals(TryGetName(window), title, StringComparison.OrdinalIgnoreCase))
            .ToArray();

        if (matches.Length == 0)
        {
            throw new AutomationCaptureException($"No top-level window titled '{title}' was found.");
        }

        if (matches.Length > 1)
        {
            throw new AutomationCaptureException(
                $"Multiple top-level windows titled '{title}' were found. Use --hwnd to select one window.");
        }

        return new WindowSelection(matches[0], null, null);
    }

    private static AutomationElement[] GetTopLevelWindows()
    {
        return AutomationElement.RootElement
            .FindAll(TreeScope.Children, Condition.TrueCondition)
            .Cast<AutomationElement>()
            .ToArray();
    }

    private static bool ContainsProcess(AutomationElement window, IReadOnlyCollection<int> processIds)
    {
        foreach (var processId in processIds)
        {
            var condition = new PropertyCondition(AutomationElement.ProcessIdProperty, processId);
            try
            {
                if (window.FindFirst(TreeScope.Descendants, condition) is not null)
                {
                    return true;
                }
            }
            catch (ElementNotAvailableException)
            {
                return false;
            }
        }

        return false;
    }

    private static string DescribeWindow(AutomationElement window)
    {
        var handle = TryGetNativeWindowHandle(window, out var nativeWindowHandle) ? nativeWindowHandle : 0;
        return $"'{TryGetName(window) ?? "<untitled>"}' (0x{handle:X})";
    }

    private static string? TryGetName(AutomationElement element)
    {
        try
        {
            return element.Current.Name;
        }
        catch (ElementNotAvailableException)
        {
            return null;
        }
    }

    private static bool TryGetProcessId(AutomationElement element, out int processId)
    {
        try
        {
            processId = element.Current.ProcessId;
            return true;
        }
        catch (ElementNotAvailableException)
        {
            processId = 0;
            return false;
        }
    }

    private static bool TryGetNativeWindowHandle(AutomationElement element, out int nativeWindowHandle)
    {
        try
        {
            nativeWindowHandle = element.Current.NativeWindowHandle;
            return true;
        }
        catch (ElementNotAvailableException)
        {
            nativeWindowHandle = 0;
            return false;
        }
    }
}
