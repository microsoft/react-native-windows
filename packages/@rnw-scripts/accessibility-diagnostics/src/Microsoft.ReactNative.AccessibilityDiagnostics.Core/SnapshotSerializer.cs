// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Core;

public static class SnapshotSerializer
{
    private static readonly UTF8Encoding Utf8WithoutBom = new(false);

    public static JsonSerializerOptions Options { get; } = CreateOptions();

    public static string Serialize<T>(T value)
    {
        try
        {
            return JsonSerializer.Serialize(value, Options).Replace("\r\n", "\n", StringComparison.Ordinal);
        }
        catch (JsonException exception)
        {
            throw new SnapshotFormatException("The accessibility data could not be serialized.", exception);
        }
    }

    public static AccessibilitySnapshot DeserializeSnapshot(string json)
    {
        AccessibilitySnapshot snapshot;
        try
        {
            snapshot = JsonSerializer.Deserialize<AccessibilitySnapshot>(json, Options)
                ?? throw new SnapshotFormatException("The snapshot file is empty.");
        }
        catch (JsonException exception)
        {
            throw new SnapshotFormatException("The snapshot is not valid JSON.", exception);
        }

        if (snapshot.SchemaVersion != AccessibilitySnapshot.CurrentSchemaVersion)
        {
            throw new SnapshotFormatException(
                $"Unsupported snapshot schema version {snapshot.SchemaVersion}. " +
                $"Expected {AccessibilitySnapshot.CurrentSchemaVersion}.");
        }

        if (!Enum.IsDefined(snapshot.CaptureStatus))
        {
            throw new SnapshotFormatException($"Unknown capture status value {(int)snapshot.CaptureStatus}.");
        }

        Validate(snapshot);
        return snapshot;
    }

    public static async Task WriteAsync<T>(string path, T value, CancellationToken cancellationToken = default)
    {
        var directory = Path.GetDirectoryName(Path.GetFullPath(path));
        if (!string.IsNullOrEmpty(directory))
        {
            Directory.CreateDirectory(directory);
        }

        await File.WriteAllTextAsync(path, Serialize(value) + "\n", Utf8WithoutBom, cancellationToken);
    }

    public static async Task<AccessibilitySnapshot> ReadSnapshotAsync(
        string path,
        CancellationToken cancellationToken = default)
    {
        var json = await File.ReadAllTextAsync(path, Utf8WithoutBom, cancellationToken);
        return DeserializeSnapshot(json);
    }

    private static JsonSerializerOptions CreateOptions()
    {
        var options = new JsonSerializerOptions
        {
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
            MaxDepth = 512,
            WriteIndented = true,
        };
        options.Converters.Add(new JsonStringEnumConverter(JsonNamingPolicy.CamelCase, allowIntegerValues: false));
        return options;
    }

    private static void Validate(AccessibilitySnapshot snapshot)
    {
        if (snapshot.Target is null)
        {
            throw new SnapshotFormatException("The snapshot target is missing.");
        }

        if (snapshot.Options is null)
        {
            throw new SnapshotFormatException("The snapshot options are missing.");
        }

        if (snapshot.Warnings is null)
        {
            throw new SnapshotFormatException("The snapshot warning collection is missing.");
        }

        if (snapshot.Options.MaxDepth <= 0 || snapshot.Options.MaxNodes <= 0)
        {
            throw new SnapshotFormatException("Snapshot capture limits must be positive.");
        }

        if (snapshot.Root is null)
        {
            throw new SnapshotFormatException("The snapshot root element is missing.");
        }

        ValidateNode(snapshot.Root, "$");
    }

    private static void ValidateNode(AccessibilityNode node, string path)
    {
        if (string.IsNullOrWhiteSpace(node.ControlType))
        {
            throw new SnapshotFormatException($"The element at {path} has no control type.");
        }

        if (node.Children is null)
        {
            throw new SnapshotFormatException($"The element at {path} has no child collection.");
        }

        for (var index = 0; index < node.Children.Count; index++)
        {
            var child = node.Children[index];
            if (child is null)
            {
                throw new SnapshotFormatException($"The element at {path} has a null child at index {index}.");
            }

            ValidateNode(child, $"{path}/children[{index}]");
        }
    }
}
