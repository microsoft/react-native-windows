// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

namespace Microsoft.ReactNative.AccessibilityDiagnostics.Core.Tests;

[TestClass]
public sealed class SnapshotDifferTests
{
    [TestMethod]
    public async Task DetectsAccessibleNameAndFocusabilityRegressions()
    {
        var before = await ReadFixtureAsync("before.json");
        var after = await ReadFixtureAsync("after.json");

        var diff = new SnapshotDiffer().Compare(before, after, "before.json", "after.json");

        Assert.AreEqual(2, diff.Changes.Count);
        Assert.IsTrue(diff.Changes.All(change => change.Impact == ChangeImpact.PotentialRegression));
        Assert.IsTrue(diff.Changes.Any(change => change.Property == "name"));
        Assert.IsTrue(diff.Changes.Any(change => change.Property == "isKeyboardFocusable"));
        Assert.IsTrue(SnapshotDiffer.ShouldFail(diff, FailOn.PotentialRegression));
    }

    [TestMethod]
    public void InsertedSiblingDoesNotCascadeIntoUnrelatedChanges()
    {
        var before = CreateSnapshot(
            Node("Window", children: [Node("Button", "First", "first"), Node("Button", "Second", "second")]));
        var after = CreateSnapshot(
            Node(
                "Window",
                children:
                [
                    Node("Text", "New"),
                    Node("Button", "First", "first"),
                    Node("Button", "Second", "second"),
                ]));

        var diff = new SnapshotDiffer().Compare(before, after, "before", "after");

        Assert.AreEqual(1, diff.Changes.Count);
        Assert.AreEqual(ChangeKind.Added, diff.Changes[0].Kind);
        Assert.AreEqual(ChangeImpact.Informational, diff.Changes[0].Impact);
    }

    [TestMethod]
    public void ReorderedAutomationIdsProduceOneOrderChange()
    {
        var before = CreateSnapshot(
            Node("Window", children: [Node("Button", "First", "first"), Node("Button", "Second", "second")]));
        var after = CreateSnapshot(
            Node("Window", children: [Node("Button", "Second", "second"), Node("Button", "First", "first")]));

        var diff = new SnapshotDiffer().Compare(before, after, "before", "after");

        Assert.AreEqual(1, diff.Changes.Count);
        Assert.AreEqual("childrenOrder", diff.Changes[0].Property);
        Assert.AreEqual(ChangeImpact.PotentialRegression, diff.Changes[0].Impact);
    }

    [TestMethod]
    public void RenamedFocusableNodeIsMatchedByWeakIdentity()
    {
        var before = CreateSnapshot(Node("Window", children: [Node("Button", "Save", focusable: true)]));
        var after = CreateSnapshot(Node("Window", children: [Node("Button", "Submit", focusable: true)]));

        var diff = new SnapshotDiffer().Compare(before, after, "before", "after");

        Assert.AreEqual(1, diff.Changes.Count);
        Assert.AreEqual("name", diff.Changes[0].Property);
        Assert.AreEqual(ChangeImpact.Informational, diff.Changes[0].Impact);
    }

    [TestMethod]
    public void IncompleteSnapshotsAreRejectedByDefault()
    {
        var before = CreateSnapshot(
            Node("Window"),
            CaptureStatus.Incomplete,
            ["Node limit reached."]);
        var after = CreateSnapshot(Node("Window"));

        Assert.ThrowsException<SnapshotFormatException>(
            () => new SnapshotDiffer().Compare(before, after, "before", "after"));

        var diff = new SnapshotDiffer().Compare(before, after, "before", "after", allowIncomplete: true);
        CollectionAssert.Contains(diff.Warnings, "Before: Node limit reached.");
    }

