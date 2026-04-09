---
name: rnw-integration
description: 'Automate React Native Windows integration with upstream React Native nightly versions. Use when: upgrading RNW to a newer React Native nightly, finding target nightly versions, preparing integration PRs, updating package dependencies.'
argument-hint: 'Describe the integration task (e.g., "find next nightly version")'
---

# RNW Integration Automation

Automates the process of integrating React Native Windows with upstream React Native nightly builds.

## When to Use

- Finding the target React Native nightly version for integration
- Upgrading RNW dependencies to a new React Native nightly
- Preparing integration pull requests
- Validating nightly version availability on npm

## Procedure

### Step 1: Get Current React Native Version

Extract the current `react-native` version from `vnext/package.json`:

```powershell
$packageJson = Get-Content -Path "vnext/package.json" | ConvertFrom-Json
$currentVersion = $packageJson.devDependencies.'react-native'
Write-Host "Current version: $currentVersion"
```

Expected format: `0.XX.0-nightly-YYYYMMDD-<commit-hash>`

### Step 2: Parse the Current Version Date

Extract the date from the nightly version string:

```powershell
# Parse version like: 0.84.0-nightly-20260107-58bc6c3e3
$versionMatch = $currentVersion -match 'nightly-(\d{8})'
$currentDate = [DateTime]::ParseExact($matches[1], 'yyyyMMdd', $null)
Write-Host "Current nightly date: $($currentDate.ToString('yyyy-MM-dd'))"
```

### Step 3: Calculate Target Date (+7 days)

```powershell
$targetDate = $currentDate.AddDays(7)
$targetDateStr = $targetDate.ToString('yyyyMMdd')
Write-Host "Target date (+7 days): $($targetDate.ToString('yyyy-MM-dd'))"
```

### Step 4: Find Nightly Version on npm

Query npm for available nightly versions around the target date:

```powershell
# Get all nightly versions for the target month
$yearMonth = $targetDate.ToString('yyyyMM')
$nightlyVersions = npm view react-native versions --json 2>$null | 
    ConvertFrom-Json | 
    Where-Object { $_ -like "*nightly-$yearMonth*" }

# Find exact match or next available
$targetVersion = $nightlyVersions | Where-Object { $_ -like "*nightly-$targetDateStr*" } | Select-Object -First 1

if (-not $targetVersion) {
    Write-Host "No nightly for $targetDateStr, finding next available..."
    $targetVersion = $nightlyVersions | Where-Object { 
        $_ -match 'nightly-(\d{8})' -and [int]$matches[1] -gt [int]$targetDateStr 
    } | Select-Object -First 1
}

Write-Host "Target nightly version: $targetVersion"
```

### Step 5: Output Summary

```powershell
Write-Host ""
Write-Host "=== Integration Summary ==="
Write-Host "Current Version: $currentVersion"
Write-Host "Current Date:    $($currentDate.ToString('yyyy-MM-dd'))"
Write-Host "Target Date:     $($targetDate.ToString('yyyy-MM-dd'))"
Write-Host "Target Version:  $targetVersion"
```

### Step 6: Run Integration Script

Run the integration script with the target nightly version to update all dependencies:

```powershell
yarn integrate-rn $targetVersion
```

This command updates all `react-native` and `@react-native/*` package references across the monorepo to the target nightly version.

### Step 7: Search and Update Remaining package.json Files

After running the integration script, search all `package.json` files for any remaining references to the previous nightly version and update them:

```powershell
# Find all package.json files containing the previous nightly version
$packageFiles = Get-ChildItem -Path . -Filter "package.json" -Recurse -File | 
    Where-Object { $_.FullName -notlike "*node_modules*" }

$updatedFiles = @()

foreach ($file in $packageFiles) {
    $content = Get-Content -Path $file.FullName -Raw
    if ($content -like "*$currentVersion*") {
        Write-Host "Found previous version in: $($file.FullName)"
        $newContent = $content -replace [regex]::Escape($currentVersion), $targetVersion
        Set-Content -Path $file.FullName -Value $newContent -NoNewline
        $updatedFiles += $file.FullName
    }
}

Write-Host ""
Write-Host "=== Updated $($updatedFiles.Count) package.json files ==="
$updatedFiles | ForEach-Object { Write-Host "  - $_" }
```

