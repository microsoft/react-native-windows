// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Runtime.InteropServices;
using System.Windows.Automation;
using Microsoft.ReactNative.AccessibilityDiagnostics.Core;

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Cli.Windows;

internal sealed class WindowsAutomationSnapshotCollector
{
    private static readonly AutomationProperty[] CachedProperties =
    [
        AutomationElement.AccessKeyProperty,
        AutomationElement.AutomationIdProperty,
        AutomationElement.ClassNameProperty,
        AutomationElement.ControlTypeProperty,
        AutomationElement.FrameworkIdProperty,
        AutomationElement.HelpTextProperty,
        AutomationElement.IsEnabledProperty,
        AutomationElement.IsKeyboardFocusableProperty,
        AutomationElement.IsOffscreenProperty,
        AutomationElement.LocalizedControlTypeProperty,
        AutomationElement.NameProperty,
        AutomationElement.NativeWindowHandleProperty,
        AutomationElement.ProcessIdProperty,
        ExpandCollapsePattern.ExpandCollapseStateProperty,
        SelectionItemPattern.IsSelectedProperty,
        TogglePattern.ToggleStateProperty,
    ];

    private static readonly AutomationPattern[] CachedPatterns =
    [
        ExpandCollapsePattern.Pattern,
        SelectionItemPattern.Pattern,
        TogglePattern.Pattern,
    ];

    public AccessibilitySnapshot Capture(CaptureCommand command)
    {
        ArgumentNullException.ThrowIfNull(command);

        var selection = WindowSelector.Select(command);
        var target = ReadTarget(selection);

        if (command.SettleMilliseconds > 0)
        {
            Thread.Sleep(command.SettleMilliseconds);
        }

        var cacheRequest = CreateCacheRequest();
        AutomationElement cachedRoot;
        try
        {
            cachedRoot = selection.Root.GetUpdatedCache(cacheRequest);
        }
        catch (ElementNotAvailableException exception)
        {
            throw new AutomationCaptureException("The selected window closed before it could be captured.", exception);
        }
        catch (InvalidOperationException exception)
        {
            throw new AutomationCaptureException(
                "Windows UI Automation could not cache the selected window.",
                exception);
        }

        var context = new CaptureContext(command.MaxDepth, command.MaxNodes);
        var root = CaptureNode(cachedRoot, depth: 0, context, TreeWalker.ControlViewWalker, cacheRequest)
            ?? throw new AutomationCaptureException("The selected window became unavailable during capture.");

        return new AccessibilitySnapshot
        {
            CaptureStatus = context.IsIncomplete ? CaptureStatus.Incomplete : CaptureStatus.Complete,
            Warnings = context.Warnings,
            CapturedAtUtc = DateTimeOffset.UtcNow,
            Target = target,
            Options = new SnapshotOptions
            {
                MaxDepth = command.MaxDepth,
                MaxNodes = command.MaxNodes,
                View = "control",
            },
            Root = root,
        };
    }

    private static CacheRequest CreateCacheRequest()
    {
        var request = new CacheRequest
        {
            AutomationElementMode = AutomationElementMode.Full,
            TreeFilter = Automation.ControlViewCondition,
            TreeScope = TreeScope.Element,
        };

        foreach (var property in CachedProperties)
        {
            request.Add(property);
        }

        foreach (var pattern in CachedPatterns)
        {
            request.Add(pattern);
        }

        return request;
    }

    private static SnapshotTarget ReadTarget(WindowSelection selection)
    {
        try
        {
            return new SnapshotTarget
            {
                RequestedProcessName = selection.RequestedProcessName,
                RequestedProcessId = selection.RequestedProcessId,
                WindowProcessId = selection.Root.Current.ProcessId,
                NativeWindowHandle = selection.Root.Current.NativeWindowHandle,
                WindowTitle = Clean(selection.Root.Current.Name),
            };
        }
        catch (ElementNotAvailableException exception)
        {
            throw new AutomationCaptureException("The selected window is no longer available.", exception);
        }
    }

    private static AccessibilityNode? CaptureNode(
        AutomationElement element,
        int depth,
        CaptureContext context,
        TreeWalker walker,
        CacheRequest cacheRequest)
    {
        if (context.NodeCount >= context.MaxNodes)
        {
            context.MarkIncomplete($"Capture stopped after reaching the {context.MaxNodes} node limit.");
            return null;
        }

        try
        {
            context.NodeCount++;
            var children = new List<AccessibilityNode>();

            if (depth >= context.MaxDepth)
            {
                if (TryGetFirstChild(element, depth, context, walker, cacheRequest) is not null)
                {
                    context.MarkIncomplete($"Capture stopped below depth {context.MaxDepth}.");
                }
            }
            else
            {
                var child = TryGetFirstChild(element, depth, context, walker, cacheRequest);
                while (child is not null)
                {
                    var capturedChild = CaptureNode(child, depth + 1, context, walker, cacheRequest);
                    if (capturedChild is not null)
                    {
                        children.Add(capturedChild);
                    }

                    if (capturedChild is null && context.NodeCount >= context.MaxNodes)
                    {
                        break;
                    }

                    child = TryGetNextSibling(child, depth + 1, context, walker, cacheRequest);
                }
            }

            return new AccessibilityNode
            {
                AutomationId = ReadString(element, AutomationElement.AutomationIdProperty),
                Name = ReadString(element, AutomationElement.NameProperty),
                ControlType = ReadControlType(element),
                LocalizedControlType = ReadString(element, AutomationElement.LocalizedControlTypeProperty),
                ClassName = ReadString(element, AutomationElement.ClassNameProperty),
                FrameworkId = ReadString(element, AutomationElement.FrameworkIdProperty),
                IsEnabled = ReadBoolean(element, AutomationElement.IsEnabledProperty, defaultValue: true),
                IsKeyboardFocusable = ReadBoolean(
                    element,
                    AutomationElement.IsKeyboardFocusableProperty,
                    defaultValue: false),
                IsOffscreen = ReadBoolean(element, AutomationElement.IsOffscreenProperty, defaultValue: false),
                HelpText = ReadString(element, AutomationElement.HelpTextProperty),
                AccessKey = ReadString(element, AutomationElement.AccessKeyProperty),
                IsSelected = ReadIsSelected(element),
                ToggleState = ReadToggleState(element),
                ExpandCollapseState = ReadExpandCollapseState(element),
                Children = children,
            };
        }
        catch (ElementNotAvailableException)
        {
            context.MarkIncomplete($"An element at depth {depth} disappeared during capture.");
            return null;
        }
        catch (InvalidOperationException exception)
        {
            context.MarkIncomplete(
                $"An element at depth {depth} did not expose all cached properties: {exception.Message}");
            return null;
        }
        catch (COMException exception)
        {
            context.MarkIncomplete($"Windows UI Automation skipped an element at depth {depth}: {exception.Message}");
            return null;
        }
    }