    [TestMethod]
    public void RemovingContainerWithInteractiveDescendantIsPotentialRegression()
    {
        var before = CreateSnapshot(
            Node(
                "Window",
                children:
                [
                    Node(
                        "Group",
                        children: [Node("Button", "Save", focusable: true)]),
                ]));
        var after = CreateSnapshot(Node("Window"));

        var diff = new SnapshotDiffer().Compare(before, after, "before", "after");

        Assert.AreEqual(1, diff.Changes.Count);
        Assert.AreEqual(ChangeKind.Removed, diff.Changes[0].Kind);
        Assert.AreEqual(ChangeImpact.PotentialRegression, diff.Changes[0].Impact);
    }

    [TestMethod]
    public void WideSiblingSetsUseBoundedMatching()
    {
        const int siblingCount = 1_500;
        var beforeChildren = Enumerable.Range(0, siblingCount)
            .Select(_ => Node("Text", "Repeated"))
            .ToList();
        var afterChildren = Enumerable.Range(0, siblingCount)
            .Select(_ => Node("Text", "Repeated"))
            .ToList();

        var diff = new SnapshotDiffer().Compare(
            CreateSnapshot(Node("Window", children: beforeChildren)),
            CreateSnapshot(Node("Window", children: afterChildren)),
            "before",
            "after");

        Assert.AreEqual(0, diff.Changes.Count);
    }

    [TestMethod]
    public void MarkdownReportIncludesPotentialRegressionCount()
    {
        var before = CreateSnapshot(Node("Window", children: [Node("Button", "Save", "save", focusable: true)]));
        var after = CreateSnapshot(Node("Window", children: [Node("Button", null, "save", focusable: true)]));
        var diff = new SnapshotDiffer().Compare(before, after, "before.json", "after.json");

        var report = DiffReporters.RenderMarkdown(diff);

        StringAssert.Contains(report, "Potential regressions: 1");
        StringAssert.Contains(report, "name changed");
    }

    [TestMethod]
    public void ConsoleReportEscapesControlCharacters()
    {
        var before = CreateSnapshot(Node("Window", children: [Node("Button", "Save", "save")]));
        var after = CreateSnapshot(Node("Window", children: [Node("Button", "\u001b[31mSave", "save")]));
        var diff = new SnapshotDiffer().Compare(before, after, "before", "after");

        var report = DiffReporters.RenderConsole(diff);

        Assert.IsFalse(report.Contains('\u001b'));
        StringAssert.Contains(report, "\\u001B");
    }

    [TestMethod]
    public void OutputSanitizerEscapesControlCharacters()
    {
        Assert.AreEqual("title\\u001B[31m", OutputSanitizer.Sanitize("title\u001b[31m"));
    }

    private static async Task<AccessibilitySnapshot> ReadFixtureAsync(string name)
    {
        return await SnapshotSerializer.ReadSnapshotAsync(Path.Combine(AppContext.BaseDirectory, "Fixtures", name));
    }

    private static AccessibilitySnapshot CreateSnapshot(
        AccessibilityNode root,
        CaptureStatus captureStatus = CaptureStatus.Complete,
        List<string>? warnings = null)
    {
        return new AccessibilitySnapshot
        {
            CaptureStatus = captureStatus,
            Warnings = warnings ?? [],
            CapturedAtUtc = new DateTimeOffset(2026, 9, 14, 12, 0, 0, TimeSpan.Zero),
            Target = new SnapshotTarget
            {
                WindowProcessId = 100,
                NativeWindowHandle = 1234,
                WindowTitle = "Test",
            },
            Options = new SnapshotOptions
            {
                MaxDepth = 100,
                MaxNodes = 10_000,
            },
            Root = root,
        };
    }

    private static AccessibilityNode Node(
        string controlType,
        string? name = null,
        string? automationId = null,
        bool focusable = false,
        List<AccessibilityNode>? children = null)
    {
        return new AccessibilityNode
        {
            AutomationId = automationId,
            Name = name,
            ControlType = controlType,
            ClassName = controlType,
            IsEnabled = true,
            IsKeyboardFocusable = focusable,
            Children = children ?? [],
        };
    }
}
