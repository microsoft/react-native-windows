// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Core.Tests;

[TestClass]
public sealed class SnapshotSerializerTests
{
    [TestMethod]
    public async Task FixtureRoundTripsDeterministically()
    {
        var path = GetFixturePath("before.json");
        var snapshot = await SnapshotSerializer.ReadSnapshotAsync(path);

        var first = SnapshotSerializer.Serialize(snapshot);
        var second = SnapshotSerializer.Serialize(SnapshotSerializer.DeserializeSnapshot(first));

        Assert.AreEqual(first, second);
        Assert.IsFalse(first.Contains("\r\n", StringComparison.Ordinal));
    }

    [TestMethod]
    public void UnsupportedSchemaVersionIsRejected()
    {
        const string json =
            """
            {
              "schemaVersion": 999,
              "captureStatus": "complete",
              "capturedAtUtc": "2026-09-14T12:00:00+00:00",
              "target": {},
              "options": {},
              "root": { "controlType": "Window" }
            }
            """;

        var exception = Assert.ThrowsException<SnapshotFormatException>(
            () => SnapshotSerializer.DeserializeSnapshot(json));

        StringAssert.Contains(exception.Message, "Unsupported snapshot schema version 999");
    }

    [TestMethod]
    public void NullRootIsRejected()
    {
        const string json =
            """
            {
              "schemaVersion": 1,
              "captureStatus": "complete",
              "warnings": [],
              "capturedAtUtc": "2026-09-14T12:00:00+00:00",
              "target": {},
              "options": { "maxDepth": 100, "maxNodes": 10000 },
              "root": null
            }
            """;

        var exception = Assert.ThrowsException<SnapshotFormatException>(
            () => SnapshotSerializer.DeserializeSnapshot(json));

        StringAssert.Contains(exception.Message, "root element is missing");
    }

    [TestMethod]
    public void NumericCaptureStatusIsRejected()
    {
        const string json =
            """
            {
              "schemaVersion": 1,
              "captureStatus": 999,
              "warnings": [],
              "capturedAtUtc": "2026-09-14T12:00:00+00:00",
              "target": {},
              "options": { "maxDepth": 100, "maxNodes": 10000 },
              "root": { "controlType": "Window", "children": [] }
            }
            """;

        Assert.ThrowsException<SnapshotFormatException>(
            () => SnapshotSerializer.DeserializeSnapshot(json));
    }

    [TestMethod]
    public void DefaultCaptureDepthCanBeSerialized()
    {
        var root = new AccessibilityNode
        {
            ControlType = "Text",
        };
        var current = root;
        for (var depth = 1; depth <= 100; depth++)
        {
            var child = new AccessibilityNode
            {
                ControlType = "Text",
            };
            current.Children.Add(child);
            current = child;
        }

        var snapshot = new AccessibilitySnapshot
        {
            CapturedAtUtc = new DateTimeOffset(2026, 9, 14, 12, 0, 0, TimeSpan.Zero),
            Target = new SnapshotTarget(),
            Options = new SnapshotOptions
            {
                MaxDepth = 100,
                MaxNodes = 10_000,
            },
            Root = root,
        };

        var json = SnapshotSerializer.Serialize(snapshot);
        var roundTripped = SnapshotSerializer.DeserializeSnapshot(json);

        Assert.AreEqual("Text", roundTripped.Root.ControlType);
    }

    private static string GetFixturePath(string name)
    {
        return Path.Combine(AppContext.BaseDirectory, "Fixtures", name);
    }
}
