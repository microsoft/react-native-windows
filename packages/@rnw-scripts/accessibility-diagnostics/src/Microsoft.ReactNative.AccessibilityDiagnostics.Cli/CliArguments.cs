// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Globalization;
using Microsoft.ReactNative.AccessibilityDiagnostics.Core;

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Cli;

internal enum TargetSelectorKind
{
    ProcessId,
    ProcessName,
    WindowHandle,
    WindowTitle,
}

internal enum ReportFormat
{
    Console,
    Json,
    Markdown,
}

internal sealed class CaptureCommand
{
    public required TargetSelectorKind SelectorKind { get; init; }

    public required string SelectorValue { get; init; }

    public required string OutputPath { get; init; }

    public int MaxDepth { get; init; } = 100;

    public int MaxNodes { get; init; } = 10_000;

    public int SettleMilliseconds { get; init; } = 100;
}

internal sealed class DiffCommand
{
    public required string BeforePath { get; init; }

    public required string AfterPath { get; init; }

    public ReportFormat Format { get; init; } = ReportFormat.Console;

    public string? OutputPath { get; init; }

    public FailOn FailOn { get; init; } = FailOn.Any;

    public bool AllowIncomplete { get; init; }
}

internal static class CliArguments
{
    private const int MaximumCaptureDepth = 200;

    private static readonly HashSet<string> CaptureOptions =
        new(StringComparer.Ordinal)
        {
            "--pid",
            "--process",
            "--hwnd",
            "--window-title",
            "--output",
            "--max-depth",
            "--max-nodes",
            "--settle-ms",
        };

    private static readonly HashSet<string> DiffOptions =
        new(StringComparer.Ordinal)
        {
            "--format",
            "--output",
            "--fail-on",
            "--allow-incomplete",
        };

    public static CaptureCommand ParseCapture(string[] args)
    {
        var parsed = Parse(args, CaptureOptions, []);
        if (parsed.Positionals.Count > 0)
        {
            throw new ArgumentException("The capture command does not accept positional arguments.");
        }

        var selectors = new[]
        {
            (Option: "--pid", Kind: TargetSelectorKind.ProcessId),
            (Option: "--process", Kind: TargetSelectorKind.ProcessName),
            (Option: "--hwnd", Kind: TargetSelectorKind.WindowHandle),
            (Option: "--window-title", Kind: TargetSelectorKind.WindowTitle),
        }.Where(selector => parsed.Values.ContainsKey(selector.Option)).ToArray();

        if (selectors.Length != 1)
        {
            throw new ArgumentException(
                "Specify exactly one target selector: --pid, --process, --hwnd, or --window-title.");
        }

        var maxDepth = ParsePositiveInt(parsed, "--max-depth", 100, MaximumCaptureDepth);
        var maxNodes = ParsePositiveInt(parsed, "--max-nodes", 10_000);
        var settleMilliseconds = ParseNonNegativeInt(parsed, "--settle-ms", 100);

        return new CaptureCommand
        {
            SelectorKind = selectors[0].Kind,
            SelectorValue = parsed.Values[selectors[0].Option],
            OutputPath = GetRequired(parsed, "--output"),
            MaxDepth = maxDepth,
            MaxNodes = maxNodes,
            SettleMilliseconds = settleMilliseconds,
        };
    }

    public static DiffCommand ParseDiff(string[] args)
    {
        var parsed = Parse(args, DiffOptions, ["--allow-incomplete"]);
        if (parsed.Positionals.Count != 2)
        {
            throw new ArgumentException("The diff command requires before and after snapshot paths.");
        }

        return new DiffCommand
        {
            BeforePath = parsed.Positionals[0],
            AfterPath = parsed.Positionals[1],
            Format = ParseEnum(parsed, "--format", ReportFormat.Console),
            OutputPath = parsed.Values.GetValueOrDefault("--output"),
            FailOn = ParseFailOn(parsed.Values.GetValueOrDefault("--fail-on")),
            AllowIncomplete = parsed.Flags.Contains("--allow-incomplete"),
        };
    }

    public static int ParseProcessId(string value)
    {
        return ParsePositiveInt(value, "--pid");
    }