This ensures all references to the previous nightly version are updated, including:
- `dependencies`
- `devDependencies`
- `peerDependencies`
- `resolutions`

### Step 8: Upgrade Platform Overrides

Run the override upgrade tool to resolve conflicts in platform-specific override files:

```powershell
npx react-native-platform-override upgrade
```

This interactive tool helps resolve conflicts between upstream React Native changes and RNW platform overrides. It will:
- Show files with merge conflicts
- Allow you to accept upstream changes, keep local changes, or manually merge
- Update the overrides.json manifest files

After completing the upgrade, verify the overrides are valid:

```powershell
yarn validate-overrides
```

### Step 9: Commit All Changes

After completing all integration steps, commit the changes:

```powershell
git add -A
git commit -m "Integrate RN $targetVersion"
```

This creates a single commit with all the integration changes including:
- Updated package.json dependencies
- Override file upgrades
- Any auto-fixed lint issues

### Step 10: Update Failing Overrides from Upstream

Run `yarn validate-overrides` to identify which folders need to be replaced with upstream versions, then download and replace them:

```powershell
# Extract commit hash from target version (last 8 characters after final hyphen)
# Example: 0.85.0-nightly-20260114-f15985f4f -> f15985f4f
$commitHash = ($targetVersion -split '-')[-1]
Write-Host "Commit hash: $commitHash"

# Run validate-overrides and capture failing paths
$validateOutput = yarn validate-overrides 2>&1
$failingPaths = $validateOutput | Where-Object { $_ -match '^ - ' } | ForEach-Object { $_.Trim(' -') }

if ($failingPaths.Count -eq 0) {
    Write-Host "No failing overrides to update"
    return
}

Write-Host "Found $($failingPaths.Count) failing override paths to update"

# Create temp directory (outside git tracking)
$tempDir = "$env:TEMP\rn-upstream-$commitHash"
if (Test-Path $tempDir) { Remove-Item -Recurse -Force $tempDir }
New-Item -ItemType Directory -Path $tempDir | Out-Null

# Download react-native repo at the specific commit
Write-Host "Downloading react-native at commit $commitHash..."
git clone --depth 1 --filter=blob:none --sparse https://github.com/facebook/react-native.git $tempDir
Push-Location $tempDir
git sparse-checkout set packages/rn-tester
git fetch --unshallow
git checkout $commitHash
Pop-Location

# Replace each failing path with upstream version
foreach ($path in $failingPaths) {
    # Convert absolute path to relative
    $relativePath = $path -replace [regex]::Escape((Get-Location).Path + '\'), ''
    
    # Map RNW paths to upstream rn-tester paths
    $upstreamPath = $null
    
    if ($relativePath -like "packages\@react-native\tester\*") {
        # packages\@react-native\tester\X -> packages\rn-tester\X
        $subPath = $relativePath -replace '^packages\\@react-native\\tester\\', ''
        $upstreamPath = "$tempDir\packages\rn-tester\$subPath"
    }
    elseif ($relativePath -like "vnext\ReactCopies\*") {
        # vnext\ReactCopies\X -> packages\rn-tester\X
        $subPath = $relativePath -replace '^vnext\\ReactCopies\\', ''
        $upstreamPath = "$tempDir\packages\rn-tester\$subPath"
    }
    
    if ($upstreamPath -and (Test-Path $upstreamPath)) {
        Write-Host "Replacing $relativePath with upstream version..."
        Remove-Item -Recurse -Force $relativePath
        Copy-Item -Recurse -Force $upstreamPath $relativePath
    } else {
        Write-Host "WARNING: Could not find upstream path for $relativePath"
    }
}

# Cleanup temp directory
Remove-Item -Recurse -Force $tempDir
Write-Host "Overrides updated from upstream commit $commitHash"
```