    private static AutomationElement? TryGetFirstChild(
        AutomationElement element,
        int depth,
        CaptureContext context,
        TreeWalker walker,
        CacheRequest cacheRequest)
    {
        try
        {
            return walker.GetFirstChild(element, cacheRequest);
        }
        catch (ElementNotAvailableException)
        {
            context.MarkIncomplete($"Could not enumerate children for an element at depth {depth}.");
            return null;
        }
        catch (InvalidOperationException exception)
        {
            context.MarkIncomplete($"Could not cache children at depth {depth}: {exception.Message}");
            return null;
        }
        catch (COMException exception)
        {
            context.MarkIncomplete(
                $"Windows UI Automation could not enumerate children at depth {depth}: {exception.Message}");
            return null;
        }
    }

    private static AutomationElement? TryGetNextSibling(
        AutomationElement element,
        int depth,
        CaptureContext context,
        TreeWalker walker,
        CacheRequest cacheRequest)
    {
        try
        {
            return walker.GetNextSibling(element, cacheRequest);
        }
        catch (ElementNotAvailableException)
        {
            context.MarkIncomplete($"A sibling element disappeared at depth {depth}.");
            return null;
        }
        catch (InvalidOperationException exception)
        {
            context.MarkIncomplete($"Could not cache a sibling at depth {depth}: {exception.Message}");
            return null;
        }
        catch (COMException exception)
        {
            context.MarkIncomplete(
                $"Windows UI Automation could not enumerate a sibling at depth {depth}: {exception.Message}");
            return null;
        }
    }

    private static string ReadControlType(AutomationElement element)
    {
        var value = element.GetCachedPropertyValue(AutomationElement.ControlTypeProperty, ignoreDefaultValue: true);
        if (value is not ControlType controlType)
        {
            return "Unknown";
        }

        const string prefix = "ControlType.";
        return controlType.ProgrammaticName.StartsWith(prefix, StringComparison.Ordinal)
            ? controlType.ProgrammaticName[prefix.Length..]
            : controlType.ProgrammaticName;
    }

    private static string? ReadString(AutomationElement element, AutomationProperty property)
    {
        var value = element.GetCachedPropertyValue(property, ignoreDefaultValue: true);
        return ReferenceEquals(value, AutomationElement.NotSupported) ? null : Clean(value as string);
    }

    private static bool ReadBoolean(AutomationElement element, AutomationProperty property, bool defaultValue)
    {
        var value = element.GetCachedPropertyValue(property, ignoreDefaultValue: true);
        return value is bool boolean ? boolean : defaultValue;
    }

    private static bool? ReadIsSelected(AutomationElement element)
    {
        return element.TryGetCachedPattern(SelectionItemPattern.Pattern, out var value) &&
            value is SelectionItemPattern pattern
                ? pattern.Cached.IsSelected
                : null;
    }

    private static string? ReadToggleState(AutomationElement element)
    {
        return element.TryGetCachedPattern(TogglePattern.Pattern, out var value) && value is TogglePattern pattern
            ? pattern.Cached.ToggleState.ToString()
            : null;
    }

    private static string? ReadExpandCollapseState(AutomationElement element)
    {
        return element.TryGetCachedPattern(ExpandCollapsePattern.Pattern, out var value) &&
            value is ExpandCollapsePattern pattern
                ? pattern.Cached.ExpandCollapseState.ToString()
                : null;
    }

    private static string? Clean(string? value)
    {
        return string.IsNullOrWhiteSpace(value) ? null : value.Trim();
    }

    private sealed class CaptureContext
    {
        private readonly HashSet<string> _warnings = new(StringComparer.Ordinal);

        public CaptureContext(int maxDepth, int maxNodes)
        {
            MaxDepth = maxDepth;
            MaxNodes = maxNodes;
        }

        public int MaxDepth { get; }

        public int MaxNodes { get; }

        public int NodeCount { get; set; }

        public bool IsIncomplete { get; private set; }

        public List<string> Warnings => _warnings.OrderBy(warning => warning, StringComparer.Ordinal).ToList();

        public void MarkIncomplete(string warning)
        {
            IsIncomplete = true;
            _warnings.Add(warning);
        }
    }
}
