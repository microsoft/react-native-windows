# RNW Conflict Resolution Patterns

Patterns mined from historical integration commits for resolving merge conflicts in React Native Windows.

## Pattern Categories

### P1: Keep Both — Upstream + Windows Props/Lines

**Signals:** Upstream adds a new prop/line, RNW has Windows-specific props adjacent. Both sides add non-overlapping content.

**Resolution:** Keep BOTH the upstream line AND the RNW Windows-specific lines. Remove conflict markers only.

**Example** (commit bb50808b9f — Image.windows.js):
```
BEFORE (conflict):
  'aria-selected': ariaSelected,
<<<<<<< Upstream
  'aria-hidden': ariaHidden,
=======
  'aria-readonly': ariaReadOnly, // Windows
  'aria-multiselectable': ariaMultiselectable, // Windows
  ...
  height,
>>>>>>> Override

AFTER (resolved):
  'aria-selected': ariaSelected,
  'aria-hidden': ariaHidden,
  'aria-readonly': ariaReadOnly, // Windows
  'aria-multiselectable': ariaMultiselectable, // Windows
  ...
  height,
```

**Example** (commit bb50808b9f — Text.windows.js):
```
BEFORE:
<<<<<<< Upstream
  importantForAccessibility: _importantForAccessibility,
=======
  // #[Windows
  accessibilityLevel: _accessibilityLevel,
  ...
  // #]Windows
>>>>>>> Override

AFTER:
  importantForAccessibility: _importantForAccessibility,
  // #[Windows
  accessibilityLevel: _accessibilityLevel,
  ...
  // #]Windows
```

---

### P2: Keep Only RNW (OURS) — Platform-Specific Logic

**Signals:** Upstream has platform-specific code (`spec.android`), RNW replaces with Windows equivalent (`spec.windows`). Upstream changed formatting/comments only.

**Resolution:** Keep the RNW (Override) side. Apply upstream's comment/formatting fixes to the RNW version.

**Example** (commit 3e3b133fcb — Platform.windows.js):
```
BEFORE:
<<<<<<< Upstream
  'android' in spec
    ? // $FlowFixMe[incompatible-type]
      spec.android
=======
  'windows' in spec
    ? // $FlowFixMe[incompatible-return]
      spec.windows
>>>>>>> Override

AFTER:
  'windows' in spec
    ? // $FlowFixMe[incompatible-type]    ← took upstream's comment fix
      spec.windows                         ← kept Windows platform
```

---

### P3: Drop RNW Side — Upstream Supersedes

**Signals:** Upstream added functionality that makes the RNW-specific version redundant, or RNW side was a workaround that's no longer needed.

**Resolution:** Keep the upstream (Upstream) side, remove the RNW override.

**Example** (commit 3e3b133fcb — TextInput.windows.js):
```
BEFORE:
<<<<<<< Upstream
  accessibilityLabel={_accessibilityLabel}
=======
  accessibilityErrorMessage={accessibilityErrorMessage}
>>>>>>> Override

AFTER:
  accessibilityLabel={_accessibilityLabel}
```
Here the RNW-only prop `accessibilityErrorMessage` was replaced with the standard upstream `accessibilityLabel`.

---

### P4: Keep Only RNW (OURS) — Large Block Override

**Signals:** Upstream has a complete component render block, RNW has a completely different render that uses `Platform.OS === 'windows'` branching. The upstream block is for Android/iOS only.

**Resolution:** Delete the upstream block, keep the RNW `// [Windows` ... `// Windows]` block. Apply any upstream comment/annotation fixes.

**Example** (commit 3e3b133fcb — Button.windows.js):
```
BEFORE:
<<<<<<< Upstream
  return (
    <NativeTouchable ...>        ← Android/iOS render
      ...
    </NativeTouchable>
  );
=======
  // [Windows
  if (Platform.OS === 'windows') {  ← Windows-specific render
    ...
  }
  // Windows]
>>>>>>> Override

AFTER:
  // [Windows
  if (Platform.OS === 'windows') {
    ...                             ← kept, with upstream annotation fixes applied
  }
  // Windows]
```

---

### P5: Merge Union Types — Add Upstream Entry to RNW List

**Signals:** Upstream adds a new entry to a union type or enum. RNW has its own entry. Both are valid.

**Resolution:** Keep both entries. May need to adjust syntax (e.g., add `|` for union continuation).

**Example** (commit ceeaddc0d1 — ViewAccessibility.windows.js):
```
BEFORE:
  | 'iconmenu'
<<<<<<< Upstream
  | string;
=======
  | 'listitem'; // RNW-only
>>>>>>> Override

AFTER:
  | 'iconmenu'
  | 'listitem' // RNW-only
  | string;
```

---

### P6: Merge Object Properties — Keep Both Sides

**Signals:** Upstream adds new properties to a type/interface, RNW adds different properties. Both are valid additions.

**Resolution:** Keep both sets of properties, deduplicate, add proper separating comments.

**Example** (commit 9f26c15efa — ViewPropTypes.win32.js):
```
BEFORE:
<<<<<<< Upstream
  experimental_accessibilityOrder?: ?Array<string>,
=======
  accessibilityLabeledBy?: ?string,  // Win32-only
  accessibilityControls?: ?string,   // Win32-only
>>>>>>> Override

AFTER:
  experimental_accessibilityOrder?: ?Array<string>,

  /** @platform win32 */
  accessibilityLabeledBy?: ?string,
  /** @platform win32 */
  accessibilityControls?: ?string,
```

---

### P7: Package.json — Always Remove Conflict Markers

**Signals:** package.json files with version conflicts from nightly version changes.

**Resolution:** Remove all conflict markers, use the target nightly version. These should already be handled by Steps 6-7.

---

## Safety Rules

### Intentional Markers — DO NOT RESOLVE
- Files managed by `react-native-platform-override` E2E may contain `>>>>>>>` markers by design
- `flowconfig.windows.conflict` files — leave all conflict markers in place

### File-Type Rules
| File type | Default bias |
|-----------|-------------|
| `.windows.js` in `vnext/src-win/` | Bias OURS (Windows override) |
| `.win32.js` in `packages/@office-iss/` | Bias OURS (Win32 override) |
| `packages/@react-native/tester/` | Bias THEIRS (should match upstream) |
| `vnext/ReactCopies/` | Bias THEIRS (exact copies of upstream) |
| `.cpp`, `.h` in `vnext/Microsoft.ReactNative/` | Bias OURS unless shared code |
| `package.json` | Use target nightly version |

### Marker Format
The override tool uses these marker labels:
- `<<<<<<< Upstream` — upstream React Native code
- `=======`
- `>>>>>>> Override` — RNW fork code

### Common Anti-Patterns to Avoid
- Dropping `// [Windows` sections when taking upstream
- Losing Windows-specific props when upstream adds new ones
- Taking upstream platform-specific code (`spec.android`) instead of keeping Windows equivalent
- Removing `$FlowFixMe` annotations that upstream updated