Verify the overrides are valid after the update:

```powershell
yarn validate-overrides
```

### Step 11: Resolve Merge Conflicts (Human-in-the-Loop)

After Steps 8-10, some override files may still contain merge conflict markers (`<<<<<<<`, `=======`, `>>>>>>>`). This step resolves them safely with human approval for each hunk.

**NON-NEGOTIABLE CONSTRAINTS:**
- Conflicts are handled hunk-by-hunk (each `<<<<<<<` ... `=======` ... `>>>>>>>` region is one hunk)
- HUMAN-IN-THE-LOOP: Do NOT apply any change automatically — ask for explicit approval for EACH hunk
- Safety exceptions:
  - Some `>>>>>>>` markers are intentional (e.g., E2E `react-native-platform-override` files) — do not "fix" those
  - Leave conflicts in `flowconfig.windows.conflict` untouched

#### Phase 1 — Build RNW Conflict-Resolution Knowledge Base

Before resolving any conflicts, mine historical "good" RNW-fork commits for conflict-resolution patterns. Create or update [references/conflict-patterns.md](./references/conflict-patterns.md).

Reference commits to study:
```
ceeaddc0d19161e4fa0e01139a721b758d0269ce
9f26c15efab7b51dd8544f50d2a26905bb875223
1d7f44c4ad58a3ffa506cdc618c92d0dcebaf238
e5d505e43d47a7cc603cd1be3d875dc0e3ab6ab9
3e3b133fcb32f39f485b075858eaa7ff55d0d912
9b40dac70b27ec2e5c3d0449bca50c841764e413
d73784caa0d7e91a6c20fa485cf9ccb7923a1d40
b6315a67451f9a2526ba1a5f3814d4ca6564342c
bb50808b9f7e63da8af5511bf4b2faf064add19f
```

For each commit:
- Identify files where merge conflicts were resolved
- Extract BEFORE/AFTER snippets for each resolved hunk (summarise if large)
- Categorise into reusable patterns (name + signals + anti-signals)
- Include example references (commit + file path + short diff summary)
- Include safety notes: intentional markers, flowconfig rule, E2E override markers

#### Phase 2 — Discover Conflicts and Split into Hunks

Scan the repository for conflict markers:

```powershell
# Find all files with conflict markers (excluding node_modules, intentional files)
$conflictFiles = Get-ChildItem -Path . -Recurse -File |
    Where-Object { $_.FullName -notlike "*node_modules*" -and $_.Name -ne "flowconfig.windows.conflict" } |
    ForEach-Object {
        $content = Get-Content -Path $_.FullName -Raw -ErrorAction SilentlyContinue
        if ($content -match '<<<<<<<') { $_.FullName }
    }

Write-Host "=== Files with conflict markers ==="
$conflictFiles | ForEach-Object { Write-Host "  - $_" }
Write-Host "Total: $($conflictFiles.Count) files"
```

Split each conflicted file into individual hunks for review.

#### Phase 3 — Propose Resolution (Pattern-First, No Auto-Apply)

**IMPORTANT: Guard against "previously-intentionally-skipped" upstream changes.**

In some prior RNW integrations, certain upstream changes were intentionally NOT taken for specific files (e.g., because RNW has a divergent Windows copy, platform overrides, or intentional conflict markers). In later integrations, those same upstream changes may reappear and must NOT be automatically accepted.

**Before resolving any conflict hunk**, you MUST first inspect the actual upstream change using the compare URL:

- **Compare URL format:** `https://github.com/facebook/react-native/compare/<previous-commit>...<target-commit>`
  - LEFT = previous nightly version commit hash (from `$currentVersion`)
  - RIGHT = target nightly version commit hash (from `$targetVersion`)
  - Example: `https://github.com/facebook/react-native/compare/58bc6c3e3...f15985f4f`

**Required steps for each hunk:**

1. **Inspect upstream changes** via the compare URL:
   - Identify the exact files changed upstream
   - Determine what lines/sections changed and why (intent)
