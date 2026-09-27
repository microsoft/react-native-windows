# RNW Accessibility Diagnostics

`rnw-a11y` captures the Windows UI Automation tree exposed by a running application and compares two captures. It
helps React Native Windows engineers detect accessibility changes that are invisible in screenshots, including lost
accessible names, control type changes, lost keyboard focusability, removed interactive controls, and tree-order
changes.

The tool is read-only. It does not invoke controls, change application state, or add code to the inspected
application.

## Prerequisites

- Windows 10 version 22H2 or newer
- .NET SDK 10
- A running desktop application to inspect

## Build and test

From the repository root:

```powershell
yarn workspace @rnw-scripts/accessibility-diagnostics build
yarn workspace @rnw-scripts/accessibility-diagnostics test
```

## Capture a snapshot

Select exactly one target using a process ID, process name, native window handle, or exact window title:

```powershell
yarn workspace @rnw-scripts/accessibility-diagnostics rnw-a11y capture `
  --process RNTesterApp-Fabric `
  --output before.json
```

```powershell
yarn workspace @rnw-scripts/accessibility-diagnostics rnw-a11y capture `
  --hwnd 0x000A04BC `
  --output after.json
```

Process-name selection supports hosted or packaged applications by finding the top-level window containing content
from the requested process. If multiple windows match, use `--hwnd` to make the target explicit.

Capture limits bound the generated snapshot and recursive processing:

```powershell
rnw-a11y capture --process MyApp --output app.json --max-depth 50 --max-nodes 5000
```

The defaults are a maximum depth of 100, a maximum of 10,000 nodes, and a 100 millisecond settling delay. The maximum
supported depth is 200. A capture
that reaches a limit or loses an element while reading is written with `captureStatus: "incomplete"` and returns exit
code 4.

## Compare snapshots

Print differences to the console:

```powershell
yarn workspace @rnw-scripts/accessibility-diagnostics rnw-a11y diff before.json after.json
```

Create a Markdown report:

```powershell
yarn workspace @rnw-scripts/accessibility-diagnostics rnw-a11y diff before.json after.json `
  --format markdown `
  --output accessibility-diff.md `
  --fail-on potential-regression
```

Available formats are `console`, `json`, and `markdown`.

`--fail-on` controls when differences return exit code 1:

| Value | Behavior |
| --- | --- |
| `none` | Always return success after a valid comparison. |
| `potential-regression` | Fail only for changes classified as potential accessibility regressions. |
| `any` | Fail for any difference. This is the default. |

Incomplete snapshots are rejected by default because they can produce misleading results. Use `--allow-incomplete`
only when intentionally investigating a partial capture.

## Snapshot contents

Snapshots use a versioned JSON schema and capture the Control View of the UI Automation tree. Each element can
include:

- accessible name and Automation ID;
- control type, localized type, class name, and framework ID;
- enabled, keyboard-focusable, and offscreen states;
- help text and access key;
- selection, toggle, and expand/collapse states;
- ordered child elements.

Runtime IDs, bounding rectangles, keyboard focus, and text-field values are excluded from comparisons because they
are volatile or may contain sensitive information.

## Matching and classification

Elements are matched only within an already matched parent. The matcher uses unique Automation IDs first, then stable
element characteristics and sequence alignment. This prevents one inserted element from turning the rest of the
subtree into unrelated changes.

The report keeps structural changes separate from risk classification. Potential regressions currently include:

- changing an element's control type;
- removing a keyboard-focusable or interactive element;
- removing the accessible name from a focusable element;
- making a previously focusable element unfocusable;
- changing the relative order of existing sibling elements.

These findings require engineering review. A clean diff does not prove that an application is fully accessible, and
the tool does not replace keyboard or screen-reader testing.

## Privacy

Snapshots remain on the local machine unless the engineer explicitly shares or checks them in. Accessible names,
help text, and item labels can contain application or user-provided content. Review snapshots before attaching them
to bugs or pull requests.

The tool intentionally does not read `ValuePattern` or text document contents.

## Future focus-path recorder

The snapshot model, target selection, cached UI Automation reader, serialization, and reporting layers are designed
to support a future focus-path recorder. That extension can subscribe to UI Automation focus events and describe
each focused element using the same property reader without changing the target application.