    public static nint ParseWindowHandle(string value)
    {
        var style = NumberStyles.Integer;
        if (value.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
        {
            value = value[2..];
            style = NumberStyles.HexNumber;
        }

        if (!long.TryParse(value, style, CultureInfo.InvariantCulture, out var handle) || handle <= 0)
        {
            throw new ArgumentException("--hwnd must be a positive decimal or hexadecimal window handle.");
        }

        return (nint)handle;
    }

    private static ParsedArguments Parse(
        IReadOnlyList<string> args,
        HashSet<string> allowedOptions,
        HashSet<string> flagOptions)
    {
        var result = new ParsedArguments();
        for (var index = 0; index < args.Count; index++)
        {
            var value = args[index];
            if (!value.StartsWith("--", StringComparison.Ordinal))
            {
                result.Positionals.Add(value);
                continue;
            }

            if (!allowedOptions.Contains(value))
            {
                throw new ArgumentException($"Unknown option '{value}'.");
            }

            if (flagOptions.Contains(value))
            {
                if (!result.Flags.Add(value))
                {
                    throw new ArgumentException($"Option '{value}' was specified more than once.");
                }

                continue;
            }

            if (result.Values.ContainsKey(value))
            {
                throw new ArgumentException($"Option '{value}' was specified more than once.");
            }

            if (index + 1 >= args.Count || args[index + 1].StartsWith("--", StringComparison.Ordinal))
            {
                throw new ArgumentException($"Option '{value}' requires a value.");
            }

            result.Values[value] = args[++index];
        }

        return result;
    }

    private static string GetRequired(ParsedArguments parsed, string option)
    {
        if (!parsed.Values.TryGetValue(option, out var value) || string.IsNullOrWhiteSpace(value))
        {
            throw new ArgumentException($"Option '{option}' is required.");
        }

        return value;
    }

    private static int ParsePositiveInt(
        ParsedArguments parsed,
        string option,
        int defaultValue,
        int? maximumValue = null)
    {
        var parsedValue = parsed.Values.TryGetValue(option, out var value)
            ? ParsePositiveInt(value, option)
            : defaultValue;

        if (maximumValue is not null && parsedValue > maximumValue)
        {
            throw new ArgumentException($"{option} cannot exceed {maximumValue}.");
        }

        return parsedValue;
    }

    private static int ParsePositiveInt(string value, string option)
    {
        if (!int.TryParse(value, NumberStyles.None, CultureInfo.InvariantCulture, out var parsed) || parsed <= 0)
        {
            throw new ArgumentException($"{option} must be a positive integer.");
        }

        return parsed;
    }

    private static int ParseNonNegativeInt(ParsedArguments parsed, string option, int defaultValue)
    {
        if (!parsed.Values.TryGetValue(option, out var value))
        {
            return defaultValue;
        }

        if (!int.TryParse(value, NumberStyles.None, CultureInfo.InvariantCulture, out var parsedValue) ||
            parsedValue < 0)
        {
            throw new ArgumentException($"{option} must be a non-negative integer.");
        }

        return parsedValue;
    }

    private static T ParseEnum<T>(ParsedArguments parsed, string option, T defaultValue)
        where T : struct, Enum
    {
        if (!parsed.Values.TryGetValue(option, out var value))
        {
            return defaultValue;
        }

        var matchingName = Enum.GetNames<T>()
            .SingleOrDefault(name => string.Equals(name, value, StringComparison.OrdinalIgnoreCase));
        if (matchingName is null)
        {
            throw new ArgumentException(
                $"{option} must be one of: {string.Join(", ", Enum.GetNames<T>().Select(name => ToKebabCase(name)))}.");
        }

        return Enum.Parse<T>(matchingName);
    }

    private static FailOn ParseFailOn(string? value)
    {
        return value?.ToLowerInvariant() switch
        {
            null => FailOn.Any,
            "none" => FailOn.None,
            "potential-regression" => FailOn.PotentialRegression,
            "any" => FailOn.Any,
            _ => throw new ArgumentException("--fail-on must be one of: none, potential-regression, any."),
        };
    }

    private static string ToKebabCase(string value)
    {
        return string.Concat(
            value.Select((character, index) =>
                char.IsUpper(character) && index > 0
                    ? $"-{char.ToLowerInvariant(character)}"
                    : char.ToLowerInvariant(character).ToString()));
    }

    private sealed class ParsedArguments
    {
        public Dictionary<string, string> Values { get; } = new(StringComparer.Ordinal);

        public HashSet<string> Flags { get; } = new(StringComparer.Ordinal);

        public List<string> Positionals { get; } = [];
    }
}