2. **Compare against the RNW Windows-copy** of the file:
   - If the RNW file is a direct copy → upstream changes usually apply (unless conflict-patterns indicates otherwise)
   - If RNW contains Windows-only edits (often marked with `// [Windows`) or is substantially different → upstream changes may be intentionally NOT applicable
3. **Check references/conflict-patterns.md** for a matching "intentional skip / divergence" pattern and apply that guidance first
4. **Only THEN** propose a hunk resolution using the structured prompt system (merge prompt + file-type prompts + source-folder instructions) and run validation to output confidence (`Low`/`Medium`/`High`)
5. **If the upstream change was intentionally skipped in a previous integration**, do NOT accept it automatically even if it reappears now:
   - Prefer the documented RNW pattern ("skip/retain RNW version") and explain the rationale
   - Set confidence to `Low` or `Medium` as appropriate and request human approval

**Output requirement for each hunk proposal:**
- The compare URL used
- A 1-2 line summary of what upstream changed
- Why we should or should not take it in RNW
- Confidence level (`Low`/`Medium`/`High`)
- Request for approval before applying

**CRITICAL: Separate upstream changes — do NOT bundle unrelated changes into one resolution.**

When the THEIRS (upstream) side contains MULTIPLE independent changes (e.g., a syntax rename `$ReadOnly` → `Readonly` AND a semantic change like adding `Omit<>`), treat each change independently:
- Only apply the change that directly caused the conflict (usually the syntax/formatting change)
- Do NOT silently adopt additional semantic changes (new props, `Omit<>`, added/removed parameters) from THEIRS unless they are explicitly part of the same conflict region AND you have confirmed via the compare URL that the semantic change is safe for RNW
- If THEIRS has both a syntax change and a semantic change, propose ONLY the syntax change merged with OURS content, and separately flag the semantic change for review
- When in doubt, keep OURS content and only apply the minimal syntax fix from THEIRS

Example of what NOT to do:
```
THEIRS: Readonly<{ ...Omit<ViewProps, 'experimental_accessibilityOrder'>,
OURS:   $ReadOnly<{ ...ViewProps,

WRONG resolution:  Readonly<{ ...Omit<ViewProps, 'experimental_accessibilityOrder'>,  ← silently adopted Omit from upstream
CORRECT resolution: Readonly<{ ...ViewProps,  ← only took the syntax change, kept OURS content
```

**CRITICAL: Line-by-line diff the OURS and THEIRS blocks before proposing.**

Before proposing any resolution, explicitly diff the OURS and THEIRS blocks line by line:
1. Identify EVERY difference between OURS and THEIRS (not just the obvious one like `$ReadOnly` → `Readonly`)
2. Classify each difference as either:
   - **Syntax-only** (e.g., `$ReadOnly` → `Readonly`, `$FlowFixMe[incompatible-return]` → `$FlowFixMe[incompatible-type]`) — safe to take from THEIRS
   - **Content addition in THEIRS** (e.g., new props, `Omit<>` additions, new parameters) — do NOT take unless confirmed safe
   - **Content in OURS not in THEIRS** (e.g., Windows-specific types, `// [Windows` blocks) — ALWAYS keep
3. The proposed resolution MUST be: OURS content + only syntax-only fixes from THEIRS
4. If OURS had no wrapper (e.g., `type Props = {` without `$ReadOnly`) and THEIRS adds one (`Readonly<{`), keep OURS as-is — do NOT add wrappers that OURS didn't have
5. If OURS uses exact objects (`{||}`, `{|`) and THEIRS uses inexact (`{}`, `{`), keep OURS exact syntax

---

For each hunk, generate a PROPOSAL:

1. **Analysis**: Show OURS (RNW fork side) vs THEIRS (upstream side) with surrounding context
2. **Pattern match**: Attempt to match against patterns in [references/conflict-patterns.md](./references/conflict-patterns.md)
3. **Proposed merged output**: Apply merge prompt + file-type rules + folder-specific bias
4. **Validation**: Judge correctness + risk, assign confidence: `Low` / `Medium` / `High`
5. **Approval request** (REQUIRED for each hunk):
   - Present: file path, hunk index, "what changed" summary, confidence, exact replacement text
   - Ask: "Approve applying this hunk? (Yes/No)"
   - If No: propose alternative OR provide manual steps
   - If Low confidence: do NOT propose auto-application; provide manual-resolution plan instead

#### Phase 3b — Present Detailed Per-Hunk Proposals

After generating the batch summary table, present EACH hunk individually for review. For every hunk show:

1. **Header**: File path, hunk index (e.g., "Hunk 1/2"), line numbers
2. **OURS block**: The exact `=======` to `>>>>>>>` content (RNW Override side)
3. **THEIRS block**: The exact `<<<<<<<` to `=======` content (Upstream side)
4. **Surrounding context**: 3-5 lines before `<<<<<<<` and after `>>>>>>>`
5. **Pattern matched**: Which pattern from `references/conflict-patterns.md` applies (P1-P7)
6. **Compare URL**: The upstream compare link used
7. **Upstream intent**: 1-2 line summary of what upstream changed and why
8. **Proposed resolution**: The exact replacement text that would replace the entire conflict block (from `<<<<<<<` through `>>>>>>>` inclusive)
9. **Confidence**: `Low` / `Medium` / `High`
10. **Approval prompt**: "Approve applying this hunk? (Yes/No)"

Wait for the user's response before proceeding to the next hunk. If the user says:
- **Yes**: Apply the replacement immediately, confirm, then show next hunk
- **No**: Ask if they want an alternative proposal, manual steps, or to skip
- **Skip**: Leave conflict markers in place, move to next hunk
- **Yes to all remaining**: Apply all remaining hunks without further prompts (only if all are High confidence)

**File-type specific rules:**
- `.js`/`.ts`/`.tsx`: Preserve `// [Windows` sections; prefer upstream for pure formatting changes
- `.cpp`/`.h`: Check for `#ifdef WINDOWS` / platform-specific guards; upstream-first for shared code
- `.flowconfig`/config files: Keep RNW-specific entries, add new upstream entries
- `package.json`: Use target nightly version for all RN-related deps

**Folder-specific bias:**
- `vnext/src-win/`: Bias toward RNW (OURS) — these are Windows-specific overrides
- `packages/@office-iss/`: Bias toward RNW (OURS) — Win32-specific
- `packages/@react-native/tester/`: Bias toward upstream (THEIRS) — should match upstream
- `vnext/ReactCopies/`: Bias toward upstream (THEIRS) — should be exact copies

#### Phase 4 — Apply (Only After Per-Hunk Approval)

After explicit approval for a hunk:
- Replace the original conflict block with the validated "Resolved Hunk"
- Never touch intentional markers (E2E platform override files)
- Never remove the `flowconfig.windows.conflict` conflict

After all hunks are resolved:

```powershell
# Verify no unintentional conflict markers remain
$remaining = Get-ChildItem -Path . -Recurse -File |
    Where-Object { $_.FullName -notlike "*node_modules*" -and $_.Name -ne "flowconfig.windows.conflict" } |
    ForEach-Object {
        $content = Get-Content -Path $_.FullName -Raw -ErrorAction SilentlyContinue
        if ($content -match '<<<<<<<') { $_.FullName }
    }

if ($remaining.Count -eq 0) {
    Write-Host "All conflicts resolved!"
} else {
    Write-Host "WARNING: $($remaining.Count) files still have conflict markers"
    $remaining | ForEach-Object { Write-Host "  - $_" }
}

# Commit resolved conflicts
git add -A
git commit -m "Resolve merge conflicts for RN $targetVersion integration"
```

## Key Files to Update

When integrating a new nightly version, these files typically need updates:

| File | Dependencies to Update |
|------|----------------------|
| `vnext/package.json` | `react-native`, `@react-native/*` packages |
| `package.json` (root) | `react-native` in resolutions |
| Various app `package.json` files | Peer dependencies |

## Notes

- The major version (e.g., 0.84 → 0.85) may change between nightlies
- Always verify the target version exists before updating dependencies
